#include "keyer.hpp"
#include "paddles.hpp"
#include "settings.hpp"
#include "sound.hpp"
#include "userinterface.hpp"
#include "winkeyer.hpp"

#define WINKEYER_MODE_1 1
#define WINKEYER_MODE_2 2
#define WINKEYER_STATUS_DEFAULT 0xC0

// Register bits
#define WINKEYER_STATUS_XOFF_BIT 0
#define WINKEYER_STATUS_BREAKIN_BIT 1
#define WINKEYER_STATUS_BUSY_BIT 2

#define WINKEYER_MODE_PADDLE_ECHOBACK_BIT 6
#define WINKEYER_MODE_SERIAL_ECHOBACK_BIT 2

#define WINKEYER_CMDBUFFER_SIZE 0x20

// Size MUST BE a power of 2
#define WINKEYER_DATA_BUFFER_SIZE 128
#define WINKEYER_DATA_BUFFER_MASK (WINKEYER_DATA_BUFFER_SIZE - 1)

#define WINKEYER_DATA_BUFFER_MODE_NORMAL 0
#define WINKEYER_DATA_BUFFER_MODE_OVERWRITE 1
#define WINKEYER_DATA_BUFFER_MODE_APPEND 2

#define OFF 0
#define ON 1

#define BAUDS_HIGH 9600
#define BAUDS_LOW 1200

static uint8_t wkMode;       // winkeyer actual mode
static uint8_t wkModeReg;    // Winkeyer mode register
static int8_t potMin = 5;    // Minimum potentiometer speed value
static int8_t potRange = 63; // Potentiometer speed range
static int8_t oldSpeed;      // For buffered speed change
static uint8_t cmdBuffer[WINKEYER_CMDBUFFER_SIZE];
static uint8_t cmdWritePtr;
static uint8_t dataBuffer[WINKEYER_DATA_BUFFER_SIZE];
static uint8_t dataWritePtr;
static uint8_t dataReadPtr;
static uint8_t dataBufferMode;
static uint8_t dataBufferCmd16Ptr;

  // Dummy settings
static uint8_t dummyModeRegister;
static uint8_t dummySidetoneFreq = 0x05;      // 800 Hz
static uint8_t dummyWeight = 0x32;            // 50%
static uint8_t dummyLeadinTime = 0x00;        // 0 milliseconds
static uint8_t dummyTailTime = 0x00;          // 0 milliseconds
static uint8_t dummyFirstExtension = 0x00;    // 0 milliseconds
static uint8_t dummyKeyCompensation = 0x00;   // 0 milliseconds
static uint8_t dummyFarnsworthWPM = 0x00;     // 0 WPM
static uint8_t dummyPaddleSetpoint = 0x32;    // 50%
static uint8_t dummyRatio = 0x32;             // 3:1
static uint8_t dummyPinConfiguration = 0x00;  // Unknown default value

static void cmdBufferClear() {
  cmdWritePtr = 0;
}

static uint8_t cmdBufferCount() {
  return cmdWritePtr;
}

static void cmdBufferWrite(uint8_t b) {
  if (cmdBufferCount() < WINKEYER_CMDBUFFER_SIZE) {
    cmdBuffer[cmdWritePtr++] = b;
  }
}

static uint8_t cmdBufferRead(uint8_t position) {
  return cmdBuffer[position];
}

static void dataBufferClear() {
  dataReadPtr = 0;
  dataWritePtr = 0;
  dataBufferCmd16Ptr = 0;
  dataBufferMode = WINKEYER_DATA_BUFFER_MODE_NORMAL;
}

