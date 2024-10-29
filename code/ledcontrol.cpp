#include "rust_types.h"
#include "config.h"

struct t_time_event {
  t_time_ms time,
            period;
};

static t_time_event event;

namespace led {

void init(void) {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  event.period = 400;
}

void control(void) {
  if(event.time != millis()) return;

// Событие обработки светодиода наступило
  event.time += event.period;
}

void control(t_time_ms now) {
  if(event.time != now) return;

// Событие обработки светодиода наступило
  event.time += event.period;
}

void on(void) {
  digitalWrite(PIN_LED, HIGH);
}

void off(void) {
  digitalWrite(PIN_LED, LOW);
}

}