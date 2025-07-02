/*************************************************************************************************************************************************
 * @file           : Mcu.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The MCU Driver [MicroController Unit] provides services for basic microcontroller initialization, power down functionality,
 *                   reset an microcontroller specific functions required from other MCAL software modules.
 *************************************************************************************************************************************************/

#ifndef MCU_H_
#define MCU_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Mcu_Cfg.h"
#include "Std_Types.h"

/******************************************************
 **                DET Error Codes                   **
 ******************************************************/

 /*******************************************************
 **                 Type definitions                  **
 *******************************************************/

typedef struct {
    uint8_t Gpios_Clocks_En;   // enable clocks of the all peripheral, gpios from A to H, adc, aes, rng
    uint8_t AdcEn;
}Mcu_ConfigType; /* SWS_Mcu_00249, SWS_Mcu_00131*/

typedef enum {
   MCU_PLL_LOCKED,
   MCU_PLL_UNLOCKED,
   MCU_PLL_STATUS_UNDEFINED
} Mcu_PllStatusType; /* SWS_Mcu_00250*/

typedef uint32_t Mcu_ClockType; /* SWS_Mcu_00251 */

typedef enum {
   MCU_POWER_ON_RESET,
   MCU_WATCHDOG_RESET,
   MCU_SW_RESET,
   MCU_RESET_UNDEFINED
} Mcu_ResetType; /* SWS_Mcu_00252 */

typedef uint32_t Mcu_RawResetType; /* SWS_Mcu_00253 */

typedef uint32_t Mcu_ModeType; /* SWS_Mcu_00254 */

typedef uint32_t Mcu_RamSectionType; /* SWS_Mcu_00255 */

typedef enum {
    MCU_RAMSTATE_INVALID,
    MCU_RAMSTATE_VALID
} Mcu_RamStateType;

 /******************************************************************
 **                      FUNCTIONS PROTOTYPES                    **
 ******************************************************************/

/* SWS_Mcu_00153, SWS_Mcu_00026 */
void Mcu_Init( const Mcu_ConfigType* ConfigPtr );

/* SWS_Mcu_00154, SWS_Mcu_00011 */
Std_ReturnType Mcu_InitRamSection( Mcu_RamSectionType RamSection );

/* SWS_Mcu_00155, SWS_Mcu_00137 */
Std_ReturnType Mcu_InitClock( Mcu_ClockType ClockSetting );

/* SWS_Mcu_00156, SWS_Mcu_00140 */
Std_ReturnType Mcu_DistributePllClock( void );

/* SWS_Mcu_00157, SWS_Mcu_00008 */
Mcu_PllStatusType Mcu_GetPllStatus( void );

/* SWS_Mcu_00158, SWS_Mcu_00005 */
Mcu_ResetType Mcu_GetResetReason( void );

/* SWS_Mcu_00159, SWS_Mcu_00135 */
Mcu_RawResetType Mcu_GetResetRawValue( void );

/* SWS_Mcu_00160, SWS_Mcu_00143 */
void Mcu_PerformReset( void );

/* SWS_Mcu_00161, SWS_Mcu_00147 */
void Mcu_SetMode( Mcu_ModeType McuMode );

/* SWS_Mcu_00162 */
void Mcu_GetVersionInfo( Std_VersionInfoType* versioninfo );

/* SWS_Mcu_00207, SWS_Mcu_00208 */
Mcu_RamStateType Mcu_GetRamState( void );

#endif /* MCU_H_ */
