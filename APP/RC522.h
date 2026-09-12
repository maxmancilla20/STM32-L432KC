/********************************************************************************
 * @file           : RC522.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 12, 2026
 * @brief          : MFRC522 RC522 reader API adapted from the public STM32 library.
 ********************************************************************************/

#ifndef RC522_H_
#define RC522_H_

#include "Std_Types.h"

#define RC522_MAX_LEN       16U
#define RC522_RST_PIN       (1U << 0)

#define PCD_IDLE            0x00U
#define PCD_AUTHENT         0x0EU
#define PCD_RECEIVE         0x08U
#define PCD_TRANSMIT        0x04U
#define PCD_TRANSCEIVE      0x0CU
#define PCD_RESETPHASE      0x0FU
#define PCD_CALCCRC         0x03U

#define PICC_REQIDL         0x26U
#define PICC_REQALL         0x52U
#define PICC_ANTICOLL       0x93U
#define PICC_SElECTTAG      0x93U
#define PICC_AUTHENT1A      0x60U
#define PICC_AUTHENT1B      0x61U
#define PICC_READ           0x30U
#define PICC_WRITE          0xA0U
#define PICC_DECREMENT      0xC0U
#define PICC_INCREMENT      0xC1U
#define PICC_RESTORE        0xC2U
#define PICC_TRANSFER       0xB0U
#define PICC_HALT           0x50U

#define MI_OK               0U
#define MI_NOTAGERR         1U
#define MI_ERR              2U

#define Reserved00          0x00U
#define CommandReg          0x01U
#define CommIEnReg          0x02U
#define DivlEnReg           0x03U
#define CommIrqReg          0x04U
#define DivIrqReg           0x05U
#define ErrorReg            0x06U
#define Status1Reg          0x07U
#define Status2Reg          0x08U
#define FIFODataReg         0x09U
#define FIFOLevelReg        0x0AU
#define WaterLevelReg       0x0BU
#define ControlReg          0x0CU
#define BitFramingReg       0x0DU
#define CollReg             0x0EU
#define Reserved01          0x0FU

#define Reserved10          0x10U
#define ModeReg             0x11U
#define TxModeReg           0x12U
#define RxModeReg           0x13U
#define TxControlReg        0x14U
#define TxAutoReg           0x15U
#define TxSelReg            0x16U
#define RxSelReg            0x17U
#define RxThresholdReg      0x18U
#define DemodReg            0x19U
#define Reserved11          0x1AU
#define Reserved12          0x1BU
#define MifareReg           0x1CU
#define Reserved13          0x1DU
#define Reserved14          0x1EU
#define SerialSpeedReg      0x1FU

#define Reserved20          0x20U
#define CRCResultRegH       0x21U
#define CRCResultRegL       0x22U
#define Reserved21          0x23U
#define ModWidthReg         0x24U
#define Reserved22          0x25U
#define RFCfgReg            0x26U
#define GsNReg              0x27U
#define CWGsPReg            0x28U
#define ModGsPReg           0x29U
#define TModeReg            0x2AU
#define TPrescalerReg       0x2BU
#define TReloadRegH         0x2CU
#define TReloadRegL         0x2DU
#define TCounterValueRegH    0x2EU
#define TCounterValueRegL    0x2FU

#define Reserved30          0x30U
#define TestSel1Reg         0x31U
#define TestSel2Reg         0x32U
#define TestPinEnReg        0x33U
#define TestPinValueReg     0x34U
#define TestBusReg          0x35U
#define AutoTestReg         0x36U
#define VersionReg          0x37U
#define AnalogTestReg       0x38U
#define TestDAC1Reg         0x39U
#define TestDAC2Reg         0x3AU
#define TestADCReg          0x3BU
#define Reserved31          0x3CU
#define Reserved32          0x3DU
#define Reserved33          0x3EU
#define Reserved34          0x3FU

void RC522_ResetPin_Init(void);
void RC522_Init(void);
void RC522_Reset(void);
void RC522_WriteRegister(uint8_t addr, uint8_t value);
uint8_t RC522_ReadRegister(uint8_t addr);
uint8_t RC522_Request(uint8_t reqMode, uint8_t *tagType);
uint8_t RC522_Anticoll(uint8_t *serialNo);
uint8_t RC522_SelectTag(uint8_t *serialNo);
uint8_t RC522_Auth(uint8_t authMode, uint8_t blockAddr, uint8_t *sectorKey, uint8_t *serialNo);
uint8_t RC522_Read(uint8_t blockAddr, uint8_t *recvData);
uint8_t RC522_Write(uint8_t blockAddr, uint8_t *writeData);
void RC522_Halt(void);

#endif /* RC522_H_ */
