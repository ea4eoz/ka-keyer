#include "keyer.hpp"
#include "oled.hpp"
#include "rotaryencoder.hpp"
#include "screendecoder.hpp"
#include "settings.hpp"
#include "sound.hpp"
#include "userinterface.hpp"
#include "winkeyer.hpp"

#define OFF 0
#define ON 1

#define UI_POLLING 100

#define SCREEN_SPEED 0
#define SCREEN_WEIGHT 1
#define SCREEN_RATIO 2
#define SCREEN_DECODER 3
#define SCREEN_SIDETONE 4
#define SCREEN_SIDETONE_FREQ 5
#define SCREEN_KEYER 6
#define SCREEN_PADDLES 7

#define TOTAL_SCREENS 8

extern Font fontSmall;
extern Font fontBig;
extern Font fontNumbers;

static uint8_t currentScreen;
static bool needRefresh;

// User interface strings
static const char header_speed[] PROGMEM = "Speed";
static const char header_weight[] PROGMEM = "Weight";
static const char header_ratio[] PROGMEM = "Ratio";
static const char header_decoder[] PROGMEM = "Decoder";
static const char header_sidetone[] PROGMEM = "Sidetone";
static const char header_keyer[] PROGMEM = "Keyer";
static const char header_paddles[] PROGMEM = "Paddles";
static const char *const header_table[] PROGMEM = {
  header_speed, header_weight, header_ratio,
  header_decoder, header_sidetone, header_sidetone, header_keyer, header_paddles
};

static const char binary_off[] PROGMEM = "OFF";
static const char binary_on[] PROGMEM = "ON";
static const char *const binary_table[] PROGMEM = { 
  binary_off, binary_on
};

static const char keyer_ultimate[] PROGMEM = "Ultimate";
static const char keyer_iambic[] PROGMEM = "Iambic";
static const char keyer_iambica[] PROGMEM = "Iambic A";
static const char keyer_iambicb[] PROGMEM = "Iambic B";
static const char *const keyer_table[] PROGMEM = {
  keyer_ultimate, keyer_iambic, keyer_iambica, keyer_iambicb
};

static const char paddles_normal[] PROGMEM = "Normal";
static const char paddles_reverse[] PROGMEM = "Reverse";
static const char *const paddles_table[] PROGMEM = {
  paddles_normal, paddles_reverse
};

static const char decoder_uppercase[] PROGMEM = "UPPERCASE";
static const char decoder_lowercase[] PROGMEM = "lowercase";
static const char *const decoder_table[] PROGMEM = {
  binary_off, decoder_uppercase, decoder_lowercase
};

static const uint8_t nextScreenSort[TOTAL_SCREENS + 1] PROGMEM = {
  SCREEN_WEIGHT, SCREEN_RATIO, SCREEN_SPEED,
  SCREEN_SIDETONE, SCREEN_SIDETONE_FREQ, SCREEN_KEYER, SCREEN_PADDLES, SCREEN_DECODER
};

static const uint8_t nextScreenLong[TOTAL_SCREENS + 1] PROGMEM = {
  SCREEN_DECODER, SCREEN_DECODER, SCREEN_DECODER,
  SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED
};

static uint8_t getNextScreen(uint8_t btn) {
  switch (btn) {
    case BUTTON_SHORT:
      return pgm_read_byte(&nextScreenSort[currentScreen]);
      break;

    case BUTTON_LONG:
      return pgm_read_byte(&nextScreenLong[currentScreen]);
      break;

    default:
      return SCREEN_SPEED;
      break;
  }
}

void userInterfaceForceRefresh() {
  needRefresh = true;
}

static void updateScreen(uint8_t screen, uint8_t fullFrame) {
  char buffer[14];

  if (fullFrame) {
    if (settingsGetDecoder()){
      oledClearRows(0, 5);
    } else {
      oledClearScreen();
    }
    // print header
    strcpy_P(buffer, (char *)pgm_read_ptr(&(header_table[screen])));
    oledSetFont(&fontSmall);
    oledPrintCenter(0, buffer);
  }

  //sprintf body
  switch (screen) {
    case SCREEN_SPEED:
      sprintf(buffer, "%d", settingsGetSpeed());
      break;

    case SCREEN_WEIGHT:
      sprintf(buffer, "%d%%", settingsGetWeight());
      break;

    case SCREEN_RATIO:
      sprintf(buffer, "%d", settingsGetRatio());
      // Insert decimal point
      buffer[3] = buffer[2];
      buffer[2] = buffer[1];
      buffer[1] = '.';
      break;

    case SCREEN_DECODER:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(decoder_table[settingsGetDecoder()])));
      break;

    case SCREEN_SIDETONE:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(binary_table[settingsGetSidetone()])));
      break;

    case SCREEN_SIDETONE_FREQ:
      sprintf(buffer, "%d Hz", settingsGetSidetone_freq());
      break;

    case SCREEN_KEYER:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(keyer_table[settingsGetKeyer()])));
      break;

    case SCREEN_PADDLES:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(paddles_table[settingsGetReverse()])));
      break;
  }

  // Print body
  switch(screen){
    case SCREEN_SPEED ... SCREEN_RATIO:
      if (settingsGetDecoder()) {
        oledSetFont(&fontBig);
        oledPrintCenter(2, buffer);
      } else {
        oledSetFont(&fontNumbers);
        oledPrintCenter(3, buffer);
      }
      break;

    case SCREEN_DECODER ... SCREEN_PADDLES:
      oledSetFont(&fontSmall);
      oledPrintCenter(3, buffer);
      break;
  }
}

