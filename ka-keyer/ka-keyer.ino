// Keyer configuration
#define WPM_LOW 15
#define WPM_HIGH 70
#define WEIGHT_LOW 30
#define WEIGHT_HIGH 50
#define RATIO_LOW 30
#define RATIO_HIGH 40

/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program.  If not, see
   <http://www.gnu.org/licenses/>
*/

// Docs
//
// https://k1el.tripod.com/files/WK2_Datasheet_v23.pdf
//
// https://hamcrafters2.com/files/WinkeyInterfaceGuide.pdf

#include "EEPROM.h"

#define VERSION " V1 "

#define OFF 0
#define ON 1

// Open CW Keyer MK2 hardware
#define PADDLE_DIT 2
#define PADDLE_DAH 6
#define KEYING_OUTPUT 11
#define PTT_OUTPUT 12
#define BUZZER_OUTPUT 4
#define POTENTIOMETER A0
#define BUTTONS_ARRAY A1

#define MAX_MORSE_LENGTH 7
#define DECODING_BUFFER_SIZE (MAX_MORSE_LENGTH + 2)

#define BUTTONS_POLLING 100
#define POTENTIOMETER_POLLING 200
#define LONG_BUTTON_PRESS (1000 / BUTTONS_POLLING)

#define CW_PLAY_WPM 25
#define CW_PLAY_WEIGHT 50
#define CW_PLAY_RATIO 30

#define SIDETONE_FREQUENCY 800
#define TONE_DURATION 50
#define TONE_HIGH 1200
#define TONE_LOW 800
#define TONE_SPACING 20

//    Settings.flags:
//   +--------+--------+--------+--------+--------+--------+---------+--------+
//   |    7   |    6   |    5   |    4   |    3   |    2   |    1    |    0   |
//   +--------+--------+--------+--------+--------+--------+---------+--------+
//   |                                   |Sidetone|  Swap  |LocalEcho| Keying |
#define BIT_KEYING 0
#define BIT_LOCALECHO 1
#define BIT_SWAP_PADDLES 2
#define BIT_SIDETONE 3

//    Settings.modes:
//   +--------+--------+--------+--------+--------+--------+--------+--------+
//   |    7   |    6   |    5   |    4   |    3   |    2   |    1   |    0   |
//   +--------+--------+--------+--------+--------+--------+--------+--------+
//   |                                   |    Keyer mode   |PotentiometerMode|
#define BIT_POTENTIOMETER_MODE 0
#define MASK_POTENTIOMETER_MODE 0x03
#define POTENTIOMETER_MODE_WPM 0
#define POTENTIOMETER_MODE_WEIGHT 1
#define POTENTIOMETER_MODE_RATIO 2
#define POTENTIOMETER_MODES 3

#define BIT_KEYER_MODE 2
#define MASK_KEYER_MODE 0x03
#define KEYER_MODE_ULTIMATE 0
#define KEYER_MODE_IAMBIC_A 1
#define KEYER_MODE_IAMBIC_B 2
#define KEYER_MODES 3

#define POTENTIOMETER_THRESHOLD (1024 / (WPM_HIGH - WPM_LOW))

// Keyer commands
#define KCMD_NONE        0x00
#define KCMD_SPACE       0x01
#define KCMD_DIT_KEYDOWN 0x02
#define KCMD_DIT_KEYUP   0x03
#define KCMD_DAH_KEYDOWN 0x04
#define KCMD_DAH_KEYUP   0x05
#define KCMD_SPEED       0x06
#define KCMD_WEIGHT      0x07
#define KCMD_RATIO       0x08
#define KCMD_SIDETONE    0x09
#define KCMD_KEYING      0x0A
#define KCMD_POTSPEED    0x0B
#define KCMD_LOCALECHO   0x0C

#define KEYER_BUFFER_SIZE 256
#define KEYER_BUFFER_SIZE_MASK (KEYER_BUFFER_SIZE - 1)

// Winkeyer
#define WINKEYER_BUFFER_SIZE 0x80
#define WINKEYER_BUFFER_MASK 0x7F
#define WINKEYER_CMDBUFFER_SIZE 0x20
#define WK_MODE_1 1
#define WK_MODE_2 2
#define WK_PADDLE_ECHO_BIT 0x40
#define WK_SERIAL_ECHO_BIT 0x04
#define WK_BUSY_BIT 0x04
#define WK_XOFF_BIT 0x01
#define WK_STATUS_DEFAULT 0xC0
#define WK_BUFFER_MODE_NORMAL 0
#define WK_BUFFER_MODE_OVERWRITE 1
#define WK_BUFFER_MODE_APPEND 2

#define NOPADDLE 0
#define DITPADDLE 1
#define DAHPADDLE 2

#define SERIAL_BUFFER_SIZE 0x10
#define SERIAL_BUFFER_MASK (SERIAL_BUFFER_SIZE - 1)

// Check speeds, weights and ratios for out of range
#if WPM_LOW < 15
#error WPM_LOW MUST BE AT LEAST 15
#endif

#if WPM_HIGH > 70
#error WPM_HIGH MUST BE AT MOST 70
#endif

#if WPM_LOW > WPM_HIGH
#error WPM_LOW CAN NOT BE HIGHER THAN WPM_HIGH
#endif

#if WEIGHT_LOW < 30
#error WEIGHT_LOW MUST BE AT LEAST 30
#endif

#if WEIGHT_HIGH > 50
#error WEIGHT_HIGH MUST BE AT MOST 50
#endif

#if WEIGHT_LOW > WEIGHT_HIGH
#error WEIGHT_LOW CAN NOT BE HIGHER THAN WEIGHT_HIGH
#endif

#if RATIO_LOW < 30
#error RATIO_LOW MUST BE AT LEAST 30
#endif

#if RATIO_HIGH > 40
#error RATIO_HIGH MUST BE AT MOST 40
#endif

#if RATIO_LOW > RATIO_HIGH
#error RATIO_LOW CAN NOT BE HIGHER THAN RATIO_HIGH
#endif

struct SETTINGS{
  byte wpm;     // Speed in WPM
  byte weight;  // dit weight
  byte ratio;   // dah ratio
  byte flags;
  byte modes;   
  unsigned long cw_clock;
  unsigned long cw_clock_x2;
  unsigned long cw_clock_x7;
  unsigned long cw_dit_keydown;
  unsigned long cw_dit_keyup;
  unsigned long cw_dah_keydown;
  unsigned long ptt_timeout;
} Settings;



struct WINKEYER{
  byte mode;      // winkeyer actual mode
  byte status;    // Winkeyer status byte
  byte modereg;   // Winkeyer mode register
  byte pot_min;   // Minimum potentiometer speed value
  byte pot_range; // Potentiometer speed range
  byte old_speed; // For buffered speed change
} Winkeyer;



struct EEPROMDATA{
  byte weight;       // dih weight
  byte ratio;        // dit/dah ratio
  byte keyer;        // Keyer mode
  byte swap;         // Swap paddles? 
  byte sidetone;     // Sidetone
  byte checksum;     // Checksum
};



