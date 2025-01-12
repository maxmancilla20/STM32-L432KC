/*
 * main.c
 * 
 * This file contains the main function and UART2 initialization function for the STM32L4 microcontroller.
 * 
 * Author: Maximiliano Mancilla
 * 
 * ============================= DATA =======================================
 * The STM32L432xx devices have two embedded universal synchronous receiver transmitters (USART1 and USART2).
 * These interfaces provide asynchronous communication, IrDA SIR ENDEC support, multiprocessor communication mode, 
 * single-wire half-duplex communication mode and have LIN Master/Slave capability. They provide hardware management
 * of the CTS and RTS signals, and RS485 Driver Enable. They are able to communicate at speeds of up to 10Mbit/s.
 * ==========================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx.h"

#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)
#define UART2EN             (1U<<17)

#define SYSFREQ_4_MHZ       4000000U

#define UART2_BAUD_RATE     115200U

#define CR1_TE              (1U<<3)
#define CR1_UE              (1U<<0)
#define ISR_TXE              (1U<<7)

static void uart_set_baud_rate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);
void write_uart2(uint8_t data);
void Uart2_TX_Init(void);


int __io_putchar(int ch)
{
    write_uart2(ch);
    return ch;
}
/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */
int main(void)
{
    // Initialization code here
    Uart2_TX_Init();

    while(1)
    {
        // Main loop
    	printf("Hello World!\n\r");
    }


    return 0;
}

/*
 * Uart2_TX_Init function
 * 
 * This function initializes UART2 for transmission.
 * It configures GPIOA and UART2 registers.
 */
void Uart2_TX_Init(void)
{
    /* ================ CONFIGURE GPIOA ================= */
    /* Enable port access to GPIOA */
    RCC->AHB2ENR |= GPIOAEN;

    /* Set PA2 mode to alternate function mode */
    GPIOA->MODER &= ~(1U << 4); /* Clear bit 4 */
    GPIOA->MODER |= (1U << 5); /* Set bit 5 */


    /* Set PA2 alternate function type to UART_TX (AF07). 
     * Note: AFR[0] is for low regs (pin 0 to 7). AFR[1] is for high reg (pin 8 to 15) */
    GPIOA->AFR[0] |= (1U << 8); /* Set bit 8 */
    GPIOA->AFR[0] |= (1U << 9); /* Set bit 9 */
    GPIOA->AFR[0] |= (1U << 10); /* Set bit 10 */
    GPIOA->AFR[0] &= ~(1U << 11); /* Clear bit 11 */

    /* ================ CONFIGURE UART2 ================= */
    /* Enable clock access to UART2 */
    RCC->APB1ENR1 |= UART2EN;
    /* Set UART2 baud rate to 115200U */
    uart_set_baud_rate(USART2, SYSFREQ_4_MHZ, UART2_BAUD_RATE);
    /* Set UART2 word length to 8 bits */
    /* Set UART2 stop bits to 1 */
    /* Configure the transfer direction 
    Note: Use only = to clear the UART reg*/
    USART2->CR1 = CR1_TE;
    /* Enable UART2 */
    USART2->CR1 |= CR1_UE;
}

static void uart_set_baud_rate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate); /* Set baud rate */
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
   return ((PeriphClk + (BaudRate/2U)) / BaudRate); /* Compute baud rate */
}

void write_uart2(uint8_t data)
{
    /* Wait until the transmit data register is empty */
    while(!(USART2->ISR & (ISR_TXE))){}
    /* Write data to the transmit data register */
    USART2->TDR = (data & 0xFFU);
}
