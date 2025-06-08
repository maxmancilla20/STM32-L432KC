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

#define GPIOB_3 (1U<<3)
#define USER_LED_PIN (GPIOB_3)

#define GPIOBEN (1U<<1)

#define VECT_TAB_BASE_ADDRESS           FLASH_BASE
#define VECT_TAB_OFFSET                 0x4000

char key = 0;
/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */

 struct Boot_Common_Apis
{
    void (*GPIO_Init)(void);
    void (*GPIO_Toggle)(void);
    void (*GPIO_On)(void);
    void (*GPIO_Off)(void);
    void (*Uart2_RXTX_Init)(void);
    char (*read_uart2)(void);
    uint8_t (*GPIO_Read)(void);
    void (*write_uart2)(uint8_t data);
};

int __io_putchar(int ch)
{
	struct Boot_Common_Apis *common_apis = (struct Boot_Common_Apis *) 0x0800C000;
    common_apis->write_uart2(ch);
    return ch;
}

int main(void)
{
    /* Shared memory */
    struct Boot_Common_Apis *common_apis = (struct Boot_Common_Apis *) 0x0800C000;

    common_apis->Uart2_RXTX_Init();
    common_apis->GPIO_Init();

    printf("\n\rSTART APP!\n\r");
    while(1)
    {

        for(int i = 0; i < 10000; i++); // Delay 1 second

        if(common_apis->GPIO_Read())
        {
        	common_apis->GPIO_On();
            printf("\n\rButton Pressed!\n\r");
        }
        else
        {
        	common_apis->GPIO_Off();
        }

    }
    return 0;
}

void SystemInit(void)
{
    SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
}

