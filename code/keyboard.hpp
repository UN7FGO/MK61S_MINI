#ifndef CLASS_KEYBOARD
#define CLASS_KEYBOARD

#include "Arduino.h"
#include "config.h"
#include "tools.hpp"
#include "rust_types.h"
#include "debug.h"

static constexpr usize KEY_IN_ROW             =   5;
static constexpr usize KEY_IN_COLUMN          =   8;
static constexpr usize LAST_SCAN_ROW          =   KEY_IN_ROW - 1;
static constexpr usize KEY_IN_KEYBOARD        =   KEY_IN_ROW * KEY_IN_COLUMN;
static constexpr usize KEY_RELEASE_BIT        =   6;

static constexpr t_time_ms  TIME_DEBOUNCE     =   30;
static constexpr t_time_ms  TIME_SCAN_LINE    =   30;
static constexpr t_time_ms  KEY_HOLD_MS       =   1500;  // константный период времени удержания клавиши до генерации события

enum class key_state {PRESSED=0, RELEASED=0x40};

static constexpr key_state PRESS             =   key_state::PRESSED;
static constexpr key_state RELEASE           =   key_state::RELEASED;

extern void idle_main_process(void);
extern void event_hold_key(i32 holded_key, i32 hold_quant);
extern void event_unhold_key(i32 unholded_key);

class   class_circular_buffer {
  private:
    static const   u32  MASK = 7;
  public:
    static const   u32  KEYBOARD_FIFO_LEN      =   8;
    static const   u32  KEYBOARD_FIFO_LAST     =   KEYBOARD_FIFO_LEN - 1;

    u32 read_count, write_count;
    i8  buff[KEYBOARD_FIFO_LEN];

    class_circular_buffer(void) : read_count(0), write_count(0) {};
    void Init(void) {read_count = 0; write_count = 0; };

    bool  IsFull(void) {
      return ((write_count - read_count) & ~MASK) != 0;
    }

    bool  IsEmpty(void) {
      return write_count == read_count;
    }

    i8  top(void) {
      return get(0);
    }

    i8  get(u32 index) {
      if(IsEmpty() || index > count()) return -1;
      return buff[(read_count + index) & MASK];
    }

    u32 count() const {
      return (write_count - read_count) & MASK;
    }

    bool  write(i8 data) {
      if(IsFull()) {
        #ifdef DEBUG
          Serial.println(" write full cir_buff");
        #endif
        return false;  // буфер переполнен
      } else {
        buff[write_count++ & MASK] = data;
        #ifdef DEBUG
          Serial.print("write cbuf "); Serial.print(data, HEX); 
          Serial.print(" : rd_cnt="); Serial.print(read_count); 
          Serial.print(" wr_cnt="); Serial.print(write_count);
          Serial.print(" isFull="); Serial.println(IsFull()? "T" : "F"); 
        #endif
        return true;
      }
    }

    i32  read(void) {
      if(IsEmpty()) { // буфер пуст 
        #ifdef DEBUG
          Serial.println(" read empty cir_buff");
        #endif
        return -1;
      } else {
        const i8 data = buff[read_count++ & MASK];
        #ifdef DEBUG
          Serial.print("read cbuf "); Serial.print(data, HEX); 
          Serial.print(" : rd_cnt="); Serial.print(read_count); 
          Serial.print(" wr_cnt="); Serial.println(write_count);
        #endif
        return data;
      }
    }
};

static const   u8   data_pins[KEY_IN_COLUMN]   =   {PIN_KBD_COL0, PIN_KBD_COL1, PIN_KBD_COL2, PIN_KBD_COL3, PIN_KBD_COL4, PIN_KBD_COL5, PIN_KBD_COL6, PIN_KBD_COL7};
static const   u8   pins[KEY_IN_ROW]           =   {PIN_KBD_ROW4, PIN_KBD_ROW3, PIN_KBD_ROW2, PIN_KBD_ROW1, PIN_KBD_ROW0};

inline isize get_set_bit_position(u8 &row_code) {
  for(usize bit_position = 0; bit_position <= KEY_IN_COLUMN; bit_position++){
    if((row_code & 1) != 0) return bit_position;
    row_code >>= 1;
  }
  return -1;
}

inline usize bus_in(void) {
  usize input_data = 0;
  for(int pin : data_pins) input_data = (input_data << 1) | digitalRead(pin);
  return input_data;
}

struct  TRowKeyStatus { // биты линий 1, 2, 4, 8, 16
  public:
    u8  now,     // данные сейчас 
        changed; // изменившиеся колонки
    u8 input(void) {
      const u8 before = now;
      now   = (u8) bus_in();
      return changed = now ^ before;
    }
    u8 get_state(usize column) {return ((~now >> column) & 1) << KEY_RELEASE_BIT;}
};

class   class_keyboard {
  private:
    t_time_ms       press_time;         // время в ms последнего нажатия (без отжатия)
    i32             holded_scan_code;   // скан код клавишы взятой на удержание
    i32             hold_quant_counter; // счетчик квантов удержания

