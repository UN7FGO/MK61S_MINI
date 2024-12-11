/* 
 * This file is part of the MK61S distribution (https://gitlab.com/vitasam/mk61s).
 * Copyright (c) 2020- vitasam.
 * 
 * Based on emu145 code from F.Lazarev:
 * https://github.com/fixelsan/emu145
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _MK61EMU_H_
#define _MK61EMU_H_

#define MK_OK 0xFFFF
#define MK61EMU_VERSION_MAJOR 1
#define MK61EMU_VERSION_MINOR 2

#include <stdint.h>
#include "rust_types.h"
#include <stdbool.h>

#ifdef EXPAND_RING_MK61
  //                                        IR1   IR2  IKF  1302 1303 1306
  static constexpr  usize   SIZE_RING_M   = 252 + 252 + 42 + 42 + 42 + 42;
  //                                        IR1  IKF  1302 1303 1306
  static constexpr  usize   OFFSET_IK1302 = 252 + 42 + 42;
  static constexpr  usize   OFFSET_IK1303 = 252 + 42 + 42 + 42;
  static constexpr  usize   OFFSET_IK1306 = 252 + 42 + 42 + 42 + 42;
#else
  //                                        IR1   IR2  1302 1303 1306
  static constexpr  usize   SIZE_RING_M   = 252 + 252 + 42 + 42 + 42;
  //                                        IR1  1302 1303 1306
  static constexpr  usize   OFFSET_IK1302 = 252 + 42;
  static constexpr  usize   OFFSET_IK1303 = 252 + 42 + 42;
  static constexpr  usize   OFFSET_IK1306 = 252 + 42 + 42 + 42;
#endif

namespace ring_M {
  struct  K745 {
    const char  *NAME;
    const usize OFFSET;
  };
  static constexpr K745 IR2_1_0 = {.NAME = "IR2.1_0", .OFFSET = 0};
  static constexpr K745 IR2_2   = {.NAME = "IR2.2",   .OFFSET = 42};
  static constexpr K745 IK130X  = {.NAME = "IK130X",  .OFFSET = 42 + 42};

#ifdef EXPAND_RING_MK61
  static constexpr K745 IK1302  = {.NAME = "IK1302",  .OFFSET = 42 + 42 + 252};
  static constexpr K745 IK1303  = {.NAME = "IK1303",  .OFFSET = 42 + 42 + 252 + 42};
  static constexpr K745 IK1306  = {.NAME = "IK1306",  .OFFSET = 42 + 42 + 252 + 42 + 42};
  static constexpr K745 IR2_1_1 = {.NAME = "IR2.1_1", .OFFSET = 42 + 42 + 252 + 42 + 42 + 42};
  static const K745 CHIP[6] = {IR2_1_0, IR2_2, IK130X, IK1302, IK1303, IK1306, IR2_1_1};
#else
  static constexpr K745 IK1302  = {.NAME = "IK1302",  .OFFSET = 42 + 252};
  static constexpr K745 IK1303  = {.NAME = "IK1303",  .OFFSET = 42 + 252 + 42};
  static constexpr K745 IK1306  = {.NAME = "IK1306",  .OFFSET = 42 + 252 + 42 + 42};
  static constexpr K745 IR2_1_1 = {.NAME = "IR2.1_1", .OFFSET = 42 + 252 + 42 + 42 + 42};
  static const K745 CHIP[6] = {IR2_1_0, IR2_2, IK1302, IK1303, IK1306, IR2_1_1};
#endif
} // namespace ring_M

typedef enum { 
  X1 = ring_M::IR2_1_1.OFFSET + (42 * 0), 
  X  = ring_M::IR2_1_1.OFFSET + (42 * 1),
  Y  = ring_M::IR2_1_1.OFFSET + (42 * 2),
  Z  = ring_M::IR2_1_1.OFFSET + (42 * 3),
  T  = ring_M::IR2_1_1.OFFSET + (42 * 4)
} stack;

enum enum_core61_stage {START, NEXT};

typedef enum {
    MK61_OK = 0,
    MK61_ERROR
} MK61Result;

typedef enum {
    NONE   = 0,
    RADIAN = 10,  // Р - радианы
    DEGREE = 11, // Г - градусы
    GRADE  = 12,  // ГРД - грады
    DEGREE_ERASE = 0xFF // Г - градусы установленные как стертое значение флеш (умолчание)
} AngleUnit;

typedef enum {
    // Offsets in RingM buffer
    REG_X1  =   580 - 42 - 42,
    REG_X   =   580 - 42,
    REG_Y   =   580,
    REG_Z   =   622,
    REG_T   =   622 + 42
} StackRegister;

typedef uint32_t microinstruction_t; // 4-байтные микрокоманды
typedef uint32_t instruction_t;  // 4-байтные команды
typedef uint32_t io_t;
typedef uint32_t mtick_t;

typedef char mk61_register_position_t;

#define MK61_REGISTER_POSITIONS_COUNT       14
#define IK13_MTICK_COUNT                    42
#define IR2_MTICK_COUNT                     252

typedef mk61_register_position_t mk61_register_t[MK61_REGISTER_POSITIONS_COUNT];

typedef struct{
    uint32_t AMK;

    uint32_t  key_y, key_x, key_xm;
    uint32_t  displayed;
    uint32_t  comma;
    uint32_t  L, S, S1, P, T, MOD, flag_FC;

    uint8_t   R[IK13_MTICK_COUNT];
    uint8_t   ST[IK13_MTICK_COUNT];

    uint8_t*  pAND_AMK1;                            // Precalc offset from microprograms for signal_I 27..35
    uint8_t*  pAND_AMK;
    uint8_t*  pM;
}  IK1302;

/**
 * @brief The MK61 emulator object
 */
