#ifndef CLASS_KEYBOARD
#define CLASS_KEYBOARD

#include "config.h"
#include "rust_types.h"

enum class key_state {PRESSED=0, RELEASED=0x40};

static constexpr key_state PRESS             =   key_state::PRESSED;
static constexpr key_state RELEASE           =   key_state::RELEASED;

extern  bool  cir_buff_write(i8 data);
extern  i8    cir_buff_get(usize index);
extern  i32   cir_buff_read(void);

namespace kbd {
  inline  void    push(i8 key_code) { cir_buff_write(key_code); }
  inline  i32     last_key(void)    { return  cir_buff_get(0); }
  inline  i32     get_key(void)     { return  cir_buff_read(); }

  extern  void    test(void);
  extern  void    debounce_init(void);
  extern  void    init(void);
  extern  i32     get_key(key_state state);
  extern  i32     get_key_wait(void);
  extern  void    exclude_before(i32 before_key);
  extern  void    clear_hold_key(void);
  extern  isize   scan(void);
  extern  isize   scan_and_debounced(void);
}

#endif