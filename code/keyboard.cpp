#include "keyboard.hpp"

class_keyboard keyboard;

isize  class_keyboard::scan_and_debounced(void) {
  const u32 now = millis();
  if (now < time_switch_scan_line) return -1;

  // переключение следующей сканлинии клавиатуры
  time_switch_scan_line = now + TIME_SCAN_LINE;   // время следующего переключения сканлинии клавиатуры (сканстолбца)
  return scan();
}
