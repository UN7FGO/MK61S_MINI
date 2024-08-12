#ifndef CONFIG
#define CONFIG

#include "Arduino.h"
#include "rust_types.h"

//#define B3_34
//#define DEBUG_BASIC
//#define DEBUG
//#define DEBUG_DISASMBLER
//#define DEBUG_LOAD

#if defined(DEBUG_BASIC) || defined(DEBUG_DISASMBLER) || defined (DEBUG_LOAD)
 #define SERIAL_OUTPUT
 //#warning Serial module included!
#endif

//#define CDU
//#define LK432
//#define SERIAL_OUTPUT
#define REVISION_V3
//#define REVISION_V2
#define MK61s
//#define MK52s

#ifdef MK61s
      const char MODEL[] = "MK61s";
      //                       0123456789ABCDEF
      const char FULL_MODEL_NAME[] = "MK61s *firmware*";
  #else 
    #ifdef MK52s
      const char MODEL[] = "MK52s";
      const char FULL_MODEL_NAME[] = "MK52s *firmware*";
    #endif
#endif

#define FIRMWARE_VER (char const[]) {__TIME__[0], __TIME__[1],__TIME__[3],__TIME__[4],__TIME__[6],__TIME__[7],' ',__DATE__[0], __DATE__[1], __DATE__[2], __DATE__[3], (__DATE__[4] == ' ' ?  '0' : __DATE__[4]), __DATE__[5], __DATE__[6], __DATE__[9], __DATE__[10], __DATE__[11]}
class class_calc_config {
  public:
    bool disassm;
    bool output_IP;
    class_calc_config(void) : disassm(false), output_IP(false) {}
};

// Конфигурация подключения микроконтроллера на макетной или печтаной плате 
#ifdef CDU
 /* Описание ног для STM32F401CDU6 aka BlackPill */
  static const u8   PIN_LCD_RS      =   PC14;
  static const u8   PIN_LCD_E       =   PC15;
  static const u8   PIN_LCD_DB4     =   PB0;
  static const u8   PIN_LCD_DB5     =   PB1;
  static const u8   PIN_LCD_DB6     =   PB2;
  static const u8   PIN_LCD_DB7     =   PB3;
  static const u8   PIN_KBD_COL0    =   PA0;
  static const u8   PIN_KBD_COL1    =   PA1;
  static const u8   PIN_KBD_COL2    =   PA2;
  static const u8   PIN_KBD_COL3    =   PA3;
  static const u8   PIN_KBD_COL4    =   PA4;
  static const u8   PIN_KBD_COL5    =   PA5;
  static const u8   PIN_KBD_COL6    =   PA6;
  static const u8   PIN_KBD_COL7    =   PA7;
  static const u8   PIN_KBD_ROW4    =   PB8;
  static const u8   PIN_KBD_ROW3    =   PB7;
  static const u8   PIN_KBD_ROW2    =   PB6;
  static const u8   PIN_KBD_ROW1    =   PB5;
  static const u8   PIN_KBD_ROW0    =   PB4;
