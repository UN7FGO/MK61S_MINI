#ifndef TERMINAL_CLASS
#define TERMINAL_CLASS
/*
 terminal - модуль работы из терминала с ядром МК61s и калькулятором.
  to do 
    - контроль приемного буфера input_from_serial на переполнение (выход за 64 символа - не контролируется)
    - при декодировании инструкции mk61s (asm) последняя в транслируемой строке должна иметь обязательный пробел в конце
    - заменить таблицу адресов в кольце, функцией вычисления адреса по алгоритму, что позволит снизить объем требуемого ROM
*/
#include "Arduino.h"
#include "EEPROM.h"
#include "config.h"
#include "rust_types.h"
#include "cross_hal.h"
#include "mk61emu_core.h"
#include "disasm.hpp"
#include "tools.hpp"

static const char terminal_symbols[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'L', 'C', '\303', 'E', ' '
};
static constexpr u32 seqNOP = seq(sw::K,sw::_0);
static const u32 key_sequence_on_cmd[15*16] = {
  seq(sw::_0),  seq(sw::_1),  seq(sw::_2),  seq(sw::_3),  seq(sw::_4), seq(sw::_5), seq(sw::_6), seq(sw::_7), seq(sw::_8), seq(sw::_9), seq(sw::DOT), seq(sw::NEG), seq(sw::POW), seq(sw::CX), seq(sw::Bx), seq(sw::F,sw::Bx),
  seq(sw::ADD), seq(sw::SUB), seq(sw::MUL), seq(sw::DIV), seq(sw::XY), seq(sw::F,sw::DOT), seq(sw::F,sw::_5), seq(sw::F,sw::_1), seq(sw::F,sw::_2), seq(sw::F,sw::_4), seq(sw::F,sw::_5), seq(sw::F,sw::_6), seq(sw::F,sw::_7), seq(sw::F,sw::_8), seq(sw::F,sw::_9), seqNOP,
  seq(sw::F,sw::ADD),seq(sw::F,sw::SUB),seq(sw::F,sw::MUL),seq(sw::F,sw::DIV),seq(sw::F,sw::XY),seq(sw::F,sw::DOT),seq(sw::K,sw::ADD),seq(sw::K,sw::SUB),seq(sw::K,sw::MUL),seq(sw::K,sw::DIV),seq(sw::K,sw::XY),seqNOP,seqNOP,seqNOP,seqNOP,seqNOP,
  seq(sw::K,sw::_3),seq(sw::K,sw::_4),seq(sw::K,sw::_5),seq(sw::K,sw::_6),seq(sw::K,sw::_7),seq(sw::K,sw::_8),seq(sw::K,sw::_9),seq(sw::K,sw::DOT),seq(sw::K,sw::NEG),seq(sw::K,sw::POW),seq(sw::K,sw::CX),seq(sw::K,sw::Bx),seqNOP,seqNOP,seqNOP,seqNOP,
  seq(sw::xP,sw::_0),seq(sw::xP,sw::_1),seq(sw::xP,sw::_2),seq(sw::xP,sw::_3),seq(sw::xP,sw::_4),seq(sw::xP,sw::_5),seq(sw::xP,sw::_6),seq(sw::xP,sw::_7),seq(sw::xP,sw::_8),seq(sw::xP,sw::_9),seq(sw::xP,sw::DOT),seq(sw::xP,sw::NEG),seq(sw::xP,sw::POW),seq(sw::xP,sw::CX),seq(sw::xP,sw::Bx),seqNOP,
  seq(sw::RUN),seq(sw::JP),seq(sw::RET),seq(sw::JSR),seqNOP,seq(sw::K,sw::_1),seq(sw::K,sw::_2),seq(sw::F,sw::RUN),seq(sw::F,sw::JP),seq(sw::F,sw::RET),seq(sw::F,sw::JSR),seq(sw::F,sw::xP),seq(sw::F,sw::BK),seq(sw::F,sw::Px),seq(sw::F,sw::FW),seqNOP,
  seq(sw::Px,sw::_0),seq(sw::Px,sw::_1),seq(sw::Px,sw::_2),seq(sw::Px,sw::_3),seq(sw::Px,sw::_4),seq(sw::Px,sw::_5),seq(sw::Px,sw::_6),seq(sw::Px,sw::_7),seq(sw::Px,sw::_8),seq(sw::Px,sw::_9),seq(sw::Px,sw::DOT),seq(sw::Px,sw::NEG),seq(sw::Px,sw::POW),seq(sw::Px,sw::CX),seq(sw::Px,sw::Bx),seqNOP,
  seq(sw::K,sw::RUN,sw::_0),seq(sw::K,sw::RUN,sw::_1),seq(sw::K,sw::RUN,sw::_2),seq(sw::K,sw::RUN,sw::_3),seq(sw::K,sw::RUN,sw::_4),seq(sw::K,sw::RUN,sw::_5),seq(sw::K,sw::RUN,sw::_6),seq(sw::K,sw::RUN,sw::_7),
    seq(sw::K,sw::RUN,sw::_8),seq(sw::K,sw::RUN,sw::_9),seq(sw::K,sw::RUN,sw::DOT),seq(sw::K,sw::RUN,sw::NEG),seq(sw::K,sw::RUN,sw::POW),seq(sw::K,sw::RUN,sw::CX),seq(sw::K,sw::RUN,sw::Bx),seqNOP,
  seq(sw::K,sw::JP,sw::_0),seq(sw::K,sw::JP,sw::_1),seq(sw::K,sw::JP,sw::_2),seq(sw::K,sw::JP,sw::_3),seq(sw::K,sw::JP,sw::_4),seq(sw::K,sw::JP,sw::_5),seq(sw::K,sw::JP,sw::_6),seq(sw::K,sw::JP,sw::_7),
    seq(sw::K,sw::JP,sw::_8),seq(sw::K,sw::JP,sw::_9),seq(sw::K,sw::JP,sw::DOT),seq(sw::K,sw::JP,sw::NEG),seq(sw::K,sw::JP,sw::POW),seq(sw::K,sw::JP,sw::CX),seq(sw::K,sw::JP,sw::Bx),seqNOP,
  seq(sw::K,sw::RET,sw::_0),seq(sw::K,sw::RET,sw::_1),seq(sw::K,sw::RET,sw::_2),seq(sw::K,sw::RET,sw::_3),seq(sw::K,sw::RET,sw::_4),seq(sw::K,sw::RET,sw::_5),seq(sw::K,sw::RET,sw::_6),seq(sw::K,sw::RET,sw::_7),
    seq(sw::K,sw::RET,sw::_8),seq(sw::K,sw::RET,sw::_9),seq(sw::K,sw::RET,sw::DOT),seq(sw::K,sw::RET,sw::NEG),seq(sw::K,sw::RET,sw::POW),seq(sw::K,sw::RET,sw::CX),seq(sw::K,sw::RET,sw::Bx),seqNOP,
  seq(sw::K,sw::JSR,sw::_0),seq(sw::K,sw::JSR,sw::_1),seq(sw::K,sw::JSR,sw::_2),seq(sw::K,sw::JSR,sw::_3),seq(sw::K,sw::JSR,sw::_4),seq(sw::K,sw::JSR,sw::_5),seq(sw::K,sw::JSR,sw::_6),seq(sw::K,sw::JSR,sw::_7),
    seq(sw::K,sw::JSR,sw::_8),seq(sw::K,sw::JSR,sw::_9),seq(sw::K,sw::JSR,sw::DOT),seq(sw::K,sw::JSR,sw::NEG),seq(sw::K,sw::JSR,sw::POW),seq(sw::K,sw::JSR,sw::CX),seq(sw::K,sw::JSR,sw::Bx),seqNOP,
  seq(sw::K,sw::xP,sw::_0),seq(sw::K,sw::xP,sw::_1),seq(sw::K,sw::xP,sw::_2),seq(sw::K,sw::xP,sw::_3),seq(sw::K,sw::xP,sw::_4),seq(sw::K,sw::xP,sw::_5),seq(sw::K,sw::xP,sw::_6),seq(sw::K,sw::xP,sw::_7),
    seq(sw::K,sw::xP,sw::_8),seq(sw::K,sw::xP,sw::_9),seq(sw::K,sw::xP,sw::DOT),seq(sw::K,sw::xP,sw::NEG),seq(sw::K,sw::xP,sw::POW),seq(sw::K,sw::xP,sw::CX),seq(sw::K,sw::xP,sw::Bx),seqNOP,
  seq(sw::K,sw::BK,sw::_0),seq(sw::K,sw::BK,sw::_1),seq(sw::K,sw::BK,sw::_2),seq(sw::K,sw::BK,sw::_3),seq(sw::K,sw::BK,sw::_4),seq(sw::K,sw::BK,sw::_5),seq(sw::K,sw::BK,sw::_6),seq(sw::K,sw::BK,sw::_7),
    seq(sw::K,sw::BK,sw::_8),seq(sw::K,sw::BK,sw::_9),seq(sw::K,sw::BK,sw::DOT),seq(sw::K,sw::BK,sw::NEG),seq(sw::K,sw::BK,sw::POW),seq(sw::K,sw::BK,sw::CX),seq(sw::K,sw::BK,sw::Bx),seqNOP,
  seq(sw::K,sw::Px,sw::_0),seq(sw::K,sw::Px,sw::_1),seq(sw::K,sw::Px,sw::_2),seq(sw::K,sw::Px,sw::_3),seq(sw::K,sw::Px,sw::_4),seq(sw::K,sw::Px,sw::_5),seq(sw::K,sw::Px,sw::_6),seq(sw::K,sw::Px,sw::_7),
    seq(sw::K,sw::Px,sw::_8),seq(sw::K,sw::Px,sw::_9),seq(sw::K,sw::Px,sw::DOT),seq(sw::K,sw::Px,sw::NEG),seq(sw::K,sw::Px,sw::POW),seq(sw::K,sw::Px,sw::CX),seq(sw::K,sw::Px,sw::Bx),seqNOP,
  seq(sw::K,sw::FW,sw::_0),seq(sw::K,sw::FW,sw::_1),seq(sw::K,sw::FW,sw::_2),seq(sw::K,sw::FW,sw::_3),seq(sw::K,sw::FW,sw::_4),seq(sw::K,sw::FW,sw::_5),seq(sw::K,sw::FW,sw::_6),seq(sw::K,sw::FW,sw::_7),
    seq(sw::K,sw::FW,sw::_8),seq(sw::K,sw::FW,sw::_9),seq(sw::K,sw::FW,sw::DOT),seq(sw::K,sw::FW,sw::NEG),seq(sw::K,sw::FW,sw::POW),seq(sw::K,sw::FW,sw::CX),seq(sw::K,sw::FW,sw::Bx),seqNOP
};

