#ifndef FONTDATA_H
#define FONTDATA_H

#include <Arduino.h>

#define FONT_HEADER_SIZE 4

static const uint8_t font_bitmap[] PROGMEM = {
    0x0A, // Font width
    0x02, // Font height
    0x20, // First character
    0x7E, // Last character

    // *space*
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // !
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x19, 0x19, 0x00, 0x00, 0x00, 0x00,

    // "
    0x00, 0x00, 0x1E, 0x1E, 0x00, 0x00, 0x1E, 0x1E, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // #
    0x00, 0x30, 0xFE, 0xFE, 0x30, 0x30, 0xFE, 0xFE, 0x30, 0x00,
    0x00, 0x03, 0x1F, 0x1F, 0x03, 0x03, 0x1F, 0x1F, 0x03, 0x00,

    // $
    0x00, 0x70, 0xF8, 0xD8, 0xFE, 0x98, 0x98, 0x00, 0x00, 0x00,
    0x00, 0x0C, 0x18, 0x18, 0x7F, 0x19, 0x1F, 0x0F, 0x00, 0x00,
    
    // %
    0x1C, 0xB6, 0xA2, 0xF6, 0xCC, 0x60, 0x60, 0x20, 0x30, 0x00,
    0x01, 0x01, 0x00, 0x0E, 0x1B, 0x11, 0x1B, 0x0E, 0x00, 0x00,
    
    // &
    0x00, 0x00, 0xDC, 0x7E, 0xE6, 0xC6, 0x86, 0x0C, 0x80, 0x80,
    0x00, 0x07, 0x0F, 0x1C, 0x18, 0x19, 0x1F, 0x1E, 0x1F, 0x13,
    
    // '
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // (
    0x00, 0x00, 0x00, 0xE0, 0xFC, 0x1E, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x07, 0x3F, 0x78, 0x40, 0x00, 0x00, 0x00,
    
    // )
    0x00, 0x00, 0x02, 0x1E, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x40, 0x78, 0x3F, 0x07, 0x00, 0x00, 0x00, 0x00,
    
    // *
    0x00, 0x48, 0x78, 0x30, 0xFE, 0x30, 0x78, 0x48, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,

    // +
    0x00, 0x80, 0x80, 0x80, 0xF0, 0xF0, 0x80, 0x80, 0x80, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x0F, 0x0F, 0x01, 0x01, 0x01, 0x00,

    // ,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x40, 0x7C, 0x3C, 0x00, 0x00, 0x00, 0x00,

    // -
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00,

    // .
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00,

    // /
    0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0x3C, 0x0E, 0x02, 0x00,
    0x00, 0x20, 0x38, 0x1E, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00,

    // 0
    0x00, 0xF0, 0xFC, 0x0E, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00,
    0x00, 0x03, 0x0F, 0x1C, 0x18, 0x18, 0x1C, 0x0F, 0x07, 0x00,

    // 1
    0x00, 0x00, 0x0C, 0x06, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x18, 0x00,

    /// 2
    0x00, 0x0C, 0x06, 0x06, 0x06, 0x86, 0xC6, 0x7C, 0x38, 0x00,
    0x00, 0x18, 0x1C, 0x1E, 0x1B, 0x19, 0x18, 0x18, 0x18, 0x00,

    // 3
    0x00, 0x0C, 0x06, 0xC6, 0xC6, 0xC6, 0xC6, 0xFC, 0x38, 0x00,
    0x00, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x19, 0x0F, 0x0F, 0x00,

    // 4
    0x00, 0x80, 0xC0, 0x70, 0x18, 0x0E, 0xFE, 0xFE, 0x00, 0x00,
    0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1F, 0x1F, 0x03, 0x00,

    // 5
    0x00, 0xFE, 0x7E, 0x66, 0x66, 0x66, 0xE6, 0xC6, 0x80, 0x00,
    0x00, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x0F, 0x07, 0x00,

    // 6
    0x00, 0xF0, 0xFC, 0xCE, 0x66, 0x66, 0xE6, 0xCC, 0x80, 0x00,
    0x00, 0x07, 0x0F, 0x1C, 0x18, 0x18, 0x1C, 0x0F, 0x07, 0x00,

    // 7
    0x00, 0x06, 0x06, 0x06, 0x06, 0xE6, 0xFE, 0x3E, 0x0E, 0x00,
    0x00, 0x00, 0x10, 0x1C, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00,

    // 8
    0x00, 0x38, 0xFC, 0xC6, 0xC6, 0xC6, 0xC6, 0xFC, 0x38, 0x00,
    0x00, 0x0F, 0x0F, 0x18, 0x18, 0x18, 0x18, 0x0F, 0x0F, 0x00,

    // 9
    0x00, 0x78, 0xFC, 0xCE, 0x86, 0x86, 0xCE, 0xFC, 0xF8, 0x00,
    0x00, 0x00, 0x0C, 0x19, 0x19, 0x19, 0x1C, 0x0F, 0x03, 0x00,

    // :
    0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00,

    // ;
    0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x40, 0x7C, 0x3C, 0x00, 0x00, 0x00, 0x00,
    
    // <
    0x00, 0x80, 0x80, 0xC0, 0x40, 0x60, 0x60, 0x20, 0x30, 0x00,
    0x00, 0x01, 0x01, 0x03, 0x02, 0x06, 0x06, 0x04, 0x0C, 0x00,
    
    // =
    0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00,
    0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00,
    
    // >
    0x00, 0x30, 0x20, 0x60, 0x60, 0x40, 0xC0, 0x80, 0x80, 0x00,
    0x00, 0x0C, 0x04, 0x06, 0x06, 0x02, 0x03, 0x01, 0x01, 0x00,

    // ?
    0x00, 0x0C, 0x06, 0x86, 0xC6, 0x66, 0x7E, 0x1C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1B, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // @
    0xE0, 0xF0, 0x38, 0x9C, 0xCC, 0xCC, 0xDC, 0xF8, 0xF0, 0x00,
    0x07, 0x1F, 0x38, 0x73, 0x67, 0x66, 0x66, 0x77, 0x27, 0x00,
    
    // A
    0x00, 0x00, 0xC0, 0xFC, 0x3E, 0x3E, 0xFC, 0xC0, 0x00, 0x00,
    0x00, 0x18, 0x1F, 0x0F, 0x03, 0x03, 0x0F, 0x1F, 0x18, 0x00,
    
    // B
    0x00, 0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xFC, 0x3C, 0x00,
    0x00, 0x1F, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x0F, 0x00,
    
    // C
    0x00, 0xF0, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x06, 0x0C, 0x00,
    0x00, 0x03, 0x0F, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x00,
    
    // D
    0x00, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0x0C, 0xFC, 0xF0, 0x00,
    0x00, 0x1F, 0x1F, 0x18, 0x18, 0x18, 0x0C, 0x0F, 0x03, 0x00,
    
    // E
    0x00, 0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00,
    0x00, 0x1F, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
    
    // F
    0x00, 0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x06, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // G
    0x00, 0xF0, 0xFC, 0x0C, 0x06, 0x86, 0x86, 0x86, 0x8C, 0x00,
    0x00, 0x03, 0x0F, 0x0C, 0x18, 0x19, 0x19, 0x1F, 0x0F, 0x00,
    
    // H
    0x00, 0xFE, 0xFE, 0xC0, 0xC0, 0xC0, 0xC0, 0xFE, 0xFE, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00,
    
    // I
    0x00, 0x00, 0x06, 0x06, 0xFE, 0xFE, 0x06, 0x06, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x00, 0x00,
    
    // J
    0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0xFE, 0xFE, 0x00,
    0x00, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0F, 0x0F, 0x00,
    
    // K
    0x00, 0xFE, 0xFE, 0xE0, 0xF0, 0xFC, 0x0E, 0x06, 0x02, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x03, 0x0F, 0x1E, 0x18, 0x00,
    
    // L
    0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1F, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
    
    // M
    0x00, 0xFE, 0xFE, 0x3E, 0xF0, 0xF0, 0x3E, 0xFE, 0xFE, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x01, 0x01, 0x00, 0x1F, 0x1F, 0x00,
    
    // N
    0x00, 0xFE, 0xFE, 0x1E, 0xF0, 0xC0, 0x00, 0xFE, 0xFE, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x03, 0x1E, 0x1F, 0x1F, 0x00,
    
    // O
    0x00, 0xF0, 0xFC, 0x0E, 0x06, 0x06, 0x0E, 0xFC, 0xF0, 0x00,
    0x00, 0x03, 0x0F, 0x1C, 0x18, 0x18, 0x1C, 0x0F, 0x03, 0x00,
    
    // P
    0x00, 0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x7C, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // Q
    0x00, 0xF0, 0xFC, 0x0E, 0x06, 0x06, 0x0E, 0xFC, 0xF0, 0x00,
    0x00, 0x03, 0x0F, 0x1C, 0x18, 0x18, 0x3C, 0x6F, 0x07, 0x00,
    
    // R
    0x00, 0xFE, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xFC, 0x3C, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x01, 0x03, 0x1F, 0x1E, 0x10,
    
    // S
    0x00, 0x38, 0x7C, 0xE6, 0xC6, 0xC6, 0xC6, 0x8C, 0x00, 0x00,
    0x00, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x19, 0x0F, 0x0F, 0x00,
    
    // T
    0x00, 0x06, 0x06, 0x06, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00,
    
    // U
    0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00,
    0x00, 0x07, 0x0F, 0x1C, 0x18, 0x18, 0x1C, 0x0F, 0x07, 0x00,
    
    // V
    0x00, 0x06, 0xFE, 0xFC, 0x00, 0x00, 0xFC, 0xFE, 0x06, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00,
    
    // W
    0x1E, 0xFE, 0xE0, 0x00, 0xF0, 0xF0, 0x00, 0xE0, 0xFE, 0x1E,
    0x00, 0x1F, 0x1F, 0x1E, 0x01, 0x01, 0x1E, 0x1F, 0x1F, 0x00,
    
    // X
    0x00, 0x02, 0x0E, 0x3E, 0xF8, 0xF8, 0x3E, 0x0E, 0x02, 0x00,
    0x00, 0x10, 0x1C, 0x1F, 0x03, 0x03, 0x1F, 0x1C, 0x10, 0x00,
    
    // Y
    0x02, 0x0E, 0x3E, 0x78, 0xE0, 0xE0, 0x78, 0x3E, 0x0E, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00,
    
    // Z
    0x00, 0x06, 0x06, 0x86, 0xC6, 0xF6, 0x7E, 0x1E, 0x0E, 0x00,
    0x00, 0x1C, 0x1E, 0x1F, 0x1B, 0x18, 0x18, 0x18, 0x18, 0x00,
    
    // [
    0x00, 0x00, 0x00, 0xFE, 0xFE, 0x02, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00, 0x00,
    
    // *backslash*
    0x00, 0x02, 0x0E, 0x38, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0E, 0x38, 0x20, 0x00,
    
    // ]
    0x00, 0x00, 0x02, 0x02, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x40, 0x40, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00,
    
    // ^
    0x10, 0x18, 0x1C, 0x0E, 0x06, 0x0E, 0x1C, 0x18, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // _
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    
    // `
    0x00, 0x00, 0x01, 0x03, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // a
    0x00, 0x00, 0x60, 0x30, 0xB0, 0xB0, 0xB0, 0xF0, 0xE0, 0x00,
    0x00, 0x0E, 0x1F, 0x1B, 0x19, 0x19, 0x0D, 0x1F, 0x1F, 0x00,
    
    // b
    0x00, 0xFE, 0xFE, 0x60, 0x30, 0x30, 0x70, 0xE0, 0xC0, 0x00,
    0x00, 0x1F, 0x1F, 0x0C, 0x18, 0x18, 0x1C, 0x0F, 0x07, 0x00,
    
    // c
    0x00, 0xC0, 0xE0, 0x70, 0x30, 0x30, 0x30, 0x30, 0x60, 0x00,
    0x00, 0x07, 0x0F, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x00,
    
    // d
    0x00, 0xC0, 0xE0, 0x70, 0x30, 0x30, 0x60, 0xFE, 0xFE, 0x00,
    0x00, 0x07, 0x0F, 0x1C, 0x18, 0x18, 0x0C, 0x1F, 0x1F, 0x00,
    
    // e
    0x00, 0xC0, 0xE0, 0xB0, 0xB0, 0xB0, 0xB0, 0xE0, 0xC0, 0x00,
    0x00, 0x07, 0x0F, 0x1D, 0x19, 0x19, 0x19, 0x19, 0x0D, 0x00,
    
    // f
    0x00, 0x30, 0x30, 0xFC, 0xFE, 0x36, 0x36, 0x36, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // g
    0x00, 0xC0, 0xE0, 0x70, 0x30, 0x30, 0x60, 0xF0, 0xF0, 0x00,
    0x00, 0x03, 0x37, 0x6E, 0x6C, 0x6C, 0x66, 0x7F, 0x3F, 0x00,
    
    // h
    0x00, 0xFE, 0xFE, 0x60, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00,
    
    // i
    0x00, 0x00, 0x30, 0x30, 0xF7, 0xF7, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x18, 0x00,
    
    // j
    0x00, 0x00, 0x30, 0x30, 0xF7, 0xF7, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x60, 0x60, 0x60, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00,
    
    // k
    0x00, 0xFE, 0xFE, 0x80, 0xE0, 0x70, 0x30, 0x10, 0x00, 0x00,
    0x00, 0x1F, 0x1F, 0x01, 0x03, 0x07, 0x1E, 0x18, 0x10, 0x00,
    
    // l
    0x00, 0x06, 0x06, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0F, 0x1F, 0x18, 0x18, 0x18, 0x00, 0x00,
    
    // m
    0x00, 0xF0, 0xF0, 0x30, 0xF0, 0xE0, 0x30, 0xF0, 0xE0, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x1F, 0x1F, 0x00, 0x1F, 0x1F, 0x00,
    
    // n
    0x00, 0xF0, 0xF0, 0x60, 0x30, 0x30, 0x30, 0xF0, 0xE0, 0x00,
    0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00,
    
    // o
    0x00, 0xC0, 0xE0, 0x70, 0x30, 0x30, 0x70, 0xE0, 0xC0, 0x00,
    0x00, 0x07, 0x0F, 0x1C, 0x18, 0x18, 0x1C, 0x0F, 0x07, 0x00,
    
    // p
    0x00, 0xF0, 0xF0, 0x60, 0x30, 0x30, 0x70, 0xE0, 0xC0, 0x00,
    0x00, 0x7F, 0x7F, 0x06, 0x0C, 0x0C, 0x0E, 0x07, 0x03, 0x00,
    
    // q
    0x00, 0xC0, 0xE0, 0x70, 0x30, 0x30, 0x60, 0xF0, 0xF0, 0x00,
    0x00, 0x03, 0x07, 0x0E, 0x0C, 0x0C, 0x06, 0x7F, 0x7F, 0x00,
    
    // r
    0x00, 0x00, 0xF0, 0xF0, 0x60, 0x30, 0x30, 0x30, 0x30, 0x00,
    0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // s
    0x00, 0xE0, 0xF0, 0xB0, 0xB0, 0xB0, 0xB0, 0x30, 0x60, 0x00,
    0x00, 0x0C, 0x19, 0x19, 0x19, 0x19, 0x1B, 0x1F, 0x0E, 0x00,
    
    // t
    0x00, 0x30, 0x30, 0xFC, 0xFC, 0x30, 0x30, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0F, 0x1F, 0x18, 0x18, 0x18, 0x00, 0x00,
    
    // u
    0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00,
    0x00, 0x0F, 0x1F, 0x18, 0x18, 0x18, 0x0C, 0x1F, 0x1F, 0x00,
    
    // v
    0x00, 0x30, 0xF0, 0xE0, 0x00, 0x00, 0xE0, 0xF0, 0x30, 0x00,
    0x00, 0x00, 0x01, 0x0F, 0x1E, 0x1E, 0x0F, 0x01, 0x00, 0x00,
    
    // w
    0x70, 0xF0, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0xF0, 0x70,
    0x00, 0x07, 0x1F, 0x1E, 0x03, 0x03, 0x1E, 0x1F, 0x07, 0x00,
    
    // x
    0x00, 0x10, 0x30, 0xF0, 0xC0, 0xC0, 0xF0, 0x30, 0x10, 0x00,
    0x00, 0x10, 0x18, 0x1E, 0x07, 0x07, 0x1E, 0x18, 0x10, 0x00,
    
    // y
    0x00, 0x10, 0xF0, 0xF0, 0x80, 0x00, 0xF0, 0xF0, 0x30, 0x00,
    0x00, 0x00, 0x60, 0x63, 0x7F, 0x1F, 0x0F, 0x01, 0x00, 0x00,
    
    // z
    0x00, 0x30, 0x30, 0x30, 0x30, 0xB0, 0xF0, 0x70, 0x30, 0x00,
    0x00, 0x18, 0x1C, 0x1E, 0x1B, 0x19, 0x18, 0x18, 0x18, 0x00,
    
    // {
    0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0x02, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x01, 0x3E, 0x7E, 0x40, 0x40, 0x00, 0x00,
    
    // |
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00,
    
    // }
    0x00, 0x00, 0x02, 0x02, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x40, 0x40, 0x7E, 0x3E, 0x01, 0x01, 0x00, 0x00,
    
    // ~
    0x00, 0x80, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x80, 0xC0, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,
  };
 

  static const uint8_t font_big[] PROGMEM = {
    0x00, // Font width (variable)
    0x05, // Font height
    0x2E, // First character
    0x39, // Last character

    0x09, 0x23, 0x1D, 0x1D, 0x1B, 0x1D, 0x1D, 0x1D, 0x1C, 0x1B, 0x1D, 0x1B, // Widths

    // . 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xF8,0xF8,0xF8,0xF8,0xF8,0x00,0x00,
    0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x00,0x00,

    // %
    0x00,0x00,0xF0,0xFC,0xFE,0xFE,0x0F,0x07,0x07,0x07,0x07,0x0F,0x3E,0xFE,0xFC,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xFC,0x3E,0x1E,0x06,0x02,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0F,0x3F,0xFF,0xFF,0xE0,0xC0,0xC0,0xC0,0xC0,0xE0,0xF8,0xFF,0x7F,0x1F,0x00,0x80,0xC0,0xF0,0xF8,0x7C,0x3F,0x0F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x80,0xE0,0xF0,0xFC,0x3E,0x1F,0x07,0xC3,0xE0,0xF0,0xF0,0x78,0x38,0x38,0x38,0x38,0x78,0xF0,0xF0,0xE0,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xF0,0xF8,0x7E,0x1F,0x0F,0x03,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x7F,0x00,0x00,
    0x00,0x00,0x00,0x00,0x04,0x06,0x07,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x0F,0x0E,0x0E,0x0E,0x0E,0x0F,0x07,0x07,0x01,0x00,0x00,0x00,

    // 0
    0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xFC,0x7E,0x3E,0x1E,0x1F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x1E,0x3E,0xFE,0xFC,0xF8,0xF0,0xC0,0x00,0x00,0x00,0x00,
    0x00,0x00,0xF0,0xFF,0xFF,0xFF,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFF,0xE0,0x00,0x00,
    0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x7F,0x00,0x00,
    0x00,0x00,0x00,0x0F,0x3F,0xFF,0xFF,0xFC,0xF0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xF0,0xFE,0xFF,0xFF,0x3F,0x0F,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,

    // 1
    0x00,0x00,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0xE0,0xF0,0xF8,0x7C,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x07,0x07,0x07,0x07,0x03,0x03,0x01,0x01,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,
    0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x00,0x00,

    // 2
    0x00,0x00,0x80,0xE0,0xF0,0xF8,0xFC,0x7E,0x3E,0x1E,0x1F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x3E,0x7E,0xFC,0xFC,0xF8,0xF0,0x80,0x00,0x00,
    0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xF0,0xFF,0xFF,0xFF,0x3F,0x0F,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xE0,0xF0,0xF8,0x7C,0x3E,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xC0,0xE0,0xF8,0xFC,0xFE,0xBF,0x9F,0x8F,0x87,0x83,0x81,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,
    0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x00,0x00,

    // 3
    0x00,0x00,0x00,0x80,0xE0,0xF8,0xFC,0xFC,0x7E,0x3E,0x1E,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x3E,0x7E,0xFC,0xFC,0xF8,0xE0,0x80,0x00,0x00,0x00,
    0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0xC0,0xF0,0xFF,0xFF,0x7F,0x3F,0x07,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x1F,0x3F,0x3D,0xF9,0xF8,0xF0,0xE0,0xC0,0x00,0x00,0x00,
    0x00,0x00,0x0C,0x7C,0xFC,0xFC,0xFC,0xF0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xF0,0xFF,0xFF,0xFF,0x7F,0x0F,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,

    //4
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xFC,0x7E,0xFE,0xFE,0xFE,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xF8,0x7E,0x3F,0x0F,0x07,0x01,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xC0,0xF0,0xF8,0xFC,0xBF,0x8F,0x87,0x83,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x80,0x00,0x00,
    0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x07,0x07,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x00,0x00,0x00,0x00,0x00,0x00,

    // 5
    0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0xFE,0xFE,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0xFF,0x80,0xC0,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x03,0x03,0x07,0x0F,0x1F,0xFF,0xFF,0xFE,0xF8,0xC0,0x00,0x00,
    0x00,0x00,0x18,0x78,0xF8,0xF8,0xF8,0xE0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xE0,0xF8,0xFF,0xFF,0x7F,0x3F,0x07,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,

    // 6
    0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xFC,0xFC,0x3E,0x1E,0x1F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x1E,0x3E,0xFE,0xFC,0xF8,0x60,0x00,0x00,0x00,0x00,
    0x00,0x00,0xF8,0xFF,0xFF,0xFF,0x7F,0x03,0x80,0xC0,0xC0,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFE,0x1F,0x07,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x03,0x03,0x07,0x1F,0xFF,0xFF,0xFE,0xF8,0xC0,0x00,0x00,
    0x00,0x00,0x03,0x1F,0x7F,0xFF,0xFF,0xF8,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xE0,0xF0,0xFF,0xFF,0xFF,0x3F,0x03,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,

    // 7
    0x00,0x00,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0xDE,0xFE,0xFE,0xFE,0x7E,0x3E,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0xF8,0xFE,0x7F,0x3F,0x0F,0x03,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xFC,0xFF,0xFF,0x3F,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xFC,0xFF,0xFF,0xFF,0x3F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    // 8
    0x00,0x00,0x00,0x80,0xF0,0xF8,0xFC,0xFC,0x7E,0x1E,0x1F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1E,0x3E,0x7E,0xFC,0xFC,0xF8,0xE0,0x80,0x00,0x00,0x00,
    0x00,0x00,0x00,0x07,0x3F,0x7F,0xFF,0xFF,0xF0,0xE0,0xC0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xC0,0xE0,0xF0,0xFF,0xFF,0x7F,0x1F,0x07,0x00,0x00,0x00,
    0x00,0x00,0x00,0xE0,0xF0,0xF8,0xFC,0x7C,0x1F,0x0F,0x0F,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0F,0x0F,0x1F,0x7C,0xFC,0xF8,0xF0,0xE0,0x00,0x00,0x00,
    0x00,0x00,0x0F,0x7F,0xFF,0xFF,0xFF,0xF0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xF0,0xFF,0xFF,0xFF,0x7F,0x07,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,

    // 9
    0x00,0x00,0x80,0xE0,0xF8,0xF8,0xFC,0x7E,0x3E,0x1F,0x1F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x3E,0x7E,0xFC,0xFC,0xF8,0xF0,0xC0,0x00,0x00,0x00,
    0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFE,0x00,0x00,
    0x00,0x00,0x01,0x07,0x1F,0x3F,0x7F,0x7E,0x7C,0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0x70,0x78,0x78,0x3C,0x1E,0x8F,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xE0,0xF8,0xFF,0xFF,0x3F,0x1F,0x03,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
  };

  // Keyer images converted thanks to 
  // https://javl.github.io/image2cpp/
  static const uint8_t keyer_bitmap[] PROGMEM = {
    0x50, // Font width 
    0x04, // Font height
    0x30, // First character
    0x31, // Last character

    // Keyer up
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x13, 0xff, 0xff, 0xff, 0x13, 0x10, 
    0x10, 0x10, 0x10, 0xf0, 0xf8, 0xf8, 0xf8, 0x98, 0x98, 0xf8, 0xf8, 0xf0, 0xf0, 0x10, 0x10, 0x18, 
    0x59, 0xfd, 0xff, 0xfd, 0x99, 0x98, 0x10, 0x10, 0x30, 0x70, 0xf0, 0xf0, 0xf0, 0xf0, 0x70, 0x30, 
    0x10, 0x10, 0x10, 0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0xc6, 0xc6, 0xce, 0xce, 
    0xfe, 0xfe, 0xfe, 0xfe, 0xce, 0xce, 0xc6, 0xc6, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf3, 0xff, 0xf3, 0xf0, 0xf0, 
    0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 
    0xf5, 0xff, 0xff, 0xfa, 0xfa, 0xfa, 0xf0, 0xf0, 0xf0, 0xf8, 0xfe, 0xfe, 0xfe, 0xfe, 0xf8, 0xf0, 
    0xf0, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
    0x3f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Keyer down
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x09, 0xff, 0xff, 0xff, 0x09, 0x08, 
    0x18, 0x10, 0x10, 0xf0, 0xf8, 0xf8, 0xf8, 0x98, 0x98, 0xf8, 0xf8, 0xf0, 0xe0, 0x20, 0x20, 0x30, 
    0xb3, 0xfb, 0xff, 0xfb, 0x73, 0x70, 0x20, 0x60, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 
    0x40, 0x40, 0x40, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x38, 0x38, 
    0xf8, 0xf8, 0xf8, 0xf8, 0x38, 0x38, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf1, 0xf7, 0xf1, 0xf0, 0xf0, 
    0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 
    0xf2, 0xff, 0xff, 0xf5, 0xf5, 0xf5, 0xf0, 0xf0, 0xf0, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xf0, 
    0xf0, 0xe0, 0x80, 0x00, 0x01, 0x03, 0x06, 0x0c, 0x08, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
    0x3f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif
