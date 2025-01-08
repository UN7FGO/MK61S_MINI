#include "config.h"

static  class_calc_config   config;

#include <Arduino.h>
#include <rust_types.h>
#include <keyboard.h>

using namespace kbd;

#include <LiquidCrystal.h>
#include "lcd_gui.hpp"
#include "mnemo.hpp"

#include "mk61emu_core.h"

#include "cross_hal.h"
#include "disasm.hpp"
#include "tools.hpp"
#include "menu.hpp"
#include "basic.hpp"

#include "ledcontrol.h"
using namespace led;

#include "debug.h"

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E, PIN_LCD_DB4, PIN_LCD_DB5, PIN_LCD_DB6, PIN_LCD_DB7);

static class_menu           mk61_menu = class_menu((t_punct**) library_mk61::MENU, library_mk61::COUNT_PUNCTS);

const  class_glyph          glyph; //(lcd);
const  class_LCD_Label      MarkLabel(0, 0);
const  class_LCD_Label      XLabel(0, 1);

static  LCD_GRD_Label       GRDLabel;
static  key_mnenonic        MnemoLabel;

static  class_disassm_mk61  disassembler;
static  usize               exeq;
static  enum_core61_stage   core_stage;
static  isize               mk61_quants;
isize                       mk61_quants_reload;

static constexpr t_time_ms  CALC_WAIT_MS           =     10;
static constexpr t_time_ms  ANGLE_SAVE_UPDATE_MS   =   3000;  // Время (мс) для запуска процесса сохранения переключателя угловых единиц Р-ГРД-Г

t_time_ms   runtime_ms; // время работы программы в ms

static  u32         update_R_GRD_G;
static  u32         wait_calc_time;

static  bool        YZ_ZT;
static  bool        lcd_hooked;
static  bool        need_draw_lock_message;

#ifdef SERIAL_OUTPUT
  #include "terminal.hpp"
  static  class_terminal      terminal;
#endif

const char terminal_symbols[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'L', 'C', '\303', 'E', ' '
};

const char display_symbols[16] = {
    'O', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'L', 'C', G_RUS, 'E', ' '
};
                                       //0123456789ABCD
char        display_text[14];          //-12345678 -12

static u8   mk61_go_basic[105];
static bool BASIC_mode;

#ifdef MK61_EXTENDED
  static bool first_key_clean_upline;
#endif

void lcd_std_display_redraw(void) { // Принудительная отрисовка стандартного экрана MK61s_mini
    lcd.clear();
    GRDLabel.print(MK61Emu_GetAngleUnit());
    display_text[0] = (char) -1;
    if(exeq == 1) {
      lcd.setCursor(0, 0); lcd.print("RUN");
    }
    mk61_display_refresh();
}

void mk61_display_refresh(void) {
  // Обновление дисплея МК61, если изменилась информация на экране
    if(!core_61::update_indicator(&display_text[0], display_symbols)) { 
      XLabel.print(display_text);
      dbgln(MINI, "[mk61_display_refresh] ", display_text);
    }

  // вывод содержимого счетчика команд в режимем АВТ, для отладки по ПП
    if(exeq == 0) { // Режим АВТ работа с калькулятором в диалоговом режиме
      disassembler.print();
    }
}

void inline lcd_stack_output(void) {
  char cvalue[15];
  cvalue[14] = 0;

  lcd.clear();
  if(YZ_ZT) {
      lcd.setCursor(0,1); lcd.print("Y "); lcd.print(read_stack_register(stack::Y, cvalue, display_symbols));
        //MK61Emu_GetStackStr(StackRegister::REG_Y, display_symbols));
      lcd.setCursor(0,0); //lcd.print("Z "); lcd.print(read_stack_register(stack::Z, cvalue, terminal_symbols));
        //MK61Emu_GetStackStr(StackRegister::REG_Z, display_symbols));
      YZ_ZT = false;  // флаг чередования вывода пар регистров YZ или ZT -> ZT
  } else {
      lcd.setCursor(0,0); lcd.print("T "); lcd.print(read_stack_register(stack::T, cvalue, display_symbols));
        //MK61Emu_GetStackStr(StackRegister::REG_T, display_symbols));
      lcd.setCursor(0,1); 
        //MK61Emu_GetStackStr(StackRegister::REG_Z, display_symbols));
      YZ_ZT = true;   // флаг чередования вывода пар регистров YZ или ZT -> YZ
  }
  lcd.print("Z "); lcd.print(read_stack_register(stack::Z, cvalue, display_symbols));
}

