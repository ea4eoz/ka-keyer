#include <Arduino.h>

#include "oled.hpp"
#include "font16.hpp"
#include "font24.hpp"
#include "font40numbers.hpp"
#include "fontlogo.hpp"

#include "keyer.hpp"
#include "morse.hpp"
#include "paddles.hpp"
#include "paddlesdecoder.hpp"
#include "ptt.hpp"
#include "rotaryencoder.hpp"
#include "screendecoder.hpp"
#include "settings.hpp"
#include "sound.hpp"
#include "storage.hpp"
#include "userinterface.hpp"
#include "winkeyer.hpp"

#ifdef ARDUINO_AVR_NANO
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define ENCODER_BUTTON A1
#define PADDLE_LEFT 5
#define PADDLE_RIGHT 6
#define KEYER_OUTPUT 11
#define PTT_OUTPUT 12
#define BUZZER_OUTPUT 4
#define KAKEYER_VERSION (F("V 2.2 Nano"))
#endif

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
#define ENCODER_PIN_A 26
#define ENCODER_PIN_B 15
#define ENCODER_BUTTON 14
#define PADDLE_LEFT 7
#define PADDLE_RIGHT 6
#define KEYER_OUTPUT 5
#define PTT_OUTPUT 4
#define BUZZER_OUTPUT 8
#define EEPROM_SDA 0
#define EEPROM_SCL 1
#define OLED_SDA 2
#define OLED_SCL 3
#define KAKEYER_VERSION (F("V 2.2 Pico"))
#endif

void setup() {
  Serial.begin(1200);
  // SplashScreen
#ifdef ARDUINO_AVR_NANO
  oledBegin();
#endif  
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  oledBegin(OLED_SCL, OLED_SDA);
#endif
  oledSetFont(&fontLogo);
  oledPrintCenter(0, F("1"));
  oledSetFont(&fontSmall);
  oledPrintCenter(4, F("KA-Keyer"));
  oledPrintCenter(6, KAKEYER_VERSION);  

  // initialize morse decoder table
  morseBegin();
  
  // Load user configuration
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  storageBegin(EEPROM_SCL, EEPROM_SDA);
#endif
  settingsBegin();

  // Setup rotary encoder and interrupts
  rotaryEncoderConfigure(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BUTTON);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoderHasChanged, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoderHasChanged, CHANGE);

  soundConfigure(BUZZER_OUTPUT);
  keyerConfigure(KEYER_OUTPUT);
  paddlesDecoderConfigure(KEYER_OUTPUT);
  paddlesBegin(PADDLE_LEFT, PADDLE_RIGHT);
  pttConfigure(PTT_OUTPUT, KEYER_OUTPUT);
  screenDecoderBegin();
  winkeyerBegin();

  // Ends splashScreen playing
  // a video of a K letter ;-)
  delay(2000);
  oledSetFont(&fontLogo);
  for (uint8_t n = 0; n < 3; n++) {
    const uint8_t k[] = { 144, 48, 144 }; // Timmings for K
    oledPrintCenter(0, F("0"));
    soundSound(800, k[n]);
    oledPrintCenter(0, F("1"));
    delay(50);
  }
  delay(500);
  oledClearScreen();

  // Start user interface
  userInterfaceStart();
}

void loop() {
  paddlesProcess();
  winkeyerProcess();
  keyerProcess();
  paddlesDecoderProcess();
  pttProcess();
  userInterfaceProcess();
  screenDecoderProcess();
  storageProcess();
}
