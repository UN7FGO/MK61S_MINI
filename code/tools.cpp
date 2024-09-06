#include <Arduino.h>
#include "EEPROM.h"
#include <LiquidCrystal.h>
#include "lcd_gui.hpp"
#include "tools.hpp"
#include "mk61emu_core.h"
#include <SPI.h>
#include <SPIFlash.h>

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

SPIFlash  flash(PIN_SPIFLASH_CS);
bool      flash_is_ok;
const int FLASH_SECTOR_SIZE = 4096;

int  calc_address(void) {
  const int n_cell = MK61Emu_GetDisplayReg();

  if(n_cell > 99) {
    lcd.setCursor(0, 0); lcd.print("Error! slot > 99!");
    return -1;
  } else {
    lcd.print("slot "); lcd.print(n_cell);
  }

  const int address = n_cell * FLASH_SECTOR_SIZE;

  #ifdef SERIAL_OUTPUT
    Serial.print("X-reg as addsress "); Serial.println(address);
  #endif
  return address;
}

void init_external_flash(void) {
  #ifdef DEBUG_SPIFLASH
    Serial.print("Init flash -> ");
  #endif

  // Инициализация SPI
  SPI.begin();
 
  // Инициализация W25Q128
  flash_is_ok = flash.begin();
  #ifdef DEBUG_SPIFLASH
    if(flash_is_ok) {
      Serial.print("OK! size = "); Serial.print(flash.getCapacity()); Serial.println(" K");
    } else {
      Serial.println("ERROR!");
    }
  #endif
}

bool Load(void) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Loading "); lcd.setCursor(8, 0); 

  const int address = calc_address();
  if(address < 0) return false; // error

  #ifdef DEBUG_SPIFLASH
    Serial.print("SPIFLASH: read from address ");
    Serial.println(address);
  #endif

  for(int i=0; i<105; i++){
    const u8 opcode = (flash_is_ok)? flash.readByte(i + address) : EEPROM.read(i);
    MK61Emu_SetCode(mk61s.get_ring_address(i), opcode);
  }
  return true;
}

bool Store(void) {
  static constexpr int block_size = 106 / 13;

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Saving "); lcd.setCursor(7, 0); 

  const int address = calc_address();
  if(address < 0) return false; // error

  #ifdef DEBUG_SPIFLASH
    Serial.print("SPIFLASH: write to address ");
    Serial.println(address);
  #endif

  #ifdef DEBUG_SPIFLASH
    Serial.print("SPIFLASH: erase sector...");
  #endif
  while (!flash.eraseSector(address));

  #ifdef SERIAL_OUTPUT
    Serial.print("Save ");
  #endif

  for(int i = 0; i < 105; i++){
    const u8 mk61_prg_word = MK61Emu_GetCode(mk61s.get_ring_address(i));
    if(flash_is_ok) {
      flash.writeByte(i + address, mk61_prg_word);
    } else {
      EEPROM.update(i, mk61_prg_word);
    }
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