#ifndef KEYER_H
#define KEYER_H

#include "Arduino.h"
#include "keyer_buffer.h"

// Keyer commands
#define KCMD_NONE        0x00
#define KCMD_SPACE       0x01
#define KCMD_DIT_KEYDOWN 0x02
#define KCMD_DIT_KEYUP   0x03
#define KCMD_DAH_KEYDOWN 0x04
#define KCMD_DAH_KEYUP   0x05
#define KCMD_SPEED       0x06

class Keyer{
  public:
    Keyer(uint8_t keyer_pin);
    void process();
    uint8_t bufferCount();
    void sendDih();
    void sendDah();
    void sendSpace();
    void sendChar (char c);
    void sendProsign (char c1, char c2);
    void sendCommand(uint8_t command, uint8_t argument);
  private:
    KeyerBuffer keyerBuffer;
    unsigned long amount = 0;
    uint8_t _keyer_pin;
    void translate(char* ptr);
};

#endif