static void dataBufferWrite(uint8_t data) {
  switch (dataBufferMode) {
    case WINKEYER_DATA_BUFFER_MODE_NORMAL:
      dataBuffer[dataWritePtr++] = data;
      dataWritePtr &= WINKEYER_DATA_BUFFER_MASK;
      break;

    case WINKEYER_DATA_BUFFER_MODE_OVERWRITE:
    case WINKEYER_DATA_BUFFER_MODE_APPEND:
      dataBuffer[dataBufferCmd16Ptr++] = data;
      dataBufferCmd16Ptr &= WINKEYER_DATA_BUFFER_MASK;
      break;
  }

  if (dataBufferCmd16Ptr > dataWritePtr) {
    dataWritePtr = dataBufferCmd16Ptr;
  }
}

static uint8_t dataBufferRead() {
  uint8_t data = dataBuffer[dataReadPtr++];
  dataReadPtr &= WINKEYER_DATA_BUFFER_MASK;
  return data;
}

static uint8_t dataBufferCount() {
  return (dataWritePtr - dataReadPtr) & WINKEYER_DATA_BUFFER_MASK;
}

static void dataBufferBackSpace() {
  if (dataBufferCount()) {
    dataWritePtr = (dataWritePtr - 1) & WINKEYER_DATA_BUFFER_MASK;
  }
}

static void dataBufferSetCmd16Ptr(uint8_t p) {
  if (p) {
    dataBufferCmd16Ptr = p + 1;  // ??? N1MM
  } else {
    dataBufferCmd16Ptr = 0;
  }
}

static void dataBufferSetMode(uint8_t mode) {
  dataBufferMode = mode;
}

static void setSerial(unsigned long baud) {
  if (Serial) {
    Serial.end();
  }
  // Warning: Winkeyer uses 8N2
  Serial.begin(baud, SERIAL_8N2);
}

void winkeyerBegin() {
  dataBufferClear();
  cmdBufferClear();
  setSerial(BAUDS_LOW);
}

// Winkeyer 2.3 manual says the potentiometer range is divided
// into 32 slots numbered 0 to 31, but the same manual states
// there are 6 bits to store those slots, this is 0 to 63.
// All programs seems to be happy with the 64 slots.
void winkeyerSendPotentiometer() {
  if (wkMode) {
    uint8_t s;
    uint8_t speed = settingsGetSpeed();

    if (speed < potMin) {
      s = 0;
    } else {
      s = speed - potMin;
      if (s > 0x3F) {
        s = 0x3F;
      }
    }

    Serial.write(0x80 | s);
  }
}

bool winkeyerGetHostOpened() {
  return wkMode != 0;
}

bool winkeyerGetSerialEchoback() {
  if (wkMode) {
    // Winkeyer active
    return (wkModeReg & (1 < WINKEYER_MODE_SERIAL_ECHOBACK_BIT)) != 0;
  } else {
    // Winkeyer inactive
    return true;
  }
}

bool winkeyerGetPaddleEchoback() {
  if (wkMode) {
    // Winkeyer active
    return (wkModeReg & (1 < WINKEYER_MODE_PADDLE_ECHOBACK_BIT)) != 0;
  } else {
    // Winkeyer inactive
    return true;
  }
}

static uint8_t getStatus() {
  uint8_t status = WINKEYER_STATUS_DEFAULT;

  // Check keyer buffer status
  uint8_t keyerBufferUsage = dataBufferCount();
  if (keyerBufferUsage > ((WINKEYER_DATA_BUFFER_SIZE * 2) / 3)) {
    status |= (1 << WINKEYER_STATUS_XOFF_BIT);
  }

  if (keyerBufferUsage < (WINKEYER_DATA_BUFFER_SIZE / 3)) {
    status &= ~(1 << WINKEYER_STATUS_XOFF_BIT);
  }

  // Check paddles break-in
  if ((micros() - paddlesGetLastTimestamp()) < settingsGet_ptt_timeout()) {
    status |= (1 << WINKEYER_STATUS_BREAKIN_BIT);
  } else {
    status &= ~(1 << WINKEYER_STATUS_BREAKIN_BIT);
  }

  // Check busy
  if (keyerIsKeying() || dataBufferCount()) {
    status |= (1 << WINKEYER_STATUS_BUSY_BIT);
  } else {
    status &= ~(1 << WINKEYER_STATUS_BUSY_BIT);
  }

  return status;
}