#define INDICATOR_STRING_LENGTH				15
typedef struct{
    char m_indicator_str[INDICATOR_STRING_LENGTH];
    char m_stack_y_str[INDICATOR_STRING_LENGTH];
    char m_stack_z_str[INDICATOR_STRING_LENGTH];
    AngleUnit m_angle_unit;
} MK61Emu;

extern  IK1302  m_IK1302;
extern  uint8_t ringM[SIZE_RING_M];

namespace core_61 {
  inline    isize get_ring_address(isize linear_address) {
    const   isize cycle_x = ((linear_address % 7) == 0)?  linear_address : (linear_address - 7);
    return 41 + cycle_x * 6;
  }

  inline    void  clear_displayed(void) { m_IK1302.displayed = 0; }

  inline    u8    get_IPH(void)         { return  m_IK1302.R[34] & 0xF; }
  inline    u8    get_IPL(void)         { return  m_IK1302.R[31] & 0xF; }
  inline    u8    get_IP (void)         { return  get_IPH()*10 + get_IPL(); }

  inline    bool  is_displayed(void)    { return (m_IK1302.displayed != 0); }
  inline    usize comma_position(void)  { return m_IK1302.comma; }

  extern    void  enable(void);
  extern    void  step(void);

  // возращает false - есть изменения в дисплейной строке/ true - нет изменений
  //  - дисплейный буфер buffer обновляется только измененным содержимым
  //  - display_symbols - массив набор символов замены знаков индикатора 
  extern    bool  update_indicator(char* buffer, const char* display_symbols);
}

/*
#ifdef __cplusplus
 extern "C" {
#endif
*/
void    MK61Emu_ON(void);
void    MK61Emu_SetDisplayed(uint32_t value);
uint32_t MK61Emu_GetDisplayed(void);
int     MK61Emu_GetDisplayReg(void);
uint32_t MK61Emu_GetComma(void);
void    MK61Emu_SetKeyPress(const int key1, const int key2);
void    MK61Emu_DoKeyPress(const int key1, const int key2);
void    MK61Emu_Cleanup(void);
//void MK61Emu_Cycle(void);
u8*     MK61Emu_UnpackRegster(u8 nReg, u8 *pack_number);
void    MK61Emu_WriteRegister(int nReg, char* buffer);
//void    MK61Emu_ReadRegister(int nReg, char* buffer);
void    MK61Emu_ReadRegister(int nReg, char* buffer, const char* display_symbols);

extern const char* read_stack_register(stack reg, char cvalue[14], const char* symbols_set);
extern void write_stack_register(stack reg, char sign, char cmantissa[8], isize pow);

usize   MK61Emu_Read_R_mantissa(usize nReg);
usize   MK61Emu_Read_X_as_byte(void);
bool    MK61Emu_IsRunning(void);

void    MK61Emu_SetCode(int addr, uint8_t data);
uint8_t MK61Emu_GetCode(int addr);
void    MK61Emu_GetCodePage(uint8_t* page);
void    MK61Emu_ClearCodePage(void);

const char* MK61Emu_GetIndicatorStr(const char* display_symbols);
//const char* MK61Emu_GetStackStr(StackRegister stack_reg);
//const char* MK61Emu_GetStackStr(StackRegister stack_reg, const char* symbols_set);
//void        MK61Emu_SetStackStr(StackRegister stack_reg, char sign, char mantissa[8], isize pow);
void MK61Emu_SetAngleUnit(AngleUnit angle);
AngleUnit MK61Emu_GetAngleUnit(void);

/* Функции с новой парадигмой использования реализующие доступ к внутренним ресурсам МК61 */
bool  MK61Emu_ReadIndicator(char* buffer, const char* display_symbols);
//int   MK61Emu_get_ring_address(int address);
//int   MK61Emu_get_IP(void);
void  MK61Emu_get_1302_R(char* buff); // experement
/*
#ifdef __cplusplus
}
#endif
*/
#endif
