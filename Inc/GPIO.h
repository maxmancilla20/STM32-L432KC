/*
 * GPIO.h
 *
 *  Created on: Apr 21, 2025
 *      Author: manci
 */

#ifndef GPIO_H_
#define GPIO_H_

void GPIO_Off(void);
void GPIO_On(void);
void GPIO_Toggle(void);
void GPIO_Init(void);
uint8_t GPIO_Read(void);

#endif /* GPIO_H_ */
