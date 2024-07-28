#ifndef WINKEYER_H
#define WINKEYER_H

#include "Arduino.h"
#include "winkeyercommandbuffer.h"
#include "winkeyerdatabuffer.h"

#define WK_MODE_1 1
#define WK_MODE_2 2
#define WK_STATUS_DEFAULT 0xC0

// Register bits
#define WK_STATUS_XOFF_BIT 0
#define WK_STATUS_BREAKIN_BIT 1
#define WK_STATUS_BUSY_BIT 2

#define WK_MODE_PADDLE_ECHOBACK_BIT 6
#define WK_MODE_SERIAL_ECHOBACK_BIT 2

#define WINKEYER_CMDBUFFER_SIZE 0x20 
class Winkeyer{
  public:
    void begin();
    void process();
    void sendPotentiometer();
    uint8_t getHostOpened();
    uint8_t getSerialEchoback();
    uint8_t getPaddleEchoback();
  private:
    WinkeyerCommandBuffer cmdBuffer;
    WinkeyerDataBuffer dataBuffer;
    uint8_t _mode;      // winkeyer actual mode
    uint8_t _modereg;   // Winkeyer mode register
    int8_t _pot_min = 5;   // Minimum potentiometer speed value
    int8_t _pot_range = 63; // Potentiometer speed range
    int8_t _old_speed; // For buffered speed change
    // Dummy settings
    uint8_t _dummyModeRegister;
    uint8_t _dummySidetoneFreq = 0x05;    // 800 Hz
    uint8_t _dummyWeight = 0x32;          // 50%
    uint8_t _dummyLeadinTime = 0x00;      // 0 milliseconds
    uint8_t _dummyTailTime = 0x00;        // 0 milliseconds
    uint8_t _dummyFirstExtension = 0x00;  // 0 milliseconds
    uint8_t _dummyKeyCompensation = 0x00; // 0 milliseconds
    uint8_t _dummyFarnsworthWPM = 0x00;   // 0 WPM
    uint8_t _dummyPaddleSetpoint = 0x32;  // 50%
    uint8_t _dummyRatio = 0x32;           // 3:1
    uint8_t _dummyPinConfiguration = 0x00;// Unknown default value

    void setSerial(unsigned long bauds);
    void processStatus();
    uint8_t getStatus();
    void processWinKeyerCommands(uint8_t cmd);
    void processWinKeyerBuffer();
};

#endif