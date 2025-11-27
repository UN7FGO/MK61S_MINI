#include "config.h"
#include "cross_hal.h"
#include "lcd_gui.hpp"

#ifdef SCAN_ROW
const TMK61_cross_key KeyPairs[40] = {
  NON, NON, NON, NON, NON, NON, NON, NON,
  F,   SB,  Px,  _7_, _8_, _9_,  SUB, DIV,
  K,   SF,  xP,  _4_, _5_, _6_,  ADD, MUL,
  NON, RET, JMP, _1_, _2_, _3_,  XY,   Bx,
  NON, RUN, JSR, _0_, DOT, NEG, POW,  Cx
};

#else
const TMK61_cross_key KeyPairs[40] = {
   Cx,  Bx, MUL, DIV, NON,
  POW,  XY, ADD, SUB, NON,
  NEG, _3_, _6_, _9_, NON,
  DOT, _2_, _5_, _8_, NON,
  _0_, _1_, _4_, _7_, NON,
  JSR, JMP,  xP,  Px, NON,
  RUN, RET,  SF,  SB, NON,
  NON, NON,   K,   F, NON
};

const char m_Cx[]   = {2, 'C', 'x'};
const char m_Bx[]   = {2, 'B', 0xC4};
const char m_mul[]  = {1, 'x'};
const char m_div[]  = {1, 0xFD};
const char m_G[]    = {3, ' ', ' ', G_RUS};
const char m_E[]    = {2, 'B', P_RUS};
const char m_XY[]   = {2, LCD_LT_ARROW_CHAR, LCD_RT_ARROW_CHAR};
const char m_add[]  = {1, '+'};
const char m_sub[]  = {1, '-'};
const char m_GRD[]  = {3, G_RUS, 'P', D_RUS};
const char m_NEG[]  = {3, '/', '-', '/'};
const char m_3[]    = {1, '3'};
const char m_6[]    = {1, '6'};
const char m_9[]    = {1, '9'};
const char m_R[]    = {1, 'P'};
const char m_DP[]   = {1, 0xA5};
const char m_2[]    = {1, '2'};
const char m_5[]    = {1, '5'};
const char m_8[]    = {1, '8'};
const char m_0[]    = {1, '0'};
const char m_1[]    = {1, '1'};
const char m_4[]    = {1, '4'};
const char m_7[]    = {1, '7'};
const char m_PP[]   = {2, P_RUS, P_RUS};
const char m_BP[]   = {2|STORE_KEY, B_RUS, P_RUS};
#ifdef B3_34
  const char m_xP[]   = {1|STORE_KEY, P_RUS};
  const char m_Px[]   = {2|STORE_KEY, I_RUS, P_RUS};
#else
  const char m_xP[]   = {3|STORE_KEY, 'x', LCD_RT_ARROW_CHAR, P_RUS};  // MK61 мнемоника x->П
  const char m_Px[]   = {3|STORE_KEY, P_RUS, LCD_RT_ARROW_CHAR, 'x'};  // MK61 мнемоника П->x
#endif
const char m_SP[]   = {3, 'C', '/', P_RUS};
const char m_BO[]   = {3, 'B', '/', 'O'};
const char m_SF[]   = {1, LCD_RT_ARROW_CHAR};
const char m_SB[]   = {1, LCD_LT_ARROW_CHAR};
const char m_F[]    = {1|STORE_KEY, 'F'};
const char m_K[]    = {1|STORE_KEY, 'K'};
const char m__[]    = {1, ' '};

const char* mnemo_code[40] = {
  m_Cx,   m_Bx,   m_mul,  m_div,  m_G,
  m_E,    m_XY,   m_add,  m_sub,  m_GRD,
  m_NEG,  m_3,    m_6,    m_9,    m_R,
  m_DP,   m_2,    m_5,    m_8,    m__,
  m_0,    m_1,    m_4,    m_7,    m__,
  m_PP,   m_BP,   m_xP,   m_Px,   m__,
  m_SP,   m_BO,   m_SF,   m_SB,   m__,
  m__,    m__,    m_K,    m_F,    m__
};
/*
const char* mnemo_code[40] = {
  "Cx",        "B\xC4",    "x",       "\xFD",     "  \x05",       
  "B\x01",     "\x7F\x7E", "+",       "-",        "\x05P\x03",    
  "/-/",       "3",        "6",       "9",        "P  ",          
  "\xA5",      "2",        "5",       "8",        "",          
  "0",         "1",        "4",       "7",        "",
  "\x01\x01",  "\x02\x01", "\x01",    "\x04\x01", "",
  "C/\x01",    "B/O",      "<<<",     ">>>",      "",
  "",          "",         "K",       "F",        ""
};*/
//"X\x7E\x01", "\x01\x7EX"

const u8 mnemo_code_register[40] = {
  D_RUS,  'E',  '?', '?', '?',
  'C',    '?',  '?', '?', '?', 
  'B',    '3',  '6', '9', '?',
  'A',    '2',  '5', '8', '?',
  '0',    '1',  '4', '7', '?',
  '?',    '?',  '?', '?', '?', 
  '?',    '?',  '?', '?', '?' 
};

