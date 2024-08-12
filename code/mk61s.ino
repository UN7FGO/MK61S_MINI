#include "config.h"

static  class_calc_config   config;

#include <Arduino.h>
#include <rust_types.h>
#include <keyboard.hpp>

#include <LiquidCrystal.h>
#include "lcd_gui.hpp"

#include "mk61emu_core.h"

static  class_mk61_core    mk61s;
//static  class_keyboard     keyboard;
//using namespace kbd;

#include "cross_hal.h"
#include "disasm.hpp"
#include "tools.hpp"
#include "menu.hpp"
//#include "menu_mk61lib.hpp"
#include "basic.hpp"

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E, PIN_LCD_DB4, PIN_LCD_DB5, PIN_LCD_DB6, PIN_LCD_DB7);

static class_menu/*_mk61lib*/  mk61_menu = class_menu((t_punct**) library_mk61::MENU, library_mk61::COUNT_PUNCTS);

const  class_glyph      glyph; //(lcd);
const  class_LCD_Label  MnemoLabel(10, 0);
const  class_LCD_Label  MarkLabel(0, 0);
const  class_LCD_Label  XLabel(0, 1);
const  class_LCD_Label  DisasmLabel(0, 0);
const  class_LCD_Label  IPLabel(14, 1);
const  class_LCD_Label  GRDLabel(6, 0);

static  class_disassm_mk61  disassembler;
static  u32                 exeq;
static  enum_core61_stage   core_stage;

static  const u32   CALC_WAIT_MS          = 10;
static  const u32   UPDATE_R_GRD_G_MS     = 3000;  // DВремя в мс для запуска процесса сохранения переключателя Р-ГРД-Г
static  u32         update_R_GRD_G;
static  u32         wait_calc_time;
static  u32         runtime_ms;

static  const u32   KEY_USER_HOLD_ms      = 550;
static  u32         time_key_hold;
static  bool        check_hold_USER_KEY; 

static  bool        YZ_ZT;

static  u32         mnemo_pos;
static  char        mnemo_buffer[8];
static  i32         last_key;

#ifdef SERIAL_OUTPUT
  #include "terminal.hpp"
  static  class_terminal      terminal;
#endif

const char display_symbols[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'L', 'C', G_RUS, 'E', ' '
};
                                       //0123456789ABCD
char        display_text[14];          //-12345678 -12
AngleUnit   cache_angle;

static u8   mk61_go_basic[105];
static bool BASIC_mode;

void lcd_std_display_redraw(void) { // Принудительная отрисовка стандартного экрана MK61s_mini
  lcd.clear();
  cache_angle = AngleUnit::NONE;  // сбросим кеш угловых единиц, необходимо перерисовать Г-ГРД-Р
  display_text[0] = (char) -1;
  mk61_display_refresh();
}

void mk61_display_refresh(void) {
  char disasm[LEN_DISASM_LINE+1];
  
  // Обновление дисплея МК61, если изменилась информация на экране
    if(!mk61s.update_indicator(&display_text[0], display_symbols)) { 
      XLabel.print(display_text);
      #ifdef SERIAL_OUTPUT
        terminal.dbg_info("[mk61_display_refresh] ", display_text);
      #endif
    }

    if(BASIC_mode) return;

  // вывод содержимого счетчика команд в режимем АВТ, для отладки по ПП
    if(exeq == 0) { // Режим АВТ работа с калькулятором в диалоговом режиме
    
      if(disassembler.is_update(&disasm[0])) { // Включен режим отображения дизассемблера МК61
        DisasmLabel.print(disasm);
      }

      if(config.output_IP && mk61s.get_comma_position() != 0) { // НЕ РЕЖИМЕ ПРГ!
        const u8 IP = mk61s.get_IP();
        IPLabel.print_hex(IP);
      }
    }

  // Переключатель Р-ГРД-Г
    const AngleUnit angle_now = MK61Emu_GetAngleUnit();
    if(angle_now != cache_angle) {
      switch(angle_now) {
        case RADIAN: 
            GRDLabel.print("P  ");
          break;
        case DEGREE:
            GRDLabel.print("  \x05");
          break;
        case GRADE:
            GRDLabel.print("\x05P\x03");
      }
      cache_angle = angle_now;
    }
}

