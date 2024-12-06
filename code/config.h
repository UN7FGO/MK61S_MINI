#ifndef CONFIG
#define CONFIG

#include "Arduino.h"
#include "rust_types.h"

//#define DEBUG_CORE61        // Полная отладочная информация по ядру mk61s (почти не слушает клавиатуру)
//#define DEBUG_MINI          // Отладочная информация по оболочке MK61S-MINI
//#define DEBUG_SPIFLASH      // Отладочная информация по обработке внешней флеш памяти
//#define DEBUG_DISASMBLER    // Отладочная информация по встроенному дисассемблеру МК61 инструкций
//#define DEBUG_KBD           // Отладочная информация по клавиатурному драйверу
//#define DEBUG_MENU          // Отладочная информация по системе меню
//#define DEBUG_BASIC         // Отладочная информация по BASIC
//#define DEBUG_LIBRARY       // Отладочная информация по библиотеке программ МК61
//#define DEBUG_MK61E         // Отладочная информация расширяющая представление вывода терминала по МК61
//#define EXPAND_RING_MK61    // Увеличить объем оперативной памяти кольца МК61 на еще один регистр IK130X
//#define DEBUG_MEASURE       // Вывод времени исполнения от С/П до С/П для вычисления производительности ядра
//#define MK61_EXTENDED
//#define B3_34
#define TERMINAL
#define SPI_FLASH
//#define DEBUG
//#define DEBUG_M61
//#define BASIC

//#define CDU
//#define LK432
//#define SERIAL_OUTPUT
#define REVISION_V3
//#define REVISION_V2
#define MK61s
//#define MK52s

#define IS_CORTEX_M4() (__ARM_ARCH == 7)
//defined(__ARM_ARCH_7EM__)
//defined(__ARM_FEATURE_SIMD32)

#if defined(MK61E) || defined(TERMINAL) || defined(DEBUG_CORE61) || defined(DEBUG_MENU) || defined(DEBUG_MINI) || defined(DEBUG) || defined(DEBUG_KBD) || defined(DEBUG_M61) || defined(DEBUG_BASIC) || defined(DEBUG_DISASMBLER) || defined(DEBUG_LIBRARY) || defined(DEBUG_SPIFLASH)
 #define SERIAL_OUTPUT
 //#warning Serial module included!
#endif

#ifdef DEBUG_MEASURE
  constexpr bool DBG_MEASURE = true;
#else
  constexpr bool DBG_MEASURE = false;
#endif

#ifdef DEBUG_MINI
  constexpr bool DBG_MINI = true;
#else
  constexpr bool DBG_MINI = false;
#endif

#ifdef DEBUG_SPIFLASH
  constexpr bool DBG_SPIROM = true;
#else
  constexpr bool DBG_SPIROM = false;
#endif

#ifdef DEBUG_DISASMBLER
  constexpr bool DBG_DISASM = true;
#else
  constexpr bool DBG_DISASM = false;
#endif

#ifdef DEBUG_KBD
  constexpr bool DBG_KBD = true;
#else
  constexpr bool DBG_KBD = false;
#endif

#ifdef DEBUG_MENU
  constexpr bool DBG_MENU = true;
#else
  constexpr bool DBG_MENU = false;
#endif

#ifdef DEBUG_BASIC
  constexpr bool DBG_BASIC = true;
#else
  constexpr bool DBG_BASIC = false;
#endif

#ifdef DEBUG_CORE61
  constexpr bool DBG_CORE61 = true;
#else
  constexpr bool DBG_CORE61 = false;
#endif

#ifdef DEBUG_LIBRARY
  constexpr bool DBG_LIB61 = true;
#else
  constexpr bool DBG_LIB61 = false;
#endif

#ifdef DEBUG_MK61E
  constexpr bool DBG_MK61E = true;
#else
  constexpr bool DBG_MK61E = false;
#endif

#ifdef ARDUINO_BLACKPILL_F411CE
  const char chip_name[] = "STM32F411CE";
  const char mem_text[] = "RAM:128 ROM:512";
#else
  #ifdef ARDUINO_BLACKPILL_F401CE
    const char chip_name[] = "STM32F401CE";
    const char mem_text[] = "RAM:96 ROM:512";
  #else
    #ifdef ARDUINO_BLACKPILL_F401CC
      const char chip_name[] = "STM32F401CC";
      const char mem_text[] = "RAM:64 ROM:256";
    #else
      #ifdef ARDUINO_GENERIC_F401CDUX
        const char chip_name[] = "STM32F401CD";
        const char mem_text[] = "RAM:96 ROM:384";
      #else
        #ifdef ARDUINO_GENERIC_F411CCUX
          const char chip_name[] = "STM32F411CC";
          const char mem_text[] = "RAM:128 ROM:256";
        #else
          #ifdef ARDUINO_GENERIC_F401CBYX
            const char chip_name[] = "STM32F401CB";
            const char mem_text[] = "RAM:64 ROM:128";
          #else
            const char chip_name[] = "unknown chip";
            const char mem_text[] = "unknown memory";
          #endif
        #endif
      #endif
    #endif
  #endif
#endif

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

namespace cfg {

static constexpr isize  CLASSIC_MK61_QUANTS    =    72500;   // Константна замедления ядра mk61s в классичесокм режиме

}

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
  static const u8   PIN_SPIFLASH_CS =   PA4;
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
  static const u8   PIN_SPIFLASH_CS =   PA4;
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
    static const u8   PIN_SPIFLASH_CS =   PA4;
    static const u8   PIN_LED         =   PC13;
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
      static const u8   PIN_SPIFLASH_CS =   PA4;
    #else 
 /* REVISION_V1 Описание ног для STM32F411CEU6 aka BlackPill MK61s-mini_v1*/
      static constexpr usize   PIN_LCD_RS      =   PB1;
      static constexpr usize   PIN_LCD_E       =   PB0;
      static constexpr usize   PIN_LCD_DB4     =   PA3;
      static constexpr usize   PIN_LCD_DB5     =   PA2;
      static constexpr usize   PIN_LCD_DB6     =   PA1;
      static constexpr usize   PIN_LCD_DB7     =   PA0;
      static constexpr usize   PIN_KBD_COL0    =   PB12;
      static constexpr usize   PIN_KBD_COL1    =   PB13;
      static constexpr usize   PIN_KBD_COL2    =   PB14;
      static constexpr usize   PIN_KBD_COL3    =   PB15;
      static constexpr usize   PIN_KBD_COL4    =   PA8;
      static constexpr usize   PIN_KBD_COL5    =   PA11;
      static constexpr usize   PIN_KBD_COL6    =   PA12;
      static constexpr usize   PIN_KBD_COL7    =   PA15;
      static constexpr usize   PIN_KBD_ROW4    =   PB8;
      static constexpr usize   PIN_KBD_ROW3    =   PB7;
      static constexpr usize   PIN_KBD_ROW2    =   PB6;
      static constexpr usize   PIN_KBD_ROW1    =   PB5;
      static constexpr usize   PIN_KBD_ROW0    =   PB4;
      static constexpr usize   PIN_SPIFLASH_CS =   PA4;
      static constexpr usize   PIN_BUZZER      =   PB10;
      static constexpr usize   PIN_LED         =   PC13;
    #endif
  #endif
 #endif  
#endif

#endif
