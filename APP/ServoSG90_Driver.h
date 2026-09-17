/****************************************************************************************************
 * @file           : ServoSG90_Driver.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 16, 2026
 * @brief          : Public API for the SG90 servo driver.
 ****************************************************************************************************/

#ifndef SERVOSG90_DRIVER_H_
#define SERVOSG90_DRIVER_H_

#include <stdint.h>


/**
 * @brief Initializes the GPIO and PWM peripherals required by the servo.
 */
void ServoSG90_Init(void);

/**
 * @brief Sets the servo position by angle.
 * @param angleDeg Angle in degrees, clamped internally to [0, 180].
 */
void ServoSG90_SetAngle(uint8_t angleDeg);

/**
 * @brief Sets the servo pulse width directly, in tenths of a millisecond.
 * @param pulseTenthsMs Pulse width in tenths of ms, clamped internally to [10, 20] (1.0-2.0 ms).
 */
void ServoSG90_SetPulseTenthsMs(uint16_t pulseTenthsMs);


#endif /* SERVOSG90_DRIVER_H_ */
