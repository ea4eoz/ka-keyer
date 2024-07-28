#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include "Arduino.h"

#define BUTTON_NONE        0
#define BUTTON_IN_PROGRESS 1
#define BUTTON_SHORT       2
#define BUTTON_LONG        3

#define ROTARYENCODER_NONE       0
#define ROTARYENCODER_RIGHT      4
#define ROTARYENCODER_LEFT       5

class RotaryEncoder{
  private:
    uint8_t _pinA,  _pinB, _button;
    uint8_t _encoderState, _encoderData;
    uint8_t readEncoder();
    uint8_t readButton();

  public:
    RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t button);
    void hasChanged();
    uint8_t read();
};

#endif