void inline lcd_stack_output(void) {
  lcd.clear();
  if(YZ_ZT) {
      lcd.setCursor(0,0); lcd.print("Z "); lcd.print(MK61Emu_GetStackStr(StackRegister::REG_Z, display_symbols));
      lcd.setCursor(0,1); lcd.print("Y "); lcd.print(MK61Emu_GetStackStr(StackRegister::REG_Y, display_symbols));
  } else {
      lcd.setCursor(0,0); lcd.print("T "); lcd.print(MK61Emu_GetStackStr(StackRegister::REG_T, display_symbols));
      lcd.setCursor(0,1); lcd.print("Z "); lcd.print(MK61Emu_GetStackStr(StackRegister::REG_Z, display_symbols));
  }
  YZ_ZT = !YZ_ZT;  // флаг чередования вывода пар регистров YZ или ZT
}

void mk61_display_draw_upline(void) {
    MarkLabel.print("          ");
}

void  InfoData(void) {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("cnt:"); lcd.print(read_counter_switch());
  lcd.print(" sw:"); lcd.print((u8) read_grade_switch());
  lcd.setCursor(0,1); 
  lcd.print("run:"); lcd.print(runtime_ms);
  keyboard.get_key_wait();
  mk61_display_refresh();
}

void setup() {
  #ifdef SERIAL_OUTPUT
    terminal.init();
    Serial.println(FIRMWARE_VER);
  #endif
  keyboard.Init();
  
  #if defined(REVISION_V2) || defined(REVISION_V3)
    pinMode(PIN_LCD_RW, OUTPUT);
    digitalWrite(PIN_LCD_RW, LOW);
  #endif
  lcd.begin(16, 2);

 // Тест ожидания нажатия кнопки на клавиатуре  
  //lcd.setCursor(0,1); lcd.print(" wait key press ");
  //keyboard.get_key_wait();
  //lcd.setCursor(0,1); lcd.print("key = "); lcd.print((u8) keycode, HEX); lcd.print("        ");
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
  delay(2000);

  const class_LCD_fonts lcd_fonts;
  lcd_fonts.load();
  
  lcd.clear();

 // Система ззагрузки программ
  init_library();

 // Запуск эмулятора MK61
  load_grade_switch();
  cache_angle = AngleUnit::NONE; // сбросим кеш угловых единиц в NONE для перерисовки подписи на ЖК

  YZ_ZT = true;
  check_hold_USER_KEY = false; 
  wait_calc_time = millis() + CALC_WAIT_MS;
  update_R_GRD_G = millis() + UPDATE_R_GRD_G_MS;
  core_stage = START;
  exeq = 0;

  mnemo_pos   = 0;
  clear_mnemo();

  mk61s.enable();
  memset(&mk61_go_basic, 0xFF, sizeof(mk61_go_basic));
  InitBasic();
  BASIC_mode = false;

  #ifdef SERIAL_OUTPUT
    terminal.dbg_info("ON");
  #endif
}

void    clear_mnemo(void) {
  mnemo_pos = 0;
  *(u32*) &mnemo_buffer[0] = 0x00000000;
  *(u32*) &mnemo_buffer[4] = 0x00000000;
}

int    next_mnemo_pos(const char* descriptor) {
  const i8 control = (i8) descriptor[0]; 
  if(control < 0) {
    const u8 len = ((u8) control & ~STORE_KEY);
    for(int i=0; i < len; i++) mnemo_buffer[mnemo_pos + i] = descriptor[i+1];
    return mnemo_pos + len;
  } else {
    for(int i=0; i < (u8) control; i++) mnemo_buffer[mnemo_pos + i] = descriptor[i+1];
    return 0;
  }
}

void   build_mnemo(i32 keycode) {
  if(mnemo_pos == 0 || keycode == KEY_K || keycode == KEY_F) { // Первый вход в построение мнемокода нажатой набираемой функции
    clear_mnemo();
    mnemo_pos = next_mnemo_pos(mnemo_code[keycode]);
  } else {
    switch(last_key) {
      case  KEY_F:
          mnemo_pos = next_mnemo_pos(mnemo_code_F[keycode]);
        break;
      case  KEY_K:
          mnemo_pos = next_mnemo_pos(mnemo_code_K[keycode]);
        break;
      case  KEY_Px:
      case  KEY_xP:
          mnemo_buffer[mnemo_pos] = mnemo_code_register[keycode];
          mnemo_pos = 0;
        break;
      case  KEY_PP:
      case  KEY_BP:
      case  KEY_RUN:
      case  KEY_RET:
      case  KEY_FRW:
      case  KEY_BKW:
          mnemo_buffer[mnemo_pos] = mnemo_code_register[keycode];
          if(mnemo_pos > 2) { 
            mnemo_pos = 0;
          } else {
            mnemo_pos++;
            return;
          }
        break;
      default:
          mnemo_pos = 0;
    }
  }
  #ifdef DEBUG
    Serial.print("mnemo_pos "); Serial.println(mnemo_pos);
  #endif
  last_key = keycode;
}

