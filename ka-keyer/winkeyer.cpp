#include "keyer.h"
#include "paddles.h"
#include "settings.h"
#include "sound.h"
#include "userinterface.h"
#include "winkeyer.h"

#define OFF 0
#define ON 1

#define BAUDS_HIGH 9600
#define BAUDS_LOW  1200

#define CMDBUFFER_SIZE  31

extern Keyer keyer;
extern Paddles paddles;
extern Settings settings;
extern Sound sound;
extern UserInterface ui;

void Winkeyer::begin(){
  setSerial(BAUDS_LOW);
}

void Winkeyer::process(){
  while((Serial.available() > 0) && (dataBuffer.count() < WK_DATABUFFER_SIZE)) {
    processWinKeyerCommands(Serial.read());
  }

  if (keyer.bufferCount() == 0){
    processWinKeyerBuffer();
  }

  processStatus();
}

// Winkeyer 2.3 manual says the potentiometer range is divided
// into 32 slots numbered 0 to 31, but the same manual states
// there are 6 bits to store those slots, this is 0 to 63.
// All programs seems to be happy with the 64 slots.
void Winkeyer::sendPotentiometer(){
  if (_mode){
    uint8_t s;
    uint8_t speed = settings.getSpeed();

    if (speed < _pot_min){
      s = 0;
    } else {
      s = speed - _pot_min;
      if (s > 0x3F){
        s = 0x3F;
      }
    }

    Serial.write(0x80 | s);
  }
}

uint8_t Winkeyer::getHostOpened(){
  return _mode != 0;
}

uint8_t Winkeyer::getSerialEchoback(){
  if (_mode){
    return (_modereg & (1 < WK_MODE_SERIAL_ECHOBACK_BIT)) != 0;
  } else {
    return 1;
  }
}

uint8_t Winkeyer::getPaddleEchoback(){
  if (_mode){
    return (_modereg & (1 < WK_MODE_PADDLE_ECHOBACK_BIT)) != 0;
  } else {
    return 1;
  }
}

void Winkeyer::setSerial(unsigned long baud){
  if (Serial){
    Serial.end();
  }
  // Warning: Winkeyer uses 8N2
  Serial.begin(baud, SERIAL_8N2);
}

void Winkeyer::processStatus(){
  static uint8_t lastStatus = WK_STATUS_DEFAULT;
  if (_mode){
    uint8_t status = getStatus();
    if (status != lastStatus){
      Serial.write(status);
      lastStatus = status;
    }
  }
}

uint8_t Winkeyer::getStatus(){
  uint8_t status = WK_STATUS_DEFAULT;

  // Check keyer buffer status
  uint8_t keyerBufferUsage = dataBuffer.count();
  if (keyerBufferUsage > ((WK_DATABUFFER_SIZE * 2) / 3)){
    status |= (1 << WK_STATUS_XOFF_BIT);
  }

  if (keyerBufferUsage < (WK_DATABUFFER_SIZE / 3)){
    status &= ~(1 << WK_STATUS_XOFF_BIT);
  }

  // Check paddles break-in
  if ((micros() - paddles.getLastTimestamp()) < settings.get_ptt_timeout()){
    status |= (1 << WK_STATUS_BREAKIN_BIT);
  } else {
    status &= ~(1 << WK_STATUS_BREAKIN_BIT);
  }

  // Check busy
  if (keyer.bufferCount() || dataBuffer.count()){
    status |= (1 << WK_STATUS_BUSY_BIT);
  } else {
    status &= ~(1 << WK_STATUS_BUSY_BIT);
  }

  return status;
}

