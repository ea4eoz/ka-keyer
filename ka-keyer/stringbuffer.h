#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include "Arduino.h"
#include "morse.h"

#define STRINGBUFFER_SIZE (MAX_MORSE_LENGTH + 1)

class StringBuffer{
  public:
    StringBuffer();
    void clear();
    void write(char c);
    char* read();
    uint8_t count();
  private:
    char buffer[STRINGBUFFER_SIZE];
    uint8_t write_ptr;
};

#endif