const   u8                  CR = 0x0D;
const   u8                  NL = 0x0A;
static  constexpr usize     MAX_INPUT_CHAR = 240;

extern  class_disassm_mk61  disassembler;
extern  LiquidCrystal       lcd;
extern  void DFU_enable(void);

static u8    input_buffer[MAX_INPUT_CHAR];

class class_terminal {
  private:
    enum class mnemo_type {ISA_61, ISA_CLASSIC};
    static constexpr usize MAX_LEN_CLASSIC_MNEMO = 8; // максимальная длинна классической инструкции

    const char ISA_CLASSIC_61[1320] = 
"0,1,2,3,4,5,6,7,8,9,.,/-/,В\317,CX,B^,Bx,+,-,*,:,XY,F10^x,Fe^x,Flg,Fln,Fasin,Facos,Fatg,Fsin,Fcos,Ftg,?,\
\317\xE8,V\"\"\",Fx^2,F1/x,Fx^y,(),Ko->',?,?,?,Ko->'\",?,?,?,?,?,Ko<-'\",|x|,3H,Ko<-',K[x],K{x},Kmax,K^,Kv,K(+),\310HB,C\327,?,Ko->'\",?,?,\
X->\3170,X->\3171,X->\3172,X->\3173,X->\3174,X->\3175,X->\3176,X->\3177,X->\3178,X->\3179,X->\317A,X->\317B,X->\317C,X->\317\304,X->\317E,?,\
C/\317,\301/\317,B/O,\317/\317,HO\317,?,?,Fx\2070,FL2,Fx>=0,FL3,FL1,Fx<0,FL0,Fx=0,?,\
\317->X0,\317->X1,\317->X2,\317->X3,\317->X4,\317->X5,\317->X6,\317->X7,\317->X8,\317->X9,\317->XA,\317->XB,\317->XC,\317->X\xC4,\317->XE,?,\
Kx\2070 0,Kx\2070 1,Kx\2070 2,Kx\2070 3,Kx\2070 4,Kx\2070 5,Kx\2070 6,Kx\2070 7,Kx\2070 8,Kx\2070 9,Kx\2070 A,Kx\2070 B,Kx\2070 C,Kx\2070 \304,Kx\2070 E,?,\
K\301\3170,K\301\3171,K\301\3172,K\301\3173,K\301\3174,K\301\3175,K\301\3176,K\301\3177,K\301\3178,K\301\3179,K\301\317A,K\301\317B,K\301\317C,K\301\317\304,K\301\317E,?,\
Kx>=0 0,Kx>=0 1,Kx>=0 2,Kx>=0 3,Kx>=0 4,Kx>=0 5,Kx>=0 6,Kx>=0 7,Kx>=0 8,Kx>=0 9,Kx>=0 A,Kx>=0 B,Kx>=0 C,Kx>=0 \304,Kx>=0 E,?,\
К\317\3170,К\317\3171,К\317\3172,К\317\3173,К\317\3174,К\317\3175,К\317\3176,К\317\3177,К\317\3178,К\317\3179,К\317\317A,К\317\317B,К\317\317C,К\317\317\304,К\317\317E,?,\
KX->\3170,KX->\3171,KX->\3172,KX->\3173,KX->\3174,KX->\3175,KX->\3176,KX->\3177,KX->\3178,KX->\3179,KX->\317A,KX->\317B,KX->\317C,KX->\317\304,KX->\317E,?,\
Kx<0 0,Kx<0 1,Kx<0 2,Kx<0 3,Kx<0 4,Kx<0 5,Kx<0 6,Kx<0 7,Kx<0 8,Kx<0 9,Kx<0 A,Kx<0 B,Kx<0 C,Kx<0 \304,Kx<0 E,?,\
K\317->X0,K\317->X1,K\317->X2,K\317->X3,K\317->X4,K\317->X5,K\317->X6,K\317->X7,K\317->X8,K\317->XA,K\317->XB,K\317->XC,K\317->X\304,K\317->XE,?,\
Kx=0 0,Kx=0 1,Kx=0 2,Kx=0 3,Kx=0 4,Kx=0 5,Kx=0 6,Kx=0 7,Kx=0 8,Kx=0 9,Kx=0 A,Kx=0 B,Kx=0 C,Kx=0 \304,Kx=0 E,?";