// Table of Morse sequences.
static const char MorseChars[][MAX_MORSE_LENGTH + 1] PROGMEM = {
  //Sequence   number Ascii
  "",       //   0    <Err>
  "-----",  //   1      0
  ".----",  //   2      1
  "..---",  //   3      2
  "...--",  //   4      3
  "....-",  //   5      4
  ".....",  //   6      5
  "-....",  //   7      6
  "--...",  //   8      7
  "---..",  //   9      8
  "----.",  //  10      9
  ".-",     //  11      A
  "-...",   //  12      B
  "-.-.",   //  13      C
  "-..",    //  14      D
  ".",      //  15      E
  "..-.",   //  16      F
  "--.",    //  17      G
  "....",   //  18      H
  "..",     //  19      I
  ".---",   //  20      J
  "-.-",    //  21      K
  ".-..",   //  22      L
  "--",     //  23      M
  "-.",     //  24      N
  "---",    //  25      O
  ".--.",   //  26      P
  "--.-",   //  27      Q
  ".-.",    //  28      R
  "...",    //  29      S
  "-",      //  30      T
  "..-",    //  31      U
  "...-",   //  32      V
  ".--",    //  33      W
  "-..-",   //  34      X
  "-.--",   //  35      Y
  "--..",   //  36      Z
  " ",      //  37      SPACE
  ".-..-.", //  38      "
  "...-..-",//  39      $
  ".----.", //  40      '
  "-.--.",  //  41      (
  "-.--.-", //  42      )
  ".-.-.",  //  43      +
  "-....-", //  44      -
  "-..-.",  //  45      /
  "---...", //  46      :
  "-.-.-.", //  47      ;
  "..--..", //  48      ?
  "-...-",  //  49      =
  "..--.-", //  50      _
  ".--.-.", //  51      @
  "-...-.-",//  52      *
  ".-.-.-", //  53      .
  "--..--", //  54      ,
  "-.-.--", //  55      !
  ".-...",  //  56      &
  // Add more here
  "X"       // End of table
};



// Ascii table of Morse sequences
static const byte AsciiMorse[256] PROGMEM = {
// x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 1x
    0, 55, 38,  0, 39,  0, 56, 40, 41, 42, 52, 43, 54, 44, 53, 45, // 2x
    1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 46, 47,  0, 49,  0, 48, // 3x
   51, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 4x
   26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,  0,  0,  0,  0, 50, // 5x
    0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 6x
   26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,  0,  0,  0,  0,  0, // 7x
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 8x
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 9x
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // Ax
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // Bx
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // Cx
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // Dx
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // Ex
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  // Fx
};



// Table for decoding CW sequences. It is generated during
// initialization using MorseChars and AsciiMorse arrays
static byte DecodingTable[256];



///////////////////////////////////////////////////////////////////////////////
// Initialization
void setup() {
  // Hardware init
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PADDLE_DIT, INPUT);
  pinMode(PADDLE_DAH, INPUT);
  pinMode(KEYING_OUTPUT, OUTPUT);
  pinMode(PTT_OUTPUT, OUTPUT);
  pinMode(BUZZER_OUTPUT, OUTPUT);
  setSerial(1200);
  
  // Some sane default values at power-up
  setWPM(CW_PLAY_WPM);
  setWeight(CW_PLAY_WEIGHT);
  setRatio(CW_PLAY_RATIO);
  setPotentiometerMode(POTENTIOMETER_MODE_WPM);
  setKeyerMode(KEYER_MODE_ULTIMATE);
  setFlag(BIT_SWAP_PADDLES, OFF);
  setFlag(BIT_SIDETONE, ON);
  setFlag(BIT_KEYING, ON);
  setFlag(BIT_LOCALECHO, ON);
  
  // Some winkeyer defaults
  Winkeyer.mode = OFF;
  Winkeyer.status = WK_STATUS_DEFAULT;
  Winkeyer.modereg = 0x00;
  Winkeyer.pot_min = 5;
  Winkeyer.pot_range = 63;
  Winkeyer.old_speed = 0;

  decodingTableInit();
  loadSettings();
  cwPlay("K");
}



// Main loop
void loop() {
  task_potentiometer();
  task_buttons();
  task_decoder();
  task_ptt();
  task_serialin();
  task_keyer();
  task_serialout();
}



///////////////////////////////////////////////////////////////////////////////
// Decoding buffer
static char DecodingBuffer[DECODING_BUFFER_SIZE];
static byte DecodingBuffer_ptr;



// Add an item followed by a \0 the the decoding buffer
void DecodingBufferWrite(char c){
  if (paddleEcho()){
    if (DecodingBuffer_ptr < (DECODING_BUFFER_SIZE - 1)){
      DecodingBuffer[DecodingBuffer_ptr] = c;
      DecodingBuffer[DecodingBuffer_ptr + 1] = '\0';
      DecodingBuffer_ptr++;
    }
  }
}



// Empty decoding buffer
void DecodingBufferClear(){
  DecodingBuffer_ptr = 0;
  DecodingBuffer[DecodingBuffer_ptr] = 0;
}



byte DecodingBufferCount(){
  return DecodingBuffer_ptr;
}



///////////////////////////////////////////////////////////////////////////////
// Keyer buffer
static byte KeyerBuffer[KEYER_BUFFER_SIZE]; // Keyer buffer
static byte KeyerBuffer_writeptr = 0; // Keyer buffer write pointer
static byte KeyerBuffer_readptr = 0;  // Keyer buffer read pointer
static int KeyerBuffer_count = 0;     // Keyer buffer counter



// Add a byte into keyer buffer
void KeyerBufferWrite(byte b){
  if (KeyerBufferCount() < KEYER_BUFFER_SIZE){
    KeyerBuffer[KeyerBuffer_writeptr++] = b;
    KeyerBuffer_writeptr &= KEYER_BUFFER_SIZE_MASK;
    KeyerBuffer_count++;
  }
}



// Read a byte from keyer buffer
byte KeyerBufferRead(){
  byte b = KeyerBuffer[KeyerBuffer_readptr++];
  KeyerBuffer_readptr &= KEYER_BUFFER_SIZE_MASK;
  KeyerBuffer_count--;
  return b;
}



// return the number of bytes in keyer buffer
int KeyerBufferCount(){
  return KeyerBuffer_count;
}



///////////////////////////////////////////////////////////////////////////////
// Command buffer
static byte CommandBuffer[WINKEYER_CMDBUFFER_SIZE];
static byte CommandBuffer_ptr;



void CommandBufferClear(){
  CommandBuffer_ptr = 0;
}



void CommandBufferWrite(byte b){
  if (CommandBufferCount() < WINKEYER_CMDBUFFER_SIZE){
    CommandBuffer[CommandBuffer_ptr++] = b;
  }
}



byte CommandBufferCount(){
  return CommandBuffer_ptr;
}



///////////////////////////////////////////////////////////////////////////////
// Winkeyer buffer
static byte WinKeyerBuffer[WINKEYER_BUFFER_SIZE]; // WinKeyerBuffer
static byte WinKeyerBuffer_writeptr;    // WinKeyerBuffer write pointer
static byte WinKeyerBuffer_readptr;     // WinKeyerBuffer read pointer
static byte WinKeyerBuffer_mode;        // WinkeyerBuffer actual mode
static byte WinKeyerBuffer_cmd16ptr;    // WinkeyerBuffer Command 16 pointer



void WinKeyerBufferClear(){
  WinKeyerBuffer_readptr = 0;
  WinKeyerBuffer_writeptr = 0;
  WinKeyerBuffer_mode = WK_BUFFER_MODE_NORMAL;
  WinKeyerBufferResetCmd16Ptr();
  updateWinKeyerStatus();
}



void WinKeyerBufferResetCmd16Ptr(){
  WinKeyerBuffer_cmd16ptr = 0;
}



void WinKeyerBufferWrite(byte data){
  switch (WinKeyerBuffer_mode){
    case WK_BUFFER_MODE_NORMAL:
      WinKeyerBuffer[WinKeyerBuffer_writeptr++] = data;
      WinKeyerBuffer_writeptr &= WINKEYER_BUFFER_MASK;
    break;

    case WK_BUFFER_MODE_OVERWRITE:
    case WK_BUFFER_MODE_APPEND:
      WinKeyerBuffer[WinKeyerBuffer_cmd16ptr++] = data;
      WinKeyerBuffer_cmd16ptr &= WINKEYER_BUFFER_MASK;
    break;
  }

  if (WinKeyerBuffer_cmd16ptr > WinKeyerBuffer_writeptr){
    WinKeyerBuffer_writeptr = WinKeyerBuffer_cmd16ptr;
  }

  updateWinKeyerStatus();
}



