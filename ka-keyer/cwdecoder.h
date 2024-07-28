#ifndef CWDECODER_H
#define CWDECODER_H

#include "Arduino.h"
#include "stringbuffer.h"

class CWDecoder{
  public:
    CWDecoder(uint8_t output_keyer_pin);
    void process();
    void write(char c);
  private:
    uint8_t _output_keyer_pin;
    StringBuffer stringBuffer;
};

#endif