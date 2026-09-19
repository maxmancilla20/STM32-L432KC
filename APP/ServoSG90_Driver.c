/****************************************************************************************************
 * @file           : ServoSG90_Driver.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 16, 2026
 * @brief          : SG90 servo-specific logic. PWM initialization is handled in Pwm_Driver.c.
 ****************************************************************************************************/

#include "stm32l4xx.h"
#include "ServoSG90_Driver.h"
#include "Pwm_Driver.h"
#include "Gpt.h"
#include "Gpt_Cfg.h"


#define SERVO_PULSE_MIN_US    1000U
#define SERVO_PULSE_MAX_US    2000U
#define SERVO_ANGLE_MIN_DEG   0U
#define SERVO_ANGLE_MAX_DEG   180U
#define SERVO_ANGLE_CENTER    90U

static uint8_t ServoSG90_ClampAngle(uint8_t angleDeg)
{
    if (angleDeg > SERVO_ANGLE_MAX_DEG)
    {
        return SERVO_ANGLE_MAX_DEG;
    }

    if (angleDeg < SERVO_ANGLE_MIN_DEG)
    {
        return SERVO_ANGLE_MIN_DEG;
    }

    return angleDeg;
}

static uint16_t ServoSG90_AngleToPulseUs(uint8_t angleDeg)
{
    uint16_t pulseUs;
    uint32_t rangeUs;
    uint32_t mapped;

    angleDeg = ServoSG90_ClampAngle(angleDeg);
    rangeUs = SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US;
    mapped = ((uint32_t)angleDeg * rangeUs) / (SERVO_ANGLE_MAX_DEG - SERVO_ANGLE_MIN_DEG);
    pulseUs = (uint16_t)(SERVO_PULSE_MIN_US + mapped);

    return pulseUs;
}

void ServoSG90_Init(void)
{
    Pwm_Gpio_Init();
}

void ServoSG90_SetAngle(uint8_t angleDeg)
{
    uint16_t pulseUs;

    pulseUs = ServoSG90_AngleToPulseUs(angleDeg);
    Pwm_SetPulseUs(pulseUs);
}

void ServoSG90_SetPulseTenthsMs(uint16_t pulseTenthsMs)
{
    uint16_t pulseUs;

    if (pulseTenthsMs < 10U)
    {
        pulseTenthsMs = 10U;
    }
    else if (pulseTenthsMs > 20U)
    {
        pulseTenthsMs = 20U;
    }

    pulseUs = (uint16_t)(pulseTenthsMs * 100U);
    Pwm_SetPulseUs(pulseUs);
}
