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
#include "adc.h"
#include "Uart.h"

#define PIN3                (1U<<3) 
#define USER_LED_PIN        (PIN3)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)


uint32_t adc_value = 0;

int main(void)
{

    /* Enable clock access to GPIOB */
    RCC->AHB2ENR |= GPIOBEN;

    /* Set PB3 AS OUTPUT */
    GPIOB->MODER |= (1U<<6);
    GPIOB->MODER &=~ (1U<<7);

    Uart2_RXTX_Init();
    pa0_adc_init();
    start_conversion();


    printf("ADC Value measuremtn start\n\r");
    while(1)
    {
        /* Read ADC value */
        adc_value = adc_read();
        printf("ADC Value: %lu\n\r", adc_value);
    }

    return 0;
}

/*
        if(FlagStatus == 0)
        {
  
            GPIOB->ODR |= USER_LED_PIN;
            FlagStatus = 1;
        }
        else
        {
  
            GPIOB->ODR &= ~USER_LED_PIN;
            FlagStatus = 0;
        }
*/
