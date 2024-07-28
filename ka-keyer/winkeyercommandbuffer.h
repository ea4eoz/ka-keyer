#ifndef WINKEYERCOMMANDBUFFER_H
#define WINKEYERCOMMANDBUFFER_H

#include "Arduino.h"

#define WK_CMDBUFFER_SIZE 20

class WinkeyerCommandBuffer{
  public:
    WinkeyerCommandBuffer();
    void clear();
    void write(uint8_t b);
    uint8_t read(uint8_t position);
    uint8_t count();
  private:
    uint8_t buffer[WK_CMDBUFFER_SIZE];
    uint8_t write_ptr;
};

#endif