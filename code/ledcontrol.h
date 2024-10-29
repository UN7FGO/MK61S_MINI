#ifndef LED_CONTROL_TOOLS
#define LED_CONTROL_TOOLS

#include "rust_types.h"

namespace led {
  extern void init(void);
  extern void on(void);
  extern void off(void);
  extern void control(void);
  extern void control(t_time_ms now);
}

#endif