void setup() {
  led::init();

  #ifdef SERIAL_OUTPUT
    terminal.init();
  #endif
  dbgln(MINI, FIRMWARE_VER);

  #ifdef SPI_FLASH
      init_external_flash();
  #endif

//  kbd::test();
  kbd::init();
  
  #if defined(REVISION_V2) || defined(REVISION_V3)
    pinMode(PIN_LCD_RW, OUTPUT);
    digitalWrite(PIN_LCD_RW, LOW);
  #endif
  lcd.begin(16, 2);

  glyph.draw(0);
  delay(1000);
  for(int i=1; i < 17; i++) {
    // смещение глифа на 1 колонку вправо
    glyph.draw(i);
    // вывод версии ПО как "бегущей строки" вслед за смещением вправо глифа
    lcd.setCursor(0,0); lcd.print((char*) &FULL_MODEL_NAME[16-i]);
    lcd.setCursor(0,1); lcd.print((char*) &FIRMWARE_VER[16-i]);
    delay(2000/16);
  }
  sound(PIN_BUZZER, 300, 200);
  led::on();
  delay(1500);
  led::off();

 //---  Настройка отрисовки экрана
  lcd_hooked = false;               // экран не перeхвачен
  need_draw_lock_message = true;    // флаг уже отрисованного сообщения блокировки ядра

  #ifdef MK61_EXTENDED
    first_key_clean_upline = false;
  #endif

  const class_LCD_fonts lcd_fonts;
  lcd_fonts.load();
  lcd.clear();

 // Система ззагрузки программ
  init_library();

 // Запуск эмулятора MK61
  GRDLabel.print(load_grade_switch()); // считаем состояние переключателя ГРД отобразим градусную меру

  YZ_ZT = true;
  wait_calc_time = millis() + CALC_WAIT_MS;
  update_R_GRD_G = millis() + ANGLE_SAVE_UPDATE_MS;

  core_stage          =   START;
  exeq                =   0;
 // Настройки режима MAXIMAL  
  mk61_quants_reload  =   1;
  mk61_quants         =   mk61_quants_reload;

  core_61::enable();
  memset(&mk61_go_basic, 0xFF, sizeof(mk61_go_basic));
  InitBasic();
  BASIC_mode = false;

  dbgln(MINI, "ON");
}

void entry_programm_mode(void) { // Вход в режим ПРГ - событие генерируется key_mnemonics
  dbgln(MINI, "AVT -> PRG")
  disassembler.enable();
}

void exit_auto_mode(void) { // Вsход bp режима ПРГ - событие генерируется key_mnemonics
  dbgln(MINI, "AVT -> PRG")
  if(!config.disassm) disassembler.disable(); // Если в конфиге не включен "ВСЕГДА"
}

void key_press_handler(i32 keycode) {
  const TMK61_cross_key cross_key = KeyPairs[keycode];  // трансляция кода клавиши в координаты клавиши mk61

  dbg(KBD, "x,y = ", cross_key.x, ",", cross_key.y); dbghex(KBD, " scancode $", keycode); dbgln(KBD, " -> mk61")

  //keyboard.reset_scan_line();  // Для ускорения опроса клавиатуры - переход к первой линии сканирования
  
  MnemoLabel.print(keycode);

  const u32 now = millis();
  #ifdef MK61_EXTENDED
    first_key_clean_upline = false; // снимем флажок первого нажатия
  #endif
  switch(keycode) {
    case KEY_DEGREE:
        MK61Emu_SetAngleUnit(DEGREE);
        GRDLabel.print("  \x05");
        update_R_GRD_G = now + ANGLE_SAVE_UPDATE_MS;
      break;
    case KEY_GRADE: 
        MK61Emu_SetAngleUnit(GRADE);
        GRDLabel.print("\x05P\x03");
        update_R_GRD_G = now + ANGLE_SAVE_UPDATE_MS;
      break;
    case KEY_RADIAN:
        MK61Emu_SetAngleUnit(RADIAN);
        GRDLabel.print("P  ");
        update_R_GRD_G = now + ANGLE_SAVE_UPDATE_MS;
      break;
    default:
      if(keycode >= 0) {
        BASIC_mode = false;
        MK61Emu_SetKeyPress(cross_key.x, cross_key.y); // передача нажатия в MK61s
      }
  }
}

void  check_and_start_basic(void) {
  const int mk61_IP = core_61::get_IP();
  const int BasicN  = mk61_go_basic[mk61_IP];

  dbgln(BASIC, "Read binding from MK61 step N ", mk61_IP, " assign BASIC program N ", BasicN); 

  if(BasicN != 0xFF){
    dbgln(BASIC, "Run BASIC code!");
    BASIC_mode = true;
    RunBasic(BasicN);
  }
}

static constexpr usize hz_STOP_SIGNAL  =   200;  // Hz
static constexpr usize ms_STOP_SIGNAL  =   850;  // ms

/* СОБЫТИЯ автомата конечных состояний МК-61 */
inline void  event_stop_in_prg_mk61(void) {
  runtime_ms = millis() - runtime_ms;
  // Для измерений производительности 
  #ifdef DEBUG_MEASURE
    char mk61_display[14];
    core_61::update_indicator(&mk61_display[0], terminal_symbols);
    dbgln(MEASURE, "time elapsed (ms): ", runtime_ms, " : ", mk61_display);
  #endif

  dbgln(MINI, "PRG: STOP dt = ", runtime_ms, " mk61_reload_quant = ", mk61_quants_reload);

  sound(PIN_BUZZER, hz_STOP_SIGNAL, ms_STOP_SIGNAL);
  MnemoLabel.enable();
  if(!config.disassm) disassembler.disable(); // если дизассемблер включен в конфигурации "ВСЕГДА" то выключение ненужно

  exeq        =   0;
  core_stage  =   START;
         
  if(BasicIsReady()) check_and_start_basic();

  #ifdef MK61_EXTENDED
    disassembler.disable();
    lcd.setCursor(0,0);
    for(usize i = 0; i < 5; i++) {
      const u8 byte = mk61s.byte_from_R(14, i);
      const char letter = (byte == 0)? ' ' : ((char) byte);
      lcd.write(letter);
    }
  #endif
}

