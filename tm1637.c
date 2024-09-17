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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <tm1637.h>
#include <stdbool.h>

#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80

#define HIGH 1
#define LOW 0

#define INPUT 0
#define OUTPUT 1

static void tm1637_dummy_pinMode(void *port, int pin, int mode);
static void tm1637_dummy_writePin(void *port, int pin, int out);
static void tm1637_dummy_portDelay(int delay);

static tm1637_lib_t tm1637_lib = { 
    .user_pinMode = tm1637_dummy_pinMode, 
    .user_writePin = tm1637_dummy_writePin, 
    .user_portDelay = tm1637_dummy_portDelay
};

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D
const uint8_t digitToSegment[] = {
    // XGFEDCBA
    0b00111111,    // 0
    0b00000110,    // 1
    0b01011011,    // 2
    0b01001111,    // 3
    0b01100110,    // 4
    0b01101101,    // 5
    0b01111101,    // 6
    0b00000111,    // 7
    0b01111111,    // 8
    0b01101111,    // 9
    0b01110111,    // A
    0b01111100,    // b
    0b00111001,    // C
    0b01011110,    // d
    0b01111001,    // E
    0b01110001     // F
};

static const uint8_t minusSegments = 0b01000000;

void tm1637_lib_init(tm1637_lib_t *lib)
{
    tm1637_lib.user_pinMode = lib->user_pinMode;
    tm1637_lib.user_writePin = lib->user_writePin;
    tm1637_lib.user_portDelay = lib->user_portDelay;
}

void tm1637_init(tm1637_dev_t *dev)
{
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, INPUT);
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, INPUT);
    tm1637_lib.user_writePin(dev->GPIOx, dev->pinClk, LOW);
    tm1637_lib.user_writePin(dev->GPIOx, dev->pinDIO, LOW);
}

void tm1637_setBrightness(
    tm1637_dev_t *dev,
    uint8_t brightness,
    bool on)
{
    dev->brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}

void tm1637_setSegments_long(
    tm1637_dev_t *dev,
    const uint8_t segments[],
    uint8_t length,
    uint8_t pos)
{
    // Write COMM1
    tm1637_start(dev);
    tm1637_writeByte(dev, TM1637_I2C_COMM1);
    tm1637_stop(dev);

    // Write COMM2 + first digit address
    tm1637_start(dev);
    tm1637_writeByte(dev, TM1637_I2C_COMM2 + (pos & 0x03));

    // Write the data bytes
    for (uint8_t k=0; k < length; k++)
        tm1637_writeByte(dev, segments[k]);

    tm1637_stop(dev);

    // Write COMM3 + brightness
    tm1637_start(dev);
    tm1637_writeByte(dev, TM1637_I2C_COMM3 + (dev->brightness & 0x0f));
    tm1637_stop(dev);
}

void tm1637_setSegments(
    tm1637_dev_t *dev,
    const uint8_t segments[])
{
    tm1637_setSegments_long(dev, segments, 4, 0);

}

void tm1637_clear(tm1637_dev_t *dev)
{
    uint8_t data[] = { 0, 0, 0, 0 };
    tm1637_setSegments_long(dev, data, 4, 0);
}

void tm1637_showNumberDec_long(
    tm1637_dev_t *dev,
    int num,
    bool leading_zero,
    uint8_t length,
    uint8_t pos)
{
    tm1637_showNumberDecEx_long(dev, num, 0, leading_zero, length, pos);
}

void tm1637_showNumberDec(
    tm1637_dev_t *dev,
    int num,
    bool leading_zero)
{
    tm1637_showNumberDecEx_long(dev, num, 0, leading_zero, 4, 0);
}

void tm1637_showNumberDecEx_long(
    tm1637_dev_t *dev,
    int num,
    uint8_t dots,
    bool leading_zero,
    uint8_t length,
    uint8_t pos)
{
    tm1637_showNumberBaseEx_long(
	dev, num < 0? -10 : 10, 
	num < 0? -num : num, 
	dots, 
	leading_zero, 
	length, 
	pos
    );
}

void tm1637_showNumberDecEx(
    tm1637_dev_t *dev,
    int num,
    uint8_t dots,
    bool leading_zero)
{
    tm1637_showNumberBaseEx_long(
	dev, num < 0? -10 : 10, 
	num < 0? -num : num, 
	dots, 
	leading_zero, 
	4, 
	0
    );
}

