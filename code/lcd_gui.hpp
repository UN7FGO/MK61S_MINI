#ifndef LCD_FONT_PACK
#define LCD_FONT_PACK
#include  "config.h"
#include  "rust_types.h"
#include  <LiquidCrystal.h>

extern LiquidCrystal lcd;

static const u8 GE                = 0x00;
static const u8 P_RUS             = 0x01;
static const u8 B_RUS             = 0x02;
static const u8 D_RUS             = 0x03;
static const u8 I_RUS             = 0x04;
static const u8 G_RUS             = 0x05;
static const u8 LCD_CHAR_POW2     = 0x06;
static const u8 LCD_CHAR_POWY     = 0x07;
static const u8 LCD_CHAR_XOR      = 0x08;

/* Стандартный набор символов из ПЗУ LCD */
static const u8 LCD_CYC_ARROW     = 0xDB;
static const u8 LCD_DIVIDE_CHAR   = 0xFD;
static const u8 LCD_NOT_EQU_CHAR  = 0xB7;
static const u8 LCD_POW_X_CHAR    = 0xEB;
static const u8 LCD_UP_ARROW_CHAR = 0xC4;
static const u8 LCD_LT_ARROW_CHAR = 0x7F;
static const u8 LCD_RT_ARROW_CHAR = 0x7E;
static const u8 LCD_PI_CHAR       = 0xF7;
static const u8 LCD_SQRT_CHAR     = 0xE8;
static const u8 LCD_Em1_CHAR      = 0xE9;
static const u8 LCD_GRAD_CHAR     = 0xDF;
static const u8 LCD_QUOTE_CHAR    = 0x60;
static const u8 LCD_DOUBLE_QUOTE_CHAR    = 0x22;
static const u8 CH_RUS            = 0xD1;

class class_LCD_Label {
  private:
    u8 x, y;
  
  public:
    class_LCD_Label(u8 to_x, u8 to_y) : x(to_x), y(to_y) {}
    void print(const char* text) const {
      lcd.setCursor(x, y);
      lcd.print(text);
    }
    void print(char symbol) const {
      lcd.setCursor(x, y);
      lcd.print(symbol);
    }
    void print(int num) const {
      lcd.setCursor(x, y);
      lcd.print(num);
    }
    void print_hex(int num) const {
      lcd.setCursor(x, y);
      if(num < 10) lcd.print(' ');
      lcd.print(num, HEX);
    }
};

const u8 ROUND_ARROW_bit[8] = {
  0b11110,
  0b01101,
  0b10101,
  0b10001,
  0b10101,
  0b10110,
  0b01111,
  0b00000
};

const u8 Alpha[9] = {
  0b11111,
  0b11011,
  0b10101,
  0b11101,
  0b11001,
  0b10101,
  0b11001,
  0b11111
};

const u8 Symbol[9] = {
  0b11111,
  0b11111,
  0b11111,
  0b11011,
  0b11011,
  0b10101,
  0b10101,
  0b11111
};

