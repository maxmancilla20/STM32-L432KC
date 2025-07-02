/*
 *  File: Std_Types.h
 *
 *  Created on: Jan 5, 2023
 *  Author: Maximiliano Mancilla
 *  Description: This file contains standard type definitions and macros used in the AUTOSAR MCAL layer.
 *               It includes definitions for standard return types, boolean values, and version information.
 */

#ifndef STD_TYPES_H_
#define STD_TYPES_H_

#include "stm32l4xx.h"
#include <stdint.h>

#define  E_OK             0x00U
#define  E_NOT_OK         0x01U

#define STD_LOW           0x00U
#define STD_HIGH          0x01U

#define STD_IDLE          0x00U
#define STD_ACTIVE        0x01U

#define STD_OFF           0x00U
#define STD_ON            0x01U

typedef uint8_t           Std_ReturnType;

typedef struct {
    // nothing
}Std_VersionInfoType;


#endif /* STD_TYPES_H_ */
