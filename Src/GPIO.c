#include "stm32l4xx.h"

#define PIN3                (1U<<3) 
#define USER_LED_PIN        (PIN3)
#define PIN7                (1U<<7)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)

void GPIO_Init(void)
{
    /* Enable clock access to GPIOB */
    RCC->AHB2ENR |= GPIOBEN;

    /* Set PB3 AS OUTPUT MODE B7 = 0, B6 =  1*/
    GPIOB->MODER |= (1U<<6);
    GPIOB->MODER &=~ (1U<<7);

    /* Set PB7 as input B14 = 0, B15 = 0 */
    GPIOB->MODER &=~ (1U<<14);
    GPIOB->MODER &=~ (1U<<15); 
}

void GPIO_Toggle(void)
{
    GPIOB->ODR ^= USER_LED_PIN;  /* Toggle the LED */
}

void GPIO_On(void)
{
    GPIOB->ODR |= USER_LED_PIN;  /* Turn on the LED */
}

void GPIO_Off(void)
{
    GPIOB->ODR &= ~USER_LED_PIN; /* Turn off the LED */
}