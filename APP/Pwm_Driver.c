/****************************************************************************************************
 * @file           : Pwm_Driver.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 16, 2026
 * @brief          : PWM driver implementation for PA8 using TIM1_CH1 in AF1.
 ****************************************************************************************************/

#include "stm32l4xx.h"
#include "Pwm_Driver.h"
#include "Gpt.h"

void Pwm_Gpio_Init(void)
{
    /* Enable GPIOA clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    /* PA8 en modo Alternate Function */
    GPIOA->MODER &= ~GPIO_MODER_MODE8;
    GPIOA->MODER |= GPIO_MODER_MODE8_1;   // 10: AF mode

    /* AF1 = TIM1_CH1 */
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8;
    GPIOA->AFR[1] |= (1U << GPIO_AFRH_AFSEL8_Pos);

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;          // push-pull
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8;     // high speed

    /* Configure TIM1 */
    tim1_init();
}


void Pwm_SetDutyCycle(uint8_t dutyCycle)
{
    uint32_t pulseUs;

    if (dutyCycle > 100U)
    {
        dutyCycle = 100U;
    }

    /* Mapea 0-100% al rango seguro de un servo (1000-2000 us),
       NO al periodo completo de 20 ms */
    pulseUs = 1000U + (((uint32_t)dutyCycle * 1000U) / 100U);

    TIM1->CCR1 = pulseUs;
}

void Pwm_SetPulseUs(uint16_t pulseUs)
{
    if (pulseUs < 1000U)
    {
        pulseUs = 1000U;
    }
    else if (pulseUs > 2000U)
    {
        pulseUs = 2000U;
    }

    /* With PSC=3, timer clock = 1 MHz and 1 tick = 1 us.
       For a 90° servo position, we send 1500 us. */
    TIM1->CCR1 = (uint32_t)pulseUs;
}
