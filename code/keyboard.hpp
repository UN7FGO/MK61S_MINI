#ifndef CLASS_KEYBOARD
#define CLASS_KEYBOARD
#include "Arduino.h"
#include "rust_types.h"
#include "config.h"

static const u32  KEY_IN_ROW             =   5;
static const u32  KEY_IN_COLUMN          =   8;
static const u32  LAST_SCAN_ROW          =   KEY_IN_ROW - 1;
static const u32  KEY_IN_KEYBOARD        =   KEY_IN_ROW * KEY_IN_COLUMN;
static const u32  TIME_DEBOUNCE          =   30;
static const u32  TIME_SCAN_LINE         =   40;

enum class key_state {PRESSED=0, RELEASED=0x40};

const   key_state PRESS             =   key_state::PRESSED;
const   key_state RELEASE           =   key_state::RELEASED;

class  TKeyMask { // биты линий 1, 2, 4, 8, 16
  public:
    u8  now, before;  // маска сейчас и до того 
// TO DO изменить алгоритм сдвинуть test_source один раз на from бит потом проверять только первый бит и не двигать маску!
    u32 get_position(u32 from) {  // -1 - нет 1 ни в одном бите
      //u32 test_mask = 1 << from;
      u32 test_source = (now ^ before) >> from;

      for(u32 i = from; i < KEY_IN_COLUMN; i++) {
        if((test_source & 1) != 0) return i;
        test_source >>= 1;
      }

/*
      for(int i = from; i < KEY_IN_COLUMN; i++) {
        if((test_mask & test_source) != 0) return i;
        test_mask <<= 1;
      }
*/
      return KEY_IN_COLUMN;
    }
};

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

    i8 get(u32 index) {
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

class   class_keyboard {
  private:
    const   u8   data_pins[KEY_IN_COLUMN]   =   {PIN_KBD_COL0, PIN_KBD_COL1, PIN_KBD_COL2, PIN_KBD_COL3, PIN_KBD_COL4, PIN_KBD_COL5, PIN_KBD_COL6, PIN_KBD_COL7};
    const   u8   pins[KEY_IN_ROW]           =   {PIN_KBD_ROW4, PIN_KBD_ROW3, PIN_KBD_ROW2, PIN_KBD_ROW1, PIN_KBD_ROW0};

  public:

    u32         scan_line;
    u32         time_switch_scan_line;
    u32         time_debounced_key;
    TKeyMask    keymask[KEY_IN_ROW];
    class_circular_buffer  cir_buff;

    class_keyboard(void) : scan_line(0) {}

    i32 get_code(u32 key_line) { // no code -1
      const i32 BitPosition = keymask[key_line].get_position(0);
      return (BitPosition * KEY_IN_ROW) + key_line;
    }
    
    void  Init(void) {
      for(u32 i=0; i < KEY_IN_ROW; i++) {
        keymask[i].now = 0;
        keymask[i].before = 0;
        digitalWrite(pins[i], HIGH);
        pinMode(pins[i], INPUT);
      }
      for(u32 i=0; i < KEY_IN_COLUMN; i++) {
        pinMode(data_pins[i], INPUT_PULLDOWN);
      }
      cir_buff.Init();

      process_init();
    }

    void  process_init(void) {
      const u32 init_time = millis();
      time_switch_scan_line = init_time + TIME_SCAN_LINE;
      time_debounced_key    = init_time + TIME_DEBOUNCE;
    }

    void  process(u32 time) {
      if (time >= time_switch_scan_line) { // переключение следующей сканлинии клавиатуры
        next_scan_line();

        const u32 now = millis();
        time_debounced_key = now + TIME_DEBOUNCE;       // время до прекращеняи дребезга контактов кнопки 
        time_switch_scan_line = now + TIME_SCAN_LINE;   // время следующего переключения сканлинии клавиатуры (сканстолбца)
      }
  
      if (time >= time_debounced_key) {  // время когда дребезг контактов кнопки (если она была нажата) прекратился
        check_scan_line();
      }
    }

    i32  get_key_wait(void) {
      i32 keycode;
      do {
        process(millis());
        keycode = get_key(key_state::PRESSED);
      } while (keycode < 0);

      return keycode;
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
        if((key_code & 0x40) == (i8) state){
          #ifdef DEBUG
            Serial.print(" get_key ret "); Serial.println(key_code, HEX);
          #endif
          return key_code;
        } 
      }
      
      return -1;
    }

    u32   get_keydata(void) { // Считать данные с порта данных клавиатуры
      u32 data = 0;
      for(u32 i=0; i < KEY_IN_COLUMN; i++) {
        data <<= 1;
        data |= digitalRead(data_pins[i]);
      }
      return data;
    }

    i32   check_scan_line(void) {
      const u8 now_mask = get_keydata(); 
      const u8 last_mask = keymask[scan_line].now;

      if(now_mask != last_mask) { // key change
        keymask[scan_line] = {.now = now_mask, .before = last_mask};  // обновляем маски было и стало
        const u32 key_code = get_code(scan_line);
        #ifdef DEBUG
          Serial.print(now_mask); Serial.write(' ');
          Serial.print(last_mask); Serial.write(' ');
          Serial.print(keymask[scan_line].get_position(0)); Serial.write(' ');
          Serial.print(key_code, HEX); Serial.write('.');
        #endif

        if(key_code < KEY_IN_KEYBOARD) {
          const i8 full_key_code = key_code | ((now_mask == 0)? 0x40 : 0);
          cir_buff.write(full_key_code);
          #ifdef DEBUG
            Serial.println(full_key_code, HEX);
          #endif
        }
        return (i32) key_code;
      }
      return -1;
    }

    void  next_scan_line(void) {
      if(scan_line == LAST_SCAN_ROW) { // Переход с включенной ранее последней линии сканирования на первую
        pinMode(pins[LAST_SCAN_ROW], INPUT);
        scan_line = 0;
      } else { // Переход на очередную линию сканирования 
        pinMode(pins[scan_line++], INPUT);
      }
      pinMode(pins[scan_line], OUTPUT); 
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

    bool  is_released() {
      return (keymask[scan_line].now == 0);
    }
};

extern class_keyboard keyboard;
#endif