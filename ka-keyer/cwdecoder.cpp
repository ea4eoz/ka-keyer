#include "cwdecoder.h"
#include "morse.h"
#include "settings.h"
#include "winkeyer.h"

#define OFF 0
#define ON 1

extern Morse morse;
extern Settings settings;
extern Winkeyer winkeyer;

CWDecoder::CWDecoder(uint8_t output_keyer_pin){
  _output_keyer_pin = output_keyer_pin;
  stringBuffer.clear();
}

void CWDecoder::process(){
  static unsigned long last_paddle;
  static uint8_t last_space_state = 1;
  static uint8_t last_state = 1;
  static uint8_t space_enable = 0;
  unsigned long elapsed;
  uint8_t state, space_state;

  if (winkeyer.getPaddleEchoback()){

    // Sniff the keying output
    if (digitalRead(_output_keyer_pin)) {
      last_paddle = micros();
    }

    //State for letter and space
    elapsed = micros() - last_paddle;
    state = elapsed > settings.get_cw_clock_x2();
    space_state = elapsed > settings.get_cw_clock_x7();

    // Decoding time?
    if ((state) & (!last_state)){
      if (stringBuffer.count()){
        char c = morse.decode(stringBuffer.read());
        if (c) {
          // Known character
          Serial.print(c);
        } else {
          // Unknown character
          if (winkeyer.getHostOpened()){
            Serial.print(F("#"));
          } else {
            Serial.print(F("["));
            Serial.print(stringBuffer.read());
            Serial.print(F("]"));
          }
          
        }
        space_enable = ON;
        // Clear buffer
        stringBuffer.clear();
      }
    }

    // Space decoding time?
    if ((space_state) & (!last_space_state) & space_enable){
      Serial.print(' ');
      space_enable = OFF;
    }

    // Save states for next round
    last_state = state;
    last_space_state = space_state;
  } else {
    // Clear buffer
    stringBuffer.clear();
  }
}

void CWDecoder::write(char c){
  stringBuffer.write(c);
}