#include "keyer.hpp"
#include "morse.hpp"
#include "paddles.hpp"
#include "screendecoder.hpp"
#include "settings.hpp"
#include "sound.hpp"
#include "winkeyer.hpp"

// Size MUST BE a power of 2
#define BUFFER_SIZE 32
#define BUFFER_MASK (BUFFER_SIZE - 1)

static uint8_t keyerPin;
static unsigned long amount = 0;
static char buffer[BUFFER_SIZE];
static uint8_t writePtr;
static uint8_t readPtr;

// Clear keyer buffer
static void bufferClear() {
  writePtr = 0;
  readPtr = 0;
}

// Return the number of elements in buffer
static uint8_t bufferCount() {
  return (writePtr - readPtr) & BUFFER_MASK;
}

// Writes into the buffer one element
static void bufferWrite(uint8_t data) {
  // Check for at least one byte free
  if (bufferCount() < BUFFER_SIZE - 1) {
    buffer[writePtr++] = data;
    writePtr &= BUFFER_MASK;
  }
}

// Writes into the buffer two elements
static void bufferWrite(uint8_t data_1, uint8_t data_2) {
  // Check for at least two bytes free
  if (bufferCount() < (BUFFER_SIZE - 2)) {
    bufferWrite(data_1);
    bufferWrite(data_2);
  }
}

// Read an element from the buffer
static uint8_t bufferRead() {
  uint8_t data = buffer[readPtr++];
  readPtr &= BUFFER_MASK;
  return data;
}

// Setup keyer GPIOs
void keyerConfigure(uint8_t pin) {
  keyerPin = pin;
  pinMode(keyerPin, OUTPUT);
  bufferClear();
}

// Manipulate a dit-dah sequence
static void translate(char* ptr) {
  while (*ptr) {
    switch (*ptr) {
      case '.':
        keyerSendDih();
        break;

      case '-':
        keyerSendDah();
        break;

      case ' ':
        keyerSendSpace();
        break;
    }
    ptr++;
  }
}

// Add the making of a dit into keyer's buffer
void keyerSendDih() {
  bufferWrite(KCMD_DIT_KEYDOWN, KCMD_DIT_KEYUP);
}

// Add the making of a dah into keyer's buffer
void keyerSendDah() {
  bufferWrite(KCMD_DAH_KEYDOWN, KCMD_DAH_KEYUP);
}

// Add the making of a space into keyer's buffer
void keyerSendSpace() {
  bufferWrite(KCMD_SPACE);
}

// Translate a char into a dit/dah keying sequence
void keyerSendChar(char c) {
  char bfr[MAX_MORSE_LENGTH + 1];
  // Get the actual sequence of dihs/dahs
  strcpy_P(bfr, morseEncode(c));
  // Check for null string
  if (bfr[0] != '\0') {
    translate(bfr);
    // Last dih/dah already make one spacing,
    // so makeSpace make the two remaining
    keyerSendSpace();
    // Add literal character for echo
    bufferWrite(c);
  }
}

// Do the keying of a prosign
void keyerSendProsign(char c1, char c2) {
  char bfr[MAX_MORSE_LENGTH + 1];

  if (c1 > 0) {
    strcpy_P(bfr, morseEncode(c1));
    translate(bfr);
  }

  if (c2 > 0) {
    strcpy_P(bfr, morseEncode(c2));
    translate(bfr);
  }

  // Last dih/dah already make one spacing,
  // so makeSpace make the two remaining
  keyerSendSpace();
}

// Add a command into keyer's buffer
void keyerSendCommand(uint8_t command, uint8_t argument) {
  bufferWrite(command, argument);
}

bool keyerIsKeying() {
  return bufferCount() || amount;
}

void keyerProcess() {
  static unsigned long waiting = 0;

  if (amount) {
    if ((micros() - waiting) < amount) {
      // Nothing to do yet
      return;
    }
  }

  amount = 0;

  if (bufferCount()) {
    // If buffer has something, process it
    uint8_t b = bufferRead();
    switch (b) {
      case KCMD_SPACE:
        amount = settingsGet_cw_clock_x2();
        waiting = micros();
        break;

      case KCMD_DIT_KEYDOWN:
        if (settingsGetSidetone()) {
          soundStart(settingsGetSidetone_freq());
        }
        digitalWrite(keyerPin, 1);
        amount = settingsGet_cw_dit_keydown();
        waiting = micros();
        paddlesSetWakeup(DAH);
        break;

      case KCMD_DIT_KEYUP:
        digitalWrite(keyerPin, 0);
        if (settingsGetSidetone()) {
          soundStop();
        }
        amount = settingsGet_cw_dit_keyup();
        waiting = micros();
        break;

      case KCMD_DAH_KEYDOWN:
        if (settingsGetSidetone()) {
          soundStart(settingsGetSidetone_freq());
        }
        digitalWrite(keyerPin, 1);
        amount = settingsGet_cw_dah_keydown();
        waiting = micros();
        paddlesSetWakeup(DIT);
        break;

      case KCMD_DAH_KEYUP:
        digitalWrite(keyerPin, 0);
        if (settingsGetSidetone()) {
          soundStop();
        }
        amount = settingsGet_cw_clock();
        waiting = micros();
        break;

      case KCMD_SPEED:
        settingsSetSpeed(bufferRead());
        break;

      case 0x20 ... 0xFF:
        if (winkeyerGetSerialEchoback()) {
          Serial.write(b);
        }
        if (settingsGetDecoder()){
          screenDecoderUpdateDecoderLine(b);
        }
        break;
    }
  }
}
