#ifndef PADDLES_H
#define PADDLES_H

#include "Arduino.h"

#define NONE 0
#define DIT  1
#define DAH  2
#define BOTH 3

#define KEYER_MODE_ULTIMATE 0
#define KEYER_MODE_IAMBIC   1
#define KEYER_MODE_IAMBIC_A 2
#define KEYER_MODE_IAMBIC_B 3

class Paddles{
  public:
    Paddles(uint8_t leftPaddle, uint8_t rightPaddle);
    void process();
    void setWakeup(uint8_t wanted);
    unsigned long getLastTimestamp();
  private:
    uint8_t _left_paddle, _right_paddle;
    uint8_t _wanted = 0;
    unsigned long _last_check, _lastTimestamp;
    uint8_t readPaddles();
    void process_ultimate();
    void process_ultimateM();    
    void process_imabic();
    void process_imabica();
    void process_imabicb();
    void ditPaddle();  
    void dahPaddle();
};

#endif