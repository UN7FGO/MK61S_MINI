#ifndef MENU_CLASS
#define MENU_CLASS

#include  "keyboard.hpp"
#include  "library_pmk.hpp"
#include  "basic.hpp"

bool  mk61_library_select(void) {
  const int n = select_program(keyboard);
  if(n < 0) return false;

  load_program(n);
  return true;
}

bool  BASIC_library_select(void) {

}

extern void InfoData(void);

typedef bool (*menu_action)(void);  // возвращает необходимость покинуть главное меню 

struct  t_punct {
    u8            size;
    menu_action   action;
    char          text[];
};

const t_punct DFU_mode_punct  = {.size = 15, .action = (menu_action) &DFU_enable,           .text = "DFU mode enable"};
const t_punct LIB_61_punct    = {.size = 12, .action = &mk61_library_select,                .text = "MK61 library"};
const t_punct LIB_BASIC_punct = {.size = 13, .action = &BASIC_library_select,               .text = "BASIC library"};
const t_punct SAVE_punct      = {.size = 14, .action = &Store,                              .text = "Store to flash"};
const t_punct LOAD_punct      = {.size = 15, .action = &Load,                               .text = "Load from flash"};
const t_punct RESET_punct     = {.size = 12, .action = (menu_action) &NVIC_SystemReset,     .text = "Reset device"};
const t_punct BASIC_punct     = {.size = 12, .action = (menu_action) &EditBasic,            .text = "BASIC editor"};
const t_punct FLASH_punct     = {.size = 11, .action = (menu_action) &InfoData,             .text = "Information"};

class class_menu {
  private:
    static constexpr int MENU_PUNCT_COUNT = 8;
    static constexpr int SIZE_MENU_WINDOW = 2;
    const t_punct* puncts[MENU_PUNCT_COUNT] = {
      &DFU_mode_punct,
      &BASIC_punct,
      &LIB_61_punct,
      &LIB_BASIC_punct,
      &SAVE_punct,
      &LOAD_punct,
      &RESET_punct,
      &FLASH_punct
    };
    u8 active_punct;
    u8 previous_up;

    void draw(void) {
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

  public:
    class_menu(void) : active_punct(0), previous_up(0) {};
    void select(void) {
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
            if(puncts[active_punct]->action() == true) return;
          case KEY_ESC_PRESS:
            return; // отмена
        }
      } while(true);
    }
};


#endif