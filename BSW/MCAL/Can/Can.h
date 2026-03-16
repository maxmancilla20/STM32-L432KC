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
#define CAN_ID_STD      0x00 /* Standard Identifier */
#define CAN_ID_EXT      0x01 /* Extended Identifier */

#define CAN_RX_FIFO0	0x00 /* Rx element is assigned to Rx FIFO 0 */
#define CAN_RX_FIFO1	0x01 /* Rx element is assigned to Rx FIFO 1 */

#define CAN_MODE_LOOPBACK  0x00
#define CAN_MODE_NORMAL  0x01

typedef struct
{
    uint32_t std_id; /* Standard Identifier (11 bits) */
    uint32_t ext_id; /* Extended Identifier (29 bits) */
    uint32_t ide;     /* Identifier Extension (0 for standard, 1 for extended*/
    uint32_t rtr;     /* Remote Transmission Request (0 for data frame, 1 for remote frame) */
    uint32_t dlc;     /* Data Length Code (0-8) */
    uint8_t trasmit_global_time; /* Transmit Global Time (0 for disabled, 1 for enabled) */
} can_tx_header_typedef;

typedef struct
{
    uint32_t std_id; /* Standard Identifier (11 bits) */
    uint32_t ext_id; /* Extended Identifier (29 bits) */
    uint32_t ide;     /* Identifier Extension (0 for standard, 1 for extended*/
    uint32_t rtr;     /* Remote Transmission Request (0 for data frame, 1 for remote frame) */
    uint32_t dlc;     /* Data Length Code (0-8) */
    uint32_t timestamp; /* Timestamp of message reception */
    uint32_t filter_match_index; /* Index of the filter that matched the received message */
} can_rx_header_typedef;


 /******************************************************************
 **                      FUNCTIONS PROTOTYPES                    **
 ******************************************************************/
/**
 * @brief Initializes the CAN peripheral.
 */
void can_gpio_init(void);
void can_params_init(uint8_t mode);
void can_start(void);
uint8_t can_add_tx_message(can_tx_header_typedef *pHeader, uint8_t aData[], uint32_t *pTxMailbox);
uint8_t can_get_rx_message(uint32_t RxFifo, can_rx_header_typedef *pHeader, uint8_t aData[]);
void can_filter_config(uint16_t std_id);

#endif /* CAN_H_ */