#include "ptt.hpp"
#include "settings.hpp"

#define OFF 0
#define ON 1

static uint8_t pttPin;
static uint8_t keyerPin;

void pttConfigure(uint8_t ptt, uint8_t keyer_output) {
  pttPin = ptt;
  keyerPin = keyer_output;
  pinMode(pttPin, OUTPUT);
}

void pttProcess() {
  static bool state;
  static bool lastState;
  static unsigned long lastPTTing;
  static unsigned long elapsed;

  // Sniff the keying output
  if (digitalRead(keyerPin)) {
    lastPTTing = micros();
    digitalWrite(pttPin, ON);
  }

  //State for ptt
  elapsed = micros() - lastPTTing;
  state = elapsed > settingsGet_ptt_timeout();

  // PTT OFF time?
  if ((state) & (!lastState)) {
    digitalWrite(pttPin, OFF);
  }

  lastState = state;
}