void Winkeyer::processWinKeyerCommands(uint8_t cmd){
  // Put cmd in buffer
  cmdBuffer.write(cmd);

  // if buffer goes full, something wrong happened. Clear it
  if (cmdBuffer.count() == WINKEYER_CMDBUFFER_SIZE){
    cmdBuffer.clear();
  }

  // There must be at lest one byte in the buffer
  if (cmdBuffer.count()){
    switch(cmdBuffer.read(0)){
      case 0x00:
        if (cmdBuffer.count() > 1){
          // Admin commands
          switch(cmdBuffer.read(1)){
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
              if (cmdBuffer.count() > 2){
                cmdBuffer.clear();
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
              cmdBuffer.clear();
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
              sound.ascent();
              dataBuffer.clear();
              cmdBuffer.clear();
              _mode = WK_MODE_1;
              Serial.write(23); // Version 2.3
              Serial.write(getStatus());
            break;

            case 0x03: 
              // (Admin)Host Close
              // CMDBuffer.buffer = [00] [03] ...
              //
              // Use this command to turn off the host interface. Winkeyer2 will
              // return to standby mode after this command is issued. Standby
              // settings will be restored.
              sound.descent();
              _mode = OFF;
              setSerial(BAUDS_LOW);
              dataBuffer.clear();
              cmdBuffer.clear();
            break;

            case 0x04: 
              // (Admin)Echo Test
              // CMDBuffer.buffer = [00] [04] [arg] ...
              //
              // Used to test the serial interface. The next character sent to
              // Winkeyer2 after this command will be echoed back to the host.
              if (cmdBuffer.count() > 2){
                Serial.write(cmdBuffer.read(2));
                cmdBuffer.clear();
              }
            break;

            case 0x05: 
              // (Admin)Paddle A2D
              // CMDBuffer.buffer = [00] [05] ...
              //
              // Historical command not supported in WK2, always returns 0.
              Serial.write(0);
              cmdBuffer.clear();
            break;

            case 0x06: 
              // (Admin)Speed A2D
              // CMDBuffer.buffer = [00] [06] ...
              //
              // Historical command not supported in WK2, always returns 0.
              Serial.write(0);
              cmdBuffer.clear();
            break;

            case 0x07: 
              // (Admin)Get values
              // CMDBuffer.buffer = [00] [07] ...
              //
              // Returns all of the internal setup parameters. They are sent
              // back in the same order as issued by the Load Defaults command.
              // Again, this command is a diagnostic aid. Only issue this command
              // when host interface is closed.
              Serial.write(_dummyModeRegister);             // (cmd 0x0E) Mode register
              Serial.write(settings.getSpeed());            // (cmd 0x02) Speed in WPM
              Serial.write(_dummySidetoneFreq);             // (cmd 0x01) Sidetone frequency
            //Serial.write(settings.getWeight());           // (cmd 0x03) Weight
              Serial.write(_dummyWeight);                   // (cmd 0x03) Weight
              Serial.write(_dummyLeadinTime);               // (cmd 0x04) Lead-in time
              Serial.write(_dummyTailTime);                 // (cmd 0x04) Tail time
              Serial.write(_pot_min);                       // (cmd 0x05) Min WPM
              Serial.write(_pot_range);                     // (cmd 0x05) WPM range
              Serial.write(_dummyFirstExtension);           // (cmd 0x10) 1st extension
              Serial.write(_dummyKeyCompensation);          // (cmd 0x11) Key compensation
              Serial.write(_dummyFarnsworthWPM);            // (cmd 0x0D) Farnsworth WPM
              Serial.write(_dummyPaddleSetpoint);           // (cmd 0x12) Paddle Setpoint
            //Serial.write((10 * settings.getRatio()) / 6); // (cmd 0x17) Dit/Dah ratio
              Serial.write(_dummyRatio);                    // (cmd 0x17) Dit/Dah ratio
              Serial.write(_dummyPinConfiguration);         // (cmd 0x09) Pin configuration
              Serial.write(0x00);                           // Don't care, always zero
              cmdBuffer.clear();
            break;

            case 0x08: 
              // (Admin)Reserved (Debug)
              // CMDBuffer.buffer = [00] [08] ...
              //
              cmdBuffer.clear();
            break;

            case 0x09: 
              // (Admin)Get Cal
              // CMDBuffer.buffer = [00] [09] ...
              //
              // Historical command not supported in WK2, always returns 0.
              Serial.write(0);
              cmdBuffer.clear();
            break;

            case 0x0A: 
              // (Admin)Set WK1 Mode
              // CMDBuffer.buffer = [00] [0A] ...
              //
              // Disables pushbutton reporting
              _mode = WK_MODE_1;
              cmdBuffer.clear();
            break;

            case 0x0B: 
              // (Admin)Set WK2 mode
              // CMDBuffer.buffer = [00] [0B] ...
              //
              // Enables pushbutton reporting, alternate WK status mode is selected.
              _mode = WK_MODE_2;
              cmdBuffer.clear();
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
              cmdBuffer.clear();
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
              cmdBuffer.clear();
            break;

            case 0x0E: 
              // (Admin)Send Standalone Message
              // CMDBuffer.buffer = [00] [0E] [arg] ...
              //
              // Command WK2 to send one of its internal messages. The command
              // syntax is: <00><14><msg number> where number is 1 through 6
              //
              // Just read the message number and do nothing
              if (cmdBuffer.count() > 2){
                cmdBuffer.clear();
              }
            break;

            case 0x0F: 
              // (Admin)Load XMODE
              // CMDBuffer.buffer = [00] [0F] [arg] ...
              //
              // Load mode extension register.
              //
              // Just read the byte and ignores it
              if (cmdBuffer.count() > 2){
                cmdBuffer.clear();
              }
            break;

            case 0x10: 
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [10] ...
              //
              // If this command is issued, WK2 will respond with a zero.
              Serial.write(0);
              cmdBuffer.clear();
            break;

            case 0x11: 
              // (Admin)Set High Baud
              // CMDBuffer.buffer = [00] [11] ...
              //
              // Change Baud Rate to 9600 baud
              setSerial(BAUDS_HIGH);
              cmdBuffer.clear();
            break;

            case 0x12: 
              // (Admin)Set Low Baud
              // CMDBuffer.buffer = [00] [12] ...
              //
              // Change Baud Rate to 1200 (default)
              setSerial(BAUDS_LOW);
              cmdBuffer.clear();
            break;

            case 0x13: 
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [13] ...
              //
              // Function TBA
              cmdBuffer.clear();
            break;

            case 0x14: 
              // (Admin)Reserved
              // CMDBuffer.buffer = [00] [14] ...
              //
              //  Currently unassigned
              cmdBuffer.clear();
            break;

            default:
              cmdBuffer.clear();
            break;
          }
        }
      break;

      case 0x01: 
        // Sidetone frequency
        // CMDBuffer.buffer = [01] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          _dummySidetoneFreq = cmdBuffer.read(1);
            cmdBuffer.clear();
        }
      break;
          
      case 0x02: 
        // Speed
        // CMDBuffer.buffer = [02] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          //setSpeed(cmdBuffer.read(1));
          settings.setSpeed(cmdBuffer.read(1));
          cmdBuffer.clear();
        }
      break;
      
      case 0x03: 
        // Weighting
        // CMDBuffer.buffer = [03] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          // Unimplemented on purpose
          _dummyWeight = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x04: 
        // PTT Lead-in/Tail
        // CMDBuffer.buffer = [04] [arg1] [arg2] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 2){
          _dummyLeadinTime = cmdBuffer.read(1);
          _dummyTailTime = cmdBuffer.read(2);
        cmdBuffer.clear();
        }
      break;
      
      case 0x05: 
        // Speed Pot Setup
        // CMDBuffer.buffer = [05] [arg1] [arg2] [arg3] ...
        //
        if (cmdBuffer.count() > 3){
          _pot_min = cmdBuffer.read(1);
          _pot_range = cmdBuffer.read(2);
          // arg3 is not used
          cmdBuffer.clear();
        }
      break;
      
      case 0x06: 
        // Pause
        // CMDBuffer.buffer = [06] [arg1] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          cmdBuffer.clear();
        }
      break;
      
      case 0x07: 
        // Get Speed Pot
        // CMDBuffer.buffer = [07] ...
        //
        sendPotentiometer();
        cmdBuffer.clear();
      break;
      
      case 0x08: 
        // Backspace
        // CMDBuffer.buffer = [08] ...
        //
        dataBuffer.bufferBackSpace();
        cmdBuffer.clear();
      break;
      
      case 0x09: 
        // Pin configuration
        // CMDBuffer.buffer = [09] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          _dummyPinConfiguration = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x0A: 
        // Clear buffer
        // CMDBuffer.buffer = [0A] ...
        //
        cmdBuffer.clear();
        dataBuffer.clear();
      break;
      
      case 0x0B: 
        // Key inmediate
        // CMDBuffer.buffer = [0B] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          cmdBuffer.clear();
        }
      break;
      
      case 0x0C: 
        // HSCW Speed
        // CMDBuffer.buffer = [0C] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          cmdBuffer.clear();
        }
      break;
      
      case 0x0D: 
        // Farnsworth speed
        // CMDBuffer.buffer = [0D] [arg] ...
        //
        // Not implemented   on purpose
        if (cmdBuffer.count() > 1){
          _dummyFarnsworthWPM = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x0E: 
        // Winkeyer2 mode
        // CMDBuffer.buffer = [0E] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          _modereg = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x0F: 
        // Load defaults
        // CMDBuffer.buffer = [0F] [arg1] [arg2] ... [arg15] 
        //
        if (cmdBuffer.count() > 16){
          _modereg = cmdBuffer.read(1);                // (cmd 0x0E) Mode register
          settings.setSpeed(cmdBuffer.read(2));        // (cmd 0x02) Speed in WPM
          _dummySidetoneFreq = cmdBuffer.read(3);      // (cmd 0x01) Sidetone frequency
          _dummyWeight = cmdBuffer.read(4);            // (cmd 0x03) Weight
          _dummyLeadinTime = cmdBuffer.read(5);        // (cmd 0x04) Lead-in time
          _dummyTailTime = cmdBuffer.read(6);          // (cmd 0x04) Tail time
          _pot_min = cmdBuffer.read(7);                // (cmd 0x05) Min WPM
          _pot_range = cmdBuffer.read(8);              // (cmd 0x05) WPM range
          _dummyFirstExtension = cmdBuffer.read(9);    // (cmd 0x10) 1st extension
          _dummyKeyCompensation = cmdBuffer.read(10);  // (cmd 0x11) Key compensation
          _dummyFarnsworthWPM = cmdBuffer.read(11);    // (cmd 0x0D) Farnsworth WPM
          _dummyPaddleSetpoint = cmdBuffer.read(12);   // (cmd 0x12) Paddle Setpoint
          _dummyRatio = cmdBuffer.read(0x0D);          // (cmd 0x17) Dit/Dah ratio
          _dummyPinConfiguration = cmdBuffer.read(14); // (cmd 0x09) Pin configuration
          cmdBuffer.clear();
        }
      break;
      
      case 0x10: 
        // First Extension
        // CMDBuffer.buffer = [10] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          _dummyFirstExtension = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x11: 
        // Key Compensation
        // CMDBuffer.buffer = [11] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          _dummyKeyCompensation = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x12: 
        // Paddle Switchpoint
        // CMDBuffer.buffer = [12] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          _dummyPaddleSetpoint = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      case 0x13: 
        // Null
        // CMDBuffer.buffer = [13] ...
        //
        // NOP
        cmdBuffer.clear();
      break;
      
      case 0x14: 
        // S/W Paddle Input
        // CMDBuffer.buffer = [14] [arg] ...
        //
        // Not implemented on purpose
        if (cmdBuffer.count() > 1){
          cmdBuffer.clear();
        }
      break;
      
      case 0x15: 
        // Winkeyer2 Status
        // CMDBuffer.buffer = [15] ...
        //
        Serial.write(getStatus());
        cmdBuffer.clear();
      break;

      // Well... this is the infamous Winkeyer's Command 16. The
      // manual says it is detailed described in a separate application
      // note but that application note was never released so all behavior
      // has been reverse engineering using N1MM, with does an extensive
      // use of this command for every conceivable operation. Current
      // implementation seems to work but nothing is guaranteed
      //
      case 0x16: //  Input Buffer Command Set
        if (cmdBuffer.count() > 1){
          switch (cmdBuffer.read(1)){
            case 0x00: 
              // Reset input buffer pointers
              // CMDBuffer.buffer = [16] [00]
              //
              dataBuffer.clear();
              cmdBuffer.clear();
            break;

            case 0x01: 
              // Move input pointer to new position in overwrite mode
              // CMDBuffer.buffer = [16] [01] [arg] ...
              //
              if (cmdBuffer.count() > 2){
                dataBuffer.setCmd16Ptr(cmdBuffer.read(2));
                dataBuffer.setMode(WK_DATABUFFER_MODE_OVERWRITE);
                cmdBuffer.clear();
              }
            break;
            
            case 0x02: 
              // Move input pointer to new position in append mode
              // CMDBuffer.buffer = [16] [01] [arg] ...
              //
              if (cmdBuffer.count() > 2){
                dataBuffer.setCmd16Ptr(cmdBuffer.read(2));
                dataBuffer.setMode(WK_DATABUFFER_MODE_APPEND);
                cmdBuffer.clear();
              }
            break;

            case 0x03: 
              // Add multiple nulls to the buffer 
              // CMDBuffer.buffer = [16] [03] [arg] ...
              //
              if (cmdBuffer.count() > 2){
                int nn = cmdBuffer.read(2);
                for (int n = 0; n < nn; n++){
                  dataBuffer.write(0x00);
                }
                cmdBuffer.clear();
              }
            break;

            default:
              cmdBuffer.clear();
            break;
          };
        }
      break;
      
      case 0x17: 
        // Dih/Dah ratio
        // CMDBuffer.buffer = [17] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          // Unimplemented on purpose
          _dummyRatio = cmdBuffer.read(1);
          cmdBuffer.clear();
        }
      break;
      
      // From this point they are all buffered
      // commands, send them to wkbuffer() to be
      // processed by processWinKeyerBuffer()

      case 0x18: 
        // PTT Control
        // CMDBuffer.buffer = [18] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          dataBuffer.write(cmdBuffer.read(0)); // Command
          dataBuffer.write(cmdBuffer.read(1)); // Argument
          cmdBuffer.clear();
        }
      break;
      
      case 0x19: 
        // Timed Key Down
        // CMDBuffer.buffer = [19] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          dataBuffer.write(cmdBuffer.read(0)); // Command
          dataBuffer.write(cmdBuffer.read(1)); // Argument
          cmdBuffer.clear();
        }
      break;
      
      case 0x1A: // Wait
        // Wait
        // CMDBuffer.buffer = [1A] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          dataBuffer.write(cmdBuffer.read(0)); // Command
          dataBuffer.write(cmdBuffer.read(1)); // Argument
          cmdBuffer.clear();
        }
      break;
      
      case 0x1B: 
        // Merge Letters
        // CMDBuffer.buffer = [1B] [arg1] [arg2] ...
        //
        if (cmdBuffer.count() > 2){
          dataBuffer.write(cmdBuffer.read(0)); // Command
          dataBuffer.write(cmdBuffer.read(1)); // Argument 1
          dataBuffer.write(cmdBuffer.read(2)); // Argument 2
          cmdBuffer.clear();
        }
      break;
      
      case 0x1C: 
        // Buffered speed change
        // CMDBuffer.buffer = [1C] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          dataBuffer.write(cmdBuffer.read(0)); // Command
          dataBuffer.write(cmdBuffer.read(1)); // Argument
          cmdBuffer.clear();
        }
      break;
      
      case 0x1D: 
        // HSCW Speed / Port select
        // CMDBuffer.buffer = [1D] [arg] ...
        //
        if (cmdBuffer.count() > 1){
          dataBuffer.write(cmdBuffer.read(0)); // Command
          dataBuffer.write(cmdBuffer.read(1)); // Argument
          cmdBuffer.clear();
        }
      break;
      
      case 0x1E: 
        // Cancel Buffered Speed Change
        // CMDBuffer.buffer = [1E] ...
        //
        dataBuffer.write(cmdBuffer.read(0)); // Command
        cmdBuffer.clear();
      break;
      
      case 0x1F: 
        // Buffered NOP
        // CMDBuffer.buffer = [1F] ...
        //
        dataBuffer.write(cmdBuffer.read(0)); // Command
        cmdBuffer.clear();
      break;

      // Ascii chars
      case 0x20 ... 0x7F:
        dataBuffer.write(cmdBuffer.read(0));
        cmdBuffer.clear();
      break;

      default:
          cmdBuffer.clear();
      break;
    }
  }
}

