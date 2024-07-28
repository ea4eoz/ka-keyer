#ifndef WINKEYERDATABUFFER_H
#define WINKEYERDATABUFFER_H

#include "Arduino.h"

// Size MUST BE a power of 2 minus one
#define WK_DATABUFFER_SIZE 127

#define WK_DATABUFFER_MODE_NORMAL 0
#define WK_DATABUFFER_MODE_OVERWRITE 1
#define WK_DATABUFFER_MODE_APPEND 2

class WinkeyerDataBuffer{
  public:
    WinkeyerDataBuffer();
    void clear();
    void resetCmd16Ptr();
    void write(uint8_t data);
    uint8_t read();
    uint8_t count();
    void bufferBackSpace();
    void setCmd16Ptr(uint8_t p);
    void setMode(uint8_t mode);
  private:
    uint8_t buffer[128];
    uint8_t write_ptr;
    uint8_t read_ptr;
    uint8_t buffer_mode;
    uint8_t cmd16_ptr;
};
#endif