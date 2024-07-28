#include <Arduino.h>
#include <Wire.h>
#include "font.h"
#include "oled.h"

// SSD1306 commands
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_DEFAULT_ADDRESS     0x78
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_SETPAGE             0xB0
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SWITCHCAPVCC        0x02
#define SSD1306_NOP                 0xE3

// SSD1306 Addresing modes
#define HORIZONTAL_ADDRESSING_MODE  0x00
#define VERTICAL_ADDRESSING_MODE    0x01
#define PAGE_ADDRESSING_MODE        0x02

// SSD1306 Control bytes
#define CB_CMD          0x80
#define CB_CMD_STREAM   0x00
#define CB_DATA         0xC0
#define CB_DATA_STREAM  0x40

OLED::OLED(uint8_t address){
  _address = address;
}

void OLED::begin(){
  uint8_t commands[] = {
    SSD1306_DISPLAYOFF,              // display off
    SSD1306_MEMORYMODE, HORIZONTAL_ADDRESSING_MODE, // Page Addressing mode
    SSD1306_COMSCANDEC,              // Scan from 127 to 0 (Reverse scan)
    SSD1306_SETSTARTLINE | 0x00,     // First line to start scanning from
    SSD1306_SETCONTRAST, 0x7F,       // contast value to 0x7F according to datasheet
    SSD1306_SEGREMAP | 0x01,         // Use reverse mapping. 0x00 - is normal mapping
    SSD1306_NORMALDISPLAY,
    SSD1306_SETMULTIPLEX, 63,        // Reset to default MUX. See datasheet
    SSD1306_SETDISPLAYOFFSET, 0x00,  // no offset
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,// set to default ratio/osc frequency
    SSD1306_SETPRECHARGE, 0x22,      // switch precharge to 0x22 // 0xF1
    SSD1306_SETCOMPINS, 0x12,        // set divide ratio
    SSD1306_SETVCOMDETECT, 0x20,     // vcom deselect to 0x20 // 0x40
    SSD1306_CHARGEPUMP, 0x14,        // Enable charge pump
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_DISPLAYON,
  };

  // I²C initialization
  Wire.begin();
  Wire.setClock(400000);
  
  // SSD1306 initialization
  sendCommand(commands, sizeof(commands));
  
  // Clear screen and move cursor to (0,0)
  clearScreen();
  
  // Set a default font
  setFont(0);
}

void OLED::sendCommand(uint8_t *commands, size_t count){
  Wire.beginTransmission(_address);
  Wire.write(CB_CMD_STREAM);
  while(count){
    Wire.write(*commands);
    commands++;
    count--;
  }
  Wire.endTransmission();
}

void OLED::sendData(const uint8_t *char_ptr, size_t count, uint8_t inverted){
  Wire.beginTransmission(_address);
  Wire.write(CB_DATA_STREAM);
  while(count){
    uint8_t data = pgm_read_byte(char_ptr);
    if (inverted) data = ~data;
    if (Wire.write(data) == 0){
        // Buffer full, start new transaction
        Wire.endTransmission();
        Wire.beginTransmission(_address);
        Wire.write(CB_DATA_STREAM);
        Wire.write(data); // The byte that failed
    }
    char_ptr++;
    count--;
  }
  Wire.endTransmission();
}

void OLED::setFont(uint8_t newFont){
  switch(newFont){
    case 0:
      font.setFont(font_bitmap);
      break;

    case 1:
      font.setFont(font_big);
      break;

    case 2:
      font.setFont(keyer_bitmap);
      break;
  }
}

void OLED::setWindow(uint8_t x, uint8_t y, uint8_t wide, uint8_t height){
  uint8_t commands[] = {
    SSD1306_COLUMNADDR, x, (uint8_t)(x + wide - 1),
    SSD1306_PAGEADDR, y, (uint8_t)(y + height - 1),
  };
  sendCommand(commands, sizeof(commands));
}

void OLED::clearScreen(){
  clearArea(0, 0, 128, 8);
  Cursor_X = 0;
  Cursor_Y = 0;
}