class class_LCD_fonts {
  private:
   const u8 fonts[9 * 8] = {
    //const u8 GE_bit[8] = {
      0b00100,
      0b00010,
      0b00001,
      0b00010,
      0b00100,
      0b01001,
      0b00010,
      0b00100,
    //const u8 P_ru[8] = {
      0b11111,
      0b10001,
      0b10001,
      0b10001,
      0b10001,
      0b10001,
      0b10001,
      0b00000,
    //const u8 B_ru[8] = {
      0b11111,
      0b10000,
      0b10000,
      0b11110,
      0b10001,
      0b10001,
      0b11110,
      0b00000,
    //const u8 D_ru[8] = {
      0b00110,
      0b01010,
      0b01010,
      0b01010,
      0b01010,
      0b01010,
      0b11111,
      0b10001,
    //const u8 I_ru[8] = {
      0b10001,
      0b10001,
      0b10001,
      0b10011,
      0b10101,
      0b11001,
      0b10001,
      0b00000,
    //const u8 G_ru[8] = {
      0b11111,
      0b10000,
      0b10000,
      0b10000,
      0b10000,
      0b10000,
      0b10000,
      0b00000,
    //const u8 POWSQR_bit[8] = {
      0b11100,
      0b00100,
      0b01100,
      0b10000,
      0b11100,
      0b00000,
      0b00000,
      0b00000,
    //const u8 POWY_bit[8] = {
      0b10100,
      0b10100,
      0b01100,
      0b00100,
      0b11000,
      0b00000,
      0b00000,
      0b00000,
    //const u8 XOR_bit[8] = {
      0b01110,
      0b10101,
      0b10101,
      0b11111,
      0b10101,
      0b10101,
      0b01110,
      0b00000
};
//    const u8* fonts = {&GE_bit, &P_ru, &B_ru, &D_ru, &I_ru, &G_ru, POWSQR_bit, &POWY_bit, &XOR_bit};
  public:
    void load(void) const {
      u32 ascii=0;
      for(int i=0; i < 9 * 8; i += 8) {
        lcd.createChar(ascii++, (uint8_t*) &fonts[i]);
      }
    }

    void load(int offset, int nChar) const {
      lcd.createChar(nChar, (uint8_t*) &fonts[offset]);
    }
};

class   class_glyph {
  private:
    static  const   u8  CUSTOM_CHARS          =     8;
    static  const   u8  CUSTOM_CHAR_BUF_SIZE  =     8;
  #ifdef MK61s
    const u8 custom_char[CUSTOM_CHARS][CUSTOM_CHAR_BUF_SIZE] = {
      { 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }, // 0
      { 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x17, 0x07, 0x07 }, // 1
      { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }, // 2
      { 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07 }, // 3
      { 0x03, 0x07, 0x0F, 0x1F, 0x1E, 0x1C, 0x18, 0x10 }, // 4
      { 0x10, 0x18, 0x1C, 0x1E, 0x1F, 0x0F, 0x07, 0x03 }, // 5
      { 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F }, // 6
      { 0x1F, 0x1F, 0x1F, 0x07, 0x07, 0x0F, 0x1E, 0x1C }  // 7
    };
    const u8 line[2][16] = { 
      {0, 1, 1, ' ', 0, 4, ' ', 0, 6, ' ', 4, 2, ' ', 0, 6, 6},  
      {2, 3, 3, ' ', 2, 5, ' ', 2, 7, ' ', ' ', 2, ' ', 6, 6, 7}
    };
  #else
   #ifdef MK52s
    const u8 custom_char[CUSTOM_CHARS][CUSTOM_CHAR_BUF_SIZE] = {
      {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}, // #0
      {0x0F, 0x1F, 0x1F, 0x17, 0x07, 0x07, 0x07, 0x07}, // #1
      {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}, // #2
      {0x07, 0x0F, 0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x10}, // #3
      {0x10, 0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x0F, 0x07}, // #4
      {0x1F, 0x1F, 0x1F, 0,  0,    0,    0,    0},      // #5
      {0,    0,    0,    0,    0,    0x1F, 0x1F, 0x1F}, // #6
      {0x1F, 0x1F, 0x1F, 0,    0,    0,    0x1F, 0x1F}  // #7
    };
    const u8 line[2][16] = {
      {0, 1, 1, ' ', 0, 3, ' ', 0, 5, ' ', 5, 0, ' ', 0, 7, 7},
      {0, 2, 2, ' ', 0, 4, ' ', 6, 0, ' ', 0, 6, ' ', 6, 6, 0}
    };
   #endif
  #endif
  public:

  //  class_glyph(LiquidCrystal to_lcd) : lcd(to_lcd) {};

    void  draw(int x) const {
      // load char set
      for(int i=0; i < CUSTOM_CHARS; i++) {
        lcd.createChar(i, (u8*) &custom_char[i]);  
      }
      // draw glyph 
      for(int j=0; j < 2; j++) {
        lcd.setCursor(x, j);
        for(int i=0; i < (16-x); i++) {
          lcd.write(line[j][i]);
        }
      }
    }
};

#endif