const char m_FCx[]  = {2, 'C', 'F'};
const char m_FBx[]  = {2, 'B', 'x'};
const char m_FX2[]  = {2, 'x', LCD_CHAR_POW2};
const char m_1DX[]  = {3, '1', '/', 'x'};
const char m_FE[]   = {3, P_RUS, 'P', G_RUS};
const char m_FXY[]  = {2, 'x', LCD_CHAR_POWY};
const char m_FPI[]  = {1, LCD_PI_CHAR};
const char m_FRT[]  = {1, LCD_SQRT_CHAR};
const char m_FNEG[] = {3, 'A', 'B', 'T'};
const char m_F3[]   = {2, 'l', 'n'};
const char m_F6[]   = {3, 't', 'g', LCD_Em1_CHAR};
const char m_F9[]   = {2, 't', 'g'};
const char m_FDP[]  = {1, LCD_CYC_ARROW};
const char m_F2[]   = {2, 'l', 'g'};
const char m_F5[]   = {4, 'c', 'o', 's', LCD_Em1_CHAR};
const char m_F8[]   = {3, 'c', 'o', 's'};
const char m_F0[]   = {3, '1', '0', LCD_POW_X_CHAR};
const char m_F1[]   = {2, 'e', LCD_POW_X_CHAR};
const char m_F4[]   = {4, 's', 'i', 'n', LCD_Em1_CHAR};
const char m_F7[]   = {3, 's', 'i', 'n'};
const char m_FSB[]  = {3, 'x', '=', '0'};
const char m_FSF[]  = {3, 'x', '<', '0'};
const char m_FSP[]  = {3, 'x', LCD_NOT_EQU_CHAR, '0'};
const char m_FBO[]  = {4, 'x', '>', '=', '0'};
const char m_FPP[]  = {2, 'L', '3'};
const char m_FBP[]  = {2, 'L', '2'};
const char m_FxP[]  = {2, 'L', '1'};
const char m_FPx[]  = {2, 'L', '0'};
const char m_FF[]   = {0|STORE_KEY, 0};
const char m_null[] = {0, 0};

const char* mnemo_code_F[40] = {
  m_FCx,  m_FBx,  m_FX2,  m_1DX,  m__,
  m_FE,   m_FXY,  m_FPI,  m_FRT,  m__,
  m_FNEG, m_F3,   m_F6,   m_F9,   m__,
  m_FDP,  m_F2,   m_F5,   m_F8,   m__,
  m_F0,   m_F1,   m_F4,   m_F7,   m__,
  m_FPP,  m_FBP,  m_FxP,  m_FPx,  m__,
  m_FSP,  m_FBO,  m_FSB,  m_FSF,  m__,
  m__,    m__,    m_K,    m_FF,   m__
};

const char m_KCx[]  = {3, I_RUS, 'H', 'B'};
const char m_KBx[]  = {2, 'C', CH_RUS};
const char m_KE[]   = {1, LCD_CHAR_XOR};
const char m_KXY[]  = {4, '"', LCD_QUOTE_CHAR, LCD_RT_ARROW_CHAR, LCD_GRAD_CHAR};
const char m_KDIV[] = {3, LCD_QUOTE_CHAR, LCD_RT_ARROW_CHAR, LCD_GRAD_CHAR};
const char m_KNEG[] = {1, 'v'};
const char m_K3[]   = {4, '"', LCD_QUOTE_CHAR, LCD_LT_ARROW_CHAR, LCD_GRAD_CHAR};
const char m_K6[]   = {3, LCD_GRAD_CHAR, LCD_LT_ARROW_CHAR, LCD_QUOTE_CHAR};
const char m_K9[]   = {3, 'm', 'a', 'x'};
const char m_KDP[]  = {1, '^'};
const char m_K5[]   = {2, '3', 'H'};
const char m_K8[]   = {3, '{', 'x', '}'};
const char m_K0[]   = {3, 'H', 'O', P_RUS};
const char m_K4[]   = {3, '|', 'x', '|'};
const char m_K7[]   = {3, '[', 'x', ']'};
const char m_KK[]   = {0|STORE_KEY, 0};
const char m_KSB[]  = {3|STORE_KEY, 'x', '=', '0'};
const char m_KSF[]  = {3|STORE_KEY, 'x', '<', '0'};
const char m_KBO[]  = {4|STORE_KEY, 'x', '>', '=', '0'};
const char m_KSP[]  = {3|STORE_KEY, 'x', LCD_NOT_EQU_CHAR, '0'};
const char m_KPP[]  = {2|STORE_KEY, P_RUS, P_RUS};

const char* mnemo_code_K[40] = {
  m_KCx,  m_KBx,  m__,    m__,    m__,
  m_KE,   m_KXY,  m_KDIV, m__,    m__,
  m_KNEG, m_K3,   m_K6,   m_K9,   m__,
  m_KDP,  m__,    m_K5,   m_K8,   m__,
  m_K0,   m__,    m_K4,   m_K7,   m__,
  m_KPP,  m_BP,   m_xP,   m_Px,   m__,
  m_KSP,  m_KBO,  m_KSF,  m_KSB,  m__,
  m__,    m__,    m_KK,   m_F,    m__
};

#endif