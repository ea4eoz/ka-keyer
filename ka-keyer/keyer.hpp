#ifndef KEYER_HPP
#define KEYER_HPP

#include "Arduino.h"

// Keyer commands
#define KCMD_NONE        0x00
#define KCMD_SPACE       0x01
#define KCMD_DIT_KEYDOWN 0x02
#define KCMD_DIT_KEYUP   0x03
#define KCMD_DAH_KEYDOWN 0x04
#define KCMD_DAH_KEYUP   0x05
#define KCMD_SPEED       0x06

void keyerConfigure(uint8_t keyer_pin);
void keyerProcess();
void keyerSendDih();
void keyerSendDah();
void keyerSendSpace();
void keyerSendChar (char c);
void keyerSendProsign (char c1, char c2);
void keyerSendCommand(uint8_t command, uint8_t argument);
bool keyerIsKeying();

#endif