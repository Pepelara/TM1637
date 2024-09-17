//MIT License

//Copyright (c) 2024 Pepelara

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

//VERSION: v0.0.1

#ifndef __TM1637DISPLAY__
#define __TM1637DISPLAY__

#include <inttypes.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"

#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000

#define DEFAULT_BIT_DELAY  100

typedef struct {
    GPIO_TypeDef *GPIOx;
    uint32_t pinClk;
    uint32_t pinDIO;
    uint32_t brightness;
    unsigned int bitDelay;
}tm1637_dev_t;

typedef struct {
    void (*user_pinMode)(void *port, int pin, int mode);
    void (*user_writePin)(void *port, int pin, int out);
    void (*user_portDelay)(int delay);
}tm1637_lib_t;

void pinMode(GPIO_TypeDef *GPIOx, int pin, int mode);


void tm1637_init(tm1637_dev_t *dev);
void tm1637_lib_init(tm1637_lib_t *lib);

//! Sets the brightness of the display.
//!
//! The setting takes effect when a command is given to change the data being
//! displayed.
//!
//! @param brightness A number from 0 (lowes brightness) to 7 (highest brightness)
//! @param on Turn display on or off
void tm1637_setBrightness(tm1637_dev_t *dev, uint8_t brightness, bool on);

//! Display arbitrary data on the module
//!
//! This function receives raw segment values as input and displays them. The segment data
//! is given as a byte array, each byte corresponding to a single digit. Within each byte,
//! bit 0 is segment A, bit 1 is segment B etc.
//! The function may either set the entire display or any desirable part on its own. The first
//! digit is given by the @ref pos argument with 0 being the leftmost digit. The @ref length
//! argument is the number of digits to be set. Other digits are not affected.
//!
//! @param segments An array of size @ref length containing the raw segment values
//! @param length The number of digits to be modified
//! @param pos The position from which to start the modification (0 - leftmost, 3 - rightmost)
void tm1637_setSegments_long(tm1637_dev_t *dev, const uint8_t segments[], uint8_t length, uint8_t pos);
void tm1637_setSegments(tm1637_dev_t *dev, const uint8_t segments[]);

//! Clear the display
void tm1637_clear(tm1637_dev_t *dev);

//! Display a decimal number
//!
//! Dispaly the given argument as a decimal number.
//!
//! @param num The number to be shown
//! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
//!        blank. NOTE: leading zero is not supported with negative numbers.
//! @param length The number of digits to set. The user must ensure that the number to be shown
//!        fits to the number of digits requested (for example, if two digits are to be displayed,
//!        the number must be between 0 to 99)
//! @param pos The position of the most significant digit (0 - leftmost, 3 - rightmost)
void tm1637_showNumberDec_long(tm1637_dev_t *dev, int num, bool leading_zeroF, uint8_t length, uint8_t pos);
void tm1637_showNumberDec(tm1637_dev_t *dev, int num, bool leading_zeroF);

//! Display a decimal number, with dot control
//!
//! Dispaly the given argument as a decimal number. The dots between the digits (or colon)
//! can be individually controlled.
//!
//! @param num The number to be shown
//! @param dots Dot/Colon enable. The argument is a bitmask, with each bit corresponding to a dot
//!        between the digits (or colon mark, as implemented by each module). i.e.
//!        For displays with dots between each digit:
//!        * 0.000 (0b10000000)
//!        * 00.00 (0b01000000)
//!        * 000.0 (0b00100000)
//!        * 0.0.0.0 (0b11100000)
//!        For displays with just a colon:
//!        * 00:00 (0b01000000)
//!        For displays with dots and colons colon:
//!        * 0.0:0.0 (0b11100000)
//! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
//!        blank. NOTE: leading zero is not supported with negative numbers.
//! @param length The number of digits to set. The user must ensure that the number to be shown
//!        fits to the number of digits requested (for example, if two digits are to be displayed,
//!        the number must be between 0 to 99)
//! @param pos The position of the most significant digit (0 - leftmost, 3 - rightmost)
void tm1637_showNumberDecEx_long(tm1637_dev_t *dev, int num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos);
void tm1637_showNumberDecEx(tm1637_dev_t *dev, int num, uint8_t dots, bool leading_zero);

//! Display a hexadecimal number, with dot control
//!
//! Dispaly the given argument as a hexadecimal number. The dots between the digits (or colon)
//! can be individually controlled.
//!
//! @param num The number to be shown
//! @param dots Dot/Colon enable. The argument is a bitmask, with each bit corresponding to a dot
//!        between the digits (or colon mark, as implemented by each module). i.e.
//!        For displays with dots between each digit:
//!        * 0.000 (0b10000000)
//!        * 00.00 (0b01000000)
//!        * 000.0 (0b00100000)
//!        * 0.0.0.0 (0b11100000)
//!        For displays with just a colon:
//!        * 00:00 (0b01000000)
//!        For displays with dots and colons colon:
//!        * 0.0:0.0 (0b11100000)
//! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
//!        blank
//! @param length The number of digits to set. The user must ensure that the number to be shown
//!        fits to the number of digits requested (for example, if two digits are to be displayed,
//!        the number must be between 0 to 99)
//! @param pos The position of the most significant digit (0 - leftmost, 3 - rightmost)
void tm1637_showNumberHexEx_long(tm1637_dev_t *dev, uint16_t num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos);
void tm1637_showNumberHexEx(tm1637_dev_t *dev, uint16_t num, uint8_t dots, bool leading_zero);

//! Translate a single digit into 7 segment code
//!
//! The method accepts a number between 0 - 15 and converts it to the
//! code required to display the number on a 7 segment display.
//! Numbers between 10-15 are converted to hexadecimal digits (A-F)
//!
//! @param digit A number between 0 to 15
//! @return A code representing the 7 segment image of the digit (LSB - segment A;
//!         bit 6 - segment G; bit 7 - always zero)
uint8_t tm1637_encodeDigit(uint8_t digit);

void tm1637_bitDelay(tm1637_dev_t *dev);
void tm1637_start(tm1637_dev_t *dev);

void tm1637_stop(tm1637_dev_t *dev);

bool tm1637_writeByte(tm1637_dev_t *dev, uint8_t b);

void tm1637_showDots(uint8_t dots, uint8_t* digits);

void tm1637_showNumberBaseEx_long(tm1637_dev_t *dev, int8_t base, uint16_t num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos);
void tm1637_showNumberBaseEx(tm1637_dev_t *dev, int8_t base, uint16_t num, uint8_t dots, bool leading_zero);

#endif // __TM1637DISPLAY__
