/****************************************************************************************************
 * @file           : Can.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The CAN Driver handles initialization, transmission, and reception of CAN messages.
 *                   This module abstracts the CAN peripheral for the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#include "stm32l4xx.h"
#include "Can.h"
#include "Can_Cfg.h"

void CAN_Init(void)
{
    // TODO: Implement CAN peripheral initialization
}

void CAN_Transmit(uint32_t id, uint8_t* data, uint8_t len)
{
    // TODO: Implement CAN message transmission
}

void CAN_Receive(uint32_t* id, uint8_t* data, uint8_t* len)
{
    // TODO: Implement CAN message reception
}