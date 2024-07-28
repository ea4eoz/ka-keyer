#include "cwdecoder.h"
#include "keyer.h"
#include "paddles.h"
#include "settings.h"

extern Keyer keyer;
extern Settings settings;
extern CWDecoder cwdecoder;

Paddles::Paddles(uint8_t leftPaddle, uint8_t rightPaddle){
  _left_paddle = leftPaddle;
  _right_paddle = rightPaddle;
  pinMode(_left_paddle, INPUT_PULLUP);
  pinMode(_right_paddle, INPUT_PULLUP);
}

void Paddles::process(){
  switch(settings.getKeyer()){
    case KEYER_MODE_ULTIMATE:
      process_ultimate();
    break;

    case KEYER_MODE_IAMBIC:
      process_imabic();
    break;

    case KEYER_MODE_IAMBIC_A:
      process_imabica();
    break;

    case KEYER_MODE_IAMBIC_B:
      process_imabicb();
    break;
  }
}

uint8_t Paddles::readPaddles(){
  uint8_t paddles = 0;
  if (settings.getReverse()){
    // tip = dah, ring = dih
    if (digitalRead(_right_paddle) == LOW){
      paddles |= 0x01;
    }

    if (digitalRead(_left_paddle) == LOW){
      paddles |= 0x02;
    }  
  } else {
    // tip = dih, ring = dah
    if (digitalRead(_right_paddle) == LOW){
      paddles |= 0x02;
    }

    if (digitalRead(_left_paddle) == LOW){
      paddles |= 0x01;
    }  
  }

  if (paddles){
    _lastTimestamp = micros();
  }

  return paddles;
}

void Paddles::process_ultimate(){
  static uint8_t last_keying = NONE;

  if (!keyer.bufferCount()){
    switch (readPaddles()){
      case NONE:
        last_keying = NONE;
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

void Paddles::process_imabic(){
  static uint8_t polling = DIT;

  if (!keyer.bufferCount()){
    switch (readPaddles() & polling){
      case DIT:
        ditPaddle();
      break;

      case DAH:
        dahPaddle();
      break;
    }

    if (polling == DIT){
      polling = DAH;
    } else {
      polling = DIT;
    }
  }
}

void Paddles::process_imabica(){
  static uint8_t polling = DIT;
  static uint8_t lastDit = 1;
  static uint8_t currentDit;

  if (keyer.bufferCount()){
    // Keyer buffer not empty
    if (_wanted && (keyer.bufferCount() < 2)){
      if (_wanted == DIT){
        currentDit = readPaddles() & DIT;
        if ((currentDit) && (!lastDit)){
          ditPaddle();
          _wanted = NONE;
          polling = DAH;
        }
      }
      lastDit = currentDit;
    }
  } else {
    // Keyer buffer is empty
    lastDit = 1;
    switch (readPaddles() & polling){
      case DIT:
        ditPaddle();
      break;

      case DAH:
        dahPaddle();
      break;
    }
    if (polling == DIT){
      polling = DAH;
    } else {
      polling = DIT;
    }
  }
}

void Paddles::process_imabicb(){
  uint8_t paddles;

  if (keyer.bufferCount()){
    // Keyer buffer not empty
    if (_wanted && (keyer.bufferCount() < 2)){
      paddles = readPaddles();
      if ((_wanted == DIT) && (paddles & DIT)){
        ditPaddle();
        _wanted = NONE;
      }
      if ((_wanted == DAH) && (paddles & DAH)){
        dahPaddle();
        _wanted = NONE;
      }
    }
  } else {
    // Keyer buffer is empty
    paddles = readPaddles();

    switch (paddles){
      case DIT:
        ditPaddle();
      break;

      case DAH:
        dahPaddle();
      break;
    }
  }
}

void Paddles::setWakeup(uint8_t wanted){
  _wanted = wanted;
}

unsigned long Paddles::getLastTimestamp(){
  return _lastTimestamp;
}

// For decoding, add a dot into the buffer and makes dit
void Paddles::ditPaddle(){  
  cwdecoder.write('.');
  keyer.sendDih();
}

// For decoding, add a dash into the buffer and makes dah
void Paddles::dahPaddle(){
  cwdecoder.write('-');
  keyer.sendDah();
}
