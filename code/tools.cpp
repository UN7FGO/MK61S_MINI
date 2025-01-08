#include <Arduino.h>
#include "EEPROM.h"
#include <LiquidCrystal.h>
#include "lcd_gui.hpp"
#include "tools.hpp"
#include "mk61emu_core.h"
#include "keyboard.h"
#include "cross_hal.h"
#ifdef SPI_FLASH
  #include <SPI.h>
  #include <SPIFlash.h>
#endif
#include "menu.hpp"
#include "config.h"
#include "debug.h"

#include "ledcontrol.h"
using namespace led;

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

bool  Confirmation(void) {
  extern void lcd_std_display_redraw(void);

    lcd.setCursor(0,0); lcd.print("press OK confirm");
    i32 key = kbd::get_key_wait();
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
  kbd::get_key_wait();
  led::off();
}

SPIFlash  flash(PIN_SPIFLASH_CS);
bool      flash_is_ok;

bool erase_slot(usize nSlot) {
  const isize segment_address = calc_address(nSlot);
  if(segment_address < 0) return false;
  
  dbgln(SPIROM, "SPIFLASH: erase sector #", segment_address);
  while (!flash.eraseSector(segment_address));
  return true;
}

usize seek_program_END(u8* code_page) {
  isize lastCommand = 105;
  while (code_page[lastCommand] == 0) {
    lastCommand--;
  }

  if(lastCommand < 105) lastCommand++;
  if(lastCommand < 105) lastCommand++;

  return lastCommand;
}

isize  calc_address(usize nSlot) {
  if(nSlot > MAX_SLOT_FOR_PROGRAM) return -1;

  const int address = nSlot * FLASH_SECTOR_SIZE;

  dbgln(SPIROM, "X-reg as addsress ", address);
  return address;
}

isize  calc_address(void) {
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

bool load_from(isize address) {
  if(load_word(address, OFFSET_FLAG_OCCUPIED) != SLOT_OCCUPIED) {
    dbgln(SPIROM, "SPIFLASH: SLOT IS EMPTY ", address, "Nothing to load! canceled!");
    lcd.print(" is empty");
    sound(PIN_BUZZER, 4000, 750);
    delay(1500);
    return false; // error
  }

  dbgln(SPIROM, "SPIFLASH: read from address ", address);

  for(isize i=0; i<105; i++){
    MK61Emu_SetCode(core_61::get_ring_address(i), load_word(address, OFFSET_MK61_PROGRAMM + i));
  }
  return true;
}

bool Load(usize nSlot) {
  const isize address = calc_address(nSlot);
  if(address < 0) return false; // error

  return load_from(address);
}

bool Load(void) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Load "); 

  const isize address = calc_address();
  if(address < 0) return false; // error

  return load_from(address);
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
  for(isize i=0; i < 105; i++) all_to_or |= (usize) MK61Emu_GetCode(/*mk61s.*/core_61::get_ring_address(i));
  if(all_to_or == 0) {
    lcd.print("No program...");
    sound(PIN_BUZZER, 4000, 750);
    delay(1500);
    return true; 
  }
  return false;
}

char* ReadSlotName(usize nSlot, char* slot_name) {
  const isize segment_address = calc_address(nSlot);
  if(segment_address < 0) return NULL;

  usize i = 0;
  while(i < SIZEOF_SLOT_NAME) {
    const char symbol = flash.readByte(segment_address + OFFSET_SLOT_NAME + i);
    slot_name[i++] = symbol;
    if(symbol == 0) return slot_name;
  }
  slot_name[16] = 0;
  return slot_name;
}

bool clear_storage(void) {
  if(!flash_is_ok) return false;

  for(usize nSlot=0; nSlot <= MAX_SLOT_FOR_PROGRAM; nSlot++) {
    while (!flash.eraseSector(nSlot * FLASH_SECTOR_SIZE));
  }
  return true;
}

bool Rename(usize nSlot, char* slot_name) {
  const isize segment_address = calc_address(nSlot);
  if(segment_address < 0) return false;
  
  if(load_word(segment_address, OFFSET_FLAG_OCCUPIED) != SLOT_OCCUPIED) {
    #ifdef SERIAL_OUTPUT
      Serial.println("Empty slot cannot be renamed!");
    #endif
    ErrorReaction();
  }
  
  if(flash_is_ok) {
    // Сотрем сектор флеша, перед этим забэкапив область программ 105 шагов и область регистров 168 байт + 1 флаг занятости
      u8 backup[1 + 105 + 168];
      for(usize i=0; i < sizeof(backup); i++) backup[i] = flash.readByte(segment_address + i);
      dbgln(SPIROM, "SPIFLASH: erase sector...");
      while (!flash.eraseSector(segment_address));
      for(usize i=0; i < sizeof(backup); i++) flash.writeByte(segment_address + i, backup[i]);
    // запись имени слота  
      for(usize i=0; i < SIZEOF_SLOT_NAME; i++) {
        const char symbol = slot_name[i];
        flash.writeByte(segment_address + OFFSET_SLOT_NAME + i, symbol);
        if(symbol == 0) break;
      }
  }
  return true;
}

bool Store(usize nSlot) {
  if(check_empty_program()) return false; // error

  const isize address = calc_address(nSlot);
  if(address < 0) return false; // error

  if(load_word(address, OFFSET_FLAG_OCCUPIED) == SLOT_OCCUPIED) {
    dbgln(SPIROM, "SPIFLASH: SLOT IS OCCUPIED ", address);
  }

  dbgln(SPIROM, "SPIFLASH: write to address ", address);
  dbgln(SPIROM, "SPIFLASH: erase sector...");
  while (!flash.eraseSector(address));

  dbg(MINI, "Save ");
  store_word(address, OFFSET_FLAG_OCCUPIED, SLOT_OCCUPIED);
  for(isize i = 0; i < 105; i++){
    const u8 mk61_prg_word = MK61Emu_GetCode(core_61::get_ring_address(i));
    store_word(address, OFFSET_MK61_PROGRAMM + i, mk61_prg_word);
    dbg(MINI, "#");
  }
  dbg(MINI, "\nProgramm saved!");
  return true;
}

bool Store(void) {
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
    if(kbd::get_key_wait() != KEY_OK) return false; // error
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
    const u8 mk61_prg_word = MK61Emu_GetCode(core_61::get_ring_address(i));
    store_word(address, OFFSET_MK61_PROGRAMM + i, mk61_prg_word);
    #ifdef SERIAL_OUTPUT
      Serial.write('#');
    #endif
    const u8 x = i / BLOCK_SIZE; 
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
  if(kbd::get_key_wait() != KEY_OK) return action::MENU_BACK; 
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