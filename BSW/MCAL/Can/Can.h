/**********************************************************************************************************
 * @file           : Can.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : Driver for handling the CAN peripheral. Provides initialization, transmission,
 *                   and reception functions for CAN messages in the AUTOSAR MCAL layer.
 **********************************************************************************************************/

#ifndef CAN_H_
#define CAN_H_

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
 * @brief Initializes the CAN peripheral.
 */
void CAN_Init(void);

/**
 * @brief Transmits a CAN message.
 * @param id   Message identifier.
 * @param data Pointer to the data to transmit.
 * @param len  Length of the data.
 */
void CAN_Transmit(uint32_t id, uint8_t* data, uint8_t len);

/**
 * @brief Receives a CAN message.
 * @param id   Pointer to store the received identifier.
 * @param data Pointer to store the received data.
 * @param len  Pointer to store the length of the received data.
 */
void CAN_Receive(uint32_t* id, uint8_t* data, uint8_t* len);

#endif /* CAN_H_ */