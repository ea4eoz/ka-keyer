#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <Arduino.h>

#define SETTINGS_SPEED_MAX 99
#define SETTINGS_SPEED_MIN 10
#define SETTINGS_WEIGHT_MAX 75
#define SETTINGS_WEIGHT_MIN 25
#define SETTINGS_RATIO_MAX 40
#define SETTINGS_RATIO_MIN 20
#define SETTINGS_SIDETONE_MAX 1500
#define SETTINGS_SIDETONE_MIN 400

#define SETTINGS_PDATA_SPEED 0
#define SETTINGS_PDATA_WEIGHT 1
#define SETTINGS_PDATA_RATIO 2
#define SETTINGS_PDATA_KMODE 3
#define SETTINGS_PDATA_TONE 4
#define SETTINGS_PDATA_FLAGS 5
#define SETTINGS_PDATA_CRC 6
#define SETTINGS_PDATA_SIZE 7

void settingsBegin();
void settingsSaveSettings();
uint8_t settingsGetSpeed();
void settingsSetSpeed(uint8_t value);
uint8_t settingsGetWeight();
void settingsSetWeight(uint8_t value);
uint8_t settingsGetRatio();
void settingsSetRatio(uint8_t value);
bool settingsGetSidetone();
void settingsSetSidetone(bool value);
int settingsGetSidetone_freq();
void settingsSetSidetone_freq(int value);
uint8_t settingsGetKeyer();
void settingsSetKeyer(uint8_t value);
bool settingsGetReverse();
void settingsSetReverse(bool value);
uint8_t settingsGetDecoder();
void settingsSetDecoder(uint8_t value);
bool settingsGetDecoderUpperCase();
unsigned long settingsGet_cw_clock();
unsigned long settingsGet_cw_clock_x2();
unsigned long settingsGet_cw_clock_x7();
unsigned long settingsGet_cw_dit_keydown();
unsigned long settingsGet_cw_dit_keyup();
unsigned long settingsGet_cw_dah_keydown();
unsigned long settingsGet_ptt_timeout();

#endif