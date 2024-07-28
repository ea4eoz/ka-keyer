#include "EEPROM.h"
#include "settings.h"
#include "winkeyer.h"

#include "userinterface.h"

extern UserInterface ui;
extern Winkeyer winkeyer;

// Class constructor
// Set default values
Settings::Settings(){
  setDefaults();
}

void Settings::setDefaults(){
  // Some sane default values
  PersistentData.speed_max = 99;
  PersistentData.speed_min = 10;
  PersistentData.weight_max = 75;
  PersistentData.weight_min = 25;
  PersistentData.ratio_max = 40;
  PersistentData.ratio_min = 20;
  PersistentData.speed = 25;
  PersistentData.weight = 50;
  PersistentData.ratio = 30;
  PersistentData.keying_mode = 0;
  PersistentData.reverse = 0;
  PersistentData.sidetone = 1;
  PersistentData.sidetone_freq = 800;
}

// Sum all eeprom items
uint8_t Settings::addPersistentDataItems(){
  uint8_t sum = 13; // Number of elements
  sum += PersistentData.speed_max;
  sum += PersistentData.speed_min;
  sum += PersistentData.weight_max;
  sum += PersistentData.weight_min;
  sum += PersistentData.ratio_max;
  sum += PersistentData.ratio_min;
  sum += PersistentData.speed;
  sum += PersistentData.weight;
  sum += PersistentData.ratio;
  sum += PersistentData.keying_mode;
  sum += PersistentData.reverse;
  sum += PersistentData.sidetone;
  sum += PersistentData.sidetone_freq;
  return sum;
}

// Add checksum
void Settings::updatePersistentDataChecksum(){
  uint8_t checksum = addPersistentDataItems();
  PersistentData.checksum = ~checksum + 1;
}

// Check checkSum
uint8_t Settings::checkPersistentDataChecksum(){
  uint8_t checksum = addPersistentDataItems();
  checksum += PersistentData.checksum;
  return checksum;
}

void Settings::begin(){
  EEPROM.get(0, PersistentData);
  uint8_t checksum = checkPersistentDataChecksum();
  if (checksum){
    // Checksum failed. Save
    // default values to eeprom
    setDefaults();
    updatePersistentDataChecksum();
    EEPROM.put(0, PersistentData);
  }
  updateTimmings();
} 

void Settings::saveSettings(){
  updatePersistentDataChecksum();
  EEPROM.put(0, PersistentData);
}

void Settings::updateTimmings(){
  // Calculates dot clock
  cw_clock = 1200000 / PersistentData.speed;

  // Some multiples of CW_CLOCK
  cw_clock_x2 = cw_clock * 2;
  cw_clock_x7 = cw_clock * 7;

  // Calculates key down time for dot and given weight
  cw_dit_keydown = (cw_clock_x2 * PersistentData.weight) / 100;

  // Calculates key up time for dot
  cw_dit_keyup = cw_clock_x2 - cw_dit_keydown;

  // Calculated key down time for dah at given ratio
  cw_dah_keydown = (cw_clock * PersistentData.ratio) / 10;

  // Calculates the PTT timeout
  ptt_timeout = cw_clock * 16;
}

uint8_t Settings::getSpeed_max(){
  return PersistentData.speed_max;
}

uint8_t Settings::getSpeed_min(){
  return PersistentData.speed_min;
}

uint8_t Settings::getWeight_max(){
  return PersistentData.weight_max;
}

uint8_t Settings::getWeight_min(){
  return PersistentData.weight_min;
}

uint8_t Settings::getRatio_max(){
  return PersistentData.ratio_max;
}

uint8_t Settings::getRatio_min(){
  return PersistentData.ratio_min;
}

uint8_t Settings::getSpeed(){
  return PersistentData.speed;
}

uint8_t Settings::getWeight(){
  return PersistentData.weight;
}

uint8_t Settings::getRatio(){
  return PersistentData.ratio;
}

uint8_t Settings::getSidetone(){
  return (PersistentData.sidetone != 0);
}

int Settings::getSidetone_freq(){
  return PersistentData.sidetone_freq;
}

uint8_t Settings::getKeyer(){
  return PersistentData.keying_mode;
}

uint8_t Settings::getReverse(){
  return (PersistentData.reverse != 0);
}

unsigned long Settings::get_cw_clock(){
  return cw_clock;
}

unsigned long Settings::get_cw_clock_x2(){
  return cw_clock_x2;
}

unsigned long Settings::get_cw_clock_x7(){
  return cw_clock_x7;
}

unsigned long Settings::get_cw_dit_keydown(){
  return cw_dit_keydown;
}

unsigned long Settings::get_cw_dit_keyup(){
  return cw_dit_keyup;
}

unsigned long Settings::get_cw_dah_keydown(){
  return cw_dah_keydown;
}
unsigned long Settings::get_ptt_timeout(){
  return ptt_timeout;
}

unsigned long Settings::getSwitchpointDelay(){
  return switchpoint_delay;
}

void Settings::setSpeedRange(uint8_t min, uint8_t max){
  PersistentData.speed_max = max;
  PersistentData.speed_min = min;
  if (PersistentData.speed > max){
    PersistentData.speed = max;
  }
  if (PersistentData.speed < min){
    PersistentData.speed = min;
  }
  updateTimmings();
}

void Settings::setWeightRange(uint8_t min, uint8_t max){
  PersistentData.weight_max = max;
  PersistentData.weight_min = min;
  if (PersistentData.weight > max){
    PersistentData.weight = max;
  }
  if (PersistentData.weight < min){
    PersistentData.weight = min;
  }
  updateTimmings();
}

void Settings::setRatioRange(uint8_t min, uint8_t max){
  PersistentData.ratio_max = max;
  PersistentData.ratio_min = min;
  if (PersistentData.ratio > max){
    PersistentData.ratio = max;
  }
  if (PersistentData.ratio < min){
    PersistentData.ratio = min;
  }
  updateTimmings();
}

void Settings::setSpeed(uint8_t value){
  if (value < ABS_SPEED_MIN){
    value = ABS_SPEED_MIN;
  }

  if (value > ABS_SPEED_MAX){
    value = ABS_SPEED_MAX;
  }

  PersistentData.speed = value;
  updateTimmings();
  ui.forceRefresh();
}

void Settings::setWeight(uint8_t value){
  PersistentData.weight = value;
  updateTimmings();
}

void Settings::setRatio(uint8_t value){
  PersistentData.ratio = value;
  updateTimmings();
}

void Settings::setSidetone(uint8_t value){
  PersistentData.sidetone = (value != 0);
}

void Settings::setSidetone_freq(int value){
  PersistentData.sidetone_freq = value;
}

void Settings::setKeyer(uint8_t value){
  PersistentData.keying_mode = value;
}

void Settings::setReverse(uint8_t value){
  PersistentData.reverse = (value != 0);
}
