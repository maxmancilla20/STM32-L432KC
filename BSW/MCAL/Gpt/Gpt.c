/****************************************************************************************************
 * @file           : Gpt.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The GPT Driver abstracts the access to the microcontroller's general purpose timers.
 *                   It provides initialization, configuration, and timer control functions for the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#include "stm32l4xx.h"
#include "Gpt.h"
#include "Gpt_Cfg.h"

// Implement GPT driver functions here
#define SYSTICK_LOAD_VALUE      (SystemCoreClock / 1000) // Load value for 1 ms delay
#define CTRL_ENABLE             (1U << 0) // SysTick enable bit
#define CTRL_CLKSRC             (1U << 2) // SysTick clock source select bit
#define CTRL_COUNTFLAG          (1U << 16) // SysTick count flag bit

uint32_t SystemCoreClock = 4000000U;

void Gpt_Init(void)
{
 // TODO: Implement GPT Init
}

void Gpt_StartTimer(uint32_t timerId, uint32_t value)
{
    // TODO: Implement timer start
}

void Gpt_StopTimer(uint32_t timerId)
{
    // TODO: Implement timer stop
}

void SystickDelay_Ms(uint32_t delay)
{   
    /* Reload with number of clock per millisecond */
    SysTick->LOAD = SYSTICK_LOAD_VALUE; // Load the SysTick counter value

    /* Clear SysTick Current Value Register */
    SysTick->VAL = 0; // Clear the current value

    /* Enable SysTick and select internal clk_src */
    SysTick->CTRL = CTRL_CLKSRC | CTRL_ENABLE; // Enable SysTick with processor clock

    for(uint32_t i = 0; i < delay; i++)
    {
        /* Wait until COUNTFLAG is set */
        while((SysTick->CTRL & CTRL_COUNTFLAG) == 0) {} // Wait until the count flag is set
    }

    SysTick->CTRL = 0; // Disable SysTick after delay
}