void key_press_handler(i32 keycode) {
  char mnemo[8];
  const TMK61_cross_key cross_key = KeyPairs[keycode];  // трансляция кода клавиши в координаты клавиши mk61

  #ifdef SERIAL_OUTPUT
    terminal.debug_key_info(keycode);
  #endif

  //keyboard.reset_scan_line();
  
  build_mnemo(keycode);
  *(u64*) &mnemo = pad_left_8_char(mnemo_buffer);
  MnemoLabel.print(mnemo);

  const u32 now = millis();
  switch(keycode) {
    case KEY_DEGREE:
        MK61Emu_SetAngleUnit(DEGREE);
        update_R_GRD_G = now + UPDATE_R_GRD_G_MS;
      break;
    case KEY_GRADE: 
        MK61Emu_SetAngleUnit(GRADE);
        update_R_GRD_G = now + UPDATE_R_GRD_G_MS;
      break;
    case KEY_RADIAN:
        MK61Emu_SetAngleUnit(RADIAN);
        update_R_GRD_G = now + UPDATE_R_GRD_G_MS;
      break;
    default:
      if(keycode >= 0) {
        BASIC_mode = false;
        MK61Emu_SetKeyPress(cross_key.x, cross_key.y); // передача нажатия в MK61s
      }
  }
}

void  check_and_start_basic(void) {
  const int mk61_IP = mk61s.get_IP();
  const int BasicN  = mk61_go_basic[mk61_IP];

  #ifdef DEBUG_BASIC
    Serial.print("Read binding from MK61 step N "); Serial.println(mk61_IP); 
    Serial.print("Assign BASIC program N "); Serial.println(BasicN); 
  #endif

  if(BasicN != 0xFF){
    #ifdef DEBUG_BASIC
      Serial.println("Run BASIC code!");
    #endif
    BASIC_mode = true;
    RunBasic(BasicN);
  }
}

inline void mk61_automate(void) {
  i32 keycode;

  switch(core_stage) {
    case  START:
      
      keycode = keyboard.get_key(PRESS);
      if(keycode >= 0) {
        key_press_handler(keycode);
        mk61s.step();
      }

      core_stage = NEXT;
      break;
    case  NEXT:
      mk61s.step();
      const bool comma_in_11 = (mk61s.get_comma_position() == 11);

      if(exeq == 0) { // MK61 в режиме вычислений
        if(comma_in_11) { // MK61 был первый шаг по программе старт с С/П
          runtime_ms = millis();
          #ifdef SERIAL_OUTPUT
            terminal.dbg_info("PRG: first step");
            terminal.dbg_info(runtime_ms);
          #endif
          DisasmLabel.print("RUN");
          exeq = 1;
        } else { 
          if(mk61s.is_displayed()) core_stage = START;
        }
      } else { // MK61 в режиме работы по программме 
        #ifdef SERIAL_OUTPUT
          terminal.dbg_info("PRG: next steps");
        #endif

        keycode = keyboard.get_key(PRESS);
        if(keycode >= 0) key_press_handler(keycode);

        if(!comma_in_11) { // MK61 произвел останов С/П при работе по программе
          runtime_ms = millis() - runtime_ms;
          #ifdef SERIAL_OUTPUT
            terminal.dbg_info("PRG: STOP");
            terminal.dbg_info(runtime_ms);
          #endif
          MnemoLabel.print("   ");
          exeq = 0;
          core_stage = START;
          
          if(BasicIsReady()) check_and_start_basic();
        }
      }
  } // end switch 
}

