/********************************************************************************
 * @file           : RC522.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 12, 2026
 * @brief          : MFRC522 driver adapted from the public RC522 STM32 library.
 ********************************************************************************/

#include "RC522.h"
#include "Spi.h"
#include "stm32l4xx.h"

#define RC522_RST_PORT        GPIOB
#define RC522_RST_PIN_BIT     (1U << 0)

void RC522_ResetPin_Init(void)
{
    /* Habilitar reloj de GPIOB */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    /* PB0 como salida */
    GPIOB->MODER &= ~(3U << 0);
    GPIOB->MODER |=  (1U << 0);

    /* Push-pull (default) */
    GPIOB->OTYPER &= ~(1U << 0);

    /* Velocidad media */
    GPIOB->OSPEEDR &= ~(3U << 0);
    GPIOB->OSPEEDR |=  (2U << 0);

    /* Sin pull-up/pull-down */
    GPIOB->PUPDR &= ~(3U << 0);

    /* Estado inactivo: HIGH */
    GPIOB->ODR |= RC522_RST_PIN;
}

static void RC522_SPI_Transfer(const uint8_t *txData, uint8_t *rxData, uint16_t size)
{
    uint16_t i;

    for (i = 0U; i < size; i++)
    {
        while ((SPI1->SR & SPI_SR_TXE) == 0U)
        {
        }

        SPI1->DR = txData[i];

        while ((SPI1->SR & SPI_SR_RXNE) == 0U)
        {
        }

        rxData[i] = (uint8_t)SPI1->DR;
    }
}

static uint8_t RC522_SPI_Byte(uint8_t data)
{
    uint8_t rx = 0U;
    RC522_SPI_Transfer(&data, &rx, 1U);
    return rx;
}

void RC522_WriteRegister(uint8_t addr, uint8_t value)
{
    uint8_t tx[2];
    uint8_t rx[2] = {0U, 0U};

    cs_enable();
    tx[0] = ((uint8_t)(addr << 1U)) & 0x7EU;
    tx[1] = value;
    RC522_SPI_Transfer(tx, rx, 2U);
    cs_disable();
}

uint8_t RC522_ReadRegister(uint8_t addr)
{
    uint8_t tx[2] = {0U, 0U};
    uint8_t rx[2] = {0U, 0U};
    uint8_t value;

    /* MFRC522 SPI read protocol: first byte is the address + read bit,
     * the first returned byte is a dummy/placeholder, and the second one
     * is the actual register value. */
    cs_enable();
    tx[0] = (((uint8_t)(addr << 1U)) & 0x7EU) | 0x80U;
    tx[1] = 0x00U;
    RC522_SPI_Transfer(tx, rx, 2U);
    value = rx[1];
    cs_disable();

    return value;
}

static void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = RC522_ReadRegister(reg);
    RC522_WriteRegister(reg, (uint8_t)(tmp | mask));
}

static void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = RC522_ReadRegister(reg);
    RC522_WriteRegister(reg, (uint8_t)(tmp & (uint8_t)(~mask)));
}

static void RC522_AntennaOn(void)
{
    uint8_t tmp = RC522_ReadRegister(TxControlReg);
    RC522_SetBitMask(TxControlReg, (uint8_t)(tmp | 0x03U));
}

static void RC522_AntennaOff(void)
{
    RC522_ClearBitMask(TxControlReg, 0x03U);
}

static void RC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
    uint8_t i;
    uint8_t n;

    RC522_ClearBitMask(DivIrqReg, 0x04U);
    RC522_SetBitMask(FIFOLevelReg, 0x80U);

    for (i = 0U; i < len; i++)
    {
        RC522_WriteRegister(FIFODataReg, pIndata[i]);
    }

    RC522_WriteRegister(CommandReg, PCD_CALCCRC);

    i = 0xFFU;
    do
    {
        n = RC522_ReadRegister(DivIrqReg);
        i--;
    } while ((i != 0U) && !(n & 0x04U));

    pOutData[0] = RC522_ReadRegister(CRCResultRegL);
    pOutData[1] = RC522_ReadRegister(CRCResultRegH);
}

