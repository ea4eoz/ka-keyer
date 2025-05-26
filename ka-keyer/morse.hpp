#ifndef MORSE_HPP
#define MORSE_HPP

#include <Arduino.h>

// Do not edit!!!
#define MAX_MORSE_LENGTH 7

const char* morseEncode(char c);
char morseDecode(char* buffer);
void morseBegin();

#endif