#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include "font.h"

class OLED{
  public:
    OLED(uint8_t address);
    void begin();
    void clearScreen();
    void setFont(uint8_t newFont);

    void print(uint8_t x, uint8_t y, const char *str);
    void print(uint8_t x, uint8_t y, const __FlashStringHelper *str);
    void print(uint8_t x, uint8_t y, const char *str, uint8_t inverted);
    void print(uint8_t x, uint8_t y, const __FlashStringHelper *str, uint8_t inverted);

    void printLeft(uint8_t y, const char *str);
    void printLeft(uint8_t y, const __FlashStringHelper *str);
    void printLeft(uint8_t y, const char *str, uint8_t inverted);
    void printLeft(uint8_t y, const __FlashStringHelper *str, uint8_t inverted);

    void printCenter(uint8_t row, const char *str);
    void printCenter(uint8_t row, const __FlashStringHelper *str);
    void printCenter(uint8_t row, const char *str, uint8_t inverted);
    void printCenter(uint8_t row, const __FlashStringHelper *str, uint8_t inverted);

    void printRight(uint8_t row, const char *str);
    void printRight(uint8_t row, const __FlashStringHelper *str);
    void printRight(uint8_t row, const char *str, uint8_t inverted);
    void printRight(uint8_t row, const __FlashStringHelper *str, uint8_t inverted);

  private:
    Font font;
    uint8_t _address, Cursor_X, Cursor_Y;
    void sendCommand(uint8_t *commands, size_t count);
    void sendData(const uint8_t *char_ptr, size_t count, uint8_t inverted);
    void setWindow(uint8_t x, uint8_t y, uint8_t wide, uint8_t height);
    void clearArea(uint8_t x, uint8_t y, uint8_t wide, uint8_t height);
    void writeChar(char c, uint8_t inverted);
    uint8_t getStringWidth(const char *str);
    uint8_t getStringWidth(const __FlashStringHelper *str);
};
#endif
