#ifndef SCREENDECODER_HPP
#define SCREENDECODER_HPP

#include <Arduino.h>

void screenDecoderBegin();
uint8_t screenDecoderWrite(uint8_t c);
void screenDecoderUpdateDecoderLine(uint8_t c);
void screenDecoderRefreshDecoderLine();
void screenDecoderClearDecoderLine();
void screenDecoderProcess();

#endif