byte WinKeyerBufferRead(){
  byte data = WinKeyerBuffer[WinKeyerBuffer_readptr++];
  WinKeyerBuffer_readptr &= WINKEYER_BUFFER_MASK;
  updateWinKeyerStatus();
  return data;
}



byte WinKeyerBufferCount(){
  return (WinKeyerBuffer_writeptr - WinKeyerBuffer_readptr) & WINKEYER_BUFFER_MASK;
}



void WinKeyerBufferBackSpace(){
  if (WinKeyerBufferCount()){
    WinKeyerBuffer_writeptr = (WinKeyerBuffer_writeptr - 1) & WINKEYER_BUFFER_MASK;
  }
  updateWinKeyerStatus();
}



void WinKeyerBufferSetCmd16Ptr(byte p){
  if (p){
    WinKeyerBuffer_cmd16ptr = p + 1; // ??? N1MM
  } else {
    WinKeyerBuffer_cmd16ptr = 0;
  }
}



void WinKeyerBufferSetMode(byte mode){
  WinKeyerBuffer_mode = mode;
}



///////////////////////////////////////////////////////////////////////////////

static byte SerialBuffer[SERIAL_BUFFER_SIZE];
static byte SerialBuffer_writeptr = 0;
static byte SerialBuffer_readptr = 0;
static byte SerialBuffer_count = 0;



void SerialBufferClear(){
  SerialBuffer_writeptr = 0;
  SerialBuffer_readptr = 0;
  SerialBuffer_count = 0;
}



void SerialBufferWrite(byte b){
  if (SerialBufferCount() < SERIAL_BUFFER_SIZE){
    SerialBuffer[SerialBuffer_writeptr++] = b;
    SerialBuffer_writeptr &= SERIAL_BUFFER_MASK;
    SerialBuffer_count++;
  }
}



byte SerialBufferRead(){
  byte b = SerialBuffer[SerialBuffer_readptr++];
  SerialBuffer_readptr &= SERIAL_BUFFER_MASK;
  SerialBuffer_count--;
  return b;
}



byte SerialBufferCount(){
  return SerialBuffer_count;
}



///////////////////////////////////////////////////////////////////////////////
void updateWinKeyerStatus(){
  if (Winkeyer.mode){
    byte status = WK_STATUS_DEFAULT;
    byte s = WinKeyerBufferCount();

    // Checks for busy bit
    if (s){
      status |= WK_BUSY_BIT;
    }

    // Check if buffer is at 2/3 of capacity
    if (s > 85){ // 2/3 of 128
      status |= WK_XOFF_BIT;
    }

    if (status != Winkeyer.status){
      Winkeyer.status = status;
      Serial.write(status);
    }
  }
}



///////////////////////////////////////////////////////////////////////////////



// Open / re-open the serial port at given speed
void setSerial(unsigned long baud){
   if (Serial){
     Serial.end();
   }
   // Warning: Winkeyer uses 8N2
   Serial.begin(baud, SERIAL_8N2);
}



void setWPM(byte wpm){
  Settings.wpm = wpm;
  updateTimmings();
}



byte getWPM(){
  return Settings.wpm;
}



void setWeight(byte weight){
  Settings.weight = weight;
  updateTimmings();
}



byte getWeight(){
  return Settings.weight;
}



void setRatio(byte ratio){
  Settings.ratio = ratio;
  updateTimmings();
}



byte getRatio(){
  return Settings.ratio;
}



void setPotentiometerMode(byte p){
  p &= MASK_POTENTIOMETER_MODE;
  Settings.modes = (Settings.modes & (~(MASK_POTENTIOMETER_MODE << BIT_POTENTIOMETER_MODE))) | (p << BIT_POTENTIOMETER_MODE);
}



byte getPotentiometerMode(){
  return (Settings.modes & (MASK_POTENTIOMETER_MODE << BIT_POTENTIOMETER_MODE)) >> BIT_POTENTIOMETER_MODE;
}



void setKeyerMode(byte km){
  km &= MASK_KEYER_MODE;
  Settings.modes = (Settings.modes & (~(MASK_KEYER_MODE << BIT_KEYER_MODE))) | (km << BIT_KEYER_MODE);
}



byte getKeyerMode(){
  return (Settings.modes & (MASK_KEYER_MODE << BIT_KEYER_MODE)) >> BIT_KEYER_MODE;
}



void setFlag(byte flag, byte status){
  if (status){
    // 1-ON-True
    Settings.flags |= (1 << flag);
  } else {
    // 0-OFF-False
    Settings.flags &= ~(1 << flag);
  }
}



byte getFlag(byte flag){
  return Settings.flags & (1 << flag);
}



// Creates the decoding table using the Morse sequences
// and the corresponding Ascii chars
void decodingTableInit(){
  char buffer[MAX_MORSE_LENGTH + 1];

  for (int n = 1; n < 256; n++){ // Yes, for n = 1
    strcpy_P(buffer, MorseChars[n]);
    if (buffer[0] == 'X'){
      break;
    } else {
      DecodingTable[getTableIndex(buffer)] = findAscii(n);
    }
  }
}



// Computes the table index in DecodingTable
// for a given Morse sequence
byte getTableIndex(const char* sequence){
  byte index = 0;
  while (*sequence){
    switch (*sequence){
      case '.':
        index = (index << 1) + 1;
      break;

      case '-':
        index = (index << 1) + 2;
      break;
    }
    sequence++;
  }
  return index;
}



// Returns the ascii equivalent of a Morse sequence
byte findAscii(byte morseSequenceNumber){
  for (int n = 0; n < 256; n++){
    byte i = (byte) n;
    if (pgm_read_byte(&AsciiMorse[i]) == morseSequenceNumber){
      return i;
    }
  }
  return 0;
}



// Save user settings in EEPROM
void saveSettings(){
  struct EEPROMDATA eepromValues;
  byte checksum = 0;
  
  eepromValues.weight = Settings.weight;
  checksum += eepromValues.weight;

  eepromValues.ratio = Settings.ratio;
  checksum += eepromValues.ratio;

  eepromValues.keyer = getKeyerMode();
  checksum += eepromValues.keyer;

  eepromValues.swap = getFlag(BIT_SWAP_PADDLES);
  checksum += eepromValues.swap;

  eepromValues.sidetone = getFlag(BIT_SIDETONE);
  checksum += eepromValues.sidetone;

  eepromValues.checksum = ~checksum + 1;

  EEPROM.put(0, eepromValues);

}



// Load user settings in EEPROM
void loadSettings(){
  struct EEPROMDATA eepromValues;
  byte checksum = 0;

  EEPROM.get(0, eepromValues);
  
  checksum += eepromValues.weight;
  checksum += eepromValues.ratio;
  checksum += eepromValues.keyer;
  checksum += eepromValues.swap;
  checksum += eepromValues.sidetone;
  checksum += eepromValues.checksum;

  if (checksum == 0x00){
    // We have good values
    setWeight(eepromValues.weight);
    setRatio(eepromValues.ratio);
    setKeyerMode(eepromValues.keyer);
    setFlag(BIT_SWAP_PADDLES, eepromValues.swap);
    setFlag(BIT_SIDETONE, eepromValues.sidetone);
  }
}



// Plays an Ascii sequence of chars using the built-in
// buzzer without keying or PTTing the transceiver
void cwPlay(const char* sequence){
  // Save previous keying parameters
  byte wpm = getWPM();
  byte weight = getWeight();
  byte ratio = getRatio();
  byte sidetone = getFlag(BIT_SIDETONE);
  byte keying = getFlag(BIT_KEYING);
  byte localecho = getFlag(BIT_LOCALECHO);
  
  // Set new keying parameters
  cwCommand(KCMD_SPEED, CW_PLAY_WPM);
  cwCommand(KCMD_WEIGHT, CW_PLAY_WEIGHT);
  cwCommand(KCMD_RATIO, CW_PLAY_RATIO);
  cwCommand(KCMD_SIDETONE, ON);
  cwCommand(KCMD_KEYING, OFF);
  cwCommand(KCMD_LOCALECHO, ON);

  // Play CW
  while(*sequence){
    cwKeying(*sequence);
    sequence++;
  }

  // Restore previous keying parameters
  cwCommand(KCMD_SPEED, wpm);
  cwCommand(KCMD_WEIGHT, weight);
  cwCommand(KCMD_RATIO, ratio);
  cwCommand(KCMD_SIDETONE, sidetone);
  cwCommand(KCMD_KEYING, keying);
  cwCommand(KCMD_LOCALECHO, localecho);
}



