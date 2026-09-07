/********************************************************************************
 * @file           : I2c_Cfg.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 01, 2026
 * @brief          : I2C configuration container.
 *                   This file is intentionally kept as a skeleton.
 ********************************************************************************/

#ifndef I2C_CFG_H_
#define I2C_CFG_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Std_Types.h"

/******************************************************
 **                 Configuration defines            **
 ******************************************************/
#define I2C_CHANNEL_0            (0U)
#define I2C_DEFAULT_CLOCK_SPEED (100000U)

/******************************************************
 **                 Type definitions                 **
 ******************************************************/
typedef struct
{
    uint32_t ClockSpeed;
    uint8_t  AddressingMode;
    uint8_t  OwnAddress;
    uint8_t  EnableDMA;
} I2c_ConfigType;

#endif /* I2C_CFG_H_ */
