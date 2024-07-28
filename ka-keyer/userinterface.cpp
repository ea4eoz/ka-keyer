#include "font.h"
#include "keyer.h"
#include "oled.h"
#include "rotaryencoder.h"
#include "settings.h"
#include "sound.h"
#include "userinterface.h"
#include "version.h"
#include "winkeyer.h"

#define OFF 0
#define ON 1

#define UI_POLLING 100

#define SCREEN_SPEED          0
#define SCREEN_WEIGHT         1
#define SCREEN_RATIO          2
#define SCREEN_SIDETONE       3
#define SCREEN_SIDETONE_FREQ  4
#define SCREEN_KEYER          5
#define SCREEN_PADDLES        6

extern RotaryEncoder rotaryEncoder;
extern OLED oled;
extern Settings settings;
extern Sound sound;
extern Winkeyer winkeyer;

// User interface strings

const uint8_t nextScreenSort[8] PROGMEM = {
  SCREEN_WEIGHT, SCREEN_RATIO, SCREEN_SPEED,
  SCREEN_SIDETONE_FREQ, SCREEN_KEYER, SCREEN_PADDLES, SCREEN_SIDETONE
};

const uint8_t nextScreenLong[8] PROGMEM = {
  SCREEN_SIDETONE, SCREEN_SIDETONE, SCREEN_SIDETONE,
  SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED, SCREEN_SPEED
};

const char header_speed[] PROGMEM = "Speed";
const char header_weight[] PROGMEM = "Weight";
const char header_ratio[] PROGMEM = "Ratio";
const char header_sidetone[] PROGMEM = "Sidetone";
const char header_sidetonefreq[] PROGMEM = "Sidetone";
const char header_keyer[] PROGMEM = "Keyer";
const char header_paddles[] PROGMEM = "Paddles";

const char *const header_table[] PROGMEM = {
  header_speed, header_weight, header_ratio, 
  header_sidetone, header_sidetonefreq, header_keyer, header_paddles
};

const char binary_off[] PROGMEM = " OFF ";
const char binary_on[] PROGMEM = " ON ";
const char *const binary_table[] PROGMEM = {binary_off, binary_on};

const char keyer_ultimate[] PROGMEM = "Ultimate";
const char keyer_iambic[] PROGMEM = " Iambic ";
const char keyer_iambica[] PROGMEM = "Iambic A";
const char keyer_iambicb[] PROGMEM = "Iambic B";
const char *const keyer_table[] PROGMEM = {keyer_ultimate, keyer_iambic, keyer_iambica, keyer_iambicb};

const char paddles_normal[] PROGMEM = " Normal ";
const char paddles_reverse[] PROGMEM = " Reverse ";
const char *const paddles_table[] PROGMEM = {paddles_normal, paddles_reverse};

void UserInterface::begin(){
  oled.begin();
  splashScreenStart();
}

void UserInterface::start(){
  splashScreenStop();
  setCurrentScreen(SCREEN_SPEED);
}

uint8_t UserInterface::getNextScreen(uint8_t btn){
  uint8_t val = SCREEN_SPEED;

  if (btn == BUTTON_SHORT){
    val = pgm_read_byte(&nextScreenSort[currentScreen]);
  }

  if (btn == BUTTON_LONG){
    val = pgm_read_byte(&nextScreenLong[currentScreen]);
  }

  return val;
}

void UserInterface::setCurrentScreen(uint8_t scr){
  currentScreen = scr;
  updateScreen(currentScreen, 1);
}

void UserInterface::refreshUI(){
  updateScreen(currentScreen, 0);
  needRefresh = 0;
}

