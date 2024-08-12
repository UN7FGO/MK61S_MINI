#ifndef MENU_CLASS
#define MENU_CLASS

#include  "keyboard.hpp"
#include  "basic.hpp"
#include  "tools.hpp"
#include  "library_pmk.hpp"

typedef bool (*menu_action)(void);  // возвращает необходимость покинуть главное меню 

struct  t_punct {
    u8            size;
    menu_action   action;
    char          text[];
};

extern void InfoData(void);
extern bool mk61_library_select(void);

namespace library_mk61 {
  const   int             COUNT_PUNCTS = 8;
  extern  const t_punct*  MENU[8];
}

class class_menu {
  private:
    static constexpr int SIZE_MENU_WINDOW = 2;
    int MENU_PUNCT_COUNT;
    u8 active_punct;
    u8 previous_up;
    t_punct** puncts;;

    void draw(void);/* {
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
    }*/

  public:
    class_menu(t_punct** punts_of_menu, int count_of_puncts) : MENU_PUNCT_COUNT(count_of_puncts), puncts(punts_of_menu), active_punct(0), previous_up(0) {};
    void select(void);/* {
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
    }*/
};

#endif