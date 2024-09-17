TM1637
======
C library for TM1637 (LED Driver)


Description
-----------
A C library for 7-segment display modules based on the TM1637 chip, such as Seeed Studio's [Grove 4 digit display](http://www.seeedstudio.com/depot/grove-4digit-display-p-1198.html). The TM1637 chip also has keyboard input capability, but it's not implemented in this library.

This library is based on https://github.com/avishorp/TM1637

Hardware Connection
-------------------
The display modules has two signal connection (and two power connections) which are CLK and DIO. These pins can be connected to any pair of digital pins on the your device. When a struct is declared, the pins should be configured.

You can share clock pins and only have different data pins. Refer to the provided example code.

Installation
------------
Put it somewhere your compiler will see.

Usage
-----

Some of the functions you are provided with:
    
* `tm1637_lib_init` - Initialize the library (pass hardware specific stuff)
* `tm1637_init` - Initialize the display.
* `setSegments` - Set the raw value of the segments of each digit
* `showNumberDec` - Display a decimal number
* `showNumberDecEx` - Display a decimal number with decimal points or colon
* `setBrightness` - Sets the brightness of the display

The information given above is only a summary. Please refer to tm1637.h for more information. An example is included, demonstrating the operation of some of the functions.
