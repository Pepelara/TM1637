#include "stm32g0xx_hal.h"
#include<stdio.h>
#include<tm1637.h>

// note: this file is illustration how to use tm1637 lib.
// You should define you own I/O functions for your device

void mypinMode(GPIO_TypeDef *GPIOx, int pin, int mode);
void mybitDelay(int c);

int main(void) {

    //gpio_init();  //UNECESSARY, this lib will do pin init.
    
    
    tm1637_lib_t myLib;
    myLib.user_pinMode = mypinMode;
    myLib.user_writePin = HAL_GPIO_WritePin;
    myLib.user_portDelay = mybitDelay;
    
    tm1637_dev_t display;
    display.pinClk =  1<<4;
    display.pinDIO =  1<<5;
    display.bitDelay = 1024;
    display.GPIOx = GPIOA;
    
    tm1637_dev_t display2;
    display.pinClk =  1<<4;
    display.pinDIO =  1<<6;
    display.bitDelay = 1024;
    display.GPIOx = GPIOA;
    
    tm1637_lib_init(&myLib); //YOU MUST CALL THIS
    
    tm1637_init(&display); //after this you can use display as is
    tm1637_init(&display2);
    tm1637_setBrightness(&display, 0x0f, true);

    int i = 0;
    while(1) {
        tm1637_showNumberDec(&display, i++, false);
        tm1637_showNumberHexEx(&display2, i, false);
        HAL_Delay(1000);
    }
}


void mypinMode(GPIO_TypeDef *GPIOx, int pin, int mode)
{
    
    //This is stm32 example. Do your own low level init.
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = mode == INPUT ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT_PP; //INPUT is often 0
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void mybitDelay(int c)
{
    for(int i = 0; i < c; i++)
        HAL_Delay(1);
    /*  If using freertos (is 10 as not to spam task scheduler)
        for(int i = 0; i < c/10; i++)
            vTaskDelay(10)
    */
    /* You may also use timer (useful for 8051, example is stm32)
    
        uint32_t start = TIM3->CNT;
        uint32_t duration = c * 8;
        while ((TIM3->CNT - start) < duration);
    
    */
}

