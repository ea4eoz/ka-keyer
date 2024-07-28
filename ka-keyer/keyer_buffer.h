#ifndef KEYER_BUFFER_H
#define KEYER_BUFFER_H

#include "Arduino.h"

// Size MUST BE a power of 2 minus one
#define KEYERBUFFER_SIZE 31

class KeyerBuffer{
  public:
    KeyerBuffer();
    void clear();
    void write(uint8_t data);
    void write(uint8_t data_1, uint8_t data_2);
    uint8_t read();
    uint8_t count();
  private:
    char buffer[KEYERBUFFER_SIZE + 1];
    uint8_t write_ptr;
    uint8_t read_ptr;
};

#endif