void tm1637_showNumberHexEx_long(
    tm1637_dev_t *dev,
    uint16_t num,
    uint8_t dots,
    bool leading_zero,
    uint8_t length,
    uint8_t pos)
{
    tm1637_showNumberBaseEx_long(
	dev,
	16, 
	num, 
	dots, 
	leading_zero, 
	length, 
	pos
    );
}

void tm1637_showNumberHexEx(
    tm1637_dev_t *dev,
    uint16_t num,
    uint8_t dots,
    bool leading_zero)
{
    tm1637_showNumberBaseEx_long(
	dev,
	16, 
	num, 
	dots, 
	leading_zero, 
	4, 
	0
    );
}

void tm1637_showNumberBaseEx_long(
    tm1637_dev_t *dev,
    int8_t base,
    uint16_t num,
    uint8_t dots,
    bool leading_zero,
    uint8_t length,
    uint8_t pos)
{
    bool negative = false;
    if (base < 0) {
        base = -base;
        negative = true;
    }


    uint8_t digits[4];

    if (num == 0 && !leading_zero) {
        // Singular case - take care separately
        for(uint8_t i = 0; i < (length-1); i++)
            digits[i] = 0;
        digits[length-1] = tm1637_encodeDigit(0);
    } else {
        //uint8_t i = length-1;
        //if (negative) {
        //	// Negative number, show the minus sign
        //    digits[i] = minusSegments;
        //	i--;
        //}

        for(int i = length-1; i >= 0; --i)
        {
            uint8_t digit = num % base;

            if (digit == 0 && num == 0 && leading_zero == false)
                // Leading zero is blank
                digits[i] = 0;
            else
                digits[i] = tm1637_encodeDigit(digit);

            if (digit == 0 && num == 0 && negative) {
                digits[i] = minusSegments;
                negative = false;
            }

            num /= base;
        }

        if(dots != 0)
        {
            tm1637_showDots(dots, digits);
        }
    }
    tm1637_setSegments_long(dev, digits, length, pos);
}

void tm1637_showNumberBaseEx(
    tm1637_dev_t *dev,
    int8_t base,
    uint16_t num,
    uint8_t dots,
    bool leading_zero)
{
    tm1637_showNumberBaseEx_long(
        dev,
        base,
        num,
        dots,
        leading_zero,
        4,
        0
    );
}

void tm1637_bitDelay(tm1637_dev_t *dev)
{
    tm1637_lib.user_portDelay(dev->bitDelay);
}

void tm1637_start(tm1637_dev_t *dev)
{
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, OUTPUT);
    tm1637_bitDelay(dev);
}

void tm1637_stop(tm1637_dev_t *dev)
{
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, OUTPUT);
    tm1637_bitDelay(dev);
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, INPUT);
    tm1637_bitDelay(dev);
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, INPUT);
    tm1637_bitDelay(dev);
}

bool tm1637_writeByte(tm1637_dev_t *dev, uint8_t b)
{
    uint8_t data = b;

    // 8 Data Bits
    for(uint8_t i = 0; i < 8; i++) {
        // CLK low
        tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, OUTPUT);
        tm1637_bitDelay(dev);

        // Set data bit
        if (data & 0x01)
            tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, INPUT);
        else
            tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, OUTPUT);

        tm1637_bitDelay(dev);

        // CLK high
        tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, INPUT);
        tm1637_bitDelay(dev);
        data = data >> 1;
    }

    // Wait for acknowledge
    // CLK to zero
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, OUTPUT);
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, INPUT);
    tm1637_bitDelay(dev);

    // CLK to high
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, INPUT);
    tm1637_bitDelay(dev);
    uint8_t ack = HAL_GPIO_ReadPin(dev->GPIOx, dev->pinDIO);
    if (ack == 0)
        tm1637_lib.user_pinMode(dev->GPIOx, dev->pinDIO, OUTPUT);


    tm1637_bitDelay(dev);
    tm1637_lib.user_pinMode(dev->GPIOx, dev->pinClk, OUTPUT);
    tm1637_bitDelay(dev);

    return ack;
}

void tm1637_showDots(uint8_t dots, uint8_t* digits)
{
    for(int i = 0; i < 4; ++i)
    {
        digits[i] |= (dots & 0x80);
        dots <<= 1;
    }
}

uint8_t tm1637_encodeDigit(uint8_t digit)
{
    return digitToSegment[digit & 0x0f];
}

static void tm1637_dummy_pinMode(void *port, int pin, int mode)
{
    return;
}

static void tm1637_dummy_writePin(void *port, int pin, int out)
{
    return;
}

static void tm1637_dummy_portDelay(int delay)
{
    return;
}