void task_potentiometer(){
  static unsigned long last_check = 0;

  // Checks if it is time to check the user controls
  if ((millis() - last_check) < POTENTIOMETER_POLLING){
    return;  
  }

  int potVal = readPotentiometer();

  if (potVal >= 0){
    switch (getPotentiometerMode()){
      case POTENTIOMETER_MODE_WPM:
        cwCommand(KCMD_POTSPEED, map((long)potVal, 0, 1023, WPM_LOW, WPM_HIGH));
      break;

      case POTENTIOMETER_MODE_WEIGHT:
        cwCommand(KCMD_WEIGHT, map((long)potVal, 0, 1023, WEIGHT_LOW, WEIGHT_HIGH));
      break;

      case POTENTIOMETER_MODE_RATIO:
        cwCommand(KCMD_RATIO, map((long)potVal, 0, 1023, RATIO_LOW, RATIO_HIGH));
      break;
    }
  }
  last_check = millis();

}



// Read buttons and execute assigned functions
void task_buttons(){
  static unsigned long last_check = 0;

  // Checks if it is time to check the user controls
  if ((millis() - last_check) < BUTTONS_POLLING){
    return;  
  }

  switch (readButton()){
    // 0 means no button pressed
    case 1:
      potentiometerModeToggle();
    break;

    case 2:
      sidetoneToggle();
    break;

    case 3:
      playCurrentSettings();
    break;

    case 4:
      keyerToggle();
    break;

    case 5:
      paddleToggle();
    break;

    case 6:
      cwPlay(VERSION);
    break;
  }
  last_check = millis();
}



// Reads the paddles and makes
// corresponding dihs and dahs

byte readPaddles(){
  static byte lastKeying = 0;
  static byte lastPaddles = 0;
  
  byte paddles = 0;
  byte retval = NOPADDLE;

  if (getFlag(BIT_SWAP_PADDLES)){
    // tip = dah, ring = dih
    if (digitalRead(PADDLE_DAH) == LOW){
      paddles |= 0x01;
    }

    if (digitalRead(PADDLE_DIT) == LOW){
      paddles |= 0x02;
    }  
  } else {
    // tip = dih, ring = dah
    if (digitalRead(PADDLE_DAH) == LOW){
      paddles |= 0x02;
    }

    if (digitalRead(PADDLE_DIT) == LOW){
      paddles |= 0x01;
    }  
  }

  switch (paddles){
    // 00 : No paddles
    case 0:
      switch(getKeyerMode()){
        case KEYER_MODE_IAMBIC_B:
          if (lastPaddles == 3){
            if (lastKeying == 1){
              retval = DAHPADDLE;
              lastKeying = 0;
            }
            if (lastKeying == 2){
              retval = DITPADDLE;
              lastKeying = 0;
            }
          }
        break;

        default:
          lastKeying = 0;
        break;
      }
    break;

    // 01: Dih paddle only
    case 1:
      retval = DITPADDLE;
      lastKeying = 1;
    break;

    // 10: Dah paddle only
    case 2:
      retval = DAHPADDLE;
      lastKeying = 2;
    break;

    // Both paddles
    case 3:
      switch(getKeyerMode()){
        case KEYER_MODE_ULTIMATE:
          if (lastKeying == 1) retval = DAHPADDLE;
          if (lastKeying == 2) retval = DITPADDLE;
        break;

        case KEYER_MODE_IAMBIC_A:
        case KEYER_MODE_IAMBIC_B:
          switch(lastKeying){
            case 1:
              retval = DAHPADDLE;
              lastKeying = 2;
            break;

            case 2:
              retval = DITPADDLE;
              lastKeying = 1;
            break;
          }
        break;
      }
    break;
  }

  lastPaddles = paddles;

  return retval;
}



// Decodes the decoding buffer and print it using the serial port
void task_decoder(){
  static unsigned long last_paddle;
  static byte last_space_state = 1;
  static byte last_state = 1;
  static byte space_enable = 0;
  unsigned long elapsed;
  byte state, space_state;

  if (paddleEcho()){

    // Sniff the keying output
    if (digitalRead(KEYING_OUTPUT)) {
      last_paddle = micros();
    }

    //State for letter and space
    elapsed = micros() - last_paddle;
    state = elapsed > Settings.cw_clock_x2;
    space_state = elapsed > Settings.cw_clock_x7;

    // Decoding time?
    if ((state) & (!last_state)){
      // Only if buffer has something
      if (DecodingBufferCount()){
        char c = (char)DecodingTable[getTableIndex(DecodingBuffer)];
        if (c) {
          // Known character
          Serial.print(c);
        } else {
          // Unknown character
          Serial.print(F("["));
          Serial.print(DecodingBuffer);
          Serial.print(F("]"));
        }
        space_enable = ON;
        // Clear buffer
        DecodingBufferClear();
      }
    }

    // Space decoding time?
    if ((space_state) & (!last_space_state) & space_enable){
      Serial.print(' ');
      space_enable = OFF;
    }

    // Save states for next round
    last_state = state;
    last_space_state = space_state;
  } else {
    // Clear buffer
    DecodingBufferClear();
  }
}



void task_ptt(){
  static byte last_state;
  static unsigned long last_ptting;
  byte state;
  unsigned long elapsed;

  // Sniff the keying output
  if (digitalRead(KEYING_OUTPUT)) {
    last_ptting = micros();
    digitalWrite(PTT_OUTPUT, ON);
  }

  //State for ptt
  elapsed = micros() - last_ptting;
  state = elapsed > Settings.ptt_timeout;
  
  // PTT OFF time?
  if ((state) & (!last_state)){
      digitalWrite(PTT_OUTPUT, OFF);
  }

  last_state = state;
}



// Check for incoming serial characters
void task_serialin(){
  while(Serial.available() > 0){
    processWinKeyerCommands(Serial.read());
  }
  
  if (KeyerBufferCount() == 0){
    processWinKeyerBuffer();
  }
}


// Arduino does have TX serial buffer, but if it gets full
// sending chars back to the computer can produce excesive
// delays as the arduino gets stalled while transmiting. To
// aliviate this we use an intermediate buffer and send
// characters one by one, executing all the other tasks in
// between.
//
// Only Winkeyer status bytes and velocity changes are
// send directly to the computer
void task_serialout(){
  if (SerialBufferCount()){
    if (Serial.availableForWrite()){
      Serial.write(SerialBufferRead());
    }
  }
}