void UserInterface::process(){
  static unsigned long last_check = 0;
  uint8_t button, value;
  int hz;
  // Checks if it is time to check the user controls
  if ((millis() - last_check) < UI_POLLING){
    return;  
  }

  button = rotaryEncoder.read();

  switch(button){
    case BUTTON_NONE:
      // Check if screen need refresh
      if (needRefresh){
        refreshUI();
      }
    break;

    case BUTTON_SHORT:
    case BUTTON_LONG:
      sound.beep();
      setCurrentScreen(getNextScreen(button));
      settings.saveSettings();
    break;

    case ROTARYENCODER_RIGHT:
    case ROTARYENCODER_LEFT:
      switch(currentScreen){
        case SCREEN_SPEED:
          value = settings.getSpeed();
          if (button == ROTARYENCODER_RIGHT){
            value++;
            if (value > settings.getSpeed_max()){
              sound.beep();
              value = settings.getSpeed_max();
            } 
          } else {
            value--;
            if (value < settings.getSpeed_min()){
              sound.beep();
              value = settings.getSpeed_min();
            } 
          }

          settings.setSpeed(value);
          if (winkeyer.getHostOpened()){
            winkeyer.sendPotentiometer();
          }
          refreshUI();

        break;

        case SCREEN_WEIGHT:
          value = settings.getWeight();
          if (button == ROTARYENCODER_RIGHT){
            value++;
            if (value > settings.getWeight_max()){
              sound.beep();
              value = settings.getWeight_max();
            } 
          } else {
            value--;
            if (value < settings.getWeight_min()){
              sound.beep();
              value = settings.getWeight_min();
            } 
          }
          settings.setWeight(value);
          refreshUI();
        break;

        case SCREEN_RATIO:
          value = settings.getRatio();
          if (button == ROTARYENCODER_RIGHT){
            value++;
            if (value > settings.getRatio_max()){
              sound.beep();
              value = settings.getRatio_max();
            } 
          } else {
            value--;
            if (value < settings.getRatio_min()){
              sound.beep();
              value = settings.getRatio_min();
            } 
          }
          settings.setRatio(value);
          refreshUI();
        break;

        case SCREEN_SIDETONE:
          value = settings.getSidetone();
          if (value){
            value = 0;
          } else {
            value = 1;
          }
          settings.setSidetone(value);
          refreshUI();
        break;

        case SCREEN_SIDETONE_FREQ:
          hz = settings.getSidetone_freq();
          if (button == ROTARYENCODER_RIGHT){
            hz += 50;
            if (hz > 1500){
              sound.beep();
              hz = 1500;
            } 
          } else {
            hz -= 50;
            if (hz < 400){
              sound.beep();
              hz = 400;
            } 
          }
          settings.setSidetone_freq(hz);
          refreshUI();
        break;

        case SCREEN_KEYER:
          value = settings.getKeyer();
          if (button == ROTARYENCODER_RIGHT){
            value++;
          } else {
            value --;
          }
          value = value & 0x03;
          settings.setKeyer(value);
          refreshUI();
        break;

        case SCREEN_PADDLES:
          value = settings.getReverse();
          value = (value + 1) % 2;
          settings.setReverse(value);
          refreshUI();
        break;
      }
    break;
  }
  last_check = millis();
}

void UserInterface::forceRefresh(){
  needRefresh = 1;
}

void UserInterface::updateScreen(uint8_t screen, uint8_t fullFrame){
  char buffer[14];

  if (fullFrame){
    oled.clearScreen();
    // print header
    oled.setFont(FONT_NORMAL);
    strcpy_P(buffer, (char *)pgm_read_ptr(&(header_table[screen])));
    oled.printCenter(0, buffer);
  }
  
  //print body
  switch(screen){
    case SCREEN_SPEED:
      sprintf(buffer, "%d", settings.getSpeed());
      oled.setFont(FONT_BIG);
      oled.printCenter(3, buffer);
    break;

    case SCREEN_WEIGHT:
      sprintf(buffer, "%d/", settings.getWeight());
      oled.setFont(FONT_BIG);
      oled.printCenter(3, buffer);
    break;

    case SCREEN_RATIO:
      sprintf(buffer, "%d", settings.getRatio());
      buffer[3] = buffer[2];
      buffer[2] = buffer[1];
      buffer[1] = '.';
      oled.setFont(FONT_BIG);
      oled.printCenter(3, buffer);
    break;

    case SCREEN_SIDETONE:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(binary_table[settings.getSidetone()])));
      oled.printCenter(4, buffer);
    break;

    case SCREEN_SIDETONE_FREQ:
      sprintf(buffer, " %d Hz ", settings.getSidetone_freq());
      oled.printCenter(4, buffer);
    break;

    case SCREEN_KEYER:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(keyer_table[settings.getKeyer()])));
      oled.printCenter(4, buffer);
    break;

    case SCREEN_PADDLES:
      strcpy_P(buffer, (char *)pgm_read_ptr(&(paddles_table[settings.getReverse()])));
      oled.printCenter(4, buffer);
    break;
  }
}

void UserInterface::splashScreenStart(){
  oled.setFont(FONT_KEYER);
  oled.printCenter(0, F("0"));
  oled.setFont(FONT_NORMAL);
  oled.printCenter(4, F("KA-Keyer"));
  oled.printCenter(6, KAKEYER_VERSION);
}

void UserInterface::splashScreenStop(){
  const uint8_t b[] = {144, 48, 144}; // Timmings for K
  uint8_t button;
  uint8_t keepinwhile = 1;
  uint8_t counter = 0;

  while ((keepinwhile) && (counter < 20)){
    delay(100);
    button = rotaryEncoder.read();
    if (button == BUTTON_LONG){
      keepinwhile = 0;
    }
    counter++;
  }

  if (button == BUTTON_LONG){
    // long press, go to setup screen
    setupScreen();
    oled.clearScreen();
    splashScreenStart();
    delay(500);
  }

  // Play a video of a K letter ;-)
  oled.setFont(FONT_KEYER);
  for (uint8_t n = 0; n < 3; n++){
    oled.printCenter(0, F("1"));
    sound.sound(800, b[n]);
    oled.printCenter(0, F("0"));
    delay (50);
  }

  delay(500);
  oled.clearScreen();
}

