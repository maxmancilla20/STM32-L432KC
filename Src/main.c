/*
 * main.c
 * 
 * This file contains the main function and UART2 initialization function for the STM32L4 microcontroller.
 * 
 * Author: Maximiliano Mancilla
 * 
 * ============================= DATA =======================================
 * The STM32L432xx devices have two embedded universal synchronous receiver transmitters (USART1 and USART2).
 * These interfaces provide asynchronous communication, IrDA SIR ENDEC support, multiprocessor communication mode, 
 * single-wire half-duplex communication mode and have LIN Master/Slave capability. They provide hardware management
 * of the CTS and RTS signals, and RS485 Driver Enable. They are able to communicate at speeds of up to 10Mbit/s.
 * ==========================================================================
 */

/*
Used registers
GPIOB->ODR |= USER_LED_PIN; // Turn on the LED
GPIOB->ODR &= ~USER_LED_PIN; // Turn off the LED
GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
*/
#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx.h"
#include "Uart.h"

#define GPIOB_3 (1U<<3)
#define USER_LED_PIN (GPIOB_3)

#define GPIOBEN (1U<<1)

char key = 0;
/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */
int main(void)
{
    RCC->AHB2ENR |= GPIOBEN; // Enable GPIOB clock access

    /* Set PB3 as OUTPUT */
    GPIOB->MODER |= (1U<<6); // Set PB3 as OUTPUT
    GPIOB->MODER &= ~(1U<<7); // Set PB3 as OUTPUT

    // Initialization code here
    Uart2_RXTX_Init();
    
    while(1)
    {
        key = read_uart2(); // Read the key pressed

        if(key == '1')
        {
            GPIOB->ODR |= USER_LED_PIN; // Turn on the LED
            printf("LED Turned ON!\n\r");
        }
        else if(key == '0')
        {
            GPIOB->ODR &= ~USER_LED_PIN; // Turn off the LED
            printf("LED Turned OFF!\n\r");
        }
        else if(key == 't')
        {
            GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
            printf("LED Toggled!\n\r");
        }
    }
    return 0;
}