void processWinKeyerCommands(byte cmd){
  // Put cmd in buffer
  CommandBufferWrite(cmd);

  // if buffer goes full, something wrong happened. Clear it
  if (CommandBufferCount() == WINKEYER_CMDBUFFER_SIZE){
    CommandBufferClear();
  };

  // There must be at lest one byte in the
  // buffer before call doWinKeyerCommand()
  if (CommandBufferCount()){
    // Is this a 600+ lines switch statement?
    //
    // Narrator: Yes, it is.
    switch(CommandBuffer[0]){
      case 0x00:  // Admin commands
        if (CommandBufferCount() > 1){
          switch(CommandBuffer[1]){
            case 0x00: 
              // (Admin)Calibrate
              // CMDBuffer.buffer = [00] [00] [FF] ...
              //
              // This is an historical command preserved for WK1
              // compatibility. It is no longer required for the more
              // accurate PIC the WK2 is implemented on. You can issue
              // the command and it will not cause ill effects, but it
              // is not processed by WK2. The command syntax is:
              // <00><00> pause 100 mSec <FF>
              //
              // Discard the 0xFF byte
              if (CommandBufferCount() > 2){
                CommandBufferClear();
              }
            break;

            case 0x01: 
              // (Admin)Reset
              // CMDBuffer.buffer = [00] [01] ...
              //
              // Resets the Winkeyer2 processor to the power up state. Do
              // not send this as part of the initialization sequence. Only
              // send this if you want to do a cold reboot of WK2.
              //
              // Not sure what to do here...
              //
              setSerial(1200);
              CommandBufferClear();
            break;

            case 0x02: 
              // (Admin)Host Open
              // CMDBuffer.buffer = [00] [02] ...
              //
              // Upon power-up, Winkeyer2 initializes with the host mode turned
              // off. To enable host mode, the PC host must issue the admin:open
              // command. Upon open, Winkeyer2 will respond by sending the revision
              // code back to the host. The host must wait for this return code before
              // any other commands or data can be sent to Winkeyer2. Upon open, WK1 mode is set.
              soundUp();
              WinKeyerBufferClear();
              CommandBufferClear();
              Winkeyer.mode = WK_MODE_1;
              Winkeyer.status = WK_STATUS_DEFAULT;
              SerialBufferWrite(23); // Version 2.3
              SerialBufferWrite(Winkeyer.status);
            break;

            case 0x03: 
              // (Admin)Host Close
              // CMDBuffer.buffer = [00] [03] ...
              //
              // Use this command to turn off the host interface. Winkeyer2 will
              // return to standby mode after this command is issued. Standby
              // settings will be restored.
              soundDown();
              Winkeyer.mode = OFF;
              setSerial(1200);
              WinKeyerBufferClear();
              CommandBufferClear();
            break;

            case 0x04: 
              // (Admin)Echo Test
              // CMDBuffer.buffer = [00] [04] [arg] ...
              //
              // Used to test the serial interface. The next character sent to
              // Winkeyer2 after this command will be echoed back to the host.
              if (CommandBufferCount() > 2){
                SerialBufferWrite(CommandBuffer[2]);
                CommandBufferClear();
              }
            break;

            case 0x05: 
              // (Admin)Paddle A2D
              // CMDBuffer.buffer = [00] [05] ...
              //
              // Historical command not supported in WK2, always returns 0.
              SerialBufferWrite(0);
              CommandBufferClear();
            break;

            case 0x06: 
              // (Admin)Speed A2D
              // CMDBuffer.buffer = [00] [06] ...
              //
              // Historical command not supported in WK2, always returns 0.
              SerialBufferWrite(0);
              CommandBufferClear();
            break;

            case 0x07: 
              // (Admin)Get values
              // CMDBuffer.buffer = [00] [07] ...
              //
              // Returns all of the internal setup parameters. They are sent
              // back in the same order as issued by the Load Defaults command.
              // Again, this command is a diagnostic aid. Only issue this command
              // when host interface is closed.
              SerialBufferWrite(Winkeyer.modereg);          // (0x0E) Mode register
              SerialBufferWrite(Settings.wpm);              // (0x02) Speed in WPM
              SerialBufferWrite(0x05);                      // (0x01) Sidetone frequency
              SerialBufferWrite(Settings.weight);           // (0x03) Weight
              SerialBufferWrite(0x00);                      // (0x04) Lead-in time
              SerialBufferWrite(0x00);                      // (0x04) Tail time
              SerialBufferWrite(Winkeyer.pot_min);          // (0x05) Min WPM
              SerialBufferWrite(Winkeyer.pot_range);        // (0x05) WPM range
              SerialBufferWrite(0x00);                      // (0x10) 1st extension
              SerialBufferWrite(0x00);                      // (0x11) Key compensation
              SerialBufferWrite(0x00);                      // (0x0D) Farnsworth WPM
              SerialBufferWrite(0x00);                      // (0x12) Paddle Setpoint
              SerialBufferWrite((10 * Settings.ratio) / 6); // (0x17) Dit/Dah ratio
              SerialBufferWrite(0x06);                      // (0x09) Pin configuration
              SerialBufferWrite(0x00);                      // Don't care, always zero
              CommandBufferClear();
            break;

            case 0x08: 
              // (Admin)Reserved (Debug)
              // CMDBuffer.buffer = [00] [08] ...
              //
              CommandBufferClear();
            break;

            case 0x09: 
              // (Admin)Get Cal
              // CMDBuffer.buffer = [00] [09] ...
              //
              // Historical command not supported in WK2, always returns 0.
              SerialBufferWrite(0);
              CommandBufferClear();
            break;

            case 0x0A: 
              // (Admin)Set WK1 Mode
              // CMDBuffer.buffer = [00] [0A] ...
              //
              // Disables pushbutton reporting
              Winkeyer.mode = WK_MODE_1;
              CommandBufferClear();
            break;

            case 0x0B: 
              // (Admin)Set WK2 mode
              // CMDBuffer.buffer = [00] [0B] ...
              //
              // Enables pushbutton reporting, alternate WK status mode is selected.
              Winkeyer.mode = WK_MODE_2;
              CommandBufferClear();
            break;

            case 0x0C: 
              // (Admin)Dump EEPROM
              // CMDBuffer.buffer = [00] [0C] ...
              //
              // Dumps all 256 bytes of WK2’s internal EEPROM
              //
              // Just send dummy data
              for (int i = 0; i < 256; i++){
                Serial.write(0);
              }
              CommandBufferClear();
            break;

            case 0x0D: 
              // (Admin)Load EEPROM
              // CMDBuffer.buffer = [00] [0D] [arg1] [arg2] [arg3] ... [arg256] 
              //
              // Download all 256 bytes of WK2’s internal EEPROM.
              //
              // Just get and discard 256 bytes
              for (int i = 0; i < 256; i++){
                while (Serial.available() == 0);
                Serial.read();
              }
              CommandBufferClear();
            break;

            case 0x0E: 
              // (Admin)Send Standalone Message
              // CMDBuffer.buffer = [00] [0E] [arg] ...
              //
              // Command WK2 to send one of its internal messages. The command
              // syntax is: <00><14><msg number> where number is 1 through 6
              //
              // Just read the message number and do nothing
              if (CommandBufferCount() > 2){
                CommandBufferClear();
              }
            break;

            case 0x0F: 
              // (Admin)Load XMODE
              // CMDBuffer.buffer = [00] [0F] [arg] ...
              //
              // Load mode extension register.
              //
              // Just read the byte and ignores it
              if (CommandBufferCount() > 2){
                CommandBufferClear();
              }
            break;

            case 0x10: 
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [10] ...
              //
              // If this command is issued, WK2 will respond with a zero.
              SerialBufferWrite(0);
              CommandBufferClear();
            break;

            case 0x11: 
              // (Admin)Set High Baud
              // CMDBuffer.buffer = [00] [11] ...
              //
              // Change Baud Rate to 9600 baud
              setSerial(9600);
              CommandBufferClear();
            break;

            case 0x12: 
              // (Admin)Set Low Baud
              // CMDBuffer.buffer = [00] [12] ...
              //
              // Change Baud Rate to 1200 (default)
              setSerial(1200);
              CommandBufferClear();
            break;

            case 0x13: 
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [13] ...
              //
              // Function TBA
              CommandBufferClear();
            break;

            case 0x14: 
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [14] ...
              //
              //  Currently unassigned
              CommandBufferClear();
            break;

            default:
              CommandBufferClear();
            break;
          }
        }
      break;

      case 0x01: 
        // Sidetone frequency
        // CMDBuffer.buffer = [01] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
            CommandBufferClear();
        }
      break;
      
      case 0x02: 
        // Speed
        // CMDBuffer.buffer = [02] [arg] ...
        //
        if (CommandBufferCount() > 1){
          wkSetSpeed(CommandBuffer[1]);
          CommandBufferClear();
        }
      break;
      
      case 0x03: 
        // Weighting
        // CMDBuffer.buffer = [03] [arg] ...
        //
        if (CommandBufferCount() > 1){
          // Unimplemented on purpose
          //wkSetWeight(CommandBuffer[1]);
          CommandBufferClear();
        }
      break;
      
      case 0x04: 
        // PTT Lead-in/Tail
        // CMDBuffer.buffer = [04] [arg1] [arg2] ...
        //
        // Not implemented
        if (CommandBufferCount() > 2){
        CommandBufferClear();
        }
      break;
      
      case 0x05: 
        // Speed Pot Setup
        // CMDBuffer.buffer = [05] [arg1] [arg2] [arg3] ...
        //
        if (CommandBufferCount() > 3){
          Winkeyer.pot_min = CommandBuffer[1];
          Winkeyer.pot_range = CommandBuffer[2];
          // arg3 is not used
          CommandBufferClear();
        }
      break;
      
      case 0x06: 
        // Pause
        // CMDBuffer.buffer = [06] [arg1] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x07: 
        // Get Speed Pot
        // CMDBuffer.buffer = [07] ...
        //
        wkSendSpeedPot();
        CommandBufferClear();
      break;
      
      case 0x08: 
        // Backspace
        // CMDBuffer.buffer = [08] ...
        //
        WinKeyerBufferBackSpace();
        CommandBufferClear();
      break;
      
      case 0x09: 
        // Pin configuration
        // CMDBuffer.buffer = [09] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x0A: 
        // Clear buffer
        // CMDBuffer.buffer = [0A] ...
        //
        CommandBufferClear();
        WinKeyerBufferClear();
      break;
      
      case 0x0B: 
        // Key inmediate
        // CMDBuffer.buffer = [0B] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x0C: 
        // HSCW Speed
        // CMDBuffer.buffer = [0C] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x0D: 
        // Farnsworth speed
        // CMDBuffer.buffer = [0D] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x0E: 
        // Winkeyer2 mode
        // CMDBuffer.buffer = [0E] [arg] ...
        //
        if (CommandBufferCount() > 1){
          Winkeyer.modereg = CommandBuffer[1];
          CommandBufferClear();
        }
      break;
      
      case 0x0F: 
        // Load defaults
        // CMDBuffer.buffer = [0F] [arg1] [arg2] ... [arg15] 
        //
        if (CommandBufferCount() > 16){
          Winkeyer.modereg = CommandBuffer[1];   // [1] (0x0E) Mode register
          wkSetSpeed(CommandBuffer[2]);          // [2] (0x02) Speed in WPM
          //                                     // [3] (0x01) Sidetone frequency
          //wkSetWeight(CommandBuffer[4]);       // [4] (0x03) Weight
          //                                     // [5] (0x04) Lead-in time
          //                                     // [6] (0x04) Tail time
          Winkeyer.pot_min = CommandBuffer[7];   // [7] (0x05) Min WPM
          Winkeyer.pot_range = CommandBuffer[8]; // [8] (0x05) WPM range
          //                                     // [9] (0x10) 1st extension
          //                                     // [A] (0x11) Key compensation
          //                                     // [B] (0x0D) Farnsworth WPM
          //                                     // [C] (0x12) Paddle Setpoint
          //wkSetRatio(CommandBuffer[0x0D]);     // [D] (0x17) Dit/Dah ratio
          //                                     // [E] (0x09) Pin configuration
          //                                     // [F] Don't care, always zero... or 0xFF
          CommandBufferClear();
        }
      break;
      
      case 0x10: 
        // First Extension
        // CMDBuffer.buffer = [10] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x11: 
        // Key Compensation
        // CMDBuffer.buffer = [11] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x12: 
        // Paddle Switchpoint
        // CMDBuffer.buffer = [12] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x13: 
        // Null
        // CMDBuffer.buffer = [13] ...
        //
        // NOP
        CommandBufferClear();
      break;
      
      case 0x14: 
        // S/W Paddle Input
        // CMDBuffer.buffer = [14] [arg] ...
        //
        // Not implemented
        if (CommandBufferCount() > 1){
          CommandBufferClear();
        }
      break;
      
      case 0x15: 
        // Winkeyer2 Status
        // CMDBuffer.buffer = [15] ...
        //
        SerialBufferWrite(Winkeyer.status);
        CommandBufferClear();
      break;

      // Well... this is the infamous Winkeyer's Command 16. The
      // manual says it is detailed described in a separate application
      // note but that application note was never released so all behavior
      // has been reverse engineering using N1MM, with does an extensive
      // use of this command for every conceivable operation. Current
      // implementation seems to work but nothing is guaranteed
      //
      case 0x16: //  Input Buffer Command Set
        if (CommandBufferCount() > 1){
          switch (CommandBuffer[1]){
            case 0x00: 
              // Reset input buffer pointers
              // CMDBuffer.buffer = [16] [00]
              //
              WinKeyerBufferClear();
              CommandBufferClear();
            break;

            case 0x01: 
              // Move input pointer to new position in overwrite mode
              // CMDBuffer.buffer = [16] [01] [arg] ...
              //
              if (CommandBufferCount() > 2){
                WinKeyerBufferSetCmd16Ptr(CommandBuffer[2]);
                WinKeyerBufferSetMode(WK_BUFFER_MODE_OVERWRITE);
                CommandBufferClear();
              }
            break;
            
            case 0x02: 
              // Move input pointer to new position in append mode
              // CMDBuffer.buffer = [16] [01] [arg] ...
              //
              if (CommandBufferCount() > 2){
                WinKeyerBufferSetCmd16Ptr(CommandBuffer[2]);
                WinKeyerBufferSetMode(WK_BUFFER_MODE_APPEND);
                CommandBufferClear();
              }
            break;

            case 0x03: 
              // Add multiple nulls to the buffer 
              // CMDBuffer.buffer = [16] [03] [arg] ...
              //
              if (CommandBufferCount() > 2){
                int nn = CommandBuffer[2];
                for (int n = 0; n < nn; n++){
                  WinKeyerBufferWrite(0x00);
                }
                CommandBufferClear();
              }
            break;

            default:
              CommandBufferClear();
            break;
          };
        }
      break;
      
      case 0x17: 
        // Dih/Dah ratio
        // CMDBuffer.buffer = [17] [arg] ...
        //
        if (CommandBufferCount() > 1){
          // Unimplemented on purpose
          //wkSetRatio(CommandBuffer[1]);
          CommandBufferClear();
        }
      break;
      
      // From this point they are all buffered
      // commands, send them to wkbuffer() to be
      // processed by processWinKeyerBuffer()

      case 0x18: 
        // PTT Control
        // CMDBuffer.buffer = [18] [arg] ...
        //
        if (CommandBufferCount() > 1){
          WinKeyerBufferWrite(CommandBuffer[0]); // Command
          WinKeyerBufferWrite(CommandBuffer[1]); // Argument
          CommandBufferClear();
        }
      break;
      
      case 0x19: 
        // Timed Key Down
        // CMDBuffer.buffer = [19] [arg] ...
        //
        if (CommandBufferCount() > 1){
          WinKeyerBufferWrite(CommandBuffer[0]); // Command
          WinKeyerBufferWrite(CommandBuffer[1]); // Argument
          CommandBufferClear();
        }
      break;
      
      case 0x1A: // Wait
        // Wait
        // CMDBuffer.buffer = [1A] [arg] ...
        //
        if (CommandBufferCount() > 1){
          WinKeyerBufferWrite(CommandBuffer[0]); // Command
          WinKeyerBufferWrite(CommandBuffer[1]); // Argument
          CommandBufferClear();
        }
      break;
      
      case 0x1B: 
        // Merge Letters
        // CMDBuffer.buffer = [1B] [arg1] [arg2] ...
        //
        if (CommandBufferCount() > 2){
          WinKeyerBufferWrite(CommandBuffer[0]); // Command
          WinKeyerBufferWrite(CommandBuffer[1]); // Argument 1
          WinKeyerBufferWrite(CommandBuffer[2]); // Argument 2
          CommandBufferClear();
        }
      break;
      
      case 0x1C: 
        // Buffered speed change
        // CMDBuffer.buffer = [1C] [arg] ...
        //
        if (CommandBufferCount() > 1){
          WinKeyerBufferWrite(CommandBuffer[0]); // Command
          WinKeyerBufferWrite(CommandBuffer[1]); // Argument
          CommandBufferClear();
        }
      break;
      
      case 0x1D: 
        // HSCW Speed / Port select
        // CMDBuffer.buffer = [1D] [arg] ...
        //
        if (CommandBufferCount() > 1){
          WinKeyerBufferWrite(CommandBuffer[0]); // Command
          WinKeyerBufferWrite(CommandBuffer[1]); // Argument
          CommandBufferClear();
        }
      break;
      
      case 0x1E: 
        // Cancel Buffered Speed Change
        // CMDBuffer.buffer = [1E] ...
        //
        WinKeyerBufferWrite(CommandBuffer[0]); // Command
        CommandBufferClear();
      break;
      
      case 0x1F: 
        // Buffered NOP
        // CMDBuffer.buffer = [1F] ...
        //
        WinKeyerBufferWrite(CommandBuffer[0]); // Command
        CommandBufferClear();
      break;
    
      // Ascii chars
      case 0x20 ... 0x7F:
        WinKeyerBufferWrite(CommandBuffer[0]);
        CommandBufferClear();
      break;

      default:
        CommandBufferClear();
      break;
    }
  }
}



