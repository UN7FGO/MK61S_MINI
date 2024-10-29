#ifndef KEY_MNEMONICS_CLASS
#define KEY_MNEMONICS_CLASS

#include "tools.hpp"
#include "cross_hal.h"

extern void entry_programm_mode(void);
extern void exit_auto_mode(void);

class key_mnenonic {
  private:
    
    static constexpr u8 X = 10;
    static constexpr u8 Y = 0;
    
    char   mnemo_buffer[8];
    bool   on;
    usize  mnemo_pos;
    i32    last_key;

  public:

    key_mnenonic(void) : on(true), mnemo_pos(0) {};

    void    clear_mnemo(void) {
      mnemo_pos = 0;
      *(u32*) &mnemo_buffer[0] = 0x00000000;
      *(u32*) &mnemo_buffer[4] = 0x00000000;
    }

    int    next_mnemo_pos(const char* descriptor) {
      const i8 control = (i8) descriptor[0]; 
      if(control < 0) {
        const usize len = ((u8) control & ~STORE_KEY);
        for(usize i=0; i < len; i++) mnemo_buffer[mnemo_pos + i] = descriptor[i+1];
        return mnemo_pos + len;
      } else {
        for(usize i=0; i < (u8) control; i++) mnemo_buffer[mnemo_pos + i] = descriptor[i+1];
        return 0;
      }
    }

    void  build_mnemo(i32 keycode) {
      if(mnemo_pos == 0 || keycode == KEY_K || keycode == KEY_F) { // Первый вход в построение мнемокода нажатой набираемой функции
        clear_mnemo();
        mnemo_pos = next_mnemo_pos(mnemo_code[keycode]);
      } else {
        switch(last_key) {
          case  KEY_F:
              mnemo_pos = next_mnemo_pos(mnemo_code_F[keycode]);

              // call back for change mode
              if(keycode == KEY_EPOWER) 
                entry_programm_mode();
              else if (keycode == KEY_NEG)
                exit_auto_mode();
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

    void  print(i32 keycode) {
      char mnemo[8];
      if(on) {
        build_mnemo(keycode);
        *(u64*) &mnemo = pad_left_8_char(mnemo_buffer);
        lcd.setCursor(X, Y); lcd.print(mnemo);
      }
    }

    void  disable(void) {
      lcd.setCursor(X, Y); lcd.print("      "); // blank
      on = false;
    }

    void  enable(void) {
      on = true;
    }
};

#endif