#else
 #ifdef LK432
 /* Описание ног для STM32L432 aka LK432 in Arduino shield (Wokwi)*/
  static const u8   PIN_LCD_RS      =   D12;
  static const u8   PIN_LCD_E       =   D11;
  static const u8   PIN_LCD_DB4     =   D10;
  static const u8   PIN_LCD_DB5     =   D9;
  static const u8   PIN_LCD_DB6     =   D8;
  static const u8   PIN_LCD_DB7     =   D8;
  static const u8   PIN_KBD_COL0    =   A0;
  static const u8   PIN_KBD_COL1    =   A1;
  static const u8   PIN_KBD_COL2    =   A2;
  static const u8   PIN_KBD_COL3    =   A3;
  static const u8   PIN_KBD_COL4    =   A4;
  static const u8   PIN_KBD_COL5    =   A5;
  static const u8   PIN_KBD_COL6    =   A6;
  static const u8   PIN_KBD_COL7    =   A7;
  static const u8   PIN_KBD_ROW4    =   D2;
  static const u8   PIN_KBD_ROW3    =   D4;
  static const u8   PIN_KBD_ROW2    =   D5;
  static const u8   PIN_KBD_ROW1    =   D6;
  static const u8   PIN_KBD_ROW0    =   D13;
 #else
  #ifdef REVISION_V2
 /* Описание ног для STM32F411CEU6 aka BlackPill MK61s-mini_v2*/
    static const u8   PIN_LCD_RS      =   PB2;
    static const u8   PIN_LCD_RW      =   PB1;
    static const u8   PIN_LCD_E       =   PB0;
    static const u8   PIN_LCD_DB4     =   PA3;
    static const u8   PIN_LCD_DB5     =   PA2;
    static const u8   PIN_LCD_DB6     =   PA1;
    static const u8   PIN_LCD_DB7     =   PC15;
    static const u8   PIN_KBD_COL0    =   PB12;
    static const u8   PIN_KBD_COL1    =   PB13;
    static const u8   PIN_KBD_COL2    =   PB14;
    static const u8   PIN_KBD_COL3    =   PB15;
    static const u8   PIN_KBD_COL4    =   PA8;
    static const u8   PIN_KBD_COL5    =   PA9;
    static const u8   PIN_KBD_COL6    =   PA10;
    static const u8   PIN_KBD_COL7    =   PA15;
    static const u8   PIN_KBD_ROW4    =   PB8;
    static const u8   PIN_KBD_ROW3    =   PB7;
    static const u8   PIN_KBD_ROW2    =   PB6;
    static const u8   PIN_KBD_ROW1    =   PB5;
    static const u8   PIN_KBD_ROW0    =   PB4;
    static const u8   PIN_J2          =   PB3;
    static const u8   PIN_BUZZER      =   PB10;
  #else
    #ifdef REVISION_V3
 /* REVISION_V3 Описание ног для STM32F411CEU6 aka BlackPill MK61s-mini_v3*/
      static const u8   PIN_LCD_RS      =   PB2;
      static const u8   PIN_LCD_E       =   PB0;
      static const u8   PIN_LCD_RW      =   PB1;
      static const u8   PIN_LCD_DB4     =   PB10;
      static const u8   PIN_LCD_DB5     =   PA3;
      static const u8   PIN_LCD_DB6     =   PA2;
      static const u8   PIN_LCD_DB7     =   PA1;
      static const u8   PIN_KBD_COL0    =   PB12;
      static const u8   PIN_KBD_COL1    =   PB13;
      static const u8   PIN_KBD_COL2    =   PB14;
      static const u8   PIN_KBD_COL3    =   PB15;
      static const u8   PIN_KBD_COL4    =   PA8;
      static const u8   PIN_KBD_COL5    =   PA9;
      static const u8   PIN_KBD_COL6    =   PA10;
      static const u8   PIN_KBD_COL7    =   PA15;
      static const u8   PIN_KBD_ROW4    =   PB8;
      static const u8   PIN_KBD_ROW3    =   PB7;
      static const u8   PIN_KBD_ROW2    =   PB6;
      static const u8   PIN_KBD_ROW1    =   PB5;
      static const u8   PIN_KBD_ROW0    =   PB4;
      static const u8   PIN_BUZZER      =   PA0;
      static const u8   PIN_LED         =   PC13;
      static const u8   PIN_OUT_PWR     =   PB9;
    #else 
 /* REVISION_V1 Описание ног для STM32F411CEU6 aka BlackPill MK61s-mini_v1*/
      static const u8   PIN_LCD_RS      =   PB1;
      static const u8   PIN_LCD_E       =   PB0;
      static const u8   PIN_LCD_DB4     =   PA3;
      static const u8   PIN_LCD_DB5     =   PA2;
      static const u8   PIN_LCD_DB6     =   PA1;
      static const u8   PIN_LCD_DB7     =   PA0;
      static const u8   PIN_KBD_COL0    =   PB12;
      static const u8   PIN_KBD_COL1    =   PB13;
      static const u8   PIN_KBD_COL2    =   PB14;
      static const u8   PIN_KBD_COL3    =   PB15;
      static const u8   PIN_KBD_COL4    =   PA8;
      static const u8   PIN_KBD_COL5    =   PA11;
      static const u8   PIN_KBD_COL6    =   PA12;
      static const u8   PIN_KBD_COL7    =   PA15;
      static const u8   PIN_KBD_ROW4    =   PB8;
      static const u8   PIN_KBD_ROW3    =   PB7;
      static const u8   PIN_KBD_ROW2    =   PB6;
      static const u8   PIN_KBD_ROW1    =   PB5;
      static const u8   PIN_KBD_ROW0    =   PB4;
    #endif
  #endif
 #endif  
#endif

#endif
