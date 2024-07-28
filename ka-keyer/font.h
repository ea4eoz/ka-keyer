#ifndef FONT_H
#define FONT_H

#include "fontdata.h"

#define FONT_NORMAL 0
#define FONT_BIG    1
#define FONT_KEYER  2

class Font{
  public:
    void setFont(const uint8_t *newFont);
    uint8_t getCharWidth(char c);
    uint8_t getCharHeight();
    const uint8_t* getCharPointer(char c);
  private:
    const uint8_t *CurrentFont;
    uint8_t FontWidth;
    uint8_t FontHeight;
    char FontFirstChar;
    char FontLastChar;
    uint8_t checkForBadChar(char c);
};

#endif