void userInterfaceSetCurrentScreen(uint8_t scr) {
  currentScreen = scr;
  updateScreen(currentScreen, 1);
}

void userInterfaceStart() {
  userInterfaceSetCurrentScreen(SCREEN_SPEED);
}

static void userInterfaceRefreshUI() {
  updateScreen(currentScreen, 0);
  needRefresh = false;
}

void userInterfaceProcess() {
  static unsigned long last_check = 0;
  uint8_t button, value;
  int hz;
  // Checks if it is time to check the user controls
  if ((millis() - last_check) < UI_POLLING) {
    return;
  }

  button = rotaryEncoderRead();

  switch (button) {
    case BUTTON_NONE:
      // Check if screen need refresh
      if (needRefresh) {
        userInterfaceRefreshUI();
      }
      break;

    case BUTTON_SHORT:
    case BUTTON_LONG:
      soundBeep();
      userInterfaceSetCurrentScreen(getNextScreen(button));
      //settings.saveSettings();
      break;

    case ROTARYENCODER_RIGHT:
    case ROTARYENCODER_LEFT:
      switch (currentScreen) {
        case SCREEN_SPEED:
          value = settingsGetSpeed();
          if (button == ROTARYENCODER_RIGHT) {
            value++;
            if (value > SETTINGS_SPEED_MAX) {
              soundBeep();
              value = SETTINGS_SPEED_MAX;
            }
          } else {
            value--;
            if (value < SETTINGS_SPEED_MIN) {
              soundBeep();
              value = SETTINGS_SPEED_MIN;
            }
          }

          settingsSetSpeed(value);
          if (winkeyerGetHostOpened()) {
            winkeyerSendPotentiometer();
          }
          break;

        case SCREEN_WEIGHT:
          value = settingsGetWeight();
          if (button == ROTARYENCODER_RIGHT) {
            value++;
            if (value > SETTINGS_WEIGHT_MAX) {
              soundBeep();
              value = SETTINGS_WEIGHT_MAX;
            }
          } else {
            value--;
            if (value < SETTINGS_WEIGHT_MIN) {
              soundBeep();
              value = SETTINGS_WEIGHT_MIN;
            }
          }
          settingsSetWeight(value);
          break;

        case SCREEN_RATIO:
          value = settingsGetRatio();
          if (button == ROTARYENCODER_RIGHT) {
            value++;
            if (value > SETTINGS_RATIO_MAX) {
              soundBeep();
              value = SETTINGS_RATIO_MAX;
            }
          } else {
            value--;
            if (value < SETTINGS_RATIO_MIN) {
              soundBeep();
              value = SETTINGS_RATIO_MIN;
            }
          }
          settingsSetRatio(value);
          break;

        case SCREEN_DECODER:
          value = settingsGetDecoder();
          if (button == ROTARYENCODER_RIGHT) {
            // Right
            value = (value + 1) % 3;
          } else {
            // Left
            if (value) {
              value--;
            } else {
              value = 2;
            }
          }
          settingsSetDecoder(value);
          switch(value){
            case 0:
              screenDecoderClearDecoderLine();
              break;

            case 1:
            case 2:
              screenDecoderRefreshDecoderLine();
              break;
          }
          break;

        case SCREEN_SIDETONE:
          value = settingsGetSidetone();
          if (value) {
            value = 0;
          } else {
            value = 1;
          }
          settingsSetSidetone(value);
          break;

        case SCREEN_SIDETONE_FREQ:
          hz = settingsGetSidetone_freq();
          if (button == ROTARYENCODER_RIGHT) {
            hz += 50;
            if (hz > SETTINGS_SIDETONE_MAX) {
              soundBeep();
              hz = SETTINGS_SIDETONE_MAX;
            }
          } else {
            hz -= 50;
            if (hz < SETTINGS_SIDETONE_MIN) {
              soundBeep();
              hz = SETTINGS_SIDETONE_MIN;
            }
          }
          settingsSetSidetone_freq(hz);
          break;

        case SCREEN_KEYER:
          value = settingsGetKeyer();
          if (button == ROTARYENCODER_RIGHT) {
            value++;
          } else {
            value--;
          }
          value = value & 0x03;
          settingsSetKeyer(value);
          break;

        case SCREEN_PADDLES:
          value = settingsGetReverse();
          value = (value + 1) % 2;
          settingsSetReverse(value);
          break;
      }
      settingsSaveSettings();
      userInterfaceRefreshUI();
      break;
  }
  last_check = millis();
}