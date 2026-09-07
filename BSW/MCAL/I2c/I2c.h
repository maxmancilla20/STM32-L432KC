/********************************************************************************
 * @file           : I2c.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 01, 2026
 * @brief          : I2C Driver interface definition.
 *                   This file is intentionally kept as a skeleton.
 ********************************************************************************/

#ifndef I2C_H_
#define I2C_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Std_Types.h"
#include "I2c_Cfg.h"

/******************************************************
 **                 Type definitions                 **
 ******************************************************/

/*********************************************************
 **                   Function Prototypes               **
 *********************************************************/
void I2c_Init(void);
void I2c_DeInit(void);
void I2c_burstWrite(uint8_t slaveAddress, uint8_t registerAddress,
					uint8_t length, const uint8_t *data);
void I2c_burstRead(uint8_t slaveAddress, uint8_t registerAddress,
				   uint8_t length, uint8_t *data);
void I2c_byteRead(uint8_t saddr, uint8_t maddr, uint8_t* data);


#endif /* I2C_H_ */
