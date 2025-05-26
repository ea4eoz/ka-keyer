#include "paddlesdecoder.hpp"
#include "morse.hpp"
#include "screendecoder.hpp"
#include "settings.hpp"
#include "winkeyer.hpp"

#define OFF 0
#define ON 1

static uint8_t keyingPin;
static unsigned long lastPaddle;
static uint8_t lastSpaceState = 1;
static uint8_t lastState = 1;
static uint8_t spaceEnable = 0;

#define BUFFER_SIZE (MAX_MORSE_LENGTH + 1)

static char buffer[BUFFER_SIZE];
static uint8_t write_ptr;

// Clear the buffer
static void bufferClear() {
  write_ptr = 0;
  buffer[write_ptr] = '\0';
}

// Write a char into the buffer followerd by a \0
static void bufferWrite(char c) {
  if (write_ptr < (BUFFER_SIZE - 1)) {
    buffer[write_ptr++] = c;
    buffer[write_ptr] = '\0';
  }
}

// Read a char from the buffer
static char* bufferRead() {
  return buffer;
}

// Returns the number of chars in buffer
static uint8_t bufferCount() {
  return write_ptr;
}

// Configures the paddles decoder
void paddlesDecoderConfigure(uint8_t output_keyer_pin) {
  keyingPin = output_keyer_pin;
  bufferClear();
}

// Add an element to the buffer
void paddlesDecoderWrite(char c) {
  bufferWrite(c);
}

void paddlesDecoderProcess() {
  unsigned long elapsed;
  uint8_t state, space_state;

  // Sniff the keying output
  if (digitalRead(keyingPin)) {
    lastPaddle = micros();
  }

  //State for letter and space
  elapsed = micros() - lastPaddle;
  state = elapsed > settingsGet_cw_clock_x2();
  space_state = elapsed > settingsGet_cw_clock_x7();

  // Decoding time?
  if ((state) & (!lastState)) {
    if (bufferCount()) {
      char c = morseDecode(bufferRead());
      if (c) {
        // Known character
        if (settingsGetDecoder()){
          screenDecoderUpdateDecoderLine(c);
        }
        if (winkeyerGetPaddleEchoback()) {
          if(!(c & 0x80)){ // Only ascii
            Serial.print(c);
          }
        }
      } else {
        // Unknown character
        if (settingsGetDecoder()){
          screenDecoderUpdateDecoderLine(0x7F);
        }
        if (winkeyerGetPaddleEchoback()) {
          Serial.write('#');
        }
      }
      spaceEnable = ON;
      // Clear buffer
      bufferClear();
    }
  }

  // Space decoding time?
  if ((space_state) & (!lastSpaceState) & spaceEnable) {
    if(settingsGetDecoder()){
      screenDecoderUpdateDecoderLine(' ');
    }
    if (winkeyerGetPaddleEchoback()) {
      Serial.write(' ');
    }
    spaceEnable = OFF;
  }

  // Save states for next round
  lastState = state;
  lastSpaceState = space_state;
}
