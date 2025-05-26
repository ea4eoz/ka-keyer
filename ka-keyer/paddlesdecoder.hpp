#ifndef PADDLESDECODER_HPP
#define PADDLESDECODER_HPP

#include <Arduino.h>

void paddlesDecoderConfigure(uint8_t output_keyer_pin);
void paddlesDecoderProcess();
void paddlesDecoderWrite(char c);

#endif