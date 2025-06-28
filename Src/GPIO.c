/*
 * GPIO.c
 *
 *  Created on: Jun 28, 2025
 *      Author: manci
 */

#include "stm32l4xx.h"
#include "GPIO.h"

void GPIO_Init(void)
{
    // Enable clock access to GPIOB
    RCC->AHB2ENR |= GPIOBEN;

    // Set PB3 as output mode (B7 = 0, B6 = 1)
    GPIOB->MODER |= (1U << 6);
    GPIOB->MODER &= ~(1U << 7);

    // Set PB7 as input (B14 = 0, B15 = 0)
    GPIOB->MODER &= ~(1U << 14);
    GPIOB->MODER &= ~(1U << 15);
}

void GPIO_ToggleLED(void)
{
    // Toggle the LED connected to PB3
    GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
}

uint8_t GPIO_ReadButton(void)
{
    // Read the state of the button connected to PB7
    return (GPIOB->IDR & GPIOB_3) ? 1 : 0; // Return 1 if pressed, 0 if not pressed
}

void GPIO_TurnOnLED(void)
{
    // Turn on the LED connected to PB3
    GPIOB->ODR |= USER_LED_PIN; // Set PB3 high
}

void GPIO_TurnOffLED(void)
{
    // Turn off the LED connected to PB3
    GPIOB->ODR &= ~USER_LED_PIN; // Set PB3 low
}
