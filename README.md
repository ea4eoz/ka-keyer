# KA-Keyer
## A simple keyer for the high speed CW operators

![KA-Keyer](https://github.com/ea4eoz/ka-keyer/blob/main/images/ka-keyer.jpg?raw=true)

KA-Keyer is a simple keyer with an emphasis on high-speed operation. It can be considered an improved fork from the Open CW Keyer MK2, an Arduino-based CW keyer from OK1CDJ.

## Features

- No bells and whistles. Just an almost dumb keyer you can use with or without a computer. If you need / want keying commands, memories, automatic serial numbers, and so on, this is not the keyer you need.

- Plain iambic, iambic A, iambic B or ultimate mode keying.

- Adjustable keying speed from 10 up to 99 WPM

- Adjustable dot weight between 25% and 75%

- Adjustable dih-dah ratio from 2.0 to 4.0

- Partial compatibility with WinKeyer

## Usage

The user interface is quite simple. A short press on the rotary button changes between menu entries and the rotary action changes the value of the selected menu.

There are two sets of menus: The most often used and the less often used.

The first set of menus contains the most used options: keying speed, (dot) weight and (dash) ratio adjustments. The second set of menus contais some less used adjustments, like sidetone adjustments, keyer mode and reversing of paddles. You can change between the two menu sets with a long press on the rotary button.

KA-Keyer has some level of compatibility with WinKeyer, so you can use it with logging and contest programs, like Fldigi, N1MM, etc.

## Compiling and flashing new firmware versions

Just like the Open CW Keyer MK2, remove the jumper marked PRG at the back panel.

The firmware uses no external libraries so it should be very easy to make it to compile. For a sucessful flashing, after loading the ka-keyer.ino file, you must set up the arduino IDE in this way:

Menu ---> Tools ---> Board ---> Arduino AVR Boards ---> Arduino Nano

Menu ---> Tools ---> Processor ---> ATmega328p (Old Bootloader)

Menu ---> Tools ---> Port ---> Select the serial port your OpenCWKeyerMK2 is attached to.

After this, using Menu ---> Sketch ---> Upload (or Ctrl-U) should flash your keyer with KA-Keyer firmware without problems. If flashing was sucessful you should hear the leter __K__ while the screen shows the actual firmware version.

While most of the Arduinos Nano I had came with the old bootloader, lately a few of them came with the new one, so if you get errors trying to flash your unit, try Menu ---> Tools ---> Processor ---> ATmega328p

Remember to put in back the jumper at the rear of the keyer after flashing it to prevent accidental flashing and / or keyer resets because the computer's operative system polling the serial ports.

Many programs can't use the keyer without the jumper because it resets itself everytime the program opens the serial port (the jumper prevent this). If this is happening, you will hear a __K__ everytime the program is trying to connect with the keyer because it is resetting itself.

KA-keyer has been developed and tested using Arduino IDE version 2.3.2 but any other version should work just fine.

## Personalization

In the case the actual speed / weight / ratio limits are too broad for you you can reconfigure those limits powering-up the unit while pressing the rotary button. A KA-Keyer Settings message will appear and you will be able to set your desired limits. Move between fields with a press on the rotary button and save them with a long press over the Save word.

## Known issues

Actual Ultimate keyer implementation does not feature dit/dash memory. Ultimate keyer does, but actual high speed operators used to a single lever paddle does not seem to miss them. They will be added when I find the correct way to add them.

KA-Keyer seems to be unstable with some USB adapters, making the CPU to crash. The cure seems to be to place a 100 nF ceramic decoupling capacitor as close as possible to the Atmega328p power pins, and/or add a 47 uF 16 to 25 volts electrolitic capacitor between the 5V and GND pins. Most Arduino Nano clones have very poor decoupling, especially regarding the ceramic capacitor near the CPU power pins.

![Decoupling](https://github.com/ea4eoz/ka-keyer/blob/main/images/decoupling.jpg?raw=true)

## Links

[https://github.com/ok1cdj/OpenCWKeyerMK2](https://github.com/ok1cdj/OpenCWKeyerMK2) : The Open CW Keyer MK2 project repository.

## License

The source files of the KA-Keyer in this repository are made available under the GPLv3 license.
