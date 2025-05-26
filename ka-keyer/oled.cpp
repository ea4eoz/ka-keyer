#include <Arduino.h>
#include <Wire.h>
#include "oled.hpp"

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
#define Wire Wire1
#endif

// SSD1306 I²C address
#define SSD1306_ADDRESS 0x3C

// SSD1306 commands
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_DEFAULT_ADDRESS 0x78
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETPAGE 0xB0
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SWITCHCAPVCC 0x02
#define SSD1306_NOP 0xE3

// SSD1306 Addresing modes
#define SSD1306_HORI_ADDR_MODE 0x00
#define SSD1306_VERT_ADDR_MODE 0x01
#define SSD1306_PAGE_ADDR_MODE 0x02

// SSD1306 Control bytes
#define SSD1306_CB_CMD 0x80
#define SSD1306_CB_CMD_STREAM 0x00
#define SSD1306_CB_DATA 0xC0
#define SSD1306_CB_DATA_STREAM 0x40

static const uint8_t initCommands[] PROGMEM = {
  SSD1306_CB_CMD_STREAM,
  SSD1306_DISPLAYOFF,                         // display off
  SSD1306_MEMORYMODE, SSD1306_HORI_ADDR_MODE, // Page Addressing mode
  SSD1306_COMSCANDEC,                         // Scan from 127 to 0 (Reverse scan)
  SSD1306_SETSTARTLINE | 0x00,                // First line to start scanning from
  SSD1306_SETCONTRAST, 0x7F,                  // contast value to 0x7F according to datasheet
  SSD1306_SEGREMAP | 0x01,                    // Use reverse mapping. 0x00 - is normal mapping
  SSD1306_NORMALDISPLAY,
  SSD1306_SETMULTIPLEX, 63,                   // Reset to default MUX. See datasheet
  SSD1306_SETDISPLAYOFFSET, 0x00,             // no offset
  SSD1306_SETDISPLAYCLOCKDIV, 0x80,           // set to default ratio/osc frequency
  SSD1306_SETPRECHARGE, 0x22,                 // switch precharge to 0x22 // 0xF1
  SSD1306_SETCOMPINS, 0x12,                   // set divide ratio
  SSD1306_SETVCOMDETECT, 0x20,                // vcom deselect to 0x20 // 0x40
  SSD1306_CHARGEPUMP, 0x14,                   // Enable charge pump
  SSD1306_DISPLAYALLON_RESUME,
  SSD1306_DISPLAYON,
};

static Font *currentFont;
static uint8_t cursorX;
static uint8_t cursorY;

// Set the current font
void oledSetFont(Font *newFont) {
  currentFont = newFont;
}

// Check if char is in font
static bool isValidChar(char c) {
  uint8_t b = (uint8_t)c;
  return (b >= currentFont->firstChar) && ((b - currentFont->firstChar) <= currentFont->charCount);
}

// Returns char's bitmap width in pixels
static uint8_t oledGetBitmapWidth(char c) {
  int charWidth = 0;
  uint8_t b = (uint8_t)c;

  if (isValidChar(c)) {
    charWidth = pgm_read_word(&((currentFont->charWidths)[b - currentFont->firstChar]));
  }

  return charWidth;
}

// Return char's width in pixels
uint8_t oledGetCharWidth(char c){
  uint8_t width = oledGetBitmapWidth(c);
  if (width){
    width = width + 2 * currentFont->columnSpacing;
  }
  return width;
}

// Returns char height in bytes
uint8_t oledGetCharHeight() {
  return currentFont->fontHeight;
}

// Returns a pointer to the bitmap of requested char
static const uint8_t *getBitmapPointer(char c) {
  const uint8_t *ptr = (const uint8_t *)pgm_read_ptr(&(currentFont->charAddresses)[(uint8_t)(c - currentFont->firstChar)]);
  return ptr;
}


