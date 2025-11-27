#include "LiquidCrystal.h"
#include "menu.hpp"    
#include "cross_hal.h"

extern LiquidCrystal lcd;
extern t_time_ms runtime_ms;
//extern isize mk61_quants_reload;

namespace library_mk61 {

bool  HardwareInfo(void) {
  lcd.clear(); 
  lcd.setCursor(0,0); lcd.print("Chip:"); lcd.print(chip_name);
  lcd.setCursor(0,1); lcd.print(mem_text);
  kbd::get_key_wait();
  return action::MENU_BACK;
}

bool  InfoData(void) {
  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("cnt:"); lcd.print(read_counter_switch());
  lcd.print(" sw:"); lcd.print((u8) read_grade_switch());
  if(flash_is_ok) lcd.print(" W25"); 
  lcd.setCursor(0,1); 
  lcd.print("run "); lcd.print(runtime_ms); lcd.print(" ms");
  kbd::get_key_wait();
  return false;
}

const t_punct DFU_mode_punct      = {.size = 15, .action = (menu_action) &DFU_enable,           .text = "DFU mode enable"};
const t_punct LIB_61_punct        = {.size = 12, .action = &mk61_library_select,                .text = "MK61 library"};
const t_punct GAME_61_punct       = {.size = 10, .action = &mk61_games_select,                  .text = "MK61 Games"};
const t_punct RESET_punct         = {.size = 12, .action = (menu_action) &NVIC_SystemReset,     .text = "Reset device"};
const t_punct ERASE_punct         = {.size = 12, .action = (menu_action) &EraseFlash,           .text = "Erase FLASH!"};
const t_punct SOUND_ON_punct      = {.size = 16, .action = (menu_action) &TurnSound,            .text = "Sound ON       "};
const t_punct SOUND_OFF_punct     = {.size = 16, .action = (menu_action) &TurnSound,            .text = "Sound OFF      "};
const t_punct SPEED_LOW_punct     = {.size = 16, .action = (menu_action) &TurnSpeed,            .text = "Speed CLASSIC  "};
const t_punct SPEED_HIGH_punct    = {.size = 16, .action = (menu_action) &TurnSpeed,            .text = "Speed MAXIMUM  "};
const t_punct FLASH_punct         = {.size = 11, .action = (menu_action) &InfoData,             .text = "Information"};
const t_punct HARDWARE_punct      = {.size = 8,  .action = (menu_action) &HardwareInfo,         .text = "Hardware"};

t_punct* MENU[MENU_PUNCT] = {
      (t_punct*) &DFU_mode_punct,
      (t_punct*) &SOUND_ON_punct,
      (t_punct*) &SPEED_HIGH_punct,
      (t_punct*) &LIB_61_punct,
      (t_punct*) &GAME_61_punct,
      (t_punct*) &RESET_punct,
      (t_punct*) &ERASE_punct,
      (t_punct*) &FLASH_punct,
      (t_punct*) &HARDWARE_punct
};

bool  sound_is_on(void) {
  return library_mk61::MENU[1] == &library_mk61::SOUND_ON_punct;
}

bool  speed_is_max(void) {
  return library_mk61::MENU[2] == &library_mk61::SPEED_HIGH_punct;
}

} // namespace library_mk61

bool   TurnSpeed(void) {
  extern isize mk61_quants_reload;

  if(library_mk61::speed_is_max()) {
    library_mk61::MENU[2] = (t_punct*) &library_mk61::SPEED_LOW_punct;
    mk61_quants_reload = cfg::CLASSIC_MK61_QUANTS;
  } else {
    library_mk61::MENU[2] = (t_punct*) &library_mk61::SPEED_HIGH_punct;
    mk61_quants_reload = 1;
  }

  return action::MENU_BACK;
}

bool   TurnSound(void) {
  if(library_mk61::sound_is_on()) {
    library_mk61::MENU[1] = (t_punct*) &library_mk61::SOUND_OFF_punct;
  } else {
    library_mk61::MENU[1] = (t_punct*) &library_mk61::SOUND_ON_punct;
  }

  return action::MENU_BACK;
}

bool  mk61_library_select(void) {
  const int n = select_program();
  if(n < 0) return action::MENU_BACK;

  load_program(n);
  return action::MENU_EXIT;
}

bool  mk61_games_select(void) {
  const int n = select_game();
  if(n < 0) return action::MENU_BACK;

  load_game(n);
  return action::MENU_EXIT;
}

void class_menu::draw(void) {
  const int delta = (active_punct + 1) - SIZE_MENU_WINDOW;
  const int up = (delta <= 0)? 0 : delta;

  for(int i=0; i < SIZE_MENU_WINDOW; i++) {
    lcd.setCursor(0,  i); 
    const int real_index = i + up;
    const int previous_real_index = i + previous_up;

    // формируем постоянную часть пункта меню
    lcd.print( (active_punct == real_index)?  '>'  :  ' ' );
    lcd.print(puncts[real_index]->text);

    int previous_punct_size = puncts[previous_real_index]->size;
    const int size = puncts[real_index]->size;
    // формируем переменную часть пункта меню
    while(previous_punct_size-- > size) {
      lcd.print(' ');
    }
  }
  previous_up = up;
}

void class_menu::select(void) {
  lcd.clear();
  do{
    draw();
    const i32 last_key_code = kbd::get_key_wait();
    switch(last_key_code) {
      case KEY_RIGHT_PRESS:
              if(active_punct < (MENU_PUNCT_COUNT-1)) active_punct++;
        break;
      case KEY_LEFT_PRESS:
              if(active_punct > 0) active_punct--;
        break;
      case KEY_OK_PRESS:
            dbgln(MENU, "Select menu: '", puncts[active_punct]->text, "\'");
            if(puncts[active_punct]->action() == action::MENU_EXIT) {
              return; 
            } else { 
              lcd.clear();
              break;
            }
      case KEY_ESC_PRESS:
            return; // отмена
    }
  } while(true);
}
