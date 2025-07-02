/**********************************************************************************************************
 * @file           : Lin.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : Driver for handling the LIN peripheral. Provides initialization, transmission,
 *                   and reception functions for LIN messages in the AUTOSAR MCAL layer.
 **********************************************************************************************************/

#ifndef LIN_H_
#define LIN_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Std_Types.h"

/******************************************************
 **                DET Error Codes                   **
 ******************************************************/

 /*******************************************************
 **                 Type definitions                  **
 *******************************************************/

 /******************************************************************
 **                      FUNCTIONS PROTOTYPES                    **
 ******************************************************************/
/**
 * @brief Initializes the LIN peripheral.
 */
void LIN_Init(void);

/**
 * @brief Transmits a LIN message.
 * @param id   Message identifier.
 * @param data Pointer to the data to transmit.
 * @param len  Length of the data.
 */
void LIN_Transmit(uint8_t id, uint8_t* data, uint8_t len);

/**
 * @brief Receives a LIN message.
 * @param id   Pointer to store the received identifier.
 * @param data Pointer to store the received data.
 * @param len  Pointer to store the length of the received data.
 */
void LIN_Receive(uint8_t* id, uint8_t* data, uint8_t* len);

#endif /* LIN_H_ */