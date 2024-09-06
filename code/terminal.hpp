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

const   u8                  CR = 0x0D;
const   u8                  NL = 0x0A;
static  const   u8          MAX_INPUT_CHAR = 64;

extern  class_disassm_mk61  disassembler;
extern  class_mk61_core     mk61s;
extern  LiquidCrystal       lcd;
extern  void DFU_enable(void);

class class_terminal {
  private:
    const char* ISA_61 = 
"0,1,2,3,4,5,6,7,8,9,.,neg,Bn,Cx,B^,Bx,\
+,-,*,:,<>,E10^,e^,lg,ln,asin,acos,atg,sin,cos,tg,?,\
pi,sqrt,sqr,l/x,x^y,@,o->',?,?,?,o->'\",?,?,?,?,?,\
o<-'\",|x|,sgn,o<-',int,mod,max,and,or,xor,not,rnd,?,0->'\",?,?,\
R0=,R1=,R2=,R3=,R4=,R5=,R6=,R7=,R8=,R9=,RA=,RB=,RC=,RD=,RE=,?,\
hlt,jp,ret,jr,nop,?,?,!=0,L2,>=0,L3,L1,<0,L0,==0,?,\
R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,RA,RB,RC,RD,RE,?,\
!=[0],!=[1],!=[2],!=[3],!=[R4],!=[5],!=[6],!=[7],!=[8],!=[9],!=[A],!=[B],!=[C],!=[D],!=[E],?,\
jp[0],jp[1],jp[2],jp[3],jp[4],jp[5],jp[6],jp[7],jp[8],jp[9],jp[A],jp[B],jp[C],jp[D],jp[E],?,\
>=[0],>=[1],>=[2],>=[3],>=[4],>=[5],>=[6],>=[7],>=[8],>=[9],>=[A],>=[B],>=[C],>=[D],>=[E],?,\
jr[0],jr[1],jr[2],jr[3],jr[4],jr[5],jr[6],jr[7],jr[8],jr[9],jr[A],jr[B],jr[C],jr[D],jr[E],?,\
[0]=,[1]=,[2]=,[3]=,[4]=,[5]=,[6]=,[7]=,[8]=,[9]=,[A]=,[B]=,[C]=,[D]=,[E]=,?,\
<[0],<[1],<[2],<[3],<[4],<[5],<[6],<[7],<[8],<[9],<[A],<0[B],<[C],<[D],<[E],?,\
[0],[1],[2],[3],[4],[5],[6],[7],[8],[9],[A],[B],[C],[D],[E],?,\
==[0],==[1],==[2],==[3],==[R4],==[5],==[6],==[7],==[8],==[9],==[A],==[B],==[C],==[D],==[E],";

    int   AT;
    u8    input_from_serial[MAX_INPUT_CHAR];
    static const u32 T_VERSION        = 0x00726576; // ver
    static const u32 T_LIST           = 0x7473696C; // list
    static const u32 T_ISA_61         = 0x00415349; // ISA
    static const u32 T_ASSEMBLED      = 0x206D7361; // asm_
    static const u32 T_RESET          = 0x00747372; // rst
    static const u32 T_REG_DUMP       = 0x00676572; // reg
    static const u32 T_REG_SET        = 0x203D3052; // R0=
    static const u32 T_SAVE           = 0x65766173; // save
    static const u32 T_LOAD           = 0x64616F6C; // load
    static const u32 T_1302           = 0x32303331; // 1302
    static const u32 T_EXECUTE        = 0x63657865; // exec
    static const u32 T_DISASM         = 0x61736964; // disa
    static const u32 T_DFU_FLASH      = 0x00756664; // dfu
    const u8* END_INPUT_BUFFER = &input_from_serial[MAX_INPUT_CHAR];

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
    i32 get_ISA_61(char*& text) {
      char* compare;
      char* pMnemo = (char*) ISA_61;
      u8 opcode = 0;
      u8 match;

      do {
        compare = text;
        match = 0;
        while(*pMnemo != ',') { 
          match |= (*pMnemo++ ^ *compare++);
          #ifdef DEBUG
            Serial.write(*pMnemo);
            Serial.write(*compare);
          #endif
        }
        #ifdef DEBUG
          Serial.println(match);
        #endif
        if(match == 0 && *compare == ' ') {
          text = compare;
          return opcode;
        } 
        opcode++;
      } while (*++pMnemo != 0);

      return -1;
    }
    void /* __attribute__((optimize("O0"))) */ output_version(void) {
      Serial.print(MODEL);
      Serial.print(" ver. ");
      Serial.print(__DATE__);
      Serial.write('(');
      Serial.print(__TIME__);
      Serial.println(')');
    }
    void DumpRegisters(void) {
      char buffer[14];

      for(int i=0; i < 15; i++) {
        Serial.write('R');
        Serial.print(i, HEX);
        Serial.write('=');
        MK61Emu_ReadRegister(i, buffer);
        Serial.println(buffer);
      }

      Serial.print("IP: "); Serial.println(mk61s.get_IP());
    }
    void Dump1302(void) {
      char buff[43];
      MK61Emu_get_1302_R(&buff[0]);
      Serial.println(buff);
    }
  public:
    u8*   recive_char;