    const char ISA_61[1259] = 
"0,1,2,3,4,5,6,7,8,9,dot,neg,pow10,clr,push,preX,\
add,sub,mul,div,swap,e10,exp,lg,ln,asin,acos,atg,sin,cos,tg,?,\
pi,sqrt,sqr,rec,pow,rot,toM,?,?,?,toMS,?,?,?,?,?,\
inMS,mod,sgn,inM,int,frc,max,and,or,xor,not,rnd,?,?,?,?,\
sto0,sto1,sto2,sto3,sto4,sto5,sto6,sto7,sto8,sto9,stoA,stoB,stoC,stoD,stoE,?,\
hlt,jmp,ret,call,nop,?,?,jz,rpt2,jl,rpt3,rpt1,jme,rpt0,jnz,?,\
ld0,ld1,ld2,ld3,ld4,ld5,ld6,ld7,ld8,ld9,ldA,ldB,ldC,ldD,ldE,?,\
jz[0],jz[1],jz[2],jz[3],jz[4],jz[5],jz[6],jz[7],jz[8],jz[9],jz[A],jz[B],jz[C],jz[D],jz[E],?,\
jmp[0],jmp[1],jmp[2],jmp[3],jmp[4],jmp[5],jmp[6],jmp[7],jmp[8],jmp[9],jmp[A],jmp[B],jmp[C],jmp[D],jmp[E],?,\
jlz[0],jlz[1],jlz[2],jlz[3],jlz[4],jlz[5],jlz[6],jlz[7],jlz[8],jlz[9],jlz[A],jlz[B],jlz[C],jlz[D],jlz[E],?,\
call[0],call[1],call[2],call[3],call[4],call[5],call[6],call[7],call[8],call[9],call[A],call[B],call[C],call[D],call[E],?,\
sto[0],sto[1],sto[2],sto[3],sto[4],sto[5],sto[6],sto[7],sto[8],sto[9],sto[A],sto[B],sto[C],sto[D],sto[E],?,\
jme[0],jme[1],jme[2],jme[3],jme[4],jme[5],jme[6],jme[7],jme[8],jme[9],jme[A],jme[B],jme[C],jme[D],jme[E],?,\
ld[0],ld[1],ld[2],ld[3],ld[4],ld[5],ld[6],ld[7],ld[8],ld[9],ld[A],ld[B],ld[C],ld[D],ld[E],?,\
jnz[0],jnz[1],jnz[2],jnz[3],jnz[R4],jnz[5],jnz[6],jnz[7],jnz[8],jnz[9],jnz[A],jnz[B],jnz[C],jnz[D],jnz[E]";

