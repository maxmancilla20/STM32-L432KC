#include "stm32l4xx.h"
#include <stdint.h>
#include "Std_Types.h"
#include "Gpt.h"
#include "HCSR04.h"
#include "Global_Timers.h"

/* Shared with the TIM2 input capture driver (TIM2_IRQHandler) */
extern volatile uint32_t echo_pulse_width;
extern volatile uint8_t  echo_ready;

/* Local Function Definitions */
static void HCSR04_ConfigTriggerPB1(void);
static void HCSR04_TriggerPulse(void);

void HCSR04_Init(void)
{   
    tim2_input_capture_init(); // Configure PA1/TIM2_CH2 as echo input capture
    HCSR04_ConfigTriggerPB1();  // Configure PB1 as trigger output
}

void HCSR04_ConfigTriggerPB1(void)
{
    /* Enable clock access to GPIOB (missing before -> writes had no effect) */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    /* Set PB1 as output mode: MODER bits [2n+1:2n], n=1 -> bits [3:2] */
    GPIOB->MODER &= ~(3U << (1 * 2)); // Clear bits 3:2
    GPIOB->MODER |=  (1U << (1 * 2)); // Set '01' -> general purpose output
}

void HCSR04_TriggerPulse(void)
{
    /* Clear any stale result from a previous reading before triggering */
    echo_ready = 0;

    // Generate a 10us pulse on PB1
    GPIOB->ODR |= (1U << 1);  // Set PB1 high
    Gpt_StartTimer(0, 10U);   // Start a timer for 10us
    while (!Gt_CheckTimer(0)) {} // Wait for the timer to expire
    GPIOB->ODR &= ~(1U << 1); // Set PB1 low
}

void HCSR04_ReadDistance(uint32_t *distance)
{
    // Trigger the ultrasonic sensor
    HCSR04_TriggerPulse();

    /* Wait for TIM2_IRQHandler to finish capturing rising+falling edges.
       NOTE: no timeout here -> if the echo never arrives (sensor
       disconnected, out of range) this will hang forever. Consider adding
       a max iteration count or a watchdog-friendly timeout in production code. */
    while (!echo_ready) {}

    /* echo_pulse_width is already in microseconds (TIM2 runs at a 1 MHz /
       1 us tick), so no SystemCoreClock scaling is needed here anymore.
       distance_mm = pulse_width_us * speed_of_sound_mm_per_us / 2
                   = pulse_width_us * 0.343 / 2 */
    *distance = (echo_pulse_width * 343U) / 2000U; // result in millimeters
}
