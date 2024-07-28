#include "rotaryencoder.h"

// Adapted from:
// http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
// https://github.com/buxtronix/arduino/tree/master/libraries/Rotary

#define ROTARYENCODER_POLLING 100
#define BUTTON_LONG_PRESS (1000 / ROTARYENCODER_POLLING)

#define R_START     0x00
#define R_CW_FINAL  0x01
#define R_CW_BEGIN  0x02
#define R_CW_NEXT   0x03
#define R_CCW_BEGIN 0x04
#define R_CCW_FINAL 0x05
#define R_CCW_NEXT  0x06
#define DIR_NONE    0x00
#define DIR_CW      0x10
#define DIR_CCW     0x20

const uint8_t EncoderTable[7][4] PROGMEM = {
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},            // R_START
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},   // R_CW_FINAL
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},            // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},            // R_CW_NEXT
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},            // R_CCW_BEGIN
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},            // R_CCW_NEXT
};

RotaryEncoder::RotaryEncoder(uint8_t pin_A, uint8_t pin_B, uint8_t button){
  _pinA = pin_A;
  _pinB = pin_B;
  _button = button;
  pinMode(_pinA, INPUT_PULLUP);
  pinMode(_pinB, INPUT_PULLUP);
  pinMode(_button, INPUT_PULLUP);
}

void RotaryEncoder::hasChanged(){
  uint8_t pinstate, data;
  pinstate = (digitalRead(_pinB) << 1) | digitalRead(_pinA);
  _encoderState = pgm_read_byte(&EncoderTable[_encoderState & 0x0F][pinstate]);
  data = _encoderState & 0x30;
  if(data){
    _encoderData = data;
  }
}

uint8_t RotaryEncoder::read(){
  int8_t data = readButton();
  if (data == ROTARYENCODER_NONE){
    data = readEncoder();
  }
  return data;
}

uint8_t RotaryEncoder::readEncoder(){
  uint8_t retval;
  uint8_t data = _encoderData;
  _encoderData = ROTARYENCODER_NONE;

  switch (data){
    case DIR_CW:
      retval = ROTARYENCODER_RIGHT;
    break;

    case DIR_CCW:
      retval = ROTARYENCODER_LEFT;
    break;

    default:
      retval = ROTARYENCODER_NONE;
    break;
  }
  return retval;
}

uint8_t RotaryEncoder::readButton(){
  static uint8_t last_state = 1;
  static uint8_t counter = 0;
  static uint8_t waitforrelease = 0;

  uint8_t state, retval;

  uint8_t pin = digitalRead(_button);

  if (waitforrelease){
    if (pin == 1){
      waitforrelease = 0;
    }
    last_state = 1;
    counter = 0;
    retval = BUTTON_NONE;
  } else {
    state = (last_state << 1) | pin ;
    switch (state & 0x03){
      case 0x00:
        // Button still pressed
        counter++;
        if (counter > BUTTON_LONG_PRESS){
          waitforrelease = 1;
          retval = BUTTON_LONG;
        } else {
          retval = BUTTON_IN_PROGRESS;
        }
      break;

      case 0x01:
        // Button is released
        if (counter < BUTTON_LONG_PRESS){
          retval = BUTTON_SHORT;
        } else {
          retval = BUTTON_LONG;
        }
        counter = 0;
      break;

      case 0x02:
        // Button just pressed
        counter++;
        retval = BUTTON_IN_PROGRESS;
      break;

      case 0x03:
        counter = 0;
        retval = BUTTON_NONE;
      break;
    }
    last_state = pin;
  }
  return retval;
}