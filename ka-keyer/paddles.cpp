#include "paddlesdecoder.hpp"
#include "keyer.hpp"
#include "paddles.hpp"
#include "settings.hpp"

#define KEYER_MODE_ULTIMATE 0
#define KEYER_MODE_IAMBIC 1
#define KEYER_MODE_IAMBIC_A 2
#define KEYER_MODE_IAMBIC_B 3

static uint8_t leftPaddle;
static uint8_t rightPaddle;
static uint8_t wanted = 0;
static unsigned long lastTimestamp;

// Configure paddles pins
void paddlesBegin(uint8_t lp, uint8_t rp) {
  leftPaddle = lp;
  rightPaddle = rp;
  pinMode(leftPaddle, INPUT_PULLUP);
  pinMode(rightPaddle, INPUT_PULLUP);
}

// Read paddles
static uint8_t readPaddles() {
  uint8_t paddles = 0;
  if (settingsGetReverse()) {
    // tip = dah, ring = dih
    if (digitalRead(rightPaddle) == LOW) {
      paddles |= 0x01;
    }
    if (digitalRead(leftPaddle) == LOW) {
      paddles |= 0x02;
    }
  } else {
    // tip = dih, ring = dah
    if (digitalRead(rightPaddle) == LOW) {
      paddles |= 0x02;
    }
    if (digitalRead(leftPaddle) == LOW) {
      paddles |= 0x01;
    }
  }
  if (paddles) {
    lastTimestamp = micros();
  }
  return paddles;
}

// For decoding, add a dot into the buffer and makes dit
static void ditPaddle() {
  paddlesDecoderWrite('.');
  keyerSendDih();
}

// For decoding, add a dash into the buffer and makes dah
static void dahPaddle() {
  paddlesDecoderWrite('-');
  keyerSendDah();
}

static void paddlesProcess_ultimate(){
  static uint8_t last_keying = NONE;
  static bool memoEnabled = 1;
  uint8_t paddles;

  if (keyerIsKeying()){
    if (wanted && memoEnabled){
      paddles = readPaddles();
      if ((wanted == DIT) && (paddles == DIT)){
        ditPaddle();
        last_keying = DIT;
        wanted = NONE;
        memoEnabled = false;
      }
      if ((wanted == DAH) && (paddles == DAH)){
        dahPaddle();
        last_keying = DAH;
        wanted = NONE;
        memoEnabled = false;
      }
    }
  } else {
    switch (readPaddles()){
      case NONE:
        last_keying = NONE;
        memoEnabled = true;
      break;

      case DIT:
        last_keying = DIT;
        ditPaddle();
      break;

      case DAH:
        last_keying = DAH;
        dahPaddle();
      break;

      case BOTH:
        if (last_keying == DIT){
          dahPaddle();
        }
        if (last_keying == DAH){
          ditPaddle();
        }
      break;
    }
  }
}

static void paddlesProcess_imabic() {
  static uint8_t polling = DIT;

  if (!keyerIsKeying()) {
    switch (readPaddles() & polling) {
      case DIT:
        ditPaddle();
        break;

      case DAH:
        dahPaddle();
        break;
    }

    if (polling == DIT) {
      polling = DAH;
    } else {
      polling = DIT;
    }
  }
}

static void paddlesProcess_imabica() {
  static uint8_t polling = DIT;
  static bool lastDit = true;
  static bool currentDit;

  if (keyerIsKeying()) {
    if (wanted) {
      if (wanted == DIT) {
        currentDit = readPaddles() & DIT;
        if ((currentDit) && (!lastDit)) {
          ditPaddle();
          wanted = NONE;
          polling = DAH;
        }
      }
      lastDit = currentDit;
    }
  } else {
    // Keyer buffer is empty
    lastDit = true;
    switch (readPaddles() & polling) {
      case DIT:
        ditPaddle();
        break;

      case DAH:
        dahPaddle();
        break;
    }
    if (polling == DIT) {
      polling = DAH;
    } else {
      polling = DIT;
    }
  }
}

static void paddlesProcess_imabicb() {
  uint8_t paddles;

  if (keyerIsKeying()) {
    if (wanted) {
      paddles = readPaddles();
      if ((wanted == DIT) && (paddles & DIT)) {
        ditPaddle();
        wanted = NONE;
      }
      if ((wanted == DAH) && (paddles & DAH)) {
        dahPaddle();
        wanted = NONE;
      }
    }
  } else {
    // Keyer buffer is empty
    paddles = readPaddles();

    switch (paddles) {
      case DIT:
        ditPaddle();
        break;

      case DAH:
        dahPaddle();
        break;
    }
  }
}

void paddlesProcess() {
  switch (settingsGetKeyer()) {
    case KEYER_MODE_ULTIMATE:
      paddlesProcess_ultimate();
      break;

    case KEYER_MODE_IAMBIC:
      paddlesProcess_imabic();
      break;

    case KEYER_MODE_IAMBIC_A:
      paddlesProcess_imabica();
      break;

    case KEYER_MODE_IAMBIC_B:
      paddlesProcess_imabicb();
      break;
  }
}

void paddlesSetWakeup(uint8_t w) {
  wanted = w;
}

unsigned long paddlesGetLastTimestamp() {
  return lastTimestamp;
}