    bool inEOF(void) {
      return recive_char != END_INPUT_BUFFER;
    }
    bool inEOL(void) {
      return *recive_char != CR;
    }
    void debug_key_info(i32 keycode) {
      const TMK61_cross_key cross_key = KeyPairs[keycode];
      Serial.print(cross_key.x); Serial.write(','); Serial.print(cross_key.y); Serial.print(" $");
      //      Serial.print(keycode, HEX);
      Serial_write_hex(keycode); Serial.println(" keycode -> mk61s ");
    }
    void dbg_info(const char* info_text, const char* text) {
      Serial.print(info_text); Serial.println(text);
    }
    void dbg_info(const char* text) {
      Serial.print("dbg_info: "); Serial.println(text);
    }
    void dbg_info(u32 value) {
      Serial.print("dbg_info: "); Serial.println(value);
    }
    void init(void) {
      AT = 0;
      Serial.begin(115200);
      recive_char = &input_from_serial[0];
      output_version();
    }
    void echo_ISA_61(void) {
      char* pMnemo = (char*) ISA_61;
      u8 opcode = 0;

      do {
        Serial_write_hex(opcode); Serial.write(' ');
        while(*pMnemo != ',') { Serial.write(*pMnemo); pMnemo++; }
        Serial.println();
        opcode++;
      } while (*++pMnemo != 0);
    }
   
    i32 serial_input_handler(void) {
      if(*recive_char == CR) {
        recive_char[0] = 0;
        Serial.println((char*) input_from_serial);

        const int nReg = (input_from_serial[1] - '0');
        const u32 token0 = *(u32*) (&input_from_serial[0]);
        const u32 token = (input_from_serial[2] == '=')? token0 & 0xFFFF00FF : token0;
        Serial.println(token, HEX);
        switch (token) {
          case T_VERSION:
              output_version();
            break;
          case T_ISA_61:
              echo_ISA_61();
            break;
          case T_LIST:
              list_mk61_code_page();
            break;
          case T_RESET:
              NVIC_SystemReset();
            break;
          case T_DFU_FLASH:
              DFU_enable();
            break;
          case T_REG_DUMP:
              DumpRegisters();
            break;
          case T_REG_SET:
              MK61Emu_WriteRegister(nReg, (char*) &input_from_serial[4]);
            break;
          case T_1302:
              Dump1302();
            break;
          case T_EXECUTE:
              recive_char = &input_from_serial[0];
            return 25;
          case T_DISASM:  // включить/выключить режим верхней строки с дизассемблером МК61
              config.disassm = disassembler.turn_on_off();
            break;
          case T_SAVE:
              Store();
            break;
          case T_LOAD:
              Load();
            break;
          case T_ASSEMBLED:
              char* ptr_input = (char*) &input_from_serial[4];
              do {
                u8 code = 0;

                if(IsDecimalDigit(*ptr_input)) {
                  do {
                    code *= 10;
                    code += *ptr_input - '0';
                  } while(IsDecimalDigit(*++ptr_input));
                } else {
                  code = get_ISA_61(ptr_input);
                }  

                Serial.print("TA = ");
                Serial.print(AT);
                Serial.print(" : ");
                Serial_writeln_hex(code);
                if(code != 0xFF) {
                  Serial.println(ptr_input);
                  MK61Emu_SetCode(mk61s.get_ring_address(AT++), code);
                }
              } while (*ptr_input++ == ' ');
        }
        recive_char = &input_from_serial[0];
      }
      return -1;
    }
    
};

#endif