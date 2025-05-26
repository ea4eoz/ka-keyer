#include "screendecoder.hpp"
#include "settings.hpp"
#include "oled.hpp"

#define BUFFER_SIZE 0x20
#define BUFFER_MASK (BUFFER_SIZE - 1)

static uint8_t buffer[BUFFER_SIZE];
static uint8_t remainingPixels;
static uint8_t writePtr;
static uint8_t readPtr;
extern Font fontBig;

// Converts a character to lower case
static uint8_t lowerCase(uint8_t c){
  if (c & 0x80){
    // special char;
    c |= 0x01;
  } else {
    // ascii
    c = tolower(c);
  }
  return c;
}

// Converts a character to upper case
static uint8_t upperCase(uint8_t c){
  if (c & 0x80){
    // special char;
    c &= 0xFE;
  } else {
    // ascii
    c = toupper(c);
  }
  return c;
}

// Clear buffer
static void bufferClear(){
  for (uint8_t p = 0; p < BUFFER_SIZE; p++) {
    buffer[p] = ' ';
  }
}

// Write a character in the buffer
static uint8_t bufferWrite(uint8_t c){
  writePtr = (writePtr + 1) & BUFFER_MASK;
  buffer[writePtr] = c;
  return writePtr;
}

// Read the character at position p
static uint8_t bufferRead(uint8_t p) {
  return buffer[p & BUFFER_MASK];
}

// Get position of last written character
static uint8_t getBufferPosition(){
  return writePtr;
}

// Initializes the screen decoder
void screenDecoderBegin() {
  remainingPixels = 0;
  bufferClear();
}

// Add a character to the screen decoder
uint8_t screenDecoderWrite(uint8_t c) {
  return bufferWrite(c);
}

void screenDecoderUpdateDecoderLine(uint8_t c){
  readPtr = bufferWrite(c);
  remainingPixels = SSD1306_WIDTH;
}

void screenDecoderRefreshDecoderLine(){
  readPtr = getBufferPosition();
  remainingPixels = SSD1306_WIDTH;
}

void screenDecoderClearDecoderLine(){
  oledClearRows(5, 3);
}

void screenDecoderProcess(){
  if (remainingPixels){
    oledSetFont(&fontBig);
    uint8_t ch = bufferRead(readPtr);
    if (settingsGetDecoderUpperCase()){
      ch = upperCase(ch);
    } else {
      ch = lowerCase(ch);
    }
    uint8_t charWidth = oledGetCharWidth(ch);
    if (remainingPixels >= charWidth){
      remainingPixels = remainingPixels - charWidth;
      oledWriteChar(remainingPixels, 5, ch);
      readPtr--;
    } else {
      oledClearArea(0, 5, remainingPixels, oledGetCharHeight());
      remainingPixels = 0;
    }
  }
}