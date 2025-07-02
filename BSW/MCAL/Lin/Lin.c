/****************************************************************************************************
 * @file           : Lin.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The LIN Driver handles initialization, transmission, and reception of LIN messages.
 *                   This module abstracts the LIN peripheral for the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#include "stm32l4xx.h"
#include "Lin.h"
#include "Lin_Cfg.h"

void LIN_Init(void)
{
    // TODO: Implement LIN peripheral initialization
}

void LIN_Transmit(uint8_t id, uint8_t* data, uint8_t len)
{
    // TODO: Implement LIN message transmission
}

void LIN_Receive(uint8_t* id, uint8_t* data, uint8_t* len)
{
    // TODO: Implement LIN message reception
}