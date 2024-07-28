#ifndef PTT_H
#define PTT_H

#include "Arduino.h"

class PTT{
  public:
    PTT(uint8_t ptt, uint8_t keyer_output);
    void process();
  private:
    uint8_t _ptt_pin, _keyer_pin, _state, _last_state;
    unsigned long _last_ptting, _elapsed;
};

#endif