// OLED initialization
#ifdef ARDUINO_AVR_NANO
void oledBegin() {
#endif

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
void oledBegin(uint8_t scl, uint8_t sda) {
  Wire.setSCL(scl);
  Wire.setSDA(sda);
#endif

  Wire.begin();
  Wire.setClock(400000);

  // SSD1306 initialization
  Wire.beginTransmission(SSD1306_ADDRESS);
  for (uint8_t count = 0; count < sizeof(initCommands); count++) {
    Wire.write(pgm_read_byte(&initCommands[count]));
  }
  Wire.endTransmission();

  // Clear screen and move cursor to (0,0)
  oledClearScreen();
}

// Send a command to the SSD1306
static void sendCommand(uint8_t *commands, size_t count) {
  Wire.beginTransmission(SSD1306_ADDRESS);
  Wire.write(SSD1306_CB_CMD_STREAM);
  while (count--) {
    Wire.write(*commands);
    commands++;
  }
  Wire.endTransmission();
}

// Send data command to the SSD1306
static void sendData(const uint8_t *char_ptr, size_t count) {
  Wire.beginTransmission(SSD1306_ADDRESS);
  Wire.write(SSD1306_CB_DATA_STREAM);
  while (count--) {
    uint8_t data = pgm_read_byte(char_ptr);
    if (Wire.write(data) == 0) {
      // Buffer full, start new transaction
      Wire.endTransmission();
      Wire.beginTransmission(SSD1306_ADDRESS);
      Wire.write(SSD1306_CB_DATA_STREAM);
      Wire.write(data);  // The byte that failed
    }
    char_ptr++;
  }
  Wire.endTransmission();
}

// Set a display area
static void setArea(uint8_t x, uint8_t y, uint8_t wide, uint8_t height) {
  uint8_t commands[] = {
    SSD1306_COLUMNADDR,
    x,
    (uint8_t)(x + wide - 1),
    SSD1306_PAGEADDR,
    y,
    (uint8_t)(y + height - 1),
  };
  sendCommand(commands, sizeof(commands));
}

// Clear a display area
void oledClearArea(uint8_t x, uint8_t y, uint8_t wide, uint8_t height) {
  int bytes_in_area = wide * height;
  setArea(x, y, wide, height);
  Wire.beginTransmission(SSD1306_ADDRESS);
  Wire.write(SSD1306_CB_DATA_STREAM);
  for (int n = 0; n < bytes_in_area; n++) {
    if (Wire.write(0x00) == 0) {
      // Buffer full, start new transaction
      Wire.endTransmission();
      Wire.beginTransmission(SSD1306_ADDRESS);
      Wire.write(SSD1306_CB_DATA_STREAM);
      Wire.write(0x00);  // The byte that failed
    }
  }
  Wire.endTransmission();
  cursorX = x + wide;
}

// Clear entire rows
void oledClearRows(uint8_t startRow, uint8_t rowCount) {
  oledClearArea(0, startRow, SSD1306_WIDTH, rowCount);
  cursorX = 0;
  cursorY = startRow;
}

// Clear the whole screen
void oledClearScreen() {
  oledClearRows(0, 8);
  cursorX = 0;
  cursorY = 0;
}

// Write a char to screen
static void writeChar(char c) {
  uint8_t width = oledGetBitmapWidth(c);
  uint8_t height = oledGetCharHeight();
  const uint8_t *ptr = getBitmapPointer(c);
  uint8_t blankColumns = currentFont->columnSpacing;
  oledClearArea(cursorX, cursorY, blankColumns, height);
  setArea(cursorX, cursorY, width, height);
  sendData(ptr, width * height);
  cursorX += width;
  blankColumns = currentFont->columnSpacing;
  oledClearArea(cursorX, cursorY, blankColumns, height);
}

// Write a char at the specified coordinates
void oledWriteChar(uint8_t x, uint8_t y, char c) {
  cursorX = x;
  cursorY = y;
  writeChar(c);
}

// Return the width in pixels of a string
static uint8_t getStringWidth(const char *str) {
  uint8_t width = 0;
  while (*str) {
    width += oledGetCharWidth(*str);
    str++;
  }
  return width;
}

// Return the width in pixels of a string
static uint8_t getStringWidth(const __FlashStringHelper *str) {
  uint8_t width = 0;
  const char *p = (const char *)str;
  while (pgm_read_byte(p)) {
    width += oledGetCharWidth(pgm_read_byte(p));
    p++;
  }
  return width;
}

// Write a string at the specified coordinates
static void printStr(uint8_t x, uint8_t y, const char *str) {
  cursorX = x;
  cursorY = y;
  while (*str) {
    writeChar(*str);
    str++;
  }
}

// Write a string at the specified coordinates
static void printStr(uint8_t x, uint8_t y, const __FlashStringHelper *str) {
  uint8_t data;
  const char *p = (const char *)str;
  cursorX = x;
  cursorY = y;
  while ((data = pgm_read_byte(p))) {
    writeChar(data);
    p++;
  }
}

//
// TO-DO: Comprobar columnas negativas y mayores de 127
//
void oledPrintCenter(uint8_t row, const char *str) {
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = (SSD1306_WIDTH / 2) - (strwidth / 2);
  oledClearArea(0, row, column, oledGetCharHeight());
  printStr(column, row, str);
  oledClearArea(column + strwidth, row, SSD1306_WIDTH - (column + strwidth), oledGetCharHeight());
}

// Write a string centered in a row
void oledPrintCenter(uint8_t row, const __FlashStringHelper *str) {
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = (SSD1306_WIDTH / 2) - (strwidth / 2);
  oledClearArea(0, row, column, oledGetCharHeight());
  printStr(column, row, str);
  oledClearArea(column + strwidth, row, SSD1306_WIDTH - (column + strwidth), oledGetCharHeight());
}