    isize   AT;
    u32     terminal_last_cmd;
    isize   nSlot;

    static constexpr u32 T_VERSION      = 0x00726576; // ver
    static constexpr u32 T_LIST         = 0x7473696C; // list
    static constexpr u32 T_ISA_61       = 0x00617369; // isa
    static constexpr u32 T_ASSEMBLED    = 0x206D7361; // asm_
    static constexpr u32 T_RESET        = 0x00747372; // rst
    static constexpr u32 T_REG_DUMP     = 0x00676572; // reg
    static constexpr u32 T_REG_SET      = 0x203D3052; // R0=
    static constexpr u32 T_SAVE         = 0x65766173; // save
    static constexpr u32 T_LOAD         = 0x64616F6C; // load
    static constexpr u32 T_1302         = 0x32303331; // 1302
    static constexpr u32 T_EXECUTE      = 0x63657865; // exec
    static constexpr u32 T_DISASM       = 0x61736964; // disa
    static constexpr u32 T_HEX_INPUT    = 0x206E6968; // hin_
    static constexpr u32 T_HEX_OUTPUT   = 0x74756F68; // hout
    static constexpr u32 T_SET_CODE     = 0x24746573; // set$
    static constexpr u32 T_POKE_CODE    = 0x656B6F70; // poke
    static constexpr u32 T_DFU_FLASH    = 0x00756664; // dfu
    static constexpr u32 T_DUMP         = 0x706D7564; // dump
    static constexpr u32 T_LASM         = 0x6D73616C; // lasm
    static constexpr u32 T_PUB_LIST     = 0x00627570; // pub
    static constexpr u32 T_MAP_FLASH    = 0x70616D73; // smap
    static constexpr u32 T_STACK_OUTPUT = 0x006B7473; // stk
    static constexpr u32 T_KBD_EMULATED = 0x2064626B; // kbd_
    static constexpr u32 T_CMD_EMULATED = 0x20646D63; // cmd_
    static constexpr u32 T_CLEAR_PRG61  = 0x00726C63; // clr
    static constexpr u32 T_RING_DUMP    = 0x676E6972; // ring
    static constexpr u32 T_RENAME       = 0x206D6E73; // snm_ (rename slot name[16])
    static constexpr u32 T_DIR          = 0x72696473; // sdir
    static constexpr u32 T_DEL_SLOT     = 0x6C656473; // sdel
    static constexpr u32 T_RUN          = 0x006E7572; // run (F АВТ, В/О, С/П)
    static constexpr u32 T_ERASE_STORAGE= 0x61726573; // sera

    bool  not_EOF(void) {
      return recive_pos < MAX_INPUT_CHAR;
    }

    bool  not_CR(void) { 
      return input_buffer[recive_pos] != CR;
    }

    void list_mk61_code_page(void) {
      u8 code_page[106];
      MK61Emu_GetCodePage(&code_page[0]);

      for(int j = 0; j < 16; j++) {
        for(int i = j; i < 105; i += 16) {
          if(i > 99) {
            Serial.write('A');
            Serial.print(i-100);
          } else {
            if(i < 10) Serial.write('0');
            Serial.print(i);
          }
          Serial.print(". ");
          if(code_page[i] < 16) Serial.write('0');
          Serial.print(code_page[i], HEX); 
          Serial.print("  ");
        }
        Serial.println();
      }
    }

    void dump_mk61_code_page(void) {
      u8 code_page[106];
      MK61Emu_GetCodePage(&code_page[0]);
		for(isize i = 0; i < 105; i++) {
		  Serial.print(i%16==8 ? " | " : " ");
		  Serial_write_hex(code_page[i]);
		  if (i%16 == 15) Serial.println();
		}
        Serial.println();
    }

    char* ISA_61_code(u8 opcode, char* text) {
      isize comma_count = opcode;
      isize i = 0;

      for(u8 symbol : ISA_61) {
        if(symbol == 0) break;
        if(symbol == ',') {
          comma_count--;
        } else if(comma_count == 0)
          text[i++] = symbol;
      }
      text[i] = 0;
      return text;
    }

    char* ISA_CLASSIC_61_code(u8 opcode, char* text) {
      isize comma_count = opcode;
      isize i = 0;

      for(u8 symbol : ISA_CLASSIC_61) {
        if(symbol == 0) break;
        if(symbol == ',') {
          comma_count--;
        } else if(comma_count == 0)
          text[i++] = symbol;
      }
      text[i] = 0;
      return text;
    }

    i32 get_ISA_61(char* &text) {
      char* compare = text;
      u8 match = 0;
      u8 opcode = 0;
      dbgln(PARSE, (char*) compare);

      for(char ISA_token_char : ISA_61) {

        if(ISA_token_char == ',') { // токен из списка команд выбран до завершения ','
          if(match != 0) { // сравнение было прервано ранее
            dbgln(PARSE, "Partial compare, next...! opcode = ", opcode);
            compare = text; match = 0; // начальные установкии для нового поиска
            opcode++; // начнем сравнениие с следующим токеном из списка команд
          } else { // сравнение было удачным 
            if(*compare == ' ' || *compare == 0) { // при этом поисковый токен выбран до ' ' или 0
              dbgln(PARSE, "Success compare! opcode = ", opcode);
              text = compare;
              return opcode;
            } else { // поисковый токен не завершен, а командный завершен
              dbgln(PARSE, "Partial compare, next...! opcode = ", opcode);
              compare = text; match = 0; // начальные установкии для нового поиска
              opcode++; // начнем сравнениие с следующим токеном из списка команд
            }
          }
        } else {
          match |= (ISA_token_char ^ *compare++);  // проверяем символ очередного токена из списка команд и очередного символа искомого токена
        }
      }

      dbgln(PARSE, "Fail compare!", opcode);
      return -1;
    }

