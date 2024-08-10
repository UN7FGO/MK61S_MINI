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

extern uint8_t ringM[252+252+42+42+42+42];

enum enum_core61_stage {START, NEXT};

typedef enum
{
    MK61_OK = 0,
    MK61_ERROR
} MK61Result;

typedef enum
{
    NONE   = 0,
    RADIAN = 10,  // Р - радианы
    DEGREE = 11, // Г - градусы
    GRADE  = 12,  // ГРД - грады
    DEGREE_ERASE = 0xFF // Г - градусы установленные как стертое значение флеш (умолчание)
} AngleUnit;

typedef enum
{
    // Offsets in RingM buffer
    REG_Y = 580,
    REG_Z = 622,
    REG_T = 622 + 42
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

typedef struct
{
    microinstruction_t microinstructions[68]; // микрокоманды
    instruction_t instructions[256];     // команды
    uint8_t microprograms[1152];   // микропрограммы (синхропрограммы - последовательности микрокоманд)
} IK13_ROM;

/**
 * mk61ROM
 */
typedef struct
{
    IK13_ROM IK1302;
    IK13_ROM IK1303;
    IK13_ROM IK1306;
} mk61ROM_t;

/**
 * IK13
 */
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

typedef struct{
    uint8_t *pM;
    uint8_t R[IK13_MTICK_COUNT];
    uint8_t ST[IK13_MTICK_COUNT];

    io_t AMK, MOD;
    io_t S, S1, L, T, P;

    uint16_t uI_hi;     // Instruction HI word
    uint8_t  flag_FC;
    uint8_t *pAND_AMK;  // Precalc offset from microprograms for signal_I 0..26
    uint8_t *pAND_AMK1; // Precalc offset from microprograms for signal_I 27..35
    uint16_t key_x, key_xm, key_y, comma;
}  IK1303;

typedef struct{
    uint16_t uI_hi;  // Instruction HI word
    uint32_t AMK;

    uint32_t  L, S, S1, P, T, MOD, flag_FC;

    uint8_t   R[IK13_MTICK_COUNT];
    uint8_t   ST[IK13_MTICK_COUNT];

    uint8_t*  pAND_AMK1; // Precalc offset from microprograms for signal_I 27..35
    uint8_t*  pAND_AMK;
    uint8_t*  pM;
}  IK1306;

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

extern IK1302 m_IK1302;

class class_mk61_core {
  private:
    //                                mantisa                     |    pow
    //                              0   1   2   3   4  5  6  7  8,  9, 10, 11
    const int indicator_pos[12] = {24, 21, 18, 15, 12, 9, 6, 3, 0, 33, 30, 27};
    u32    backstep_comma_position;
  public:
    const int reg_Y_pow         = 580;
    enum  class mode_edge  {UNCHANGED=-1, TURN_PRG=0, TURN_AUTO=8, TURN_COMPUTING=11};
    void  enable();
    void  cycle(void);
    void  step(void);

    mode_edge  get_calculator_mode(void) {
      const u32 now_comma_position = get_comma_position();
      if(backstep_comma_position != now_comma_position) {
        backstep_comma_position = now_comma_position;  // перезапишем измеение запятой
        return (mode_edge) now_comma_position;
      } else {
        return mode_edge::UNCHANGED;
      }
    }

    int get_ring_address(int linear_address) const {
      const int cycle_x = ((linear_address % 7) == 0)?  linear_address : (linear_address - 7);
      return 41 + cycle_x * 6;
    }

    void set_code(int linear_address, u8 data) {
      const int ring_address = get_ring_address(linear_address);
      ringM[ring_address] = data >> 4;
      ringM[ring_address - 3] = data & 0x0F;
    }

    u8  get_code(int linear_address){
      const int ring_address = get_ring_address(linear_address);
      return (ringM[ring_address]<<4)|(ringM[ring_address-3]);
    }

    u8 get_IP(void) const {
      return  get_IPH()*10 + get_IPL();
    }

    u8 get_IPH(void) const {
      return  m_IK1302.R[34] & 0xF;
    }

    u8 get_IPL(void) const {
      return  m_IK1302.R[31] & 0xF;
    }

    void clear_displayed(void) {m_IK1302.displayed = 0;}

    bool is_displayed(void) const {
      return (m_IK1302.displayed != 0);
    }

    uint32_t get_comma_position(void) const {
      return m_IK1302.comma;
    }

    void  read_indicator(char* buffer, const char* display_symbols) { // считывает в буфер содержимое дисплея МК61
      const int comma_pos = 9 - m_IK1302.comma + 1;

        for(int i=0; i<12; i++) {
          if(i == comma_pos) *buffer++ = '.';
          *buffer++ = display_symbols[ m_IK1302.R[ indicator_pos[i] ] ];
        }

      *buffer = 0;
    }

    void  read_Y(char* buffer, const char* display_symbols);

    // возращает false - есть изменения в дисплейной строке/ true - нет изменений
    //  - дисплейный буфер buffer обновляется только измененным содержимым
    //  - display_symbols - массив набор символов замены знаков индикатора 
    bool  update_indicator(char* buffer, const char* display_symbols);
};

/*
#ifdef __cplusplus
 extern "C" {
#endif
*/
void MK61Emu_ON(void);
void MK61Emu_SetDisplayed(uint32_t value);
uint32_t MK61Emu_GetDisplayed(void);
uint32_t MK61Emu_GetComma(void);
void MK61Emu_SetKeyPress(const int key1, const int key2);
void MK61Emu_DoKeyPress(const int key1, const int key2);
void MK61Emu_Cleanup(void);
//void MK61Emu_Cycle(void);
u8* MK61Emu_UnpackRegster(u8 nReg, u8 *pack_number);
void MK61Emu_WriteRegister(int nReg, char* buffer);
void MK61Emu_ReadRegister(int nReg, char* buffer);
bool MK61Emu_IsRunning(void);
void MK61Emu_SetCode(int addr, uint8_t data);
uint8_t MK61Emu_GetCode(int addr);
void MK61Emu_GetCodePage(uint8_t* page);
const char* MK61Emu_GetIndicatorStr(const char* display_symbols);
const char* MK61Emu_GetStackStr(StackRegister stack_reg);
const char* MK61Emu_GetStackStr(StackRegister stack_reg, const char* symbols_set);
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