static uint8_t RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
                            uint8_t *backData, uint16_t *backLen)
{
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00U;
    uint8_t waitIRq = 0x00U;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    switch (command)
    {
        case PCD_AUTHENT:
            irqEn = 0x12U;
            waitIRq = 0x10U;
            break;
        case PCD_TRANSCEIVE:
            irqEn = 0x77U;
            waitIRq = 0x30U;
            break;
        default:
            break;
    }

    RC522_WriteRegister(CommIEnReg, (uint8_t)(irqEn | 0x80U));
    RC522_ClearBitMask(CommIrqReg, 0x80U);
    RC522_SetBitMask(FIFOLevelReg, 0x80U);
    RC522_WriteRegister(CommandReg, PCD_IDLE);

    for (i = 0U; i < sendLen; i++)
    {
        RC522_WriteRegister(FIFODataReg, sendData[i]);
    }

    RC522_WriteRegister(CommandReg, command);

    if (command == PCD_TRANSCEIVE)
    {
        RC522_SetBitMask(BitFramingReg, 0x80U);
    }

    i = 2000U;
    do
    {
        n = RC522_ReadRegister(CommIrqReg);
        i--;
    } while ((i != 0U) && !(n & 0x01U) && !(n & waitIRq));

    RC522_ClearBitMask(BitFramingReg, 0x80U);

    if (i != 0U)
    {
        if (!(RC522_ReadRegister(ErrorReg) & 0x1BU))
        {
            status = MI_OK;

            if (n & irqEn & 0x01U)
            {
                status = MI_NOTAGERR;
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = RC522_ReadRegister(FIFOLevelReg);
                lastBits = RC522_ReadRegister(ControlReg) & 0x07U;

                if (lastBits)
                {
                    *backLen = (uint16_t)((n - 1U) * 8U + lastBits);
                }
                else
                {
                    *backLen = (uint16_t)(n * 8U);
                }

                if (n == 0U)
                {
                    n = 1U;
                }
                if (n > RC522_MAX_LEN)
                {
                    n = RC522_MAX_LEN;
                }

                for (i = 0U; i < n; i++)
                {
                    backData[i] = RC522_ReadRegister(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

    return status;
}

void RC522_Reset(void)
{
    /* Reset activo en LOW */
    GPIOB->ODR &= ~RC522_RST_PIN;

    /* Espera breve para asegurar el pulso */
    for (volatile uint32_t i = 0; i < 5000U; i++)
    {
    }

    GPIOB->ODR |= RC522_RST_PIN;
}

void RC522_Init(void)
{
    /* Match the repo reference: configure the reset pin first, then SPI */
    RC522_ResetPin_Init();
    spi1_gpio_init();
    spi1_config();

    RC522_Reset();

    /* Verify the chip is alive after reset */
    {
        uint8_t version = RC522_ReadRegister(VersionReg);
        (void)version;
    }

    RC522_WriteRegister(TModeReg, 0x8DU);
    RC522_WriteRegister(TPrescalerReg, 0x3EU);
    RC522_WriteRegister(TReloadRegL, 30U);
    RC522_WriteRegister(TReloadRegH, 0U);
    RC522_WriteRegister(TxAutoReg, 0x40U);
    RC522_WriteRegister(ModeReg, 0x3DU);

    RC522_AntennaOn();
}

uint8_t RC522_Request(uint8_t reqMode, uint8_t *tagType)
{
    uint8_t status;
    uint16_t backBits;

    RC522_WriteRegister(BitFramingReg, 0x07U);
    tagType[0] = reqMode;
    status = RC522_ToCard(PCD_TRANSCEIVE, tagType, 1U, tagType, &backBits);

    if ((status != MI_OK) || (backBits != 0x10U))
    {
        status = MI_ERR;
    }

    return status;
}

uint8_t RC522_Anticoll(uint8_t *serialNo)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0U;
    uint16_t unLen;

    RC522_WriteRegister(BitFramingReg, 0x00U);
    serialNo[0] = PICC_ANTICOLL;
    serialNo[1] = 0x20U;
    status = RC522_ToCard(PCD_TRANSCEIVE, serialNo, 2U, serialNo, &unLen);

    if (status == MI_OK)
    {
        for (i = 0U; i < 4U; i++)
        {
            serNumCheck ^= serialNo[i];
        }

        if (serNumCheck != serialNo[i])
        {
            status = MI_ERR;
        }
    }

    return status;
}

uint8_t RC522_SelectTag(uint8_t *serialNo)
{
    uint8_t i;
    uint8_t status;
    uint8_t size;
    uint16_t recvBits;
    uint8_t buffer[9];

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70U;
    for (i = 0U; i < 5U; i++)
    {
        buffer[i + 2U] = serialNo[i];
    }

    RC522_CalculateCRC(buffer, 7U, &buffer[7U]);
    status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9U, buffer, &recvBits);

    if ((status == MI_OK) && (recvBits == 0x18U))
    {
        size = buffer[0];
    }
    else
    {
        size = 0U;
    }

    return size;
}

uint8_t RC522_Auth(uint8_t authMode, uint8_t blockAddr, uint8_t *sectorKey, uint8_t *serialNo)
{
    uint8_t status;
    uint16_t recvBits;
    uint8_t i;
    uint8_t buff[12];

    buff[0] = authMode;
    buff[1] = blockAddr;

    for (i = 0U; i < 6U; i++)
    {
        buff[i + 2U] = sectorKey[i];
    }

    for (i = 0U; i < 4U; i++)
    {
        buff[i + 8U] = serialNo[i];
    }

    status = RC522_ToCard(PCD_AUTHENT, buff, 12U, buff, &recvBits);

    if ((status != MI_OK) || (!(RC522_ReadRegister(Status2Reg) & 0x08U)))
    {
        status = MI_ERR;
    }

    return status;
}

uint8_t RC522_Read(uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint16_t unLen;
    uint8_t buffer[4];

    buffer[0] = PICC_READ;
    buffer[1] = blockAddr;
    RC522_CalculateCRC(buffer, 2U, &buffer[2U]);
    status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 4U, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90U))
    {
        status = MI_ERR;
    }

    return status;
}

uint8_t RC522_Write(uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint16_t recvBits;
    uint8_t i;
    uint8_t buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    RC522_CalculateCRC(buff, 2U, &buff[2U]);
    status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4U, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4U) || ((buff[0] & 0x0FU) != 0x0AU))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        for (i = 0U; i < 16U; i++)
        {
            buff[i] = writeData[i];
        }

        RC522_CalculateCRC(buff, 16U, &buff[16U]);
        status = RC522_ToCard(PCD_TRANSCEIVE, buff, 18U, buff, &recvBits);

        if ((status != MI_OK) || (recvBits != 4U) || ((buff[0] & 0x0FU) != 0x0AU))
        {
            status = MI_ERR;
        }
    }

    return status;
}

void RC522_Halt(void)
{
    uint16_t unLen;
    uint8_t buffer[4];

    buffer[0] = PICC_HALT;
    buffer[1] = 0U;
    RC522_CalculateCRC(buffer, 2U, &buffer[2U]);
    RC522_ToCard(PCD_TRANSCEIVE, buffer, 4U, buffer, &unLen);
}