    void /* __attribute__((optimize("O0"))) */ output_version(void) {
      Serial.print("sizeof Serial "); Serial.println(sizeof(HardwareSerial));
	  Serial.print(MODEL " ver. " __DATE__ "(" __TIME__ ")\n");
    }

    void DumpRegisters(void) {
     // -0.12345678 -9A (14)
      char buffer[15];

      for(int i=0; i < 15; i++) {
        Serial.write('R');
        Serial.print(i, HEX);
        Serial.print(" = ");
        MK61Emu_ReadRegister(i, buffer, terminal_symbols);
        //Serial.write((char*) &buffer[0], 3); //Serial.write(buffer[1]); Serial.write('.'); 
        Serial.println((char*) &buffer);
      }

      Serial.print("IP: "); Serial.println(core_61::get_IP());
    }

    void Dump1302(void) {
      char buff[43];
      MK61Emu_get_1302_R(&buff[0]);
      Serial.println(buff);
    }

    u8    len_code_command(u8 cod) {
      static const u8 values[10] = {81,83,87,88,89,90,91,92,93,94};
      for (int i = 0; i<10; i++) {
        if (cod == values[i] ) { return 2; }
      }
      return 1;
    }

    void  print_address_as_MK61(usize addr) {
      if(addr > 99) {
        Serial.write('A');
        Serial.print(addr-100);
      } else {
        if(addr < 10) Serial.write('0');
        Serial.print(addr);
      }
    }

  public:
    usize recive_pos;

    void  init(void) {
      AT                    = 0;
      recive_pos            = 0;
      terminal_last_cmd     = 0;
      nSlot                 = -1;
      Serial.begin(115200);
      delay(1800);
      output_version();
    }

    void  echo_mk61_stack(void) {
      char cvalue[15];
      cvalue[14] = 0;

      Serial.print("X1 = "); Serial.println(read_stack_register(stack::X1, cvalue, terminal_symbols)); 
      Serial.print("T  = "); Serial.println(read_stack_register(stack::T, cvalue, terminal_symbols)); 
      Serial.print("Z  = "); Serial.println(read_stack_register(stack::Z, cvalue, terminal_symbols)); 
      Serial.print("Y  = "); Serial.println(read_stack_register(stack::Y, cvalue, terminal_symbols)); 
      Serial.print("X  = "); Serial.println(read_stack_register(stack::X, cvalue, terminal_symbols));
      Serial.print("IP =  "); Serial.println(core_61::get_IP());
    }

    void  echo_ISA_61(void) {
      static constexpr usize COLUMN_SIZE = 10;
      char* pMnemo = (char*) ISA_61;
      u8 opcode = 0;
      isize column = 4;

      do {
        Serial_write_hex(opcode); Serial.write(' ');
        const char* pMnemo_backup = pMnemo;
        while(*pMnemo != ',') { Serial.write(*pMnemo); pMnemo++; }
        if(column-- <= 0) {
          column = 4;
          Serial.println();
        } else {
          const usize len = pMnemo - pMnemo_backup;
          for(usize i=0; i < COLUMN_SIZE - len; i++) Serial.write(' ');
        }
        opcode++;
      } while (*++pMnemo != 0);
      Serial.println();
    }

    void pub_mk61_code_page(void) {
      char op[MAX_LEN_CLASSIC_MNEMO+1];
      u8 code_page[106];
      MK61Emu_GetCodePage(&code_page[0]);
      isize last_cmd_addr = seek_program_END(&code_page[0]);
      isize address = 0;     
      for (int i=0; i<30; i++) {
        if(i > last_cmd_addr) break;
        for (int ii=0; ii<4; ii++) {
          address = ii*30 + i;
          if(address > last_cmd_addr) break;
          if (address < 105) {
            print_address_as_MK61(address); Serial.print(". ");  

            const u8 code = code_page[address];
            if(address > 0 && len_code_command(code_page[address-1]) == 2) {
              Serial_write_hex(code);
			  for(usize cnt_space=2; cnt_space < MAX_LEN_CLASSIC_MNEMO + 2; cnt_space++) Serial.write(' ');
            } else {
              Serial.print(ISA_CLASSIC_61_code(code, &op[0])); 
              for(usize ln=strlen(op); ln < MAX_LEN_CLASSIC_MNEMO; ln++) Serial.write(' ');
              Serial.print("  ");
            }
          }
        }
        Serial.println(); 
      }
    }

