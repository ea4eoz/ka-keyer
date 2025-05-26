#ifndef PTT_HPP
#define PTT_HPP

#include <Arduino.h>

void pttConfigure(uint8_t ptt, uint8_t keyer_output);
void pttProcess();

#endif