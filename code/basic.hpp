#ifndef BASIC_COMPILER
#define BASIC_COMPILER

static constexpr int BASIC_MAXIMUM_STRING = 14;

enum class BASIC_WORD {_PRINT=1, _INPUT, _IF, _THEN, _ELSE, _STOP, _BEGIN, _END};


extern bool BASIC_library_select(void);
extern bool CompileBasic(char* program);
extern void InitBasic(void);
extern int  AssignBasic(void);
extern bool BasicIsReady(void);
extern void RunBasic(int BasicN);
extern void EditBasic(void);

#endif