inline void  event_start_prg_mk61(void) {
  dbgln(MINI, "PRG: first step dt = ", runtime_ms, " mk61_reload_quant = ", mk61_quants_reload);
  MnemoLabel.disable();
  disassembler.disable("RUN");
  exeq        =   1;
  mk61_quants =   mk61_quants_reload;
  runtime_ms  =   millis();
}

/* АВТОМАТ конечных состояний МК-61 */
inline void mk61_automate(void) {
  i32 keycode;

  switch(core_stage) {
    case  START:
      
      keycode = kbd::get_key(PRESS);
      if(keycode >= 0) {
        key_press_handler(keycode);
        core_61::step();
      }

      core_stage = NEXT;
      break;
    case  NEXT:
      core_61::step();
      const bool comma_in_11 = (core_61::comma_position() == 11);

      if(exeq == 0) { // MK61 в режиме вычислений
        if(comma_in_11) { // MK61 был первый шаг по программе старт с С/П
          event_start_prg_mk61();
        } else { 
          if(core_61::is_displayed()) core_stage = START;
        }
      } else { // MK61 в режиме работы по программме 
        dbgln(MINI, "PRG: next steps");

        keycode = kbd::get_key(PRESS);
        if(keycode >= 0) key_press_handler(keycode);

        if(!comma_in_11) { // MK61 произвел останов С/П при работе по программе
          event_stop_in_prg_mk61();
        }
      }
  } // end switch 
}

inline void monitor_switch_angle_unit(t_time_ms now) {
  const AngleUnit new_angle = MK61Emu_GetAngleUnit();
  const AngleUnit old_angle = read_grade_switch();
  
  if( old_angle != new_angle && now >= update_R_GRD_G) { // Сохраним состояние переключателя градусной меры
    update_R_GRD_G = now + ANGLE_SAVE_UPDATE_MS;
    store_grade_switch(new_angle);
    dbgln(MINI, "store switch from ", old_angle, " to ", new_angle);
  }
}

inline void mk61_process(void) {
  mk61_automate();
  if(core_61::is_displayed()) {
      core_61::clear_displayed();
      if(!lcd_hooked) mk61_display_refresh();
  }
}

void loop() {

  #ifdef TERMINAL // Подмена полученной с терминала клавиши через буфер клавиатуры
    const i32 key_from_terminal = terminal.serial_input_handler();
    if(key_from_terminal >= 0) kbd::push((i8) key_from_terminal);
  #endif

  switch(kbd::last_key()) {
    case KEY_ESC_PRESS:
        kbd::get_key(); // очистим буфер клавиатуры от этого кода
        mk61_menu.select();
        lcd_std_display_redraw();
        dbgln(MENU, "menu exit");
      break;
    case  KEY_LOAD:
        kbd::get_key(); // очистим буфер клавиатуры от этого кода
        if(Load()) message_and_waitkey(" press any key! ");
        lcd_std_display_redraw(); 
      break;
    case  KEY_SAVE:
        kbd::get_key(); // очистим буфер клавиатуры от этого кода
        if(Store()) message_and_waitkey(" press any key! ");
        lcd_std_display_redraw(); 
      break;
    default:
      if(exeq == 0) { // Режим автоматической работы (АВТ) без задержки!
        const u32 now = millis();
        monitor_switch_angle_unit(now); // слежение за положением b сохранением в flash переключателя градусной меры (только в АВТ режиме)
        mk61_process();
      } else {        // Режим работы по программе (СЧЕТ) задержка устанавливается в меню Speed CLASSIC/MAXIMAL
        if(--mk61_quants == 0) {
          mk61_process();
          mk61_quants = mk61_quants_reload;
        }
      }
  }
  
  kbd::scan();
}

void idle_main_process(void) {
  led::control();
}

void event_hold_key(i32 holded_key, i32 hold_quant) {
  switch(holded_key) {
      case KEY_USER_PRESS: // Удержание USER KEY, вывод стека XYZT на экран
          lcd_hooked = true;  // перехват экрана
          dbgln(KBD, "HOLD RIGHT KEY, quant = ", hold_quant);
          lcd_stack_output();
        break;
      default:
        kbd::clear_hold_key();
  }
}

void event_unhold_key(i32 unholded_key) {
  switch(unholded_key) {
      case KEY_USER_PRESS:
          lcd_hooked = false;
          dbgln(KBD, "UNHOLD RIGHT KEY");
          kbd::exclude_before(KEY_USER_PRESS); // уберем все коды отпускания/нажатия клавиш включая нажатие KEY_USER, из очереди клавиатуры
          lcd_std_display_redraw();
        break;
  }
}
