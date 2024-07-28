#include "ptt.h"
#include "settings.h"

#define OFF 0
#define ON 1

extern Settings settings;

PTT::PTT(uint8_t ptt, uint8_t keyer_output) {
  _ptt_pin = ptt;
  _keyer_pin = keyer_output;
  pinMode(_ptt_pin, OUTPUT);
}

void PTT::process() {
  // Sniff the keying output
  if (digitalRead(_keyer_pin)) {
    _last_ptting = micros();
    digitalWrite(_ptt_pin, ON);
  }

  //State for ptt
  _elapsed = micros() - _last_ptting;
  _state = _elapsed > settings.get_ptt_timeout();

  // PTT OFF time?
  if ((_state) & (!_last_state)) {
    digitalWrite(_ptt_pin, OFF);
  }

  _last_state = _state;
}