void loop() {
  const u32 now = millis();
  const AngleUnit new_angle = MK61Emu_GetAngleUnit();
  const AngleUnit old_angle = read_grade_switch();
  if( old_angle != new_angle && now >= update_R_GRD_G) { // Сохраним состояние переключателя градусной меры
    update_R_GRD_G = now + UPDATE_R_GRD_G_MS;
    store_grade_switch(new_angle);
    #ifdef SERIAL_OUTPUT
      Serial.print("store switch from "); Serial.print((u8) old_angle); Serial.print(" to "); Serial.println((u8) new_angle);
    #endif      
  }

  keyboard.next_scan_line();

  if(check_hold_USER_KEY) { // ожидания события удержания клавиши USER 

    if(time_key_hold < millis()) {
      // Покажем все регистры стека MK61
      lcd_stack_output();
      time_key_hold += 3*1000;
    }

    const i32 last_key_code = keyboard.get_key();
    if(last_key_code == KEY_USER_RELEASE) { // отжата клавиша USER

      keyboard.exclude_before(KEY_USER_PRESS); // уберем все коды отпускания/нажатия клавиш включая нажатие KEY_USER, из очереди клавиатуры
      check_hold_USER_KEY = false;  
      lcd_std_display_redraw();
    }


  } else { // обычная работа, клавиша USER не была активной
    const i32 last_key_code = keyboard.last_key();
    const u32 now_time = millis();

    if (last_key_code == KEY_USER_PRESS) { // последний код из буфера клавиатуры  - нажатая клавиша KEY_USER
        
        time_key_hold = now_time + KEY_USER_HOLD_ms; // установим время удержания USER
        check_hold_USER_KEY = true;
        YZ_ZT = true;

        keyboard.get_key(); // очистим буфер клавиатуры от этого кода
        #ifdef DEBUG
          Serial.print("set hold time: "); Serial.print(time_key_hold); Serial.println("ms, del USER key!"); 
        #endif 
    } else if (last_key_code == KEY_ESC_PRESS) { // последний код из буфера клавиатуры  - нажатая клавиша KEY_ESC (выбор меню)
        keyboard.get_key(); // очистим буфер клавиатуры от этого кода
        mk61_menu.select();
        lcd_std_display_redraw();
        #ifdef SERIAL_OUTPUT
          Serial.println("menu exit.");
        #endif 
    } else if (last_key_code == KEY_BASIC) {
        keyboard.get_key(); // очистим буфер клавиатуры от этого кода
        const int assign_code = AssignBasic();
        const int mk61_IP = mk61s.get_IP();
        #ifdef DEBUG_BASIC
          Serial.print("Assign MK61 step N "); Serial.print(mk61_IP); Serial.print(" BASIC program N "); Serial.println(assign_code);
        #endif
        mk61_go_basic[mk61_IP] = assign_code;
        lcd_std_display_redraw(); 
    } else {

      mk61_automate();

      if(mk61s.is_displayed()) {
        mk61s.clear_displayed();
        mk61_display_refresh();
      }

      const class_mk61_core::mode_edge changing_mode = mk61s.get_calculator_mode();
      if(!BASIC_mode && changing_mode != class_mk61_core::mode_edge::UNCHANGED) { // Ловим только фронт перехода из одного состояния в другое
        #ifdef DEBUG
          Serial.println("changin mode PRG/AVT");
        #endif
        if(changing_mode == class_mk61_core::mode_edge::TURN_PRG) { // режим ПРГ включился
          disassembler.on();
          IPLabel.print("  ");
        } else { // режим АВТ, либо СЧЕТ включились
          cache_angle = AngleUnit::NONE;  // для обновления подписи, сбросим кеш угловых единиц измерения (переключатель Р_ГРД_Г)
          if(!config.disassm) {
            disassembler.off();
            if(exeq == 0) { // режим АВТ включен
              DisasmLabel.print("      ");
            } 
          }
        }
      }

    }

    #ifdef SERIAL_OUTPUT
      const i32 key_from_terminal = terminal.serial_input_handler();
      if(key_from_terminal >= 0) keyboard.cir_buff.write((i8) key_from_terminal);
    #endif
  }

  keyboard.check_scan_line();
}

#ifdef SERIAL_OUTPUT
void serialEvent() {
  if(Serial.available() && terminal.inEOF() && terminal.inEOL()){ // В UART что то свалилось и не выставлен флаг нуждается в обработке, принимаем
    const u8 inChar = Serial.read(); // вычитываем один символ
    *terminal.recive_char++ = inChar;
    if(inChar == CR) { 
      terminal.recive_char--;
    } 
  }
}
#endif