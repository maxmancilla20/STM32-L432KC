/*
 * GPIO.h
 *
 *  Created on: Jun 28, 2025
 *      Author: manci
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

#define USER_LED_PIN (GPIOB_3)
#define PIN7                (1U<<7)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)
#define GPIOB_3 (1U<<3)

void GPIO_Init(void);
void GPIO_ToggleLED(void);
uint8_t GPIO_ReadButton(void);
void GPIO_TurnOnLED(void);
void GPIO_TurnOffLED(void);

#endif /* GPIO_H_ */
