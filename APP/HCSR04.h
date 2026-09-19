/****************************************************************************************************
 * @file           : HCSR04.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The HCSR04 Driver provides initialization and control functions for the HC-SR04 ultrasonic sensor
 *                   in the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#ifndef HCSR04_H_
#define HCSR04_H_

void HCSR04_ReadDistance(uint32_t *distance);
void HCSR04_Init(void);

#endif /* HCSR04_H_ */
