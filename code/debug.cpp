#include "rust_types.h"
#include "tools.hpp"
#include "config.h"

namespace dbg {

void print(const char* text) {
    Serial.print(text);
}

void println(const char* text) {
    Serial.println(text);
}

void printhex(const isize var, const char* text) {
    if(var < 0x10) Serial.write('0'); 
    Serial.print(var, HEX); 
    Serial.print(text); 
}

void printhex(const isize var, const char symbol) {
    if(var < 0x10) Serial.write('0'); 
    Serial.print(var, HEX); 
    Serial.write(symbol); 
}

void printhex(const char symbol_0, const isize var, const char symbol_1) {
    Serial.write(symbol_0); 
    if(var < 0x10) Serial.write('0'); 
    Serial.print(var, HEX); 
    Serial.write(symbol_1); 
}

void printhex(const char* text, const isize var, const char symbol) {
    Serial.write(text); 
    if(var < 0x10) Serial.write('0'); 
    Serial.print(var, HEX); 
    Serial.write(symbol); 
    Serial.println();
}

void printhex(const char* text, const isize var) {
    Serial.print(text); 
    if(var < 0x10) Serial.write('0');
    Serial.print(var, HEX);
}

void printhexln(const usize var) {
    if(var < 0x10) Serial.write('0');
    Serial.println(var, HEX);
}

void printhexln(const char* text, const isize var) {
    Serial.print(text); if(var < 0x10) Serial.write('0');
    Serial.println(var, HEX);
}

void printhexln(const char* text_0, const isize var, const char* text_1) {
    Serial.print(text_0); 
    if(var < 0x10) Serial.write('0');
    Serial.print(var, HEX);
    Serial.println(text_1);
}

void println(const char* text_0, const isize var, const char* text_1) {
    Serial.print(text_0); Serial.print(var); Serial.println(text_1);
}

void println(const char* text_0, const char* text_1, const char* text_2) {
    Serial.print(text_0); Serial.print(text_1); Serial.println(text_2);
}

void println(const char* text_0, const isize var_0) {
    Serial.print(text_0); Serial.println(var_0); 
}

void println(const char* text_0, const char* text_1) {
    Serial.print(text_0); Serial.println(text_1); 
}

void print(const char* text_0, const isize var_0, const char* text_1, const isize var_1) {
    Serial.print(text_0); Serial.print(var_0); Serial.print(text_1); Serial.print(var_1);
}

void println(const char* text_0, const isize var_0, const char* text_1, const char* text_2) {
    Serial.print(text_0); Serial.print(var_0); Serial.print(text_1); Serial.println(text_2);
}

void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1) {
    Serial.print(text_0); Serial.print(var_0); Serial.print(text_1); Serial.println(var_1);
}

void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1, const char* text_2) {
    Serial.print(text_0); Serial.print(var_0); 
    Serial.print(text_1); Serial.print(var_1); 
    Serial.println(text_2);
}

void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1, const char* text_2, const isize var_2) {
    Serial.print(text_0); Serial.print(var_0); Serial.print(text_1); Serial.print(var_1); Serial.print(text_2); Serial.println(var_2);
}

void println(const char* text_0, const isize var_0, const char* text_1, const isize var_1, const char* text_2, const isize var_2, const char* text_3, const isize var_3) {
    Serial.print(text_0); Serial.print(var_0); Serial.print(text_1); Serial.print(var_1); Serial.print(text_2); Serial.print(var_2); Serial.print(text_3); Serial.println(var_3);
}

}

