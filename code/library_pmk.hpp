#ifndef LIBRARY_PMK
#define LIBRARY_PMK

#include "keyboard.hpp"

#include "rust_types.h"
static  const u8    CountPrograms = 9;
struct  TPunct {
  char  text[16];
  int   offset;
};

int  select_program(class_keyboard keyboard);
void load_program(int nProg_for_load);
void init_library(void);

#endif