void Winkeyer::processWinKeyerBuffer(){
  if (dataBuffer.count()){
    uint8_t b = dataBuffer.read();
    // Buffered commands
    switch (b){
      case 0x18: // PTT Control
        // Not implemented
        dataBuffer.read();
      break;
      
      case 0x19: // Timed Key Down
        // Not implemented
        dataBuffer.read();
      break;
      
      case 0x1A: // Wait
        // Not implemented
        dataBuffer.read();
      break;
      
      case 0x1B: // Merge Letters (Prosigns)
        {
          char c1 = dataBuffer.read();
          char c2 = dataBuffer.read();
          keyer.sendProsign(c1, c2);
        }
      break;
      
      case 0x1C: // Speed change
        _old_speed = settings.getSpeed();
        //setSpeed(dataBuffer.read());
        keyer.sendCommand(KCMD_SPEED, dataBuffer.read());
      break;
      
      case 0x1D: // HSCW Speed / Port select
        // Not implemented
        dataBuffer.read();
      break;
      
      case 0x1E: // Cancel Buff Speed
        if (_old_speed){
          //setSpeed(old_speed);
          keyer.sendCommand(KCMD_SPEED, _old_speed);
          _old_speed = 0;
        }
      break;
      
      case 0x1F: // Buffered NOP
        // Do nothing
      break;

      // Ascii chars
      case 0x20 ... 0x7F:
        keyer.sendChar(b);
      break;
    }
  }
}