static void processStatus() {
  static uint8_t lastStatus = WINKEYER_STATUS_DEFAULT;
  if (wkMode) {
    uint8_t status = getStatus();
    if (status != lastStatus) {
      Serial.write(status);
      lastStatus = status;
    }
  }
}

static void processCommands(uint8_t cmd) {
  // Put cmd in buffer
  cmdBufferWrite(cmd);

  // if buffer goes full, something wrong happened. Clear it
  if (cmdBufferCount() == WINKEYER_CMDBUFFER_SIZE) {
    cmdBufferClear();
  }

  // There must be at lest one byte in the buffer
  if (cmdBufferCount()) {
    switch (cmdBufferRead(0)) {
      case 0x00:
        if (cmdBufferCount() > 1) {
          // Admin commands
          switch (cmdBufferRead(1)) {
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
              if (cmdBufferCount() > 2) {
                cmdBufferClear();
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
              setSerial(BAUDS_LOW);
              cmdBufferClear();
              break;

            case 0x02:
              // (Admin)Host Open
              // CMDBuffer.buffer = [00] [02] ...
              //
              // Upon power-up, Winkeyer2 initializes with the host mode turned
              // off. To enable host mode, the PC host must issue the admin:open
              // command. Upon open, Winkeyer2 will respond by sending the revision
              // code back to the host. The host must wait for this return code
              // before any other commands or data can be sent to Winkeyer2. Upon
              // open, WK1 mode is set.
              soundAscent();
              dataBufferClear();
              cmdBufferClear();
              wkMode = WINKEYER_MODE_1;
              Serial.write(23);  // Version 2.3
              Serial.write(getStatus());
              break;

            case 0x03:
              // (Admin)Host Close
              // CMDBuffer.buffer = [00] [03] ...
              //
              // Use this command to turn off the host interface. Winkeyer2 will
              // return to standby mode after this command is issued. Standby
              // settings will be restored.
              soundDescent();
              wkMode = OFF;
              setSerial(BAUDS_LOW);
              dataBufferClear();
              cmdBufferClear();
              break;

            case 0x04:
              // (Admin)Echo Test
              // CMDBuffer.buffer = [00] [04] [arg] ...
              //
              // Used to test the serial interface. The next character sent to
              // Winkeyer2 after this command will be echoed back to the host.
              if (cmdBufferCount() > 2) {
                Serial.write(cmdBufferRead(2));
                cmdBufferClear();
              }
              break;

            case 0x05:
              // (Admin)Paddle A2D
              // CMDBuffer.buffer = [00] [05] ...
              //
              // Historical command not supported in WK2, always returns 0.
              Serial.write((uint8_t)0);
              cmdBufferClear();
              break;

            case 0x06:
              // (Admin)Speed A2D
              // CMDBuffer.buffer = [00] [06] ...
              //
              // Historical command not supported in WK2, always returns 0.
              Serial.write((uint8_t)0);
              cmdBufferClear();
              break;

            case 0x07:
              // (Admin)Get values
              // CMDBuffer.buffer = [00] [07] ...
              //
              // Returns all of the internal setup parameters. They are sent
              // back in the same order as issued by the Load Defaults command.
              // Again, this command is a diagnostic aid. Only issue this command
              // when host interface is closed.
              Serial.write(dummyModeRegister);      // (cmd 0x0E) Mode register
              Serial.write(settingsGetSpeed());      // (cmd 0x02) Speed in WPM
              Serial.write(dummySidetoneFreq);      // (cmd 0x01) Sidetone frequency
                                                     //Serial.write(settings.getWeight());           // (cmd 0x03) Weight
              Serial.write(dummyWeight);            // (cmd 0x03) Weight
              Serial.write(dummyLeadinTime);        // (cmd 0x04) Lead-in time
              Serial.write(dummyTailTime);          // (cmd 0x04) Tail time
              Serial.write(potMin);                // (cmd 0x05) Min WPM
              Serial.write(potRange);              // (cmd 0x05) WPM range
              Serial.write(dummyFirstExtension);    // (cmd 0x10) 1st extension
              Serial.write(dummyKeyCompensation);   // (cmd 0x11) Key compensation
              Serial.write(dummyFarnsworthWPM);     // (cmd 0x0D) Farnsworth WPM
              Serial.write(dummyPaddleSetpoint);    // (cmd 0x12) Paddle Setpoint
                                                     //Serial.write((10 * settings.getRatio()) / 6); // (cmd 0x17) Dit/Dah ratio
              Serial.write(dummyRatio);             // (cmd 0x17) Dit/Dah ratio
              Serial.write(dummyPinConfiguration);  // (cmd 0x09) Pin configuration
              Serial.write((uint8_t)0x00);          // Don't care, always zero
              cmdBufferClear();
              break;

            case 0x08:
              // (Admin)Reserved (Debug)
              // CMDBuffer.buffer = [00] [08] ...
              //
              cmdBufferClear();
              break;

            case 0x09:
              // (Admin)Get Cal
              // CMDBuffer.buffer = [00] [09] ...
              //
              // Historical command not supported in WK2, always returns 0.
              Serial.write((uint8_t)0);
              cmdBufferClear();
              break;

            case 0x0A:
              // (Admin)Set WK1 Mode
              // CMDBuffer.buffer = [00] [0A] ...
              //
              // Disables pushbutton reporting
              wkMode = WINKEYER_MODE_1;
              cmdBufferClear();
              break;

            case 0x0B:
              // (Admin)Set WK2 mode
              // CMDBuffer.buffer = [00] [0B] ...
              //
              // Enables pushbutton reporting, alternate WK status mode is selected.
              wkMode = WINKEYER_MODE_2;
              cmdBufferClear();
              break;

            case 0x0C:
              // (Admin)Dump EEPROM
              // CMDBuffer.buffer = [00] [0C] ...
              //
              // Dumps all 256 bytes of WK2’s internal EEPROM
              //
              // Just send dummy data
              for (int i = 0; i < 256; i++) {
                Serial.write((uint8_t)0);
              }
              cmdBufferClear();
              break;

            case 0x0D:
              // (Admin)Load EEPROM
              // CMDBuffer.buffer = [00] [0D] [arg1] [arg2] [arg3] ... [arg256]
              //
              // Download all 256 bytes of WK2’s internal EEPROM.
              //
              // Just get and discard 256 bytes
              for (int i = 0; i < 256; i++) {
                while (Serial.available() == 0)
                  ;
                Serial.read();
              }
              cmdBufferClear();
              break;

            case 0x0E:
              // (Admin)Send Standalone Message
              // CMDBuffer.buffer = [00] [0E] [arg] ...
              //
              // Command WK2 to send one of its internal messages. The command
              // syntax is: <00><14><msg number> where number is 1 through 6
              //
              // Just read the message number and do nothing
              if (cmdBufferCount() > 2) {
                cmdBufferClear();
              }
              break;

            case 0x0F:
              // (Admin)Load XMODE
              // CMDBuffer.buffer = [00] [0F] [arg] ...
              //
              // Load mode extension register.
              //
              // Just read the byte and ignores it
              if (cmdBufferCount() > 2) {
                cmdBufferClear();
              }
              break;

            case 0x10:
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [10] ...
              //
              // If this command is issued, WK2 will respond with a zero.
              Serial.write((uint8_t)0);
              cmdBufferClear();
              break;

            case 0x11:
              // (Admin)Set High Baud
              // CMDBuffer.buffer = [00] [11] ...
              //
              // Change Baud Rate to 9600 baud
              setSerial(BAUDS_HIGH);
              cmdBufferClear();
              break;

            case 0x12:
              // (Admin)Set Low Baud
              // CMDBuffer.buffer = [00] [12] ...
              //
              // Change Baud Rate to 1200 (default)
              setSerial(BAUDS_LOW);
              cmdBufferClear();
              break;

            case 0x13:
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [13] ...
              //
              // Function TBA
              cmdBufferClear();
              break;

            case 0x14:
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [14] ...
              //
              //  Currently unassigned
              cmdBufferClear();
              break;

            default:
              cmdBufferClear();
              break;
          }
        }
        break;

      case 0x01:
        // Sidetone frequency
        // CMDBuffer.buffer = [01] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          dummySidetoneFreq = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x02:
        // Speed
        // CMDBuffer.buffer = [02] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          //setSpeed(cmdBuffer.read(1));
          settingsSetSpeed(cmdBufferRead(1));
          cmdBufferClear();
        }
        break;

      case 0x03:
        // Weighting
        // CMDBuffer.buffer = [03] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          // Unimplemented on purpose
          dummyWeight = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x04:
        // PTT Lead-in/Tail
        // CMDBuffer.buffer = [04] [arg1] [arg2] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 2) {
          dummyLeadinTime = cmdBufferRead(1);
          dummyTailTime = cmdBufferRead(2);
          cmdBufferClear();
        }
        break;

      case 0x05:
        // Speed Pot Setup
        // CMDBuffer.buffer = [05] [arg1] [arg2] [arg3] ...
        //
        if (cmdBufferCount() > 3) {
          potMin = cmdBufferRead(1);
          potRange = cmdBufferRead(2);
          // arg3 is not used
          cmdBufferClear();
        }
        break;

      case 0x06:
        // Pause
        // CMDBuffer.buffer = [06] [arg1] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          cmdBufferClear();
        }
        break;

      case 0x07:
        // Get Speed Pot
        // CMDBuffer.buffer = [07] ...
        //
        winkeyerSendPotentiometer();
        cmdBufferClear();
        break;

      case 0x08:
        // Backspace
        // CMDBuffer.buffer = [08] ...
        //
        dataBufferBackSpace();
        cmdBufferClear();
        break;

      case 0x09:
        // Pin configuration
        // CMDBuffer.buffer = [09] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          dummyPinConfiguration = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x0A:
        // Clear buffer
        // CMDBuffer.buffer = [0A] ...
        //
        cmdBufferClear();
        dataBufferClear();
        break;

      case 0x0B:
        // Key inmediate
        // CMDBuffer.buffer = [0B] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          cmdBufferClear();
        }
        break;

      case 0x0C:
        // HSCW Speed
        // CMDBuffer.buffer = [0C] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          cmdBufferClear();
        }
        break;

      case 0x0D:
        // Farnsworth speed
        // CMDBuffer.buffer = [0D] [arg] ...
        //
        // Not implemented   on purpose
        if (cmdBufferCount() > 1) {
          dummyFarnsworthWPM = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x0E:
        // Winkeyer2 mode
        // CMDBuffer.buffer = [0E] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          wkModeReg = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x0F:
        // Load defaults
        // CMDBuffer.buffer = [0F] [arg1] [arg2] ... [arg15]
        //
        if (cmdBufferCount() > 16) {
          wkModeReg = cmdBufferRead(1);                 // (cmd 0x0E) Mode register
          settingsSetSpeed(cmdBufferRead(2));          // (cmd 0x02) Speed in WPM
          dummySidetoneFreq = cmdBufferRead(3);       // (cmd 0x01) Sidetone frequency
          dummyWeight = cmdBufferRead(4);             // (cmd 0x03) Weight
          dummyLeadinTime = cmdBufferRead(5);         // (cmd 0x04) Lead-in time
          dummyTailTime = cmdBufferRead(6);           // (cmd 0x04) Tail time
          potMin = cmdBufferRead(7);                 // (cmd 0x05) Min WPM
          potRange = cmdBufferRead(8);               // (cmd 0x05) WPM range
          dummyFirstExtension = cmdBufferRead(9);     // (cmd 0x10) 1st extension
          dummyKeyCompensation = cmdBufferRead(10);   // (cmd 0x11) Key compensation
          dummyFarnsworthWPM = cmdBufferRead(11);     // (cmd 0x0D) Farnsworth WPM
          dummyPaddleSetpoint = cmdBufferRead(12);    // (cmd 0x12) Paddle Setpoint
          dummyRatio = cmdBufferRead(0x0D);           // (cmd 0x17) Dit/Dah ratio
          dummyPinConfiguration = cmdBufferRead(14);  // (cmd 0x09) Pin configuration
          cmdBufferClear();
        }
        break;

      case 0x10:
        // First Extension
        // CMDBuffer.buffer = [10] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          dummyFirstExtension = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x11:
        // Key Compensation
        // CMDBuffer.buffer = [11] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          dummyKeyCompensation = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x12:
        // Paddle Switchpoint
        // CMDBuffer.buffer = [12] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          dummyPaddleSetpoint = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

      case 0x13:
        // Null
        // CMDBuffer.buffer = [13] ...
        //
        // NOP
        cmdBufferClear();
        break;

      case 0x14:
        // S/W Paddle Input
        // CMDBuffer.buffer = [14] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBufferCount() > 1) {
          cmdBufferClear();
        }
        break;

      case 0x15:
        // Winkeyer2 Status
        // CMDBuffer.buffer = [15] ...
        //
        Serial.write(getStatus());
        cmdBufferClear();
        break;

      // Well... this is the infamous Winkeyer's Command 16. The
      // manual says it is detailed described in a separate application
      // note but that application note was never released so all behavior
      // has been reverse engineering using N1MM, with does an extensive
      // use of this command for every conceivable operation. Current
      // implementation seems to work but nothing is guaranteed
      //
      case 0x16:  //  Input Buffer Command Set
        if (cmdBufferCount() > 1) {
          switch (cmdBufferRead(1)) {
            case 0x00:
              // Reset input buffer pointers
              // CMDBuffer.buffer = [16] [00]
              //
              dataBufferClear();
              cmdBufferClear();
              break;

            case 0x01:
              // Move input pointer to new position in overwrite mode
              // CMDBuffer.buffer = [16] [01] [arg] ...
              //
              if (cmdBufferCount() > 2) {
                dataBufferSetCmd16Ptr(cmdBufferRead(2));
                dataBufferSetMode(WINKEYER_DATA_BUFFER_MODE_OVERWRITE);
                cmdBufferClear();
              }
              break;

            case 0x02:
              // Move input pointer to new position in append mode
              // CMDBuffer.buffer = [16] [01] [arg] ...
              //
              if (cmdBufferCount() > 2) {
                dataBufferSetCmd16Ptr(cmdBufferRead(2));
                dataBufferSetMode(WINKEYER_DATA_BUFFER_MODE_APPEND);
                cmdBufferClear();
              }
              break;

            case 0x03:
              // Add multiple nulls to the buffer
              // CMDBuffer.buffer = [16] [03] [arg] ...
              //
              if (cmdBufferCount() > 2) {
                int nn = cmdBufferRead(2);
                for (int n = 0; n < nn; n++) {
                  dataBufferWrite(0x00);
                }
                cmdBufferClear();
              }
              break;

            default:
              cmdBufferClear();
              break;
          };
        }
        break;

      case 0x17:
        // Dih/Dah ratio
        // CMDBuffer.buffer = [17] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          // Unimplemented on purpose
          dummyRatio = cmdBufferRead(1);
          cmdBufferClear();
        }
        break;

        // From this point they are all buffered
        // commands, send them to wkbuffer() to be
        // processed by processWinKeyerBuffer()

      case 0x18:
        // PTT Control
        // CMDBuffer.buffer = [18] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          dataBufferWrite(cmdBufferRead(0));  // Command
          dataBufferWrite(cmdBufferRead(1));  // Argument
          cmdBufferClear();
        }
        break;

      case 0x19:
        // Timed Key Down
        // CMDBuffer.buffer = [19] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          dataBufferWrite(cmdBufferRead(0));  // Command
          dataBufferWrite(cmdBufferRead(1));  // Argument
          cmdBufferClear();
        }
        break;

      case 0x1A:  // Wait
        // Wait
        // CMDBuffer.buffer = [1A] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          dataBufferWrite(cmdBufferRead(0));  // Command
          dataBufferWrite(cmdBufferRead(1));  // Argument
          cmdBufferClear();
        }
        break;

      case 0x1B:
        // Merge Letters
        // CMDBuffer.buffer = [1B] [arg1] [arg2] ...
        //
        if (cmdBufferCount() > 2) {
          dataBufferWrite(cmdBufferRead(0));  // Command
          dataBufferWrite(cmdBufferRead(1));  // Argument 1
          dataBufferWrite(cmdBufferRead(2));  // Argument 2
          cmdBufferClear();
        }
        break;

      case 0x1C:
        // Buffered speed change
        // CMDBuffer.buffer = [1C] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          dataBufferWrite(cmdBufferRead(0));  // Command
          dataBufferWrite(cmdBufferRead(1));  // Argument
          cmdBufferClear();
        }
        break;

      case 0x1D:
        // HSCW Speed / Port select
        // CMDBuffer.buffer = [1D] [arg] ...
        //
        if (cmdBufferCount() > 1) {
          dataBufferWrite(cmdBufferRead(0));  // Command
          dataBufferWrite(cmdBufferRead(1));  // Argument
          cmdBufferClear();
        }
        break;

      case 0x1E:
        // Cancel Buffered Speed Change
        // CMDBuffer.buffer = [1E] ...
        //
        dataBufferWrite(cmdBufferRead(0));  // Command
        cmdBufferClear();
        break;

      case 0x1F:
        // Buffered NOP
        // CMDBuffer.buffer = [1F] ...
        //
        dataBufferWrite(cmdBufferRead(0));  // Command
        cmdBufferClear();
        break;

      // Ascii chars
      case 0x20 ... 0x7F:
        dataBufferWrite(cmdBufferRead(0));
        cmdBufferClear();
        break;

      default:
        cmdBufferClear();
        break;
    }
  }
}

