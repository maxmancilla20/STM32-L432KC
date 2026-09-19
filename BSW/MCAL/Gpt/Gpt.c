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

/* =========== Function Declatarions ==========*/
void tim2_1us_Init(void);
void tim2_input_capture_init(void);
void tim1_init(void);
void SystickDelay_Ms(uint32_t delay);
void Gpt_Systick_1ms_Init(void);
void Gpt_Init(void);


// Implement GPT driver functions here
#define SYSTICK_LOAD_VALUE      (SystemCoreClock / 1000) // Load value for 1 ms delay
#define CTRL_ENABLE             (1U << 0) // SysTick enable bit
#define CTRL_CLKSRC             (1U << 2) // SysTick clock source select bit
#define CTRL_COUNTFLAG          (1U << 16) // SysTick count flag bit

#define TIM2EN                  (1U << 0) // Timer 2 enable bit 0
//#define TIM_CR1_CEN             (1U << 0) // Timer enable bit in CR1 register
uint32_t SystemCoreClock = 4000000U;
extern volatile uint32_t g_gt_tick_ms;
/* Shared with HCSR04.c: result of the last echo pulse measurement */
volatile uint32_t tim2_overflow_count   = 0;
volatile uint32_t echo_rising_ts        = 0;
volatile uint32_t echo_rising_overflow  = 0;
volatile uint32_t echo_pulse_width      = 0;
volatile uint8_t  echo_ready            = 0;

/* Timer as an output compare */
/* Controlling PB3 Cfg  TIM2 CH2*/
#define OC_TOGGLE               (1U << 12) | (1u << 13) // Output compare toggle mode bit 
#define CCER_CC2E               (1U << 4) // Update interrupt flag bit in SR register
#define CCER_CC1E               (1U << 0) // Update interrupt flag bit in SR register
#define CR1_CEN                (1U << 0) // Timer enable bit in CR1 register



/* ================ TIM 2 FUNCTIONS ======================*/

