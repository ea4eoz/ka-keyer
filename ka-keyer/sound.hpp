#ifndef SOUND_HPP
#define SOUND_HPP

#include <Arduino.h>

void soundConfigure(uint8_t pin);
void soundStart(int frequency);
void soundStop();
void soundSound(int frequency, int duration);
void soundBeep();
void soundAscent();
void soundDescent();

#endif