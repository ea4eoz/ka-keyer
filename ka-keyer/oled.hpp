#ifndef OLED_HPP
#define OLED_HPP

#include <Arduino.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 8

struct Font {
  uint8_t fontHeight;
  uint8_t firstChar;
  uint8_t charCount;
  uint8_t columnSpacing;
  const uint8_t *charWidths;
  const uint8_t *const *charAddresses;
};

#ifdef ARDUINO_AVR_NANO
void oledBegin();
#endif
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
void oledBegin(uint8_t scl, uint8_t sda);
#endif
void oledSetFont(Font *newFont);
uint8_t oledGetCharWidth(char c);
uint8_t oledGetCharHeight();
void oledClearArea(uint8_t x, uint8_t y, uint8_t wide, uint8_t height);
void oledClearRows(uint8_t startRow, uint8_t rowCount);
void oledClearScreen();
void oledWriteChar(uint8_t x, uint8_t y, char c);
void oledPrintCenter(uint8_t row, const char *str);
void oledPrintCenter(uint8_t row, const __FlashStringHelper *str);

#endif