void OLED::clearArea(uint8_t x, uint8_t y, uint8_t wide, uint8_t height){
  int bytes_in_area = wide * height;
  setWindow(x, y, wide, height);
  Wire.beginTransmission(_address);
  Wire.write(CB_DATA_STREAM);
  for (int n = 0; n < bytes_in_area; n++){
    if(Wire.write(0x00) == 0){
      // Buffer full, start new transaction
      Wire.endTransmission();
      Wire.beginTransmission(_address);
      Wire.write(CB_DATA_STREAM);
      Wire.write(0x00); // The byte that failed
    }
  }
  Wire.endTransmission();
  Cursor_X = x + wide;
}

void OLED::writeChar(char c, uint8_t inverted) {
  uint8_t width = font.getCharWidth(c);
  uint8_t height = font.getCharHeight();
  const uint8_t *ptr = font.getCharPointer(c);
  setWindow(Cursor_X, Cursor_Y, width, height);
  sendData(ptr, width * height, inverted);
  Cursor_X += width;
}

uint8_t OLED::getStringWidth(const char *str){
  uint8_t width = 0;
  while(*str){
    width += font.getCharWidth(*str);
    str++;
  }
  return width;
}

uint8_t OLED::getStringWidth(const __FlashStringHelper *str){
  uint8_t width = 0;
  const char* p = (const char*) str;
  while(pgm_read_byte(p)){
    width += font.getCharWidth(pgm_read_byte(p));
    p++;
  }
  return width;
}

void OLED::print(uint8_t x, uint8_t y, const char *str){
  Cursor_X = x;
  Cursor_Y = y;
  while(*str){
    writeChar(*str, 0);
    str++;
  }
}

void OLED::print(uint8_t x, uint8_t y, const __FlashStringHelper *str){
  uint8_t data;
  const char* p = (const char*) str;
  Cursor_X = x;
  Cursor_Y = y;
  while((data = pgm_read_byte(p))){
    writeChar(data, 0);
    p++;
  }
}

void OLED::print(uint8_t x, uint8_t y, const char *str, uint8_t inverted){
  Cursor_X = x;
  Cursor_Y = y;
  while(*str){
    writeChar(*str, inverted);
    str++;
  }
}

void OLED::print(uint8_t x, uint8_t y, const __FlashStringHelper *str, uint8_t inverted){
  uint8_t data;
  const char* p = (const char*) str;
  Cursor_X = x;
  Cursor_Y = y;
  while((data = pgm_read_byte(p))){
    writeChar(data, inverted);
    p++;
  }
}

void OLED::printLeft(uint8_t y, const char *str){
  print(0, y, str);
}

void OLED::printLeft(uint8_t y, const __FlashStringHelper *str){
  print(0, y, str);
}

void OLED::printLeft(uint8_t y, const char *str, uint8_t inverted){
  print(0, y, str, inverted);
}

void OLED::printLeft(uint8_t y, const __FlashStringHelper *str, uint8_t inverted){
  print(0, y, str, inverted);
}

void OLED::printCenter(uint8_t row, const char *str){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = (128/2) - (strwidth / 2);
  print(column, row, str);
}

void OLED::printCenter(uint8_t row, const __FlashStringHelper *str){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = (128/2) - (strwidth / 2);
  print(column, row, str);
}

void OLED::printCenter(uint8_t row, const char *str, uint8_t inverted){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = (128/2) - (strwidth / 2);
  print(column, row, str, inverted);
}

void OLED::printCenter(uint8_t row, const __FlashStringHelper *str, uint8_t inverted){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = (128/2) - (strwidth / 2);
  print(column, row, str, inverted);
}

void OLED::printRight(uint8_t row, const char *str){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = 128 - strwidth;
  print(column, row, str);
}

void OLED::printRight(uint8_t row, const __FlashStringHelper *str){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = 128 - strwidth;
  print(column, row, str);
}

void OLED::printRight(uint8_t row, const char *str, uint8_t inverted){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = 128 - strwidth;
  print(column, row, str, inverted);
}

void OLED::printRight(uint8_t row, const __FlashStringHelper *str, uint8_t inverted){
  uint8_t strwidth = getStringWidth(str);
  uint8_t column = 128 - strwidth;
  print(column, row, str, inverted);
}
