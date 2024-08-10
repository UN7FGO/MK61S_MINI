#include <Arduino.h>
#include "EEPROM.h"
#include <LiquidCrystal.h>
#include "lcd_gui.hpp"
#include "tools.hpp"
#include "mk61emu_core.h"

extern class_mk61_core mk61s;

const  class_LCD_Label  DFU_message(0, 0);
const  class_LCD_Label  STORE_message(0, 0);
const  class_LCD_Label  STORE_progress_message(0, 1);

void DFU_enable(void) {
	void (*SysMemBootJump)(void);

  lcd.clear();
  DFU_message.print(" DFU flash mode!");
    __enable_irq();
    HAL_RCC_DeInit();
    HAL_DeInit();
    SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

    const uint32_t p = (*((uint32_t *) 0x1FFF0000));
    __set_MSP( p );

    SysMemBootJump = (void (*)(void)) (*((uint32_t *) 0x1FFF0004));
    SysMemBootJump();

  while(true) {};
 
}

bool Load(void) {
  for(int i=0; i<105; i++){
    const u8 opcode = EEPROM.read(i);
    //mk61s.set_code(i, opcode); 
    MK61Emu_SetCode(mk61s.get_ring_address(i), opcode);
  }
  return true;
}

bool Store(void) {
  const int block_size = 106 / 13;
  lcd.clear();
  STORE_message.print("save..."); 
  #ifdef SERIAL_OUTPUT
    Serial.print("Save ");
  #endif
  for(int i=0; i<105; i++){
    EEPROM.update(i, MK61Emu_GetCode(mk61s.get_ring_address(i)));// mk61s.get_code(i);
    #ifdef SERIAL_OUTPUT
      Serial.write('#');
    #endif
    const u8 x = i / block_size; 
    lcd.setCursor(x, 1); lcd.print((char) 0xFF); lcd.print(i);
  }
  #ifdef SERIAL_OUTPUT
    Serial.println("\nProgramm saved!");
  #endif
  return true;
}              