#include <Arduino.h>
#include "EEPROM.h"
#include <LiquidCrystal.h>
#include "lcd_gui.hpp"
#include "tools.hpp"
#include "mk61emu_core.h"
#include "keyboard.hpp"
#include "cross_hal.h"
#include <SPI.h>
#include <SPIFlash.h>
#include "menu.hpp"
#include "config.h"
#include "debug.h"

#include "ledcontrol.h"
using namespace led;

extern class_mk61_core mk61s;
extern class_keyboard keyboard;

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
    SysTick->CTRL = 0; SysTick->LOAD = 0; SysTick->VAL = 0;
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

    const uint32_t p = (*((uint32_t *) 0x1FFF0000));
    __set_MSP( p );

    SysMemBootJump = (void (*)(void)) (*((uint32_t *) 0x1FFF0004));
    SysMemBootJump();

  while(true) {};
 
}

bool  Сonfirmation(void) {
  extern void lcd_std_display_redraw(void);

    lcd.setCursor(0,0); lcd.print("press OK confirm");
    i32 key = keyboard.get_key_wait();
    lcd_std_display_redraw();

    return (key == KEY_OK);
}

void  sound(usize pin, isize freq_Hz, usize duration_ms) {
  if(library_mk61::sound_is_on()) {
    tone(pin, freq_Hz, duration_ms);
  }
}

void message_and_waitkey(const char* lcd_message) {
  led::on();
  lcd.setCursor(0, 1); lcd.print(lcd_message);
  keyboard.get_key_wait();
  led::off();
}

SPIFlash  flash(PIN_SPIFLASH_CS);
bool      flash_is_ok;

usize seek_program_END(u8* code_page) {
  isize lastCommand = 105;
  while (code_page[lastCommand] == 0) {
    lastCommand--;
  }
  lastCommand++;
  lastCommand++;
  return lastCommand;
}

int  calc_address(void) {
  const isize n_cell = MK61Emu_GetDisplayReg();

  if(n_cell > MAX_SLOT_FOR_PROGRAM) {
    lcd.setCursor(0, 0); lcd.print("Error! slot > "); lcd.print(MAX_SLOT_FOR_PROGRAM);
    return -1;
  } else {
    /*lcd.print("slot ");*/ lcd.print(n_cell);
  }

  const int address = n_cell * FLASH_SECTOR_SIZE;

  #ifdef SERIAL_OUTPUT
    Serial.print("X-reg as addsress "); Serial.println(address);
  #endif
  return address;
}

void init_external_flash(void) {
  dbgln(SPIROM, "Init flash -> ");

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

u8 load_word(isize segment_address, isize offset) {
  return (flash_is_ok)? flash.readByte(offset + segment_address) : EEPROM.read(offset);
}

bool Load(void) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Load "); 

  const isize address = calc_address();
  if(address < 0) return false; // error

  if(load_word(address, OFFSET_FLAG_OCCUPIED) != SLOT_OCCUPIED) {
    dbgln(SPIROM, "SPIFLASH: SLOT IS EMPTY ", address, "Nothing to load! canceled!");
    lcd.print(" is empty");
    sound(PIN_BUZZER, 4000, 750);
    delay(1500);
    return false; // error
  }

  dbgln(SPIROM, "SPIFLASH: read from address ", address);

  for(isize i=0; i<105; i++){
    MK61Emu_SetCode(mk61s.get_ring_address(i), load_word(address, OFFSET_MK61_PROGRAMM + i));
  }
  return true;
}

inline void store_word(isize segment_address, isize offset, u8 data) {
    if(flash_is_ok) {
      flash.writeByte(offset + segment_address, data);
    } else {
      EEPROM.update(offset, data);
    }
}

inline bool check_empty_program(void) {
  usize all_to_or = 0;
  for(isize i=0; i < 105; i++) all_to_or |= (usize) MK61Emu_GetCode(mk61s.get_ring_address(i));
  if(all_to_or == 0) {
    lcd.print("No program...");
    sound(PIN_BUZZER, 4000, 750);
    delay(1500);
    return true; 
  }
  return false;
}

bool Store(void) {
  static constexpr int block_size = 106 / 13;

  lcd.clear(); lcd.setCursor(0, 0);

  if(check_empty_program()) return false; // error

  lcd.print("Save "); //lcd.setCursor(7, 0); 

  const isize address = calc_address();
  if(address < 0) return false; // error

  if(load_word(address, OFFSET_FLAG_OCCUPIED) == SLOT_OCCUPIED) {
    #ifdef DEBUG_SPIFLASH
      Serial.print("SPIFLASH: SLOT IS OCCUPIED ");
      Serial.println(address);
    #endif
    sound(PIN_BUZZER, 4000, 750);
    lcd.setCursor(0, 0); lcd.print("OVER"); lcd.setCursor(8, 0); lcd.print("press OK");
    if(keyboard.get_key_wait() != KEY_OK) return false; // error
  }

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

  store_word(address, OFFSET_FLAG_OCCUPIED, SLOT_OCCUPIED);
  for(isize i = 0; i < 105; i++){
    const u8 mk61_prg_word = MK61Emu_GetCode(mk61s.get_ring_address(i));
    store_word(address, OFFSET_MK61_PROGRAMM + i, mk61_prg_word);
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

using namespace action;

bool  EraseFlash(void) {
  sound(PIN_BUZZER, 4000, 750);   
  lcd.setCursor(0, 0); lcd.print("press OK ERASED!"); 
  if(keyboard.get_key_wait() != KEY_OK) return action::MENU_BACK; 
 // стираем внешний флеш 
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("Erase slot ");
  for(usize i=0; i <= MAX_SLOT_FOR_PROGRAM; i++){
     while (!flash.eraseSector(i * FLASH_SECTOR_SIZE));
     lcd.setCursor(11, 0); lcd.print(i);
  }
  sound(PIN_BUZZER, 1000, 300);
  message_and_waitkey(" press any key! ");
  return action::MENU_EXIT;
}
