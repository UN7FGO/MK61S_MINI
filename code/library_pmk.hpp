#ifndef LIBRARY_PMK
#define LIBRARY_PMK

#include "keyboard.h"
#include "rust_types.h"

static  constexpr usize    COUNT_PROGRAMS = 6;
static  constexpr usize    COUNT_GAMES    = 5;
struct  TPunct {
  char  text[16];
  int   offset;
};

int  select_program(void);
int  select_game(void);
void load_program(usize nProg_for_load);
void load_game(usize nGame_for_load);
void init_library(void);

#endif