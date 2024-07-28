#include "cwdecoder.h"
#include "rotaryencoder.h"
#include "keyer.h"
#include "paddles.h"
#include "ptt.h"
#include "oled.h"
#include "settings.h"
#include "sound.h"
#include "userinterface.h"
#include "winkeyer.h"

// Hardware definitions
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define ENCODER_BUTTON A1
#define PADDLE_LEFT 5
#define PADDLE_RIGHT 6
#define KEYER_OUTPUT 11
#define PTT_OUTPUT 12
#define BUZZER_OUTPUT 4
#define OLED_ADDRESS 0x3C

// Constructors
CWDecoder cwdecoder(KEYER_OUTPUT);
RotaryEncoder rotaryEncoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BUTTON);
Keyer keyer(KEYER_OUTPUT);
Morse morse;
Paddles paddles(PADDLE_LEFT, PADDLE_RIGHT);
PTT ptt(PTT_OUTPUT, KEYER_OUTPUT);
OLED oled(OLED_ADDRESS);
Settings settings;
Sound sound(BUZZER_OUTPUT);
UserInterface ui;
Winkeyer winkeyer;

// Interrupt service routine for rotary encoder
void encoderChanged(){
  rotaryEncoder.hasChanged();
}

void setup() {
  settings.begin();
  // Setup encoder interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderChanged, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoderChanged, CHANGE);
  ui.begin();
  winkeyer.begin();
  ui.start();
}

void loop() {
  ui.process();
  ptt.process();
  cwdecoder.process();
  winkeyer.process();
  paddles.process();
  keyer.process();
}
