# ka-keyer
## Simple keyer firmware for the Open CW Keyer MK2

![OpenCWKeyerMK2](https://github.com/ea4eoz/ka-keyer/blob/main/images/OpenCWKeyerMK2.jpg?raw=true)

ka-keyer is a simple firmware with an emphasis on high-speed operation for the Open CW Keyer MK2, an Arduino-based CW keyer from OK1CDJ. You can find the Open CW Keyer MK2 already assembled and ready for use or in kit form in all popular web shops.

## Features

- No bells and whistles. Just an almost dumb keyer you can use without a computer. If you need / want keying commands, memories, automatic serial numbers, and so on, this is not the keyer firmware you need.

- Iambic A, Iambic B or Ultimate mode keying.

- Adjustable keying speed from 15 up to 70 WPM

- Adjustable dot weight between 30% and 50%

- Adjustable dih-dah ratio from 3.0 to 4.0

- Partial compatibility with WinKeyer 2.3

## Usage

The __Potentiometer__ is used to change keying speed, weight and ratio.

The __Set__ button will change between speed / weight / ratio adjustemnt modes for the potentiometer. Each time this button is pressed, the keyer will say __S__ for speed mode, __W__ for weight mode and __R__ for ratio mode and the potentiometer will change that setting. The keyer starts always in speed mode, but weight and ratio adjustments are preserved in EEPROM to survive power off.

__A long press on Set__ (larger than one second) will change between Iambic A, Iambic B and Ultimate keying modes. Iambic A will be announced with __IA__, Iambic B with __IB__ and ultimate mode with __UL__. This setting will be preserved in EEPROM to survive a power off.

The __Button 1__ enables or disables the sidetone function. This setting will  be preserved in EEPROM to survive a power off.

__A long press on Button 1__ will swap the paddles. Taking as reference the 3.5mm jack, you will hear __A__ for the dit at the tip and dah at the ring of the jack or __N__ for dah at the tip and dit at the ring of the jack. This setting will be preserved in EEPROM to survive a power off.

![jackPaddles](https://github.com/ea4eoz/ka-keyer/blob/main/images/jackPaddles.jpg?raw=true)

The __Button 2__ will announce using the built-in buzzer the current keying speed, weight or ratio according to the potentiometer mode selected with __Set__ button. For example __W20__ for 20 WPM, __W45__ for a weight of 45% and __R32__ for a 1:3.2 dit/dah ratio.

__A long press on Button 2__ will announce the firmware version. Current version is __V1__.

If you use ka-keyer next to a computer, you can connect to it and open its serial port (1200-8N2) with any terminal program for some extra functions:

- Any character sent to the serial port will be keyed using the current speed / weight / ratio values

- Ka-keyer will __try__ to decode your actions to the paddles, sending it back to the computer using the serial port.

- Ka-keyer has some level of compatibility with WinKeyer, so you can use it with logging and contest programs, like Fldigi, N1MM, etc.

## Compiling and flashing

Before trying to to flash your Open CW Keyer MK2, remove the jumper marked PRG at the back panel.

The firmware uses no external libraries, and it is contained in just one .ino file so it should be very easy to make it to compile. Most (if not all) Open CW Keyers MK2 come with an Arduino Nano using at ATmega328p flashed with the old bootloader, so for a sucessful flashing, after loading the ka-keyer.ino file, you must set up the arduino IDE in this way:

Menu ---> Tools ---> Board ---> Arduino AVR Boards ---> Arduino Nano

Menu ---> Tools ---> Processor ---> ATmega328p (Old Bootloader)

Menu ---> Tools ---> Port ---> Select the serial port your OpenCWKeyerMK2 is attached to.

After this, using Menu ---> Sketch ---> Upload (or Ctrl-U) should flash your Open CW Keyer MK2 with ka-keyer firmware without problems. If flashing was sucessful you should hear the leter __K__.

Remember to put in back the jumper at the rear of the keyer to prevent accidental flashing and / or keyer resets because the computer's operative system polling the serial ports.

Many programs can't use the keyer without the jumper because it resets itself everytime the program opens the serial port (the jumper prevent this). If this is happening, you will hear a __K__ everytime the program is trying to connect with the keyer.

Ka-keyer has been developed and tested using Arduino IDE version 2.3.2 but any other version should work just fine.

## Recomendations

After changing a weight or ratio value using the potentiometer it is recommended to press again the button __Set__ to store the value into the internal EEPROM memory. Values are stored after pressing __Set__.

The Open CW Keyer MK2, both in kit and mounted form seems to come with a logarithmic 20k potentiometer. This is not optimal because the higher values are very compressed at the end of the potentiometer's range. If this is a issue for you, I recommend you to replace the potentiometer with a linear one.

## Personalization

The first lines in the ka-keyer.ino file are defines. You can tweak them a bit to suit your needs:

```
    #define WPM_LOW 15
    #define WPM_HIGH 70
```

These two lines define the maximun and minumum speed values in WPM for the potentiometer. You can tweak them a bit if you do not plan to use too high or too low speeds. Please do not go over 70 WPM (probably only one of two persons in the worls can) and do not set it under 15 WPM: it is just too slow for paddles. Using WinKeyer commands through a computer you can go as low as 5 WPM and as high as 99 WPM.

```
    #define WEIGHT_LOW 30
    #define WEIGHT_HIGH 50
```

These two lines define the maximum and minimumdit weight. Please, do not go over 50% (it sounds awful) and do not go under 30%, as the dih is too short to be easily heard. Nominal value is 50%.

```
    #define RATIO_LOW 30
    #define RATIO_HIGH 40
```

These two lines define the maximum and minimum ratios for the dash. Nominal value is 30 (1:3).

These ranges has been selected as the most useful ones for high speed CW intelligibility as well as normal and casual Morse speeds. I don't recomment you to change them.

You will get a compiler error if you put bad values into these defines.

## Return to the original firmware

In the case you want to return to the original K3NG firmware that came with your Open CW Keyer MK2, just follow these steps:

- Remove the PROG jumper at the rear of the keyer and connect it to your computer

- Go to https://github.com/k3ng/k3ng_cw_keyer/archive/refs/heads/master.zip and download the zip file containing the source code. It will be named k3ng_cw_keyer-master.zip

- Uncompress the zip file into its own directory (k3ng_cw_keyer-master)

- Look for the ino file located at k3ng_cw_keyer-master/k3ng_keyer/k3ng_keyer.ino and open it in the Arduino IDE. It will take a while.

- Once all tabs are loaded, look for a tab named keyer_hardware.h

- In the tab keyer_hardware.h, go down to the line 29, you will see:

```
//#define HARDWARE_OPENCWKEYER_MK2 // https://github.com/ok1cdj/OpenCWKeyerMK2  edit these files: keyer_features_and_options_opencwkeyer_mk2.h keyer_pin_settings_opencwkeyer_mk2.h keyer_settings_opencwkeyer_mk2.h
```

- Remove the two // at the beginning of the line:

```
#define HARDWARE_OPENCWKEYER_MK2 // https://github.com/ok1cdj/OpenCWKeyerMK2  edit these files: keyer_features_and_options_opencwkeyer_mk2.h keyer_pin_settings_opencwkeyer_mk2.h keyer_settings_opencwkeyer_mk2.h
```

- Now make sure you have the right settings in the Arduino IDE: Board (Arduino Nano), Processor (ATmega328p - Old bootloader), and the right COM port.

- In the Menu, select Sketch, and then Upload.

- If everything was fine, you will hear HI coming from your keyer.

- Remove the keyer from the computer and place the jumper back.

## Links

[https://github.com/ok1cdj/OpenCWKeyerMK2](https://github.com/ok1cdj/OpenCWKeyerMK2) : The Open CW Keyer MK2 project repository.

[https://github.com/k3ng/k3ng_cw_keyer](https://github.com/k3ng/k3ng_cw_keyer) : K3NG Keyer. All Open CW Keyer MK2 seem to come with this firmware preloaded.

## License

The source files of the ka-keyer in this repository are made available under the GPLv3 license.
