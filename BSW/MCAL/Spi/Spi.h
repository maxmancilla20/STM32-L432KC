/********************************************************************************
 * @file           : Spi.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 10, 2026
 * @brief          : SPI Driver interface definition.
 *                   This file is intentionally kept as a skeleton.
 ********************************************************************************/

#ifndef SPI_H_
#define SPI_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Std_Types.h"
#include "Spi_Cfg.h"

/******************************************************
 **                Function prototypes               **
 ******************************************************/
void spi1_gpio_init(void);
void spi1_config(void);
void spi1_transmit(uint8_t *data, uint32_t size);
void spi1_receive(uint8_t *data, uint32_t size);
void spi1_loopback_test(void);
void cs_enable(void);
void cs_disable(void);

#endif /* SPI_H_ */
