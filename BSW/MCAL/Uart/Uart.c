/****************************************************************************************************
 * @file           : Uart.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The UART Driver handles initialization, transmission, and reception for UART2.
 *                   This module abstracts the UART peripheral for the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#include "Uart.h"
#include "Uart_Cfg.h"

#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)
#define UART2EN             (1U<<17)

#define SYSFREQ_4_MHZ       4000000U


#define CR1_TE              (1U<<3)
#define CR1_RE              (1U<<2)
#define CR1_UE              (1U<<0)
#define ISR_TXE              (1U<<7)
#define ISR_RXN             (1U<<5)

static void uart_set_baud_rate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);
void write_uart2(uint8_t data);

int __io_putchar(int ch)
{
    write_uart2(ch);
    return ch;
}

/*
 * Uart2_TX_Init function
 * 
 * This function initializes UART2 for transmission.
 * It configures GPIOA and UART2 registers.
 */
void Uart2_RXTX_Init(void)
{
    /* ================ CONFIGURE GPIOA ================= */
    /* Enable port access to GPIOA */
    //RCC->AHB2ENR |= GPIOAEN;

    /* ================ TX CFG ===================*/
    /* Set PA2 mode to alternate function mode */
    GPIOA->MODER &= ~(1U << 4); /* Clear bit 4 */
    GPIOA->MODER |= (1U << 5); /* Set bit 5 */

    /* Set PA2 alternate function type to UART_TX (AF07). 
     * Note: AFR[0] is for low regs (pin 0 to 7). AFR[1] is for high reg (pin 8 to 15) */
    GPIOA->AFR[0] |= (1U << 8); /* Set bit 8 */
    GPIOA->AFR[0] |= (1U << 9); /* Set bit 9 */
    GPIOA->AFR[0] |= (1U << 10); /* Set bit 10 */
    GPIOA->AFR[0] &= ~(1U << 11); /* Clear bit 11 */

    /* ================ RX CFG ===================*/
    /* Set PA3 mode to alternate function mode*/
    GPIOA->MODER &= ~(1U << 6); /* Clear bit 6 */
    GPIOA->MODER |= (1U << 7); /* Set bit 7 */

    /* Set PA3 alternate function type to UART_TX (AF07)*/
    GPIOA->AFR[0] |= (1U << 12); /* Set bit 12 */
    GPIOA->AFR[0] |= (1U << 13); /* Set bit 13 */
    GPIOA->AFR[0] |= (1U << 14); /* Set bit 14 */
    GPIOA->AFR[0] &= ~(1U << 15); /* Clear bit 15 */
    
    /* ================ CONFIGURE UART2 ================= */
    /* Enable clock access to UART2 */
    RCC->APB1ENR1 |= UART2EN;
    /* Set UART2 baud rate to 115200U */
    uart_set_baud_rate(USART2, SYSFREQ_4_MHZ, UART2_BAUD_RATE);
    /* Set UART2 word length to 8 bits */
    /* Set UART2 stop bits to 1 */
    /* Configure the transfer direction 
    Note: Use only = to clear the UART reg*/
    USART2->CR1 = (CR1_TE | CR1_RE);
    /* Enable UART2 */
    USART2->CR1 |= CR1_UE;
}
/*
 * uart_set_baud_rate function
 * 
 * This function sets the baud rate for the UART peripheral.
 */
static void uart_set_baud_rate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate); /* Set baud rate */
}

/*
 * compute_uart_bd function
 * 
 * This function computes the baud rate for the UART peripheral.
 */
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
   return ((PeriphClk + (BaudRate/2U)) / BaudRate); /* Compute baud rate */
}

/*
 * write_uart2 function
 * 
 * This function writes data to the UART2 peripheral.
 */
void write_uart2(uint8_t data)
{
    /* Wait until the transmit data register is empty */
    while(!(USART2->ISR & (ISR_TXE))){}
    /* Write data to the transmit data register */
    USART2->TDR = (data & 0xFFU);
}

/*
 * read_uart2 function
 * 
 * This function reads data from the UART2 peripheral.
 */
char read_uart2(void)
{
    /* Wait until the receive data register is not empty */
    while(!(USART2->ISR & (ISR_RXN))){}
    /* Read data from the receive data register */
    return (char)(USART2->RDR & 0xFFU);
}
