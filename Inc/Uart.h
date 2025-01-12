/*
 * Uart.h
 * 
 * This file contains the main function and UART2 initialization function for the STM32L4 microcontroller.
 * 
 * Author: Maximiliano Mancilla
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx.h"

void Uart2_RXTX_Init(void);
char read_uart2(void);

#endif /* UART_H_ */
