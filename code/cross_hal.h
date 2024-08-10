#ifndef MK61_MK52_CROSS_HAL
#define MK61_MK52_CROSS_HAL

#include "rust_types.h"
#include "keyboard.hpp"

static const u32               KEY_Px     =   28;
static const u32               KEY_xP     =   27;
static const u32               KEY_BP     =   26;
static const u32               KEY_PP     =   25;
static const u32               KEY_RUN    =   30;
static const u32               KEY_RET    =   31;
static const u32               KEY_FRW    =   32;
static const u32               KEY_BKW    =   33;
static const u32               KEY_BASIC  =   36;
static const u32               KEY_K      =   37;
static const u32               KEY_F      =   38;
static const u32               KEY_ESC    =   39;

static constexpr i32 KEY_PUSH_B   =   1;
static const i32 KEY_DEGREE       =   4;
static const i32 KEY_GRADE        =   9;
static const i32 KEY_RADIAN       =   14;
static const i32 KEY_USER         =   19;
static const i32 KEY_LOAD         =   35;
static const i32 KEY_LEFT         =   34;
static const i32 KEY_RIGHT        =   24;
static constexpr i32 KEY_ALPHA    =   KEY_F;
static const i32 KEY_OK           =   29;

static const i32 KEY_LEFT_PRESS   =   KEY_LEFT  | (i32) key_state::PRESSED;
static const i32 KEY_RIGHT_PRESS  =   KEY_RIGHT | (i32) key_state::PRESSED;
static const i32 KEY_OK_PRESS     =   KEY_OK    | (i32) key_state::PRESSED;

static const i32 KEY_LOAD_PRESS   =   35 | (i32) key_state::PRESSED;
static const i32 KEY_USER_PRESS   =   19 | (i32) key_state::PRESSED;
static const i32 KEY_USER_RELEASE =   19 | (i32) key_state::RELEASED;
static const i32 KEY_ESC_PRESS    =   39 | (i32) key_state::PRESSED;
static const i32 KEY_ESC_RELEASE  =   39 | (i32) key_state::RELEASED;

class __attribute__((__packed__)) TMK61_cross_key {
  public:
    u8 x, y;
    u16 as_u16(void) const {
      return *(u16*) this;
    }
};

static const TMK61_cross_key   NON     =   {.x=0, .y=0};
static const TMK61_cross_key   F       =   {.x=11, .y=9};  // F
static const TMK61_cross_key   K       =   {.x=10, .y=9};  // K
static const TMK61_cross_key   SB      =   {.x=7, .y=9};   // ШГ->
static const TMK61_cross_key   SF      =   {.x=9, .y=9};   // ШГ<-
static const TMK61_cross_key   Px      =   {.x=8, .y=9};   // П->X
static const TMK61_cross_key   xP      =   {.x=6, .y=9};   // X->П
static const TMK61_cross_key   _0_     =   {.x=2, .y=1};   // 0
static const TMK61_cross_key   _1_     =   {.x=3, .y=1};   // 1
static const TMK61_cross_key   _2_     =   {.x=4, .y=1};   // 2
static const TMK61_cross_key   _3_     =   {.x=5, .y=1};   // 3
static const TMK61_cross_key   _4_     =   {.x=6, .y=1};   // 4
static const TMK61_cross_key   _5_     =   {.x=7, .y=1};   // 5
static const TMK61_cross_key   _6_     =   {.x=8, .y=1};   // 6
static const TMK61_cross_key   _7_     =   {.x=9, .y=1};   // 7
static const TMK61_cross_key   _8_     =   {.x=10, .y=1};  // 8
static const TMK61_cross_key   _9_     =   {.x=11, .y=1};  // 9
static const TMK61_cross_key   SUB     =   {.x=3, .y=8};   // -
static const TMK61_cross_key   ADD     =   {.x=2, .y=8};   // +
static const TMK61_cross_key   DIV     =   {.x=5, .y=8};   // /
static const TMK61_cross_key   MUL     =   {.x=4, .y=8};   // *
static const TMK61_cross_key   DOT     =   {.x=7, .y=8};   // .
static const TMK61_cross_key   POW     =   {.x=9, .y=8};   // ВП
static const TMK61_cross_key   NEG     =   {.x=8, .y=8};   // /-/
static const TMK61_cross_key   Cx      =   {.x=10, .y=8};  // Cx
static const TMK61_cross_key   Bx      =   {.x=11, .y=8};  // Bx
static const TMK61_cross_key   XY      =   {.x=6, .y=8};   // X<->Y
static const TMK61_cross_key   RET     =   {.x=4, .y=9};   // В/О
static const TMK61_cross_key   JMP     =   {.x=3, .y=9};   // БП
static const TMK61_cross_key   JSR     =   {.x=5, .y=9};   // ПП
static const TMK61_cross_key   RUN     =   {.x=2, .y=9};   // С/П

extern const TMK61_cross_key KeyPairs[40];

struct TMnemo {
  u8      fix;
  char*   txt;
};

extern const char* mnemo_code[40];
extern const u8    mnemo_code_register[40];
extern const char* mnemo_code_F[40];
extern const char* mnemo_code_K[40];
static const u8    STORE_KEY = 0x80;

#endif