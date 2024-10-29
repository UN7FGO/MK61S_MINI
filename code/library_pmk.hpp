#ifndef LIBRARY_PMK
#define LIBRARY_PMK

#include "keyboard.hpp"
#include "rust_types.h"

static  constexpr usize    COUNT_PROGRAMS = 6;
static  constexpr usize    COUNT_GAMES    = 5;
struct  TPunct {
  char  text[16];
  int   offset;
};

int  select_program(class_keyboard keyboard);
int  select_game(class_keyboard keyboard);
void load_program(usize nProg_for_load);
void load_game(usize nGame_for_load);
void init_library(void);

#endif