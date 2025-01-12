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

#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx.h"
#include "Uart.h"


/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */
int main(void)
{
    // Initialization code here
    Uart2_RXTX_Init();
    while(1)
    {
        // Main loop
    	printf("Hello World!\n\r");
    }
    return 0;
}


