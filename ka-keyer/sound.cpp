#include "sound.hpp"

#define TONE_DURATION 50
#define TONE_SPACING 20
#define TONE_HIGH 1200
#define TONE_LOW 800

static uint8_t buzzerPin;

void soundConfigure(uint8_t pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
}

void soundStart(int frequency) {
  tone(buzzerPin, frequency);
}

void soundStop() {
  noTone(buzzerPin);
}

void soundSound(int frequency, int duration) {
  soundStart(frequency);
  delay(duration);
  soundStop();
}

void soundBeep() {
  soundSound(TONE_HIGH, TONE_DURATION);
}

void soundAscent() {
  soundSound(TONE_LOW, TONE_DURATION);
  delay(TONE_SPACING);
  soundSound(TONE_HIGH, TONE_DURATION);
}

void soundDescent() {
  soundSound(TONE_HIGH, TONE_DURATION);
  delay(TONE_SPACING);
  soundSound(TONE_LOW, TONE_DURATION);
}
