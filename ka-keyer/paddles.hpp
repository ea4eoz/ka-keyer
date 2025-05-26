#ifndef PADDLES_HPP
#define PADDLES_HPP

#include <Arduino.h>

#define NONE 0
#define DIT 1
#define DAH 2
#define BOTH 3

void paddlesBegin(uint8_t lp, uint8_t rp);
void paddlesProcess();
void paddlesSetWakeup(uint8_t w);
unsigned long paddlesGetLastTimestamp();

#endif