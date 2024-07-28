#include "sound.h"

#define TONE_DURATION 50
#define TONE_SPACING 20
#define TONE_HIGH 1200
#define TONE_LOW 800

Sound::Sound(uint8_t pin){
  _buzzer_pin = pin;
  pinMode(_buzzer_pin, OUTPUT);
}

void Sound::start(int frequency){
  tone(_buzzer_pin, frequency);
}

void Sound::stop(){
  noTone(_buzzer_pin);
}

void Sound::sound(int frequency, int duration){
  start(frequency);
  delay(duration);
  stop();
}

void Sound::beep(){
  sound(TONE_HIGH, TONE_DURATION);
}

void Sound::ascent(){
  sound(TONE_LOW, TONE_DURATION);
  delay(TONE_SPACING);
  sound(TONE_HIGH, TONE_DURATION);
}

void Sound::descent(){
  sound(TONE_HIGH, TONE_DURATION);
  delay(TONE_SPACING);
  sound(TONE_LOW, TONE_DURATION);
}

