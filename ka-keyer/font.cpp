#include "font.h"

// Check if requested char is defined in font
uint8_t Font::checkForBadChar(char c){
  return (c < FontFirstChar) || (c > FontLastChar);
}

// Set a font
void Font::setFont(const uint8_t *newFont){
  CurrentFont = newFont;
  FontWidth = pgm_read_byte(&CurrentFont[0]);
  FontHeight = pgm_read_byte(&CurrentFont[1]);
  FontFirstChar = pgm_read_byte(&CurrentFont[2]);
  FontLastChar = pgm_read_byte(&CurrentFont[3]);
  CurrentFont += FONT_HEADER_SIZE;
}

// Returns char width in pixels
uint8_t Font::getCharWidth(char c){
  uint8_t width;

  if (checkForBadChar(c)){
    return 0;
  }
  
  if (FontWidth){
    // Fixed width font
    width = FontWidth;
  } else {
    // Variable width font
    width = pgm_read_byte(CurrentFont + (c - FontFirstChar));
  }
  return width;
}

// Returns char height in bytes
uint8_t Font::getCharHeight(){
  return FontHeight;
}

// Returns a pointer to the bitmap of requested char
const uint8_t* Font::getCharPointer(char c){
  const uint8_t *ptr;

  if (checkForBadChar(c)){
    return 0;
  }

  if (FontWidth){
    // Fixed width font
    ptr = CurrentFont + ((c - FontFirstChar) * FontWidth * FontHeight);
  } else {
    // Variable width font
    const uint8_t *p = CurrentFont;
    int offset = 0;
    for (int n = 0; n < (c - FontFirstChar); n++){
      offset = offset + (pgm_read_byte(&p[n]) * FontHeight);
    }
    ptr = p + (FontLastChar - FontFirstChar + 1) + offset;
  }
  return ptr;
}