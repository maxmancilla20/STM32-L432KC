/*
 * Maximiliano Mancilla
 * ============================= DATA ======================================
 * User LD3: the green LED is a user LED connected to Arduino Nano signal D13
 * corresponding to the STM32 I/O PB3 (pin 26).
 * | AHB2 | 0x4800 0400 - 0x4800 07FF | 1 KB | GPIOB
 * ==========================================================================
 * */
#include <stdint.h>
#include "stm32l4xx.h"

#define PIN3                (1U<<3) 
#define USER_LED_PIN        (PIN3)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)

int main(void)
{
    int8_t FlagStatus = 0;

    /* Enable clock access to GPIOB */
    RCC->AHB2ENR |= GPIOBEN;

    /* Set PB3 AS OUTPUT */
    GPIOB->MODER |= (1U<<6);
    GPIOB->MODER &=~ (1U<<7);

    while(1)
    {
        if(FlagStatus == 0)
        {
            /* Turn on the LED */
            GPIOB->ODR |= USER_LED_PIN;
            FlagStatus = 1;
        }
        else
        {
            /* Turn off the LED */
            GPIOB->ODR &= ~USER_LED_PIN;
            FlagStatus = 0;
        }
        /* Toggle the LED */
        //GPIOB->ODR ^= USER_LED_PIN;


        /* Delay */
        for(int i = 0; i < 100000; i++){}
    }

    return 0;
}