void processWinKeyerBuffer(){
  if (WinKeyerBufferCount()){
    byte b = WinKeyerBufferRead();
    // Buffered commands
    switch (b){
      case 0x18: // PTT Control
        // Not implemented
        WinKeyerBufferRead();
      break;
      
      case 0x19: // Timed Key Down
        // Not implemented
        WinKeyerBufferRead();
      break;
      
      case 0x1A: // Wait
        // Not implemented
        WinKeyerBufferRead();
      break;
      
      case 0x1B: // Merge Letters (Prosigns)
        {
          char c1 = WinKeyerBufferRead();
          char c2 = WinKeyerBufferRead();
          cwProsign(c1, c2);
        }
      break;
      
      case 0x1C: // Speed change
        if (Winkeyer.old_speed == 0){
          Winkeyer.old_speed = Settings.wpm;
        }
        wkSetSpeed(WinKeyerBufferRead());
      break;
      
      case 0x1D: // HSCW Speed / Port select
        // Not implemented
        WinKeyerBufferRead();
      break;
      
      case 0x1E: // Cancel Buff Speed
        wkSetSpeed(Winkeyer.old_speed);
        Winkeyer.old_speed = 0;
      break;
      
      case 0x1F: // Buffered NOP
        // Do nothing
      break;

      // Ascii chars
      case 0x20 ... 0x7F:
        cwKeying(b);
      break;
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
void task_keyer(){
  static unsigned long waiting = 0;
  static unsigned long amount = 0;

  if (amount){
    if ((micros() - waiting) < amount){
      // Nothing to do yet
      return;
    }
  }

  amount = 0;

  if (KeyerBufferCount()){
    // If buffer has something, process it
    byte b = KeyerBufferRead();
    switch (b){
      case KCMD_SPACE:
        amount = Settings.cw_clock_x2;
        waiting = micros();
      break;

      case KCMD_DIT_KEYDOWN:
        if (getFlag(BIT_SIDETONE)){
          tone(BUZZER_OUTPUT, SIDETONE_FREQUENCY);
        }
        if (getFlag(BIT_KEYING)){
          digitalWrite(KEYING_OUTPUT, 1);
        }
        amount = Settings.cw_dit_keydown;
        waiting = micros();
      break;

      case KCMD_DIT_KEYUP:
        if (getFlag(BIT_KEYING)){
          digitalWrite(KEYING_OUTPUT, 0);
        }
        if (getFlag(BIT_SIDETONE)){
          noTone(BUZZER_OUTPUT);
        }
        amount = Settings.cw_dit_keyup;
        waiting = micros();
      break;

      case KCMD_DAH_KEYDOWN:
        if (getFlag(BIT_SIDETONE)){
          tone(BUZZER_OUTPUT, SIDETONE_FREQUENCY);
        }
        if (getFlag(BIT_KEYING)){
          digitalWrite(KEYING_OUTPUT, 1);
        }
        amount = Settings.cw_dah_keydown;
        waiting = micros();
      break;

      case KCMD_DAH_KEYUP:
        if (getFlag(BIT_KEYING)){
          digitalWrite(KEYING_OUTPUT, 0);
        }
        if (getFlag(BIT_SIDETONE)){
          noTone(BUZZER_OUTPUT);
        }
        amount = Settings.cw_clock;
        waiting = micros();
      break;

      case KCMD_SPEED:
        setWPM(KeyerBufferRead());
      break;

      case KCMD_WEIGHT:
        setWeight(KeyerBufferRead());
      break;

      case KCMD_RATIO:
        setRatio(KeyerBufferRead());
      break;

      case KCMD_SIDETONE:
        setFlag(BIT_SIDETONE, KeyerBufferRead());
      break;

      case KCMD_KEYING:
        setFlag(BIT_KEYING, KeyerBufferRead());
      break;

      case KCMD_POTSPEED:
        setWPM(KeyerBufferRead());
          if (Winkeyer.mode){
            wkSendSpeedPot();
          }
      break;

      case KCMD_LOCALECHO:
        setFlag(BIT_LOCALECHO, KeyerBufferRead());
      break;

      case 0x80 ... 0xFF:
        if (getFlag(BIT_LOCALECHO) && serialEcho()){
          SerialBufferWrite(b & 0x7F);
        }
      break;
    }
  } else {
    // If buffer is empty, process the paddles
    byte paddle = readPaddles();
    switch (paddle){
      case DITPADDLE:
        ditPaddle();
      break;

      case DAHPADDLE:
        dahPaddle();
      break;
    }
  }
}



// Calculates CW timming parameters from a given speed / weight / ratio
void updateTimmings(){  
  // Calculates dot clock
  Settings.cw_clock = 1200000 / Settings.wpm;

  // Some multiples of CW_CLOCK
  Settings.cw_clock_x2 = Settings.cw_clock * 2;
  Settings.cw_clock_x7 = Settings.cw_clock * 7;

  // Calculates key down time for dot and given weight
  Settings.cw_dit_keydown = (Settings.cw_clock_x2 * Settings.weight) / 100;

  // Calculates key up time for dot
  Settings.cw_dit_keyup = Settings.cw_clock_x2 - Settings.cw_dit_keydown;

  // Calculated key down time for dah at given ratio
  Settings.cw_dah_keydown = (Settings.cw_clock * Settings.ratio) / 10;

  // Calculates the PTT timeout
  Settings.ptt_timeout = Settings.cw_clock * 16;
}



// Add a command into keyer's buffer
void cwCommand(byte command, byte argument){
  KeyerBufferWrite(command);
  KeyerBufferWrite(argument);
}



// Translate a char into a dit/dah keying sequence
void cwKeying (char c){
  char buffer[MAX_MORSE_LENGTH + 1];
  char *ptr = buffer;

  // Get the actual sequence of dihs/dahs
  strcpy_P(buffer, MorseChars[pgm_read_byte(&AsciiMorse[(byte)c])]);

  while(*ptr){
    switch (*ptr){
      case '.':
        makeDih();
      break;

      case '-':
        makeDah();
      break;

      case ' ':
        makeSpace();
      break;
    }
    ptr++;
  }

  // Last dih/dah already make one spacing,
  // so makeSpace make the two remaining
  makeSpace();

  // Add literal character for echo
  KeyerBufferWrite(c | 0x80);
}



// Do the keying of a prosign
void cwProsign (char c1, char c2){
  char buffer[2 * MAX_MORSE_LENGTH + 1];
  char *ptr = buffer;

  // Get the sequence of dihs/dahs for the first character
  strcpy_P(buffer, MorseChars[pgm_read_byte(&AsciiMorse[(byte)c1])]);
  
  ptr = buffer;
  // Find the end of first string
  while(*ptr){
    ptr++;
  }

  // Get the second sequence
  strcpy_P(ptr, MorseChars[pgm_read_byte(&AsciiMorse[(byte)c2])]);
  
  // restore ptr to the begin of buffer
  ptr = buffer;

  while(*ptr){
    switch (*ptr){
      case '.':
        makeDih();
      break;

      case '-':
        makeDah();
      break;

      case ' ':
        makeSpace();
      break;
    }
    ptr++;
  }

  // Last dih/dah already make one spacing,
  // so makeSpace make the two remaining
  makeSpace();
}



int readPotentiometer(){
  static int lastValue = -512;  // Force update at power-up
  
  // In some units the potentiometer readings using the ADC
  // are very noisy. The average of three consecutive readings
  // seems to work fine
  int value = analogRead(POTENTIOMETER);
  value += analogRead(POTENTIOMETER);
  value += analogRead(POTENTIOMETER);
  value /= 3;

  if (abs(lastValue - value) > POTENTIOMETER_THRESHOLD){
    lastValue = value;
    return value;
  } else {
    return -1;
  }
}



// Returns the last pressed button (1, 2, 3)
// Returns 4, 5, 6 for long press
// otherwise it returns zero
byte readButton(){
  static byte last_button = 0;
  static byte button_counter = 0;
  byte button;

  switch(analogRead(BUTTONS_ARRAY)){
    case 0 ... 49:
      button = 1;
    break;

    case 50 ... 133:
      button = 2;
    break;

    case 134 ... 205:
      button = 3;
    break;

    default:
      button = 0;
  }

  if (last_button){
    button_counter++;
  }

  if ((button == 0) && (last_button != 0)){
    byte btn = last_button;
    last_button = button;
    if (button_counter > LONG_BUTTON_PRESS){
      btn += 3;
    }
    button_counter = 0;
    return btn;
  } else {
    last_button = button;
    return 0;
  }
}



// Cycle through speed, weight and ratio
void potentiometerModeToggle(){
  byte pot = getPotentiometerMode();
  pot = (pot + 1) % POTENTIOMETER_MODES;
  setPotentiometerMode(pot);

  switch (pot){
    case POTENTIOMETER_MODE_WPM:
      cwPlay(" S ");
    break;

    case POTENTIOMETER_MODE_WEIGHT:
      cwPlay(" W ");
    break;

    case POTENTIOMETER_MODE_RATIO:
      cwPlay(" R ");
    break;
  }

  saveSettings();
}



// Turn sidetone on/off and save it in EEPROM
void sidetoneToggle(){
  if (getFlag(BIT_SIDETONE)){
    setFlag(BIT_SIDETONE, OFF);
    soundDown();
  } else {
    setFlag(BIT_SIDETONE, ON);
    soundUp();
  }
  saveSettings();
}



// Plays through buzzer current settings
void playCurrentSettings(){
  char buffer[16];
  //printCurrentSettings();
  switch(getPotentiometerMode()){
    case POTENTIOMETER_MODE_WPM:
      sprintf(buffer, " S%d ", getWPM());
    break;

    case POTENTIOMETER_MODE_WEIGHT:
      sprintf(buffer, " W%d ", getWeight());
    break;

    case POTENTIOMETER_MODE_RATIO:
      sprintf(buffer, " R%d ", getRatio());
    break;
  }
  cwPlay(buffer);
}



// Toggle between ultimate, iambic-a and iambic-b keyer modes
void keyerToggle(){
  byte keyer = getKeyerMode();
  keyer = (keyer + 1) % KEYER_MODES;
  setKeyerMode(keyer);

  switch (keyer){
    case KEYER_MODE_ULTIMATE:
      cwPlay(" UL ");
    break;

    case KEYER_MODE_IAMBIC_A:
      cwPlay(" IA ");
    break;

    case KEYER_MODE_IAMBIC_B:
      cwPlay(" IB ");
    break;    
  }
  
  saveSettings();
}



// Toggle paddles
void paddleToggle(){
  if (getFlag(BIT_SWAP_PADDLES)){
    setFlag(BIT_SWAP_PADDLES, OFF);
    cwPlay(" A ");
  } else {
    setFlag(BIT_SWAP_PADDLES, ON);
    cwPlay(" N ");
  }
  saveSettings();
}



// Winkeyer 2.3 manual says the potentiometer range is divided
// into 32 slots numbered 0 to 31, but the same manual states
// there are 6 bits to store those slots, this is 0 to 63.
// All programs seems to be happy with the 64 slots.
void wkSendSpeedPot(){
  byte s;
  byte speed = getWPM();

  if (speed < Winkeyer.pot_min){
    s = 0;
  } else {
    s = speed - Winkeyer.pot_min;
    if (s > 0x3F){
      s = 0x3F;
    }
  }

  Serial.write(0x80 | s);
}



// For decoding, add a dot into the buffer and makes dit
void ditPaddle(){  
  DecodingBufferWrite('.');
  makeDih();
}




// For decoding, add a dash into the buffer and makes dah
void dahPaddle(){
  DecodingBufferWrite('-');
  makeDah();
}



// Add the making of a dit into keyer's buffer
void makeDih(){
  KeyerBufferWrite(KCMD_DIT_KEYDOWN);
  KeyerBufferWrite(KCMD_DIT_KEYUP);
}



// Add the making of a dah into keyer's buffer
void makeDah(){
  KeyerBufferWrite(KCMD_DAH_KEYDOWN);
  KeyerBufferWrite(KCMD_DAH_KEYUP);
}



// Add the making of a space into keyer's buffer
void makeSpace(){
  KeyerBufferWrite(KCMD_SPACE);
}



void soundUp(){
  tone(BUZZER_OUTPUT, TONE_LOW);
  delay(TONE_DURATION);
  noTone(BUZZER_OUTPUT);
  delay(TONE_SPACING);
  tone(BUZZER_OUTPUT, TONE_HIGH);
  delay(TONE_DURATION);
  noTone(BUZZER_OUTPUT);
}



void soundDown(){
  tone(BUZZER_OUTPUT, TONE_HIGH);
  delay(TONE_DURATION);
  noTone(BUZZER_OUTPUT);
  delay(TONE_SPACING);
  tone(BUZZER_OUTPUT, TONE_LOW);
  delay(TONE_DURATION);
  noTone(BUZZER_OUTPUT);
}



// Must paddle characters be echoed back?
byte paddleEcho(){
  return (Winkeyer.mode == OFF) || (Winkeyer.modereg & WK_PADDLE_ECHO_BIT);
}



// Must serial characters be echoed back?
byte serialEcho(){
  return (Winkeyer.mode == OFF) || (Winkeyer.modereg & WK_SERIAL_ECHO_BIT);
}



void wkSetSpeed(byte value){
  if ((value >= 5) && (value <= 99)){
    cwCommand(KCMD_SPEED, value);
  }else{
    cwCommand(KCMD_SPEED, CW_PLAY_WPM); // TO-DO
  }
}



// void wkSetWeight(byte value){
//   if((value >= 10) && (value <= 90)){
//     CW_Command(KCMD_WEIGHT, value);
//   }
// }



// void wkSetRatio(byte value){
//   CW_Command(KCMD_RATIO, (value * 6) / 10);
// }
