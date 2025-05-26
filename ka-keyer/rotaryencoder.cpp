#include "rotaryencoder.hpp"

// Adapted from:
// http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
// https://github.com/buxtronix/arduino/tree/master/libraries/Rotary

#define R_START 0x00
#define R_CW_FINAL 0x01
#define R_CW_BEGIN 0x02
#define R_CW_NEXT 0x03
#define R_CCW_BEGIN 0x04
#define R_CCW_FINAL 0x05
#define R_CCW_NEXT 0x06
#define DIR_NONE 0x00
#define DIR_CW 0x10
#define DIR_CCW 0x20

#define ROTARYENCODER_POLLING 100
#define BUTTON_LONG_PRESS (1000 / ROTARYENCODER_POLLING)

static uint8_t pinA;
static uint8_t pinB;
static uint8_t button;
static uint8_t encoderData;

static const uint8_t EncoderTable[7][4] PROGMEM = {
  { R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START },           // R_START
  { R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW },  // R_CW_FINAL
  { R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START },           // R_CW_BEGIN
  { R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START },           // R_CW_NEXT
  { R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START },           // R_CCW_BEGIN
  { R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW }, // R_CCW_FINAL
  { R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START },           // R_CCW_NEXT
};

void rotaryEncoderConfigure(uint8_t pin_A, uint8_t pin_B, uint8_t btn) {
  pinA = pin_A;
  pinB = pin_B;
  button = btn;
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(button, INPUT_PULLUP);
}

void rotaryEncoderHasChanged() {
  static uint8_t encoderState;
  uint8_t pinState;
  uint8_t data;

  pinState = (digitalRead(pinB) << 1) | digitalRead(pinA);
  encoderState = pgm_read_byte(&EncoderTable[encoderState & 0x0F][pinState]);
  data = encoderState & 0x30;
  if (data) {
    encoderData = data;
  }
}

static uint8_t readButton() {
  static uint8_t lastState = 1;
  static uint8_t counter = 0;
  static bool waitForRelease = 0;

  uint8_t state, retval;

  uint8_t pin = digitalRead(button);

  if (waitForRelease) {
    if (pin == 1) {
      waitForRelease = false;
    }
    lastState = 1;
    counter = 0;
    retval = BUTTON_NONE;
  } else {
    state = (lastState << 1) | pin;
    switch (state & 0x03) {
      case 0x00:
        // Button still pressed
        counter++;
        if (counter > BUTTON_LONG_PRESS) {
          waitForRelease = true;
          retval = BUTTON_LONG;
        } else {
          retval = BUTTON_IN_PROGRESS;
        }
        break;

      case 0x01:
        // Button is released
        if (counter < BUTTON_LONG_PRESS) {
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
    lastState = pin;
  }
  return retval;
}

static uint8_t readEncoder() {
  uint8_t retval;
  uint8_t data = encoderData;
  encoderData = ROTARYENCODER_NONE;

  switch (data) {
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

uint8_t rotaryEncoderRead() {
  int8_t data = readButton();
  if (data == ROTARYENCODER_NONE) {
    data = readEncoder();
  }
  return data;
}


