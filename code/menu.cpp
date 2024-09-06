#include "LiquidCrystal.h"
#include "menu.hpp"    
#include "cross_hal.h"

extern LiquidCrystal lcd;


namespace library_mk61 {

const t_punct DFU_mode_punct  = {.size = 15, .action = (menu_action) &DFU_enable,           .text = "DFU mode enable"};
const t_punct LIB_61_punct    = {.size = 12, .action = &mk61_library_select,                .text = "MK61 library"};
const t_punct RESET_punct     = {.size = 12, .action = (menu_action) &NVIC_SystemReset,     .text = "Reset device"};
const t_punct FLASH_punct     = {.size = 11, .action = (menu_action) &InfoData,             .text = "Information"};

const t_punct* MENU[MENU_PUNCT] = {
      &DFU_mode_punct,
      &LIB_61_punct,
      &RESET_punct,
      &FLASH_punct
};

}

bool  mk61_library_select(void) {
  const int n = select_program(keyboard);
  if(n < 0) return false;

  load_program(n);
  return true;
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
  do{
    draw();
    const i32 last_key_code = keyboard.get_key_wait();
    switch(last_key_code) {
      case KEY_RIGHT_PRESS:
              if(active_punct < (MENU_PUNCT_COUNT-1)) active_punct++;
        break;
      case KEY_LEFT_PRESS:
              if(active_punct > 0) active_punct--;
        break;
      case KEY_OK_PRESS:
            #ifdef SERIAL_OUTPUT
              Serial.print("Select menu: '"); Serial.print(puncts[active_punct]->text); Serial.println('\'');
            #endif 
            if(puncts[active_punct]->action() == true) {
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
