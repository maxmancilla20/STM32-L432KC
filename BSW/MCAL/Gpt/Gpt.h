/****************************************************************************************************
 * @file           : Gpt.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The GPT Driver provides initialization and control functions for general purpose timers
 *                   in the AUTOSAR MCAL layer.
 ***************************************************************************************************/

#ifndef GPT_H_
#define GPT_H_

#include <stdint.h>
#include "Std_Types.h"

/******************************************************
 **                Type definitions                  **
 ******************************************************/
typedef uint32_t Gpt_ChannelType;

/******************************************************
 **             Function Prototypes                  **
 ******************************************************/
void Gpt_Init(void);
void Gpt_StartTimer(Gpt_ChannelType timerId, uint32_t value);
void Gpt_StopTimer(Gpt_ChannelType timerId);
void SystickDelay_Ms(uint32_t delay);
/******************************************************
 **                Macros and Constants               **
 ******************************************************/

 
#endif /* GPT_H_ */