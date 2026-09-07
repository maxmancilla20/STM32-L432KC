/****************************************************************************************************
 * @file           : I2c.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 01, 2026
 * @brief          : I2C Driver.
 *
 ****************************************************************************************************/
/****************************************************************************************************
 * @pinout
 * PA9 - I2C1_SCL
 * PA10 - I2C1_SDA
 * 
****************************************************************************************************/
#include "stm32l4xx.h"
#include "I2c.h"

#define GPIOAEN             (1U<<0)
#define I2C1_TIMING_100KHZ_4MHZ (0x0010061AU)
#define I2C_TIMEOUT_COUNT   (1000000U)

static uint8_t I2c_WaitForSet(uint32_t flag)
{
    uint32_t timeout = I2C_TIMEOUT_COUNT;

    while (((I2C1->ISR & flag) == 0U) && (timeout > 0U))
    {
        timeout--;
    }

    return (uint8_t)((I2C1->ISR & flag) != 0U);
}

static uint8_t I2c_WaitForClear(uint32_t flag)
{
    uint32_t timeout = I2C_TIMEOUT_COUNT;

    while (((I2C1->ISR & flag) != 0U) && (timeout > 0U))
    {
        timeout--;
    }

    return (uint8_t)((I2C1->ISR & flag) == 0U);
}

void I2c_Init(void)
{
    /* Enable Clock Access for I2C1  GPIOA */
    //RCC->AHB2ENR |= GPIOAEN;

    /* Set PA9 and PA10 to alternate function */
    GPIOA->MODER &= ~(1U << 18); /* Clear bit 18 */
    GPIOA->MODER |= (1U << 19); /* Set bit 19 */

    GPIOA->MODER &= ~(1U << 20); /* Clear bit 20 */
    GPIOA->MODER |= (1U << 21); /* Set bit 21 */

    /* Select AF4 for I2C1 on PA9 and PA10 */
    GPIOA->AFR[1] &= ~((0xFU << 4U) | (0xFU << 8U));
    GPIOA->AFR[1] |= ((4U << 4U) | (4U << 8U));

    /* Set PA9 and PA10 outpot type to open drain */
    GPIOA->OTYPER |= (1U << 9); /* Set bit 9 */
    GPIOA->OTYPER |= (1U << 10); /* Set bit 10 */

    /* Enable Pullup for PA9 and PA10 */
    GPIOA->PUPDR &= ~(1U << 18); /* Clear bit 18 */
    GPIOA->PUPDR |= (1U << 19); /* Set bit 19 */

    GPIOA->PUPDR &= ~(1U << 20); /* Clear bit 20 */
    GPIOA->PUPDR |= (1U << 21); /* Set bit 21 */

    /* Enable clock access to I2C1 */
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    /* Enter reset mode */
    I2C1->CR1 &= ~ I2C_CR1_PE;

    /* APB1 = 4 MHz, standard-mode I2C = 100 kHz */
    I2C1->TIMINGR = I2C1_TIMING_100KHZ_4MHZ;

    /* Enable I2C1 */
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2c_DeInit(void)
{
    /* TODO: Reset or disable the I2C peripheral */
}

void I2c_burstWrite(uint8_t slaveAddress, uint8_t registerAddress,
                    uint8_t length, const uint8_t *data)
{
    uint8_t index;

    if ((data == 0) || (length == 0U) || (length >= 255U))
    {
        return;
    }

    if (!I2c_WaitForClear(I2C_ISR_BUSY))
    {
        return;
    }

    I2C1->CR2 = ((uint32_t)slaveAddress << 1U) |
                ((uint32_t)(length + 1U) << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_AUTOEND |
                I2C_CR2_START;

    if (!I2c_WaitForSet(I2C_ISR_TXIS))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        I2C1->ICR = I2C_ICR_NACKCF;
        return;
    }

    I2C1->TXDR = registerAddress;

    for (index = 0U; index < length; index++)
    {
        if (!I2c_WaitForSet(I2C_ISR_TXIS))
        {
            I2C1->CR2 |= I2C_CR2_STOP;
            I2C1->ICR = I2C_ICR_NACKCF;
            return;
        }

        I2C1->TXDR = data[index];
    }

    if (!I2c_WaitForSet(I2C_ISR_STOPF))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    I2C1->ICR = I2C_ICR_STOPCF;
}

void I2c_byteRead(uint8_t saddr, uint8_t maddr, uint8_t *data)
{
    if ((data == 0) || !I2c_WaitForClear(I2C_ISR_BUSY))
    {
        return;
    }

    I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    I2C1->CR2 = ((uint32_t)saddr << 1U) |
                (1U << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_START;

    if (!I2c_WaitForSet(I2C_ISR_TXIS))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    I2C1->TXDR = maddr;

    if (!I2c_WaitForSet(I2C_ISR_TC))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    I2C1->CR2 = ((uint32_t)saddr << 1U) |
                (1U << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_RD_WRN |
                I2C_CR2_AUTOEND |
                I2C_CR2_START;

    if (!I2c_WaitForSet(I2C_ISR_RXNE))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    *data = (uint8_t)I2C1->RXDR;

    if (!I2c_WaitForSet(I2C_ISR_STOPF))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    I2C1->ICR = I2C_ICR_STOPCF;
}

void I2c_burstRead(uint8_t slaveAddress, uint8_t registerAddress,
                   uint8_t length, uint8_t *data)
{
    uint8_t index;

    if ((data == 0) || (length == 0U))
    {
        return;
    }

    if (!I2c_WaitForClear(I2C_ISR_BUSY))
    {
        return;
    }

    I2C1->CR2 = ((uint32_t)slaveAddress << 1U) |
                (1U << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_START;

        if (!I2c_WaitForSet(I2C_ISR_TXIS))
        {
            I2C1->CR2 |= I2C_CR2_STOP;
            return;
        }

    I2C1->TXDR = registerAddress;

        if (!I2c_WaitForSet(I2C_ISR_TC))
        {
            I2C1->CR2 |= I2C_CR2_STOP;
            return;
        }

    I2C1->CR2 = ((uint32_t)slaveAddress << 1U) |
                ((uint32_t)length << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_RD_WRN |
                I2C_CR2_AUTOEND |
                I2C_CR2_START;

    for (index = 0U; index < length; index++)
    {
        if (!I2c_WaitForSet(I2C_ISR_RXNE))
        {
            I2C1->CR2 |= I2C_CR2_STOP;
            return;
        }

        data[index] = (uint8_t)I2C1->RXDR;
    }

    if (!I2c_WaitForSet(I2C_ISR_STOPF))
    {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    I2C1->ICR = I2C_ICR_STOPCF;
}
