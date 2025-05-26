#include "settings.hpp"
#include "storage.hpp"
#include "userinterface.hpp"
#include "winkeyer.hpp"

#define SETTINGS_FLAGS_SIDETONE 0x01
#define SETTINGS_FLAGS_REVERSE 0x02
#define SETTINGS_FLAGS_DECODER 0x04
#define SETTINGS_FLAGS_DECODERUPPERCASE 0x08

#define SETTINGS_DECODER_OFF 0
#define SETTINGS_DECODER_UPPERCASE 1
#define SETTINGS_DECODER_LOWERCASE 2

static uint8_t persistentData[SETTINGS_PDATA_SIZE];
static unsigned long cw_clock;
static unsigned long cw_clock_x2;
static unsigned long cw_clock_x7;
static unsigned long cw_dit_keydown;
static unsigned long cw_dit_keyup;
static unsigned long cw_dah_keydown;
static unsigned long ptt_timeout;
static int sidetone_freq;

void settingsSaveSettings() {
  storageSave(persistentData);
}

static void updateTimmings() {
  // Calculates dot clock
  cw_clock = 1200000 / persistentData[SETTINGS_PDATA_SPEED];

  // Some multiples of CW_CLOCK
  cw_clock_x2 = cw_clock * 2;
  cw_clock_x7 = cw_clock * 7;

  // Calculates key down time for dot and given weight
  cw_dit_keydown = (cw_clock_x2 * persistentData[SETTINGS_PDATA_WEIGHT]) / 100;

  // Calculates key up time for dot
  cw_dit_keyup = cw_clock_x2 - cw_dit_keydown;

  // Calculated key down time for dah at given ratio
  cw_dah_keydown = (cw_clock * persistentData[SETTINGS_PDATA_RATIO]) / 10;

  // Calculates the PTT timeout
  ptt_timeout = cw_clock * 16;

  // Sidetone frequency in Hz
  sidetone_freq = persistentData[SETTINGS_PDATA_TONE] * 50;
}

void settingsBegin() {
  // Some sane default values
  persistentData[SETTINGS_PDATA_SPEED] = 25;
  persistentData[SETTINGS_PDATA_WEIGHT] = 50;
  persistentData[SETTINGS_PDATA_RATIO] = 30;
  persistentData[SETTINGS_PDATA_KMODE] = 0;
  persistentData[SETTINGS_PDATA_TONE] = 800 / 50;
  persistentData[SETTINGS_PDATA_FLAGS] = SETTINGS_FLAGS_SIDETONE;
  storageLoad(persistentData);
  updateTimmings();
}

uint8_t settingsGetSpeed() {
  return persistentData[SETTINGS_PDATA_SPEED];
}

void settingsSetSpeed(uint8_t value) {
  if (value < SETTINGS_SPEED_MIN) {
    value = SETTINGS_SPEED_MIN;
  }

  if (value > SETTINGS_SPEED_MAX) {
    value = SETTINGS_SPEED_MAX;
  }

  persistentData[SETTINGS_PDATA_SPEED] = value;
  updateTimmings();
  userInterfaceForceRefresh();
}

uint8_t settingsGetWeight() {
  return persistentData[SETTINGS_PDATA_WEIGHT];
}

void settingsSetWeight(uint8_t value) {
  if (value < SETTINGS_WEIGHT_MIN) {
    value = SETTINGS_WEIGHT_MIN;
  }

  if (value > SETTINGS_WEIGHT_MAX) {
    value = SETTINGS_WEIGHT_MAX;
  }

  persistentData[SETTINGS_PDATA_WEIGHT] = value;
  updateTimmings();
}

uint8_t settingsGetRatio() {
  return persistentData[SETTINGS_PDATA_RATIO];
}

void settingsSetRatio(uint8_t value) {
  if (value < SETTINGS_RATIO_MIN) {
    value = SETTINGS_RATIO_MIN;
  }

  if (value > SETTINGS_RATIO_MAX) {
    value = SETTINGS_RATIO_MAX;
  }
  
  persistentData[SETTINGS_PDATA_RATIO] = value;
  updateTimmings();
}

bool settingsGetSidetone() {
  return (persistentData[SETTINGS_PDATA_FLAGS] & SETTINGS_FLAGS_SIDETONE) != 0;
}

void settingsSetSidetone(bool value) {
  if (value) {
    persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] | SETTINGS_FLAGS_SIDETONE;
  } else {
    persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] & ~SETTINGS_FLAGS_SIDETONE;
  }
}

int settingsGetSidetone_freq() {
  return sidetone_freq;
}

void settingsSetSidetone_freq(int value) {
  persistentData[SETTINGS_PDATA_TONE] = value / 50;
  updateTimmings();
}

uint8_t settingsGetKeyer() {
  return persistentData[SETTINGS_PDATA_KMODE];
}

void settingsSetKeyer(uint8_t value) {
  persistentData[SETTINGS_PDATA_KMODE] = value;
}

bool settingsGetReverse() {
  return (persistentData[SETTINGS_PDATA_FLAGS] & SETTINGS_FLAGS_REVERSE) != 0;
}

void settingsSetReverse(bool value) {
  if (value) {
    persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] | SETTINGS_FLAGS_REVERSE;
  } else {
    persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] & ~SETTINGS_FLAGS_REVERSE;
  }
}

bool settingsGetDecoderUpperCase(){
  return (persistentData[SETTINGS_PDATA_FLAGS] & SETTINGS_FLAGS_DECODERUPPERCASE) != 0;
}

uint8_t settingsGetDecoder(){
  uint8_t value = 0;
  if ((persistentData[SETTINGS_PDATA_FLAGS] & SETTINGS_FLAGS_DECODER) != 0){
    if (settingsGetDecoderUpperCase()){
      value = SETTINGS_DECODER_UPPERCASE;
    } else {
      value = SETTINGS_DECODER_LOWERCASE;
    }
  }
  return value;
}

void settingsSetDecoder(uint8_t value){
  switch (value){
    case SETTINGS_DECODER_OFF:
      persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] & ~SETTINGS_FLAGS_DECODER;
      persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] & ~SETTINGS_FLAGS_DECODERUPPERCASE;
      break;
    case SETTINGS_DECODER_UPPERCASE:
      persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] | SETTINGS_FLAGS_DECODER;
      persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] | SETTINGS_FLAGS_DECODERUPPERCASE;
      break;
    case SETTINGS_DECODER_LOWERCASE:
      persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] | SETTINGS_FLAGS_DECODER;
      persistentData[SETTINGS_PDATA_FLAGS] = persistentData[SETTINGS_PDATA_FLAGS] & ~SETTINGS_FLAGS_DECODERUPPERCASE;
      break;
  }
}

unsigned long settingsGet_cw_clock() {
  return cw_clock;
}

unsigned long settingsGet_cw_clock_x2() {
  return cw_clock_x2;
}

unsigned long settingsGet_cw_clock_x7() {
  return cw_clock_x7;
}

unsigned long settingsGet_cw_dit_keydown() {
  return cw_dit_keydown;
}

unsigned long settingsGet_cw_dit_keyup() {
  return cw_dit_keyup;
}

unsigned long settingsGet_cw_dah_keydown() {
  return cw_dah_keydown;
}
unsigned long settingsGet_ptt_timeout() {
  return ptt_timeout;
}
