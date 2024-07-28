#ifndef SOUND_H
#define SOUND_H

#include "Arduino.h"

class Sound{
  public:
    Sound(uint8_t pin);
    void start(int frequency);
    void stop();
    void sound(int frequency, int duration);
    void beep();
    void ascent();
    void descent();
  private:
    uint8_t _buzzer_pin;
};

#endif