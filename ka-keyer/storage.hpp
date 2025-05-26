#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include "settings.hpp"

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
void storageBegin(uint8_t scl, uint8_t sda);
#endif
void storageLoad(uint8_t *destiny);
void storageSave(uint8_t *d);
void storageProcess();

#endif