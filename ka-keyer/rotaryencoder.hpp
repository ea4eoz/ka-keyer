#ifndef ROTARYENCODER_HPP
#define ROTARYENCODER_HPP

#include <Arduino.h>

#define BUTTON_NONE 0
#define BUTTON_IN_PROGRESS 1
#define BUTTON_SHORT 2
#define BUTTON_LONG 3

#define ROTARYENCODER_NONE 0
#define ROTARYENCODER_RIGHT 4
#define ROTARYENCODER_LEFT 5

void rotaryEncoderConfigure(uint8_t pinA, uint8_t pinB, uint8_t btn);
void rotaryEncoderHasChanged();
uint8_t rotaryEncoderRead();

#endif