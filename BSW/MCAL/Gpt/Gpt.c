/****************************************************************************************************
 * @file           : Gpt.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The GPT Driver abstracts the access to the microcontroller's general purpose timers.
 *                   It provides initialization, configuration, and timer control functions for the AUTOSAR MCAL layer.
 ***************************************************************************************************/
/* Default Clock for APB1 16MHz, reduce to 1Hz*/
#include "stm32l4xx.h"
#include "Gpt.h"
#include "Gpt_Cfg.h"

// Implement GPT driver functions here
#define SYSTICK_LOAD_VALUE      (SystemCoreClock / 1000) // Load value for 1 ms delay
#define CTRL_ENABLE             (1U << 0) // SysTick enable bit
#define CTRL_CLKSRC             (1U << 2) // SysTick clock source select bit
#define CTRL_COUNTFLAG          (1U << 16) // SysTick count flag bit

#define TIM2EN                  (1U << 0) // Timer 2 enable bit 0
//#define TIM_CR1_CEN             (1U << 0) // Timer enable bit in CR1 register
uint32_t SystemCoreClock = 4000000U;

/* Timer as an output compare */
/* Controlling PB3 Cfg  TIM2 CH2*/
#define OC_TOGGLE               (1U << 12) | (1u << 13) // Output compare toggle mode bit 
#define CCER_CC2E               (1U << 4) // Update interrupt flag bit in SR register
#define CCER_CC1E               (1U << 0) // Update interrupt flag bit in SR register
#define CR1_CEN                (1U << 0) // Timer enable bit in CR1 register

/* PA8 TIM_CH1*/
void tim2_input_capture(void)
{
    /* Enable Clock Access to GPIOA 8 */
    //RCC->AHB2ENR |= GPIOAEN; // Enable clock for GPIOA

    /* Set PA8 mode to alternate function */
    GPIOA->MODER &= ~(3U << 16); // Clear bits for PA8
    GPIOA->MODER |=  (2U << 16); // Set '10' in MODER[17:16] for alternate function

    /* Set PA8 alternate function type to TIM1_CH1 (AF1) */
    GPIOA->AFR[1] &= ~(0xF << 0); // Clear 4 bits for PA8
    GPIOA->AFR[1] |=  (0x1 << 0); // Set AF1 for PA8

    /* Enable Clock Access to TIM1 */
    RCC->APB2ENR |= (1U << 11); // Enable clock for TIM1
    
    /* Set Prescaler Value */
    // 4 000 000 / 1 000 = 4 000 Hz, so for 1 Hz we need a prescaler of 1600
    TIM1->PSC = (SystemCoreClock / 1000) - 1; // Set prescaler for 1 Hz tick
    
    TIM1->CCMR1 &= ~(0xFF); // Clear CCMR1 for CH1
    TIM1->CCMR1 |= CCER_CC1E; // Set CC1

    /* Set CH1 to capture at every edge */
    TIM1->CCMR1 &= ~(0x3 << 4); // Clear bits for CH1
    TIM1->CCMR1 |= (0x3 << 4); // Set '11' in CCMR1[5:4] for capture on both edges
    TIM1->CCER |= CCER_CC1E; // Enable output for channel 1

    TIM1->CR1 = CR1_CEN; // Enable Timer 1

    /* Set Auto-reaload Value */
    //TIM1->ARR = 1000 - 1; // 4 000 / 4 000 = 1

    /* Clear Counter */
    //TIM1->CNT = 0; // Reset the counter to 0

    /* Set CH1 to capture at every edge */
    

    /* Enable CH1 to capture at rising edge */  
    /* Enable TIM1 */
    TIM1->CR1 |= TIM_CR1_CEN; // Enable clock for Timer 1

}


void tim2_output_pb3_compare_1hz(void)
{
    /* Enable Clock Access to GPIOB */
    //RCC->AHB2ENR |= GPIOBEN; // Enable clock for GPIOB this is set in Mcu.c

    /* Set PB3 mode to alternate function */
    GPIOB->MODER &= ~(3U << 6); // clear both bits
    GPIOB->MODER |=  (2U << 6); // Set'10' in MODER[7:6]

    /* Set PB3 alternate function type to TIM2_CH2 (AF1) */
    GPIOB->AFR[0] &= ~(0xF << 12); // Clear 4 bits for PB3
    GPIOB->AFR[0] |=  (0x1 << 12); // Set AF1 for PB3

    /* Enable Clock Access to Timer 2 */
    RCC->APB1ENR1 |= TIM2EN; // Enable clock for Timer 2

    /* Set Prescaler Value */
    // 4 000 000 / 1 000 = 4 000 Hz, so for 1 Hz we need a prescaler of 1600
    TIM2->PSC = (SystemCoreClock / 1000) - 1; // Set prescaler for 1 Hz tick
    
    /* Set Auto-reaload Value */
    TIM2->ARR = 1000 - 1; // 4 000 / 4 000 = 1

    /* Enable Tim2 CH2 in compare mode */
    TIM2->CCMR1 &= ~(0xFF); // Limpia CCMR2 para CH2
    TIM2->CCMR1 |= OC_TOGGLE; // Set output compare mode for channel 2
    TIM2->CCER |= CCER_CC2E; // Enable output for channel 2

    /* Clear Counter */
    TIM2->CNT = 0; // Reset the counter to 0

    /* Enable Timer */
    TIM2->CR1 |= TIM_CR1_CEN; // Enable Timer 2
}

void Gpt_Init(void)
{
 // TODO: Implement GPT Init
}

void tim2_1hz_init(void)
{
    /* Enable Clock Access to Timer 2 */
    RCC->APB1ENR1 |= TIM2EN; // Enable clock for Timer 2

    /* Set Prescaler Value */
    // 4 000 000 / 1 000 = 4 000 Hz, so for 1 Hz we need a prescaler of 1600
    TIM2->PSC = (SystemCoreClock / 1000) - 1; // Set prescaler for 1 Hz tick
    
    /* Set Auto-reaload Value */
    TIM2->ARR = 1000 - 1; // 4 000 / 4 000 = 1

    /* Clear Counter */
    TIM2->CNT = 0; // Reset the counter to 0

    /* Enable Timer */
    TIM2->CR1 |= TIM_CR1_CEN; // Enable Timer 2
}

void tim2_1hz_delay(void)
{
    /* Wait until the update interrupt flag is set */
    while(!(TIM2->SR & SR_UIF)) {} // Wait for the update interrupt flag

    /* Clear the update interrupt flag */
    TIM2->SR &= ~SR_UIF; // Clear the update interrupt flag
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
