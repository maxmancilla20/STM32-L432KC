/********************************************************************************
 * @file           : Spi.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 10, 2026
 * @brief          : SPI Driver implementation.
 *                   SPI1 PINOUT: PA5(SCK), PA6(MISO), PA7(MOSI), PA4(NSS/CS).
 ********************************************************************************/

#include "stm32l4xx.h"
#include "Spi.h"
#include <stdio.h>

void spi1_gpio_init(void)
{
    /* Enable clock access to GPIOA */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    /* Enable clock access to SPI1 */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /* Configure PA5, PA6, PA7, PA4 as alternate function */
    GPIOA->MODER &= ~((3U << 10) | (3U << 12) | (3U << 14) | (3U << 8));
    GPIOA->MODER |=  ((2U << 10) | (2U << 12) | (2U << 14) | (2U << 8));

    /* AF5 for SPI1 on PA5..PA7 and PA4 */
    GPIOA->AFR[0] &= ~((0xFU << 20) | (0xFU << 24) | (0xFU << 28) | (0xFU << 16));
    GPIOA->AFR[0] |=  ((5U << 20) | (5U << 24) | (5U << 28) | (5U << 16));

    /* Use PA4 as chip select */
    GPIOA->MODER &= ~(3U << 8);
    GPIOA->MODER |=  (1U << 8);
    GPIOA->ODR |= (1U << 4);
}

void spi1_config(void)
{
    /* Baud rate: fPCLK/4 */
    SPI1->CR1 &= ~(0x7U << SPI_CR1_BR_Pos);
    SPI1->CR1 |= (0x1U << SPI_CR1_BR_Pos);

    /* SPI mode: CPOL=0, CPHA=0 */
    SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

    /* Full duplex */
    SPI1->CR1 &= ~SPI_CR1_RXONLY;

    /* MSB first */
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

    /* Set master mode */
    SPI1->CR1 |= SPI_CR1_MSTR;

    /* 8-bit data frame */
    SPI1->CR2 &= ~(0xFU << SPI_CR2_DS_Pos);
    SPI1->CR2 |= (7U << SPI_CR2_DS_Pos);

    /* Software slave management */
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

    /* Enable SPI1 */
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_transmit(uint8_t *data, uint32_t size)
{
    uint32_t i = 0;

    while (i < size)
    {
        while (!(SPI1->SR & SPI_SR_TXE))
        {
        }

        SPI1->DR = data[i];
        i++;
    }

    while (!(SPI1->SR & SPI_SR_TXE))
    {
    }

    while (SPI1->SR & SPI_SR_BSY)
    {
    }
}

void spi1_receive(uint8_t *data, uint32_t size)
{
    uint32_t i = 0;

    while (i < size)
    {
        SPI1->DR = 0x00U;

        while (!(SPI1->SR & SPI_SR_RXNE))
        {
        }

        data[i] = (uint8_t)SPI1->DR;
        i++;
    }

    while (SPI1->SR & SPI_SR_BSY)
    {
    }
}

void spi1_loopback_test(void)
{
    uint8_t txData[4] = {0x01U, 0x02U, 0x03U, 0x04U};
    uint8_t rxData[4] = {0U, 0U, 0U, 0U};
    uint32_t i = 0U;

    cs_enable();

    for (i = 0U; i < 4U; i++)
    {
        while (!(SPI1->SR & SPI_SR_TXE))
        {
        }

        SPI1->DR = txData[i];

        while (!(SPI1->SR & SPI_SR_RXNE))
        {
        }

        rxData[i] = (uint8_t)SPI1->DR;
    }

    cs_disable();

    if ((rxData[0] == txData[0]) &&
        (rxData[1] == txData[1]) &&
        (rxData[2] == txData[2]) &&
        (rxData[3] == txData[3]))
    {
        printf("SPI loopback OK\r\n");
    }
    else
    {
        printf("SPI loopback FAIL\r\n");
    }
}

void cs_enable(void)
{
    /* CS active low */
    GPIOA->ODR &= ~(1U << 4);
}

void cs_disable(void)
{
    /* CS inactive high */
    GPIOA->ODR |= (1U << 4);
}
