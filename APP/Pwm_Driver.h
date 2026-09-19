/****************************************************************************************************
 * @file           : Pwm_Driver.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 16, 2026
 * @brief          : Public API for PA8 PWM using TIM1_CH1.
 ****************************************************************************************************/

#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_

#include <stdint.h>
#include "stm32l4xx.h"

void Pwm_Gpio_Init(void);
void Pwm_SetDutyCycle(uint8_t dutyCycle);
void Pwm_SetPulseUs(uint16_t pulseUs);

#endif /* PWM_DRIVER_H_ */
