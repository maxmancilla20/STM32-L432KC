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
#define PIN7                (1U<<7)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)


/*Modules:
 * FPU
 * UART
 * TIMEBASE
 * GPIO (BSP)
 * ADC
 * */



#define  GPIOAEN		(1U<<0)
#define  PIN5			(1U<<5)
#define  LED_PIN		PIN5

//bool btn_state;


unsigned char __attribute__((section(".custom_ram_block"))) custom_ram_buff[10];   //10 bytes
unsigned char __attribute__((section(".custom_flash_block"))) custom_flash_buff[50];   //10 bytes

int main(void)
{
    /* Enable clock access to GPIOB */
    RCC->AHB2ENR |= GPIOBEN;

    /* Set PB3 AS OUTPUT MODE B7 = 0, B6 =  1*/
    GPIOB->MODER |= (1U<<6);
    GPIOB->MODER &=~ (1U<<7);

    /* Set PB7 as input B14 = 0, B15 = 0 */
    GPIOB->MODER &=~ (1U<<14);
    GPIOB->MODER &=~ (1U<<15); 

    while(1)
    {
        //GPIOB->ODR |= USER_LED_PIN;  /* Turn on the LED */
        //GPIOB->ODR &= ~USER_LED_PIN; /* Turn off the LED */
        //GPIOB->ODR ^= USER_LED_PIN;  /* Toggle the LED */
        //for(int i = 0; i < 100000; i++){} /* Delay */
        
        if(GPIOB->IDR & PIN7) /* Check if the button is pressed */
        {
            GPIOB->BSRR = USER_LED_PIN;
        }
        else
        {
            GPIOB->BSRR = (1U << 19);
        }
    }
    return 0;
}

