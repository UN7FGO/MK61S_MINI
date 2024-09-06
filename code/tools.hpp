#ifndef TOOLS
#define TOOLS

#include "rust_types.h"
#include "EEPROM.h"
#include "mk61emu_core.h"


static const int switch_R_GRD_G = 106;
static const int count_switch_R_GRD_G = 107;

extern bool flash_is_ok;
extern void DFU_enable(void);
extern bool Store(void);
extern bool Load(void);
extern void init_external_flash(void);

inline bool IsDecimalDigit(char symbol) {
 return ((symbol >= '0') && (symbol <= '9'));
}

#ifdef SERIAL_OUTPUT
inline void Serial_write_hex(u8 code) {
    if(code < 0x10) Serial.write('0');
    Serial.print(code, HEX);
}

inline void Serial_writeln_hex(u8 code) {
    if(code < 0x10) Serial.write('0');
    Serial.println(code, HEX);
}
#endif

inline u64 pad_left_8_char(char* string_8_char) {
  u64 result = *(uint64_t*) string_8_char;
  while((result & 0x0000FF0000000000) == 0) {
    result = (result << 8) | 0x0000000000000020;
  }
  return result;
}

inline AngleUnit read_grade_switch(void) {
  return (AngleUnit) EEPROM.read(switch_R_GRD_G);
}

inline u8 read_counter_switch(void) {
  return EEPROM.read(count_switch_R_GRD_G);
}

inline void load_grade_switch(void) {
  static const AngleUnit rom_angle = (AngleUnit) EEPROM.read(switch_R_GRD_G);
  if(rom_angle == RADIAN || rom_angle == GRADE || rom_angle == DEGREE) { // состояние переключателя считано из флеш как определнное (радианы или грады)
    MK61Emu_SetAngleUnit(rom_angle);
  } else { // из флеш считано либо "чистое" значение (FF - градусы), либо неопределенное в следствии сбоя очистки и записи в флеш
    MK61Emu_SetAngleUnit(DEGREE);
  }
  #ifdef SERIAL_OUTPUT
    Serial.print("get grade_switch "); Serial.println(MK61Emu_GetAngleUnit());
  #endif
}

inline void store_grade_switch(AngleUnit angle_unit) {
  EEPROM.update(switch_R_GRD_G, (u8) angle_unit);
  EEPROM.update(count_switch_R_GRD_G, read_counter_switch() + 1);
}

#endif