static void processBuffer() {
  if (dataBufferCount()) {
    uint8_t b = dataBufferRead();
    // Buffered commands
    switch (b) {
      case 0x18:  // PTT Control
        // Not implemented
        dataBufferRead();
        break;

      case 0x19:  // Timed Key Down
        // Not implemented
        dataBufferRead();
        break;

      case 0x1A:  // Wait
        // Not implemented
        dataBufferRead();
        break;

      case 0x1B:  // Merge Letters (Prosigns)
        {
          uint8_t c1 = dataBufferRead();
          uint8_t c2 = dataBufferRead();
          keyerSendProsign(c1, c2);
        }
        break;

      case 0x1C:  // Speed change
        oldSpeed = settingsGetSpeed();
        //setSpeed(dataBuffer.read());
        keyerSendCommand(KCMD_SPEED, dataBufferRead());
        break;

      case 0x1D:  // HSCW Speed / Port select
        // Not implemented
        dataBufferRead();
        break;

      case 0x1E:  // Cancel Buff Speed
        if (oldSpeed) {
          //setSpeed(old_speed);
          keyerSendCommand(KCMD_SPEED, oldSpeed);
          oldSpeed = 0;
        }
        break;

      case 0x1F:  // Buffered NOP
        // Do nothing
        break;

      // Ascii chars
      case 0x20 ... 0xFF:
        keyerSendChar(b);
        break;
    }
  }
}

void winkeyerProcess() {
  while ((Serial.available() > 0) && (dataBufferCount() < (WINKEYER_DATA_BUFFER_SIZE - 1))) {
    processCommands(Serial.read());
  }

  if (keyerIsKeying() == 0) {
    processBuffer();
  }

  processStatus();
}