    void  lasm_mk61_code_page(mnemo_type type) {
      char op[7+1];
      u8 code_page[106];

      MK61Emu_GetCodePage(&code_page[0]);
      isize lastCommand = seek_program_END(&code_page[0]);
      isize j = 0;     
      for (isize i=0; i<30; i++) {
        for (isize ii=0; ii<4; ii++) {
          j = ii*30 + i;
          if (lastCommand > j) {
            Serial.print("00");
            print_address_as_MK61(j);
            const u8 code = code_page[j];
            const u8 code2 = code_page[j+1];
            Serial.write(' '); Serial_write_hex(code); Serial.write(' ');
            if (j > 0){
              if (len_code_command(code_page[j-1]) == 2) {
                Serial.print("      ");
              } else {
                const char* mnemo = (type == mnemo_type::ISA_CLASSIC)? ISA_CLASSIC_61_code(code, &op[0]) : ISA_61_code(code, &op[0]);
                Serial.print(mnemo); 
                for(usize ln=strlen(op); ln < 6; ln++) Serial.write(' '); 
              }
            } else {
              const char* mnemo = (type == mnemo_type::ISA_CLASSIC)? ISA_CLASSIC_61_code(code, &op[0]) : ISA_61_code(code, &op[0]);
              Serial.print(mnemo); 
              for(usize ln=strlen(op); ln < 6; ln++) Serial.write(' '); 
            }            
            if ( len_code_command(code) == 2 ) {
              Serial_write_hex(code2); 
              Serial.print("  ");
            } else {
              Serial.print("    ");
            }
            Serial.print("  ");
          }
         }
         if (i > lastCommand) { break; }
         Serial.println();
      }
    }

    isize parse_addr(u8* stream) {
      isize addr = 0;
      for(usize i=0; i < 4; i++) {
        const u8 digit = stream[i] - '0';
        if(digit > 9) return -1;
        addr = addr*10 + digit;
      }
      Serial.print("parsing address "); Serial.println(addr);
      return addr;
    }

    void  GetHexString(void) {
        usize i = 4 + 4 + 1;
        isize linear_addr = parse_addr(&input_buffer[4]);

        Serial.println();
          while(i < MAX_INPUT_CHAR) {

            if(linear_addr < 0 || linear_addr > 105) {
              ErrorReaction();
              Serial.println(); Serial.println("BAD address!");
              return;
            }

            const u8 hi_char = input_buffer[i++];
            if(hi_char == 0) {
              Serial.println("\n\rStream recived!");
              break;
            }
            const isize hi_digit = HexdecimalDigit(hi_char);

            if(hi_digit < 0) {
              Serial.print("Input from "); Serial.print(i); Serial.print(" recive non hexdecimal hi digit '"); Serial.write(hi_char); Serial.print("'$"); Serial.print(hi_char, HEX); Serial.println(") process halted!"); 
              break;
            } 
                
            const u8 lo_char = input_buffer[i++];
            const isize lo_digit = HexdecimalDigit(lo_char);
            
            if( lo_digit < 0 ) {
              Serial.print("Input from "); Serial.print(i); Serial.print(" recive non hexdecimal lo digit '"); Serial.write(hi_char); Serial.print("'$"); Serial.print(hi_char, HEX); Serial.println(") process halted!"); 
              break;
            } 
                
            const u8 byte_code = (hi_digit << 4) | lo_digit;
			Serial.print(byte_code, HEX); Serial.write( input_buffer[i] ? ',' : ';');
            MK61Emu_SetCode(core_61::get_ring_address(linear_addr++), byte_code);
          }
    }

    void  PutHexString(void) {
      u8 code_page[106];
      MK61Emu_GetCodePage(&code_page[0]);
      isize last_cmd_addr = seek_program_END(&code_page[0]);
      isize j = 0;
      while (j < last_cmd_addr) {
        Serial.print("00"); print_address_as_MK61(j); Serial.write(' ');
        isize jk = 0;
        while (jk < 24) {
          Serial_write_hex(code_page[j]);
          j++; jk++;
          if (j >= last_cmd_addr) break;
        }
        Serial.println();
      }
    }

    void  Assembler(void) {
              char* ptr_input = (char*) &input_buffer[4];
              const u32 newTA = *((u32*) &input_buffer[4]);
              const u32 word  = newTA - 0x30303030;
              
              dbghexln(PARSE, "newTA as u32 ", newTA);
              dbghexln(PARSE, "digit0 ", word & 0x000000FF, " digit1 ", word & 0x0000FF00, " digit2 ", word & 0x00FF0000, " digit3 ", word & 0xFF000000);

              Serial.print("Assembled to mk61 ");

              if((word & 0x000000FF) <= 0x09 && (word & 0x0000FF00) <= 0x00000900 && (word & 0x00FF0000) <= 0x00090000 && (word & 0xFF000000) <= 0x09000000) {
                AT = parse_addr((u8*) ptr_input);
                //0 1 2 3 4 5 6 7 8 9
                //a s m _ 0 0 0 0 _ ?
                ptr_input += 5;
                Serial.print(" from new TA ("); Serial.print(AT); Serial.println(") ");
              }

              do {
                Serial.print("Parsing: "); Serial.println(ptr_input);
                const isize code = get_ISA_61(ptr_input);

                if(code == -1) {
                  Serial.print("Unexpected token: "); Serial.println(ptr_input);
                  return;
                }

                Serial.print("TA = "); Serial.print(AT); Serial.print(" : "); Serial_writeln_hex((u8) code);
                MK61Emu_SetCode(core_61::get_ring_address(AT++), (u8) code);
              } while (*ptr_input++ == ' ');
    }

    void  flash_map_list(void) {
      Serial.print("     0  1  2  3  4  5  6  7  8  9\r\n0 - ");
      usize slot = 0;
      do { // пробежим все слоты от 0 до 99
        if(load_word(slot * FLASH_SECTOR_SIZE, OFFSET_FLAG_OCCUPIED) == SLOT_OCCUPIED) 
          Serial.print("[X]");
        else
          Serial.print("[ ]");

        if(++slot == 100) break;

        if((slot % 10) == 0) {
          Serial.println();
          Serial.print(slot / 10);
          Serial.print(" - ");
        }
      } while(true);
      Serial.println();
    }