void tim2_1us_Init(void)
{
    /* TIM2 is used as a 1 us time base for timing measurements. */
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = (uint16_t)((SystemCoreClock / 1000000U) - 1U);
    TIM2->ARR = 0xFFFFU;
    TIM2->CNT = 0U;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

/* PA1 -> TIM2_CH2 : shared 1 us time base for DHT11 + echo input capture for HCSR04.
   Both drivers call this same function from their _Init(); it's idempotent
   (safe to call twice) and neither driver should touch PSC/ARR/CR1 again
   after this — they only read TIM2->CNT for their own delay_us()-style waits. */
void tim2_input_capture_init(void)
{

    /* Set PA1 mode to alternate function */
    GPIOA->MODER &= ~(3U << (1 * 2));   // Clear bits for PA1 (bits 3:2)
    GPIOA->MODER |=  (2U << (1 * 2));   // Set '10' -> alternate function mode

    /* Set PA1 alternate function to TIM2_CH2 (AF1) */
    GPIOA->AFR[0] &= ~(0xF << (1 * 4)); // Clear 4 bits for PA1 (bits 7:4)
    GPIOA->AFR[0] |=  (0x1 << (1 * 4)); // Set AF1 -> TIM2_CH2

    /* Map IC2 to TI2 (the physical PA1 pin) */
    TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
    TIM2->CCMR1 |=  TIM_CCMR1_CC2S_0;   // CC2S = 01 -> IC2 mapped to TI2

    /* No input filter, no input prescaler: capture every valid edge */
    TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
    TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;

    /* Start by capturing the rising edge (echo pulse start) */
    TIM2->CCER &= ~TIM_CCER_CC2P;        // CC2P = 0 -> rising edge
    TIM2->CCER |=  TIM_CCER_CC2E;        // Enable capture on channel 2

    /* Clear any stale update flag left over from tim2_1us_Init()'s
       forced EGR|=UG, so we don't count a phantom overflow before
       the first real measurement. */
    TIM2->SR &= ~TIM_SR_UIF;

    /* Enable capture/compare interrupt for channel 2 */
    TIM2->DIER |= TIM_DIER_CC2IE;

    /* Enable TIM2 interrupt in the NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);

}

/* ================ TIM 1 FUNCTIONS ======================*/

void tim1_init(void)
{
    /* Configuration for PWM */
    
    /* Enable TIM1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    /* 4 MHz / (PSC+1) = 1 MHz timer clock.
       With ARR = 19999, period = 20 ms exactly.
       1 tick = 1 us, so CCR1 directly equals pulse width in microseconds. */
    TIM1->PSC = 3U;
    TIM1->ARR = 19999U;

    /* CH1 in PWM mode 1 with preload enabled */
    TIM1->CCMR1 = 0U;
    TIM1->CCMR1 |= (0x6U << 4) | (0x1U << 3);

    /* Enable CH1 output */
    TIM1->CCER = 0U;
    TIM1->CCER |= TIM_CCER_CC1E;

    /* Enable main output (required on advanced timers like TIM1) */
    TIM1->BDTR = 0U;
    TIM1->BDTR |= TIM_BDTR_MOE;

    /* Center position = 1.5 ms pulse */
    TIM1->CCR1 = 1500U;

    /* Force update to load PSC/ARR/CCR1 into the shadow registers */
    TIM1->EGR |= TIM_EGR_UG;

    /* Start timer */
    TIM1->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}

/* ================ SysTick FUNCTIONS ======================*/
void SystickDelay_Ms(uint32_t delay)
{
    volatile uint32_t start_tick;

    if (delay == 0U)
    {
        return;
    }

    start_tick = g_gt_tick_ms;

    while ((g_gt_tick_ms - start_tick) < delay)
    {
        __ASM volatile ("nop");
    }
}

void Gpt_Systick_1ms_Init(void)
{
    /* SysTick clock source = processor clock (4 MHz) */
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

/* ================ GPT FUNCTIONS ======================*/
void Gpt_Init(void)
{
    Gpt_Systick_1ms_Init();
}

void Gpt_StartTimer(uint32_t timerId, uint32_t value)
{
    (void)timerId;
    (void)value;
}

void Gpt_StopTimer(uint32_t timerId)
{
    (void)timerId;
    /* Keep the global system tick alive. It is the time base used by the app timers. */
}



/* ========== Interrupt Timer Functions ====================*/
void SysTick_Handler(void)
{
    extern volatile uint32_t g_gt_tick_ms;
    g_gt_tick_ms++;
}

void TIM2_IRQHandler(void)
{
    /* Handle timer overflow first: every time CNT wraps from 0xFFFF
       back to 0, this fires. We count these so a pulse that spans
       multiple wraps (e.g. no-echo condition) still measures correctly. */
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        tim2_overflow_count++;
    }

    if (TIM2->SR & TIM_SR_CC2IF)
    {
        TIM2->SR &= ~TIM_SR_CC2IF;

        if ((TIM2->CCER & TIM_CCER_CC2P) == 0)
        {
            /* Rising edge: remember both the CCR2 timestamp AND how many
               overflows had happened up to this point. */
            echo_rising_ts       = TIM2->CCR2;
            echo_rising_overflow = tim2_overflow_count;
            TIM2->CCER |= TIM_CCER_CC2P; // now capture falling edge
        }
        else
        {
            /* Falling edge: reconstruct the true elapsed time by adding
               back one full timer period (ARR+1) for every overflow that
               happened between the rising and falling edge. */
            uint32_t falling_ts = TIM2->CCR2;
            uint32_t overflows_during_pulse = tim2_overflow_count - echo_rising_overflow;

            echo_pulse_width = (overflows_during_pulse * (TIM2->ARR + 1U))
                                + falling_ts - echo_rising_ts;
            echo_ready = 1;

            TIM2->CCER &= ~TIM_CCER_CC2P; // back to rising edge for next pulse
        }
    }
}


/* ============= LEGACY FUNCTIONS NOT USED ===================*/
/* Not used */
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

/* Not used*/
void tim2_1hz_delay(void)
{
    /* Wait until the update interrupt flag is set */
    while(!(TIM2->SR & SR_UIF)) {} // Wait for the update interrupt flag

    /* Clear the update interrupt flag */
    TIM2->SR &= ~SR_UIF; // Clear the update interrupt flag
}

/* Not used */
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
