/*
 * ============================= DATA ======================================
 * User LD3: the green LED is a user LED connected to Arduino Nano signal D13
 * corresponding to the STM32 I/O PB3 (pin 26).
 * | AHB2 | 0x4800 0400 - 0x4800 07FF | 1 KB | GPIOB
 * PIN D4(5) PB7 AS INPUT ABH2 ALSO

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
#include "GPIO.h"
/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */
int main(void)
{
  
    // Initialization code here
    Uart2_RXTX_Init(); // Initialize UART2 for transmission and reception
    GPIO_Init(); // Initialize GPIO
  
    while(1)
    {
        //GPIOB->ODR |= USER_LED_PIN;  /* Turn on the LED */
        //GPIOB->ODR &= ~USER_LED_PIN; /* Turn off the LED */
        //GPIOB->ODR ^= USER_LED_PIN;  /* Toggle the LED */
        //for(int i = 0; i < 100000; i++){} /* Delay */
        //key = read_uart2(); // Read the key pressed
  
        for(int i = 0; i < 100000; i++){} /* Delay */
        GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
   
    }
    return 0;
}