    isize parse_dec_numeric(char *buff) {
      usize dec = 0;
      while(*buff != 0 && *buff != ' ' && *buff != CR) {
        const isize digit = DecimalDigit(*buff++);
        if(digit < 0) {
          Serial.print((char) buff[-1]); Serial.println(" - non decimal digit!");
          return -1;
        }
        dec = dec*10 + digit;
      }
      return dec;
    }

    isize parse_hex_numeric(char* buff) {
      usize hex = 0;

      while(*buff != 0 && *buff != ' ' && *buff != CR) {
        const isize digit = HexdecimalDigit(*buff++);
        if(digit < 0) {
          Serial.print((char) buff[-1]); Serial.println(" - non hexdecimal digit!");
          return -1;
        }
        hex = hex*16 + digit;
      }

      return hex;
    }

    i32  command_to_kbd(void) {
      const usize code_61 = parse_hex_numeric((char*) &input_buffer[4]);
      usize sequence = key_sequence_on_cmd[code_61];
      dbgln(MK61E, "mk61 <- ", (u8) code_61);
      dbghexln(MK61E, "mk61 <- $", (i32) sequence);

      for(isize i=0; i<4; i++) {
        const i8 scan_code = sequence & 0xFF;
        if(scan_code < 0) break;

        kbd::push(scan_code);
        sequence >>= 8;
      }
      return -1;//key_sequence_on_cmd[code_61];
    }

    i32  scancode_to_kbd(void) { // simulate mk61 scancode from keyboard buffer
      const usize key_code = parse_hex_numeric((char*) &input_buffer[4]);//high * 16 + low;//(input_buffer[4] - '0')*10 + (input_buffer[5] - '0');
      dbghexln(MK61E, "mk61 <- ", key_code, "H");
      if(key_code > 40) {
        Serial.println("Unsuccessful scan code! < 28H");
        return -1;
      }
      return key_code;
    }

    void  input_R_stack(void) { // value format poke X 1.25e02
      char buffer[14]; // -12345678-12
      memset(&buffer[0], 0, sizeof(buffer));

      const char R_name = input_buffer[5];
      const char sign   = input_buffer[7];
      u8* p_mantissa = (sign == '-')? &input_buffer[8] : &input_buffer[7];

      isize mpow = 0;
      isize pos = 0;
      char* p_buff = &buffer[0];
      for(pos=0; pos < 9; pos++) {
        const char digit = p_mantissa[pos];
        if(digit == 'e') break;
        if(digit == '.') {
          mpow = pos;
          continue;
        }

        if(!IsDecimalDigit(digit)) {
          ErrorReaction();
          Serial.print("\n\rNon decimal digit _"); Serial.write(digit); Serial.println("_ !'");
          return;
        }

        *p_buff++ = digit;
        Serial_write_hex(digit); Serial.print("h "); Serial.write(digit); Serial.write(',');
      }
      if(pos < 1 || pos > 9 /*1.2345678e*/) {
        ErrorReaction();
        Serial.println("Abnormal mantissa size!");
        return;
      }
      p_mantissa += (pos + 1);
      while(pos++ <= 8) *p_buff++ = '0';

      Serial.print("\n\rvalue '"); Serial.print(buffer); Serial.print("' mpow "); Serial.println(mpow);

      const char sign_pow = *p_mantissa;
      if(sign_pow == '-') p_mantissa++;

      isize pow = 0;
      while(*p_mantissa != 0) {
        const usize decimal_digit = *p_mantissa - '0';

        if(decimal_digit > 10) {
          ErrorReaction();
          Serial.print("\n\rNon decimal digit _"); Serial_write_hex(decimal_digit); Serial.println("_ !'");
          return;
        }

        pow = pow*10 + decimal_digit;
        Serial_write_hex(*p_mantissa); Serial.print("h "); Serial.write((usize) *p_mantissa); Serial.write(',');
        p_mantissa++;
      }
      if(sign_pow == '-') pow = -pow;
      pow += (mpow - 1);
      if(pow > 99) {
        ErrorReaction();
        Serial.print("\n\rPower of vale is big > 99!");
      }
      Serial.print("\n\rvalue "); Serial.print(buffer); Serial.print(" pow "); Serial.println(pow);

      switch(R_name) {
        case 'x':
        case 'X':
            write_stack_register(stack::X, sign, buffer, pow);
          break;
        case 'y':
        case 'Y':
            write_stack_register(stack::Y, sign, buffer, pow);
          break;
        case 'z':
        case 'Z':
            write_stack_register(stack::Z, sign, buffer, pow);
          break;
        case 't':
        case 'T':
            write_stack_register(stack::T, sign, buffer, pow);
          break;
        default:
          ErrorReaction();
          Serial.print("Illegal stack register name '"); Serial.print(R_name); Serial.println("' available X,x,Y,y,Z,z,T,t.");
      }
    }

