/**********************************************************************************************************
 * @file           : Dio.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The DIO Driver abstracts the access to the microcontroller's hardware pins.
 *                   Furthermore, it allows the grouping of those pins. 
 **********************************************************************************************************/

#ifndef DIO_H_
#define DIO_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Std_Types.h"


/******************************************************
 **                DET Error Codes                   **
 ******************************************************/
#define USER_LED_PIN (GPIOB_3)
#define PIN7                (1U<<7)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)
#define GPIOB_3 (1U<<3)

/*******************************************************
 **                 Type definitions                  **
 *******************************************************/


/******************************************************************
 **                      FUNCTIONS PROTOTYPES                    **
 ******************************************************************/
void Dio_Init(void);
void Dio_ToggleLED(void);
uint8_t Dio_ReadButton(void);
void Dio_TurnOnLED(void);
void Dio_TurnOffLED(void);

#endif /* DIO_H_ */