void UserInterface::setupScreen(){
  char buffer[4];
  uint8_t values[6];
  uint8_t keepInWhile = 1;
  uint8_t selected = 0;
  int8_t btn;

  values[0] = settings.getSpeed_min();
  values[1] = settings.getSpeed_max();
  values[2] = settings.getWeight_min();
  values[3] = settings.getWeight_max();
  values[4] = settings.getRatio_min();
  values[5] = settings.getRatio_max();

  sound.beep();
  oled.clearScreen();
  oled.printCenter(2, F("KA-Keyer"));
  oled.printCenter(4, F("Setup"));
  delay(2000);

  oled.clearScreen();
  //        (0, 0, F("XX:  XX - XX"));
  oled.print(0, 0, F("Sp:     -   "));
  oled.print(0, 2, F("We:     -   "));
  oled.print(0, 4, F("Ra:     -   "));

  while(keepInWhile){
    delay(100);
    btn = rotaryEncoder.read();

    if (btn == ROTARYENCODER_RIGHT){
      switch(selected){
        case 0:
          values[0]++;
          if (values[0] > values[1]){
            values[0] = values[1];
            sound.beep();
          }
        break;

        case 1:
          values[1]++;
          if (values[1] > ABS_SPEED_MAX){
            values[1] = ABS_SPEED_MAX;
            sound.beep();
          }
        break;

        case 2:
          values[2]++;
          if (values[2] > values[3]){
            values[2] = values[3];
            sound.beep();
          }
        break;

        case 3:
          values[3]++;
          if (values[3] > ABS_WEIGHT_MAX){
            values[3] = ABS_WEIGHT_MAX;
            sound.beep();
          }
        break;

        case 4:
          values[4]++;
          if (values[4] > values[5]){
            values[4] = values[5];
            sound.beep();
          }
        break;

        case 5:
          values[5]++;
          if (values[5] > ABS_RATIO_MAX){
            values[5] = ABS_RATIO_MAX;
            sound.beep();
          }
        break;
      }
    }

    if (btn == ROTARYENCODER_LEFT){
      switch(selected){
        case 0:
          values[0]--;
          if (values[0] < ABS_SPEED_MIN){
            values[0] = ABS_SPEED_MIN;
            sound.beep();
          }
        break;

        case 1:
          values[1]--;
          if (values[1] < values[0]){
            values[1] = values[0];
            sound.beep();
          }
        break;

        case 2:
          values[2]--;
          if (values[2] < ABS_WEIGHT_MIN){
            values[2] = ABS_WEIGHT_MIN;
            sound.beep();
          }
        break;

        case 3:
          values[3]--;
          if (values[3] < values[2]){
            values[3] = values[2];
            sound.beep();
          }
        break;

        case 4:
          values[4]--;
          if (values[4] < ABS_RATIO_MIN){
            values[4] = ABS_RATIO_MIN;
            sound.beep();
          }
        break;

        case 5:
          values[5]--;
          if (values[5] < values[4]){
            values[5] = values[4];
            sound.beep();
          }
        break;
      }
    }
    
    if (btn == BUTTON_SHORT){
      sound.beep();
      selected = (selected + 1) % 7;
    }

    if ((btn == BUTTON_LONG) && (selected == 6)){
        keepInWhile = 0;
    }

    for (uint8_t n = 0; n < 7; n++){
      switch (n){
        case 0:
          sprintf(buffer, "%d", values[0]);
          oled.print(50, 0, buffer, selected == n);
          break;

        case 1:
          sprintf(buffer, "%d", values[1]);
          oled.print(100, 0, buffer, selected == n);
          break;

        case 2:
          sprintf(buffer, "%d", values[2]);
          oled.print(50, 2, buffer, selected == n);
          break;

        case 3:
          sprintf(buffer, "%d", values[3]);
          oled.print(100, 2, buffer, selected == n);
          break;

        case 4:
          sprintf(buffer, "%d", values[4]);
          oled.print(50, 4, buffer, selected == n);
          break;

        case 5:
          sprintf(buffer, "%d", values[5]);
          oled.print(100, 4, buffer, selected == n);
          break;

        case 6:
          oled.printCenter(6, F("Save"),selected == n);
        break;
      }
    }
  }

  settings.setSpeedRange(values[0], values[1]);
  settings.setWeightRange(values[2], values[3]);
  settings.setRatioRange(values[4], values[5]);
  settings.saveSettings();

  sound.beep();

  oled.clearScreen();
  oled.printCenter(2, F("Setup"));
  oled.printCenter(4, F("Saved"));

  delay(2000);
}
