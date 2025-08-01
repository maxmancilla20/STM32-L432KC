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
#include "Std_Types.h"
#include <stdio.h>
#include "Mcu.h"
#include "Mcu_Cfg.h"
#include "Uart.h"
#include "Uart_Cfg.h"
#include "Dio.h"
#include "Dio_Cfg.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Gpt.h"
#include "Gpt_Cfg.h"

extern Mcu_ConfigType McuDriverConfiguration;
/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */

 uint32_t adc_value = 0;

int main(void)
{
    // Initialization code here
    Mcu_Init(&McuDriverConfiguration);
    Uart2_RXTX_Init(); // Initialize UART2 for transmission and reception
    Dio_Init(); // Initialize GPIO
    pa0_adc_init();
    start_conversion();
    //tim2_1hz_init(); // Initialize Timer 2 for 1Hz operation

    tim2_output_pb3_compare_1hz(); // Initialize Timer 2 for output compare on PB3
    while(1)
    {
        //GPIOB->ODR |= USER_LED_PIN;  /* Turn on the LED */
        //GPIOB->ODR &= ~USER_LED_PIN; /* Turn off the LED */
        //GPIOB->ODR ^= USER_LED_PIN;  /* Toggle the LED */
        //for(int i = 0; i < 100000; i++){} /* Delay */
        //key = read_uart2(); // Read the key pressed
        
        /* Read ADC value */
        //adc_value = adc_read();
        //printf("ADC Value: %lu\n\r", adc_value);
        //SystickDelay_Ms(1000);

        //tim2_1hz_delay(); // Wait for 1 second (1Hz)
        //GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
        
        //printf("LED toggled\n\r");
    }
    return 0;
}


