#include "keyer.h"
#include "keyer_buffer.h"
#include "morse.h"
#include "paddles.h"
#include "settings.h"
#include "sound.h"
#include "winkeyer.h"

#define OFF 0
#define ON 1

extern Paddles paddles;
extern Settings settings;
extern Morse morse;
extern Sound sound;
extern Winkeyer winkeyer;

Keyer::Keyer(uint8_t pin){
  _keyer_pin = pin;
  pinMode(_keyer_pin, OUTPUT);
}

void Keyer::process(){
  static unsigned long waiting = 0;

  if (amount){
    if ((micros() - waiting) < amount){
      // Nothing to do yet
      return;
    }
  }

  amount = 0;

  if (keyerBuffer.count()){
    // If buffer has something, process it
    uint8_t b = keyerBuffer.read();
    switch (b){
      case KCMD_SPACE:
        amount = settings.get_cw_clock_x2();
        waiting = micros();
      break;

      case KCMD_DIT_KEYDOWN:
        if (settings.getSidetone()){
          sound.start(settings.getSidetone_freq());
        }
        digitalWrite(_keyer_pin, 1);
        amount = settings.get_cw_dit_keydown();
        waiting = micros();
        paddles.setWakeup(DAH);
      break;

      case KCMD_DIT_KEYUP:
        digitalWrite(_keyer_pin, 0);
        if (settings.getSidetone()){
          sound.stop();
        }
        amount = settings.get_cw_dit_keyup();
        waiting = micros();
      break;

      case KCMD_DAH_KEYDOWN:
        if (settings.getSidetone()){
          sound.start(settings.getSidetone_freq());
        }
        digitalWrite(_keyer_pin, 1);
        amount = settings.get_cw_dah_keydown();
        waiting = micros();
        paddles.setWakeup(DIT);
      break;

      case KCMD_DAH_KEYUP:
        digitalWrite(_keyer_pin, 0);
        if (settings.getSidetone()){
          sound.stop();
        }
        amount = settings.get_cw_clock();
        waiting = micros();
      break;

      case KCMD_SPEED:
        settings.setSpeed(keyerBuffer.read());
      break;

      case 0x80 ... 0xFF:
        if (winkeyer.getSerialEchoback()){
          Serial.write(b & 0x7F);
        }
      break;
    }
  }
}

// Add the making of a dit into keyer's buffer
void Keyer::sendDih(){
  keyerBuffer.write(KCMD_DIT_KEYDOWN, KCMD_DIT_KEYUP);
}

// Add the making of a dah into keyer's buffer
void Keyer::sendDah(){
  keyerBuffer.write(KCMD_DAH_KEYDOWN, KCMD_DAH_KEYUP);
}

// Add the making of a space into keyer's buffer
void Keyer::sendSpace(){
  keyerBuffer.write(KCMD_SPACE);
}

// Translate a char into a dit/dah keying sequence
void Keyer::sendChar (char c){
  char bfr[MAX_MORSE_LENGTH + 1];

  if (c > 0){
    // Get the actual sequence of dihs/dahs
    strcpy_P(bfr, morse.encode(c));
    // Check for null string
    if (bfr[0] != '\0'){
      translate(bfr);
      // Last dih/dah already make one spacing,
      // so makeSpace make the two remaining
      sendSpace();
      // Add literal character for echo
      keyerBuffer.write(c | 0x80);
    }
  }
}

// Do the keying of a prosign
void Keyer::sendProsign (char c1, char c2){
  char bfr[MAX_MORSE_LENGTH + 1];

  if (c1 > 0){
    strcpy_P(bfr, morse.encode(c1));
    translate(bfr);
  }

  if (c2 > 0){
    strcpy_P(bfr, morse.encode(c2));
    translate(bfr);
  }

  // Last dih/dah already make one spacing,
  // so makeSpace make the two remaining
  sendSpace();
}

// Manipulate a dit-dah sequence
void Keyer::translate(char* ptr){
    while(*ptr){
      switch (*ptr){
        case '.':
          sendDih();
        break;

        case '-':
          sendDah();
        break;

        case ' ':
          sendSpace();
        break;
      }
    ptr++;
  }
}

// Add a command into keyer's buffer
void Keyer::sendCommand(uint8_t command, uint8_t argument){
  keyerBuffer.write(command, argument);
}

uint8_t Keyer::bufferCount(){
  return keyerBuffer.count() || amount;
}
