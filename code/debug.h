#ifndef DEBUG_OUTPUT_TO_SERIAL
#define DEBUG_OUTPUT_TO_SERIAL

#include "rust_types.h"
#include "config.h"

#define dbg(MODULE, ...) if constexpr (DBG_##MODULE) dbg::print(__VA_ARGS__) ;
#define dbgln(MODULE, ...) if constexpr (DBG_##MODULE) dbg::println(__VA_ARGS__) ;
#define dbghex(MODULE, ...) if constexpr (DBG_##MODULE) dbg::printhex(__VA_ARGS__) ;
#define dbghexln(MODULE, ...) if constexpr (DBG_##MODULE) dbg::printhexln(__VA_ARGS__) ;

namespace dbg {
  extern void print(const char* text);
  extern void println(const char* text);
  extern void printhex(const char* text, const isize var);
  extern void printhex(const isize var, const char symbol);
  extern void printhex(const isize var, const char* text);
  extern void printhex(const char symbol_0, const isize var, const char symbol_1);
  extern void printhexln(const usize var);
  extern void printhexln(const char* text, const isize var, const char symbol);
  extern void printhexln(const char* text, const isize var);
  extern void printhexln(const char* text_0, const isize var, const char* text_1);
  extern void println(const char* text, const isize var);
  extern void println(const char* text_0, const char* text_1);
  extern void println(const char* text_0, const isize var, const char* text_1);
  extern void println(const char* text_0, const char* text_1, const char* text_2);
  extern void print(const char* text_0, const isize var_0, const char* text_1, const isize var_1);
  extern void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1);
  extern void println(const char* text_0, const isize var_0, const char* text_1, const char* text_2);
  extern void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1, const char* text_2, const isize var_2);
  extern void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1, const char* text_2, const isize var_2, const char* text_3, const isize var_3);
}

#endif