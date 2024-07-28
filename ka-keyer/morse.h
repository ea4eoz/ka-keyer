#ifndef MORSE_H
#define MORSE_H

#include "Arduino.h"
#define MAX_MORSE_LENGTH 7

class Morse{
  public:
    const char* encode(char c);
    char decode(char *buffer);    
  private:
    uint8_t getIndex(const char* sequence);
};

#endif