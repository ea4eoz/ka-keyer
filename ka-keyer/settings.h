#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#define ABS_SPEED_MAX 99
#define ABS_SPEED_MIN 10
#define ABS_WEIGHT_MAX 75
#define ABS_WEIGHT_MIN 25
#define ABS_RATIO_MAX 40
#define ABS_RATIO_MIN 20

class Settings{
  public:
    Settings();
    void begin();
    void saveSettings();
    uint8_t getSpeed_max();
    uint8_t getSpeed_min();
    uint8_t getWeight_max();
    uint8_t getWeight_min();
    uint8_t getRatio_max();
    uint8_t getRatio_min();
    uint8_t getSpeed();
    uint8_t getWeight();
    uint8_t getRatio();
    uint8_t getSidetone();
    int getSidetone_freq();
    uint8_t getKeyer();
    uint8_t getReverse();
    unsigned long get_cw_clock();
    unsigned long get_cw_clock_x2();
    unsigned long get_cw_clock_x7();
    unsigned long get_cw_dit_keydown();
    unsigned long get_cw_dit_keyup();
    unsigned long get_cw_dah_keydown();
    unsigned long getSwitchpointDelay();
    unsigned long get_ptt_timeout();
    void setSpeedRange(uint8_t min, uint8_t max);
    void setWeightRange(uint8_t min, uint8_t max);
    void setRatioRange(uint8_t min, uint8_t max);
    void setSpeed(uint8_t value);
    void setWeight(uint8_t value);
    void setRatio(uint8_t value);
    void setSidetone(uint8_t value);
    void setSidetone_freq(int value);
    void setKeyer(uint8_t value);
    void setReverse(uint8_t value);
  private:
    struct EEpromArray{
      uint8_t speed_max;
      uint8_t speed_min;
      uint8_t weight_max;
      uint8_t weight_min;
      uint8_t ratio_max;
      uint8_t ratio_min;
      uint8_t speed;
      uint8_t weight;
      uint8_t ratio;
      uint8_t keying_mode;
      uint8_t reverse;
      uint8_t sidetone;
      int sidetone_freq;
      uint8_t checksum;
    } PersistentData;
    unsigned long cw_clock;
    unsigned long cw_clock_x2;
    unsigned long cw_clock_x7;
    unsigned long cw_dit_keydown;
    unsigned long cw_dit_keyup;
    unsigned long cw_dah_keydown;
    unsigned long switchpoint_delay;
    unsigned long ptt_timeout;
    void setDefaults();
    uint8_t addPersistentDataItems();
    void updatePersistentDataChecksum();
    uint8_t checkPersistentDataChecksum();
    void updateTimmings();
};

#endif