    bool	is_release(i32 scan_code) {
      return ((scan_code & (1 << KEY_RELEASE_BIT)) != 0);
    }

    void 	check_hold_key(void) {
      if(holded_scan_code < 0) return;

      const t_time_ms now = millis();
      if(press_time >= now) return;
      hold_quant_counter++;
      dbgln(KBD, "hold time ", now, " hold count ", hold_quant_counter, " scan #", holded_scan_code);
      press_time = now + KEY_HOLD_MS;   // продалжаем опрашивать удержание до сброса удерживаемого скан-кода
      event_hold_key(holded_scan_code, hold_quant_counter); // генерация события удержания кнопки
    }

  public:

    usize         scan_line;
    u32           time_switch_scan_line;
    u32           time_debounced_key;

    TRowKeyStatus RowArray[KEY_IN_ROW];

    class_circular_buffer  cir_buff;

    class_keyboard(void) : holded_scan_code(-1), scan_line(0) {}

    void  Init(void) {
      for(u32 i=0; i < KEY_IN_ROW; i++) {
        RowArray[i] = {.now = 0, .changed = 0};
        digitalWrite(pins[i], HIGH);
        pinMode(pins[i], INPUT);
      }
      for(usize pin : data_pins) pinMode(pin, INPUT_PULLDOWN);
      cir_buff.Init();

      process_init();
    }

    void  process_init(void) {
      const u32 init_time = millis();
      time_switch_scan_line = init_time + TIME_SCAN_LINE;
      time_debounced_key    = init_time + TIME_DEBOUNCE;
    }

    i32   get_key_wait(void) {
      do {
        idle_main_process();  // отдаем безделье в основной поток бездействия
        const i32 scan_code = scan_and_debounced();
        if( scan_code >= 0 && scan_code < (1 << KEY_RELEASE_BIT) ) {
          exclude_before(scan_code);
          return scan_code;
        } 
      } while (true);
    }

    bool  check_key(i32 key_in, key_state state) { // поиск в буфере клавиши с конкретным статусом
      const u8 seek_key_code =(u8) (key_in | (i32) state);
      for(u32 i=0; i < cir_buff.count(); i++) {
        if(cir_buff.get(i) == seek_key_code) return true;
      }
      return false;
    }

    i32   last_key(void) {
      return  cir_buff.get(0);
    }

    i32   get_key(void) {
      return  cir_buff.read();
    }

    i32   get_key(key_state state) {
      i32 key_code;

      while(!cir_buff.IsEmpty()) {
        key_code = cir_buff.read();
        #ifdef DEBUG
          Serial.print(" get_key "); Serial.println(key_code, HEX);
        #endif
        if((key_code & (1 << KEY_RELEASE_BIT)) == (i8) state){
          #ifdef DEBUG
            Serial.print(" get_key ret "); Serial.println(key_code, HEX);
          #endif
          return key_code;
        } 
      }
      
      return -1;
    }

    void  clear_hold_key(void) {
      holded_scan_code = -1;
    }

    isize scan(void) {
      u8 bit_changed = RowArray[scan_line].input();
        const u8 row = scan_line;
    
        pinMode(pins[scan_line], INPUT);
        if(scan_line == (KEY_IN_ROW-1)) scan_line = 0; else scan_line++;
        pinMode(pins[scan_line], OUTPUT);
        

      if(bit_changed == 0) {  // нет изменений в столбцах клавиатуры (выход)
         check_hold_key();      // Проверка врремени удержания 
         return -1;             
      }

      const usize column = get_set_bit_position(bit_changed);
      const u8 state     = RowArray[row].get_state(column);
      const u8 code      = (column*KEY_IN_ROW + row);
      const u8 scan_code = state | code;

      dbgln(KBD, "changed ", bit_changed, ",column ", column, ",row ", row,", scan_code ", scan_code);
      
      if(state == 0) sound(PIN_BUZZER, 140, 14);
      cir_buff.write(scan_code);

      if(state == 0) {
        // было нажатие, принимаем на удержание клавишу (учет только одного последнего удержания)
        hold_quant_counter  =   -1;
        holded_scan_code    =   scan_code;
        press_time          =   millis() + KEY_HOLD_MS;
        dbgln(KBD, "fixed press time: ", press_time, "ms, (hold) scan_code #", scan_code);
      } else {
        // было отжатие удержанной клавиши
        if(holded_scan_code == code) {
          event_unhold_key(holded_scan_code);
          holded_scan_code = -1; // снимаем удержание 
        }
      }

      return (isize) scan_code;
    }

    void  exclude_before(i32 before_key) { // уберать все коды клавиш в том числе before_key, из очереди клавиатуры
      i32 exclude_key;
      do {
        exclude_key = get_key();
        #ifdef DEBUG
          Serial.print("del "); Serial.println(exclude_key, HEX);
        #endif 
        //if(exclude_key < 0) break;
      } while(exclude_key > 0 && exclude_key != before_key);
    }

    void  reset_scan_line(void) {
        scan_line = KEY_IN_ROW-1;
    }

    isize  scan_and_debounced(void);
};

extern class_keyboard keyboard;
#endif