    isize execute(void) {
        if(input_buffer[0] == 'y' || input_buffer[0] == 'Y') {
          switch(terminal_last_cmd) {
            case  T_CLEAR_PRG61:
                MK61Emu_ClearCodePage();
                Serial.println("Code page cleared!");
              break;
            case  T_DEL_SLOT:
                if(!erase_slot(nSlot)) Serial.println("Error: address out of range!");
              break;
            case  T_SAVE:
                if(!Store(nSlot)) Serial.println("Failed save attempt!");
              break;
            case  T_ERASE_STORAGE:
                clear_storage();
              break;
          }
        }

        input_buffer[--recive_pos] = 0;
        Serial.println();
        dbgln(MINI,"[", recive_pos, "] '", (char*) input_buffer);

        const int nReg = (input_buffer[1] - '0');
        const u32 token0 = *(u32*) (&input_buffer[0]);
        const u32 token = (input_buffer[2] == '=')? token0 & 0xFFFF00FF : token0;
        terminal_last_cmd = token;
        dbghexln(MINI, token);
        switch (token) {
          case  T_VERSION:
              output_version();
            break;
          case  T_ISA_61:
              echo_ISA_61();
            break;
          case  T_LIST:
              list_mk61_code_page();
            break;
          case  T_RESET:
              if(Confirmation()) NVIC_SystemReset();
            break;
          case  T_DFU_FLASH:
              DFU_enable();
            break;
          case  T_REG_DUMP:
              DumpRegisters();
            break;
          case  T_REG_SET:
              MK61Emu_WriteRegister(nReg, (char*) &input_buffer[4]);
            break;
          case  T_1302:
              Dump1302();
            break;
          case  T_CLEAR_PRG61:
          case  T_ERASE_STORAGE:
              Serial.println("Enter Y/y to confirm the operation!");
            break;
          case  T_CMD_EMULATED: 
              recive_pos = 0;
            return command_to_kbd();
          case  T_KBD_EMULATED: {
              recive_pos = 0;
              const i32 scancode = scancode_to_kbd();
              if(scancode < 0) break;
              return scancode;
            }
          case  T_DISASM:  // включить/выключить режим верхней строки с дизассемблером МК61
              config.disassm = disassembler.turn_on_off();
            break;
          case  T_SAVE: 
              nSlot = parse_dec_numeric((char*) &input_buffer[5]);
              Serial.println("Enter Y/y to confirm the operation!");
            break;
          case  T_LOAD: {
              const isize nSlot = parse_dec_numeric((char*) &input_buffer[5]);
              if(!Load(nSlot)) Serial.println("Failed load attempt!");
            }
            break;
          case  T_PUB_LIST:
              pub_mk61_code_page();
            break;
          case  T_DUMP:
              dump_mk61_code_page();
            break;
          case  T_RING_DUMP: {
              usize i = 0;
              usize n_chip = 0;
              usize count = 0;
              Serial.print("MK61 ring M dump:");
              for(u8 ring_cell : ringM) {
                if(i == 0) {
                  i = 42;
                  Serial.println(); 
                  if(ring_M::CHIP[n_chip].OFFSET == count) Serial.println(ring_M::CHIP[n_chip++].NAME);

                  if(count < 100) Serial.write('0');
                  if(count < 10) Serial.write('0');
                  Serial.print(count); Serial.write(' ');
                }
                Serial.write((char) (ring_cell < 10)? ring_cell + '0' : ring_cell - 10 + 'A');
                count++;
                i--;
              }
            }
            Serial.println();
            break;
          case  T_LASM:
              lasm_mk61_code_page(mnemo_type::ISA_61);
            break;
          case  T_STACK_OUTPUT:
              echo_mk61_stack();
            break;
          case  T_HEX_OUTPUT:
              PutHexString();
            break;
          case  T_MAP_FLASH:
              flash_map_list();
            break;
          case  T_RUN: 
              kbd::push(38); // F
              kbd::push(10); // /-/
              kbd::push(31); // В/О
              kbd::push(30); // C/П
            break;
          case  T_POKE_CODE:
              input_R_stack();
            break;
          case  T_DIR: {
              char slot_name[17];
              for(usize i=0; i < 100; i++) {
                if(IsOcupped(i)) {
                  Serial.print(i); Serial.print(". "); Serial.println(ReadSlotName(i, (char*) &slot_name[0]));
                }
              }
            }
            break;
          case  T_DEL_SLOT: {
              if(!flash_is_ok) {
                Serial.println("Error: spiflash chip is not installed!");
                break;
              }
              nSlot = parse_dec_numeric((char*) &input_buffer[5]);
              Serial.print("\n\rClear slot #"); Serial.println(nSlot);
              if(nSlot < 0 || nSlot > 99) {
                 Serial.println("Error: is out of range 0..99 !");
                break;
              }
              if(!IsOcupped(nSlot)) {
                Serial.println("Warning: slot is already empty.");
                break;
              }
              Serial.println("Enter Y/y to confirm the operation!");
            }
            break;
          case  T_RENAME: {
              usize nSlot = 0;
              usize pos = 4;
              while(pos < 6) {
                const char symbol = input_buffer[pos];
                if(symbol == ' ') break;

                const usize dec_digit = symbol - '0';
                if(dec_digit >= 10) {
                  Serial.print("Invalid digit in slot number => '"); Serial.print(dec_digit); Serial.println("'"); 
                  ErrorReaction();
                  break;
                }
                nSlot = nSlot*10 + dec_digit;
                pos++;
              }
              if(input_buffer[pos++] != ' ') {
                Serial.println("Slot number > 99"); 
                ErrorReaction();
                break;
              }
              
              Serial.print("Rename slot N"); Serial.print(nSlot); Serial.print(" to "); Serial.println((char*) &input_buffer[pos]);
              if(!Rename(nSlot,(char*) &input_buffer[pos])) {
                  Serial.println("Check slot number (<100) or name of slot (16 char)!");
                  ErrorReaction();
              }
            }
            break;
          case  T_HEX_INPUT:
          case  T_SET_CODE:
              GetHexString();
            break;
          case  T_ASSEMBLED:
              Assembler();
        }

      recive_pos = 0;
      return -1;
    }

    i32 serial_input_handler() {
      //t_time_ms wait = millis();
      //do {
      while(Serial.peek() >= 0) { // получен символ
        const u8 rx_char = Serial.read(); // уберем с буфера
        Serial.write(rx_char); // эхо

        if(not_EOF()) 
          input_buffer[recive_pos++] = rx_char;
        else
          sound(PIN_BUZZER, 4000, 750);

        if(rx_char == CR) return execute();
        //wait += 2000;
      }
      //} while(wait > millis());
      return -1;
    }
    
};

#endif