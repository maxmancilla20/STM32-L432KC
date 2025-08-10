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

#define SR_UIF            (1U << 0) // Update interrupt flag
#define SR_CC1IF         (1U << 1) // Capture/Compare 1 interrupt flag
#define SR_CC2IF         (1U << 2) // Capture/Compare 2 interrupt flag
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
void tim2_1hz_init(void);
void tim2_1hz_delay(void);
void tim2_output_pb3_compare_1hz(void);
void tim2_input_capture(void);
/******************************************************
 **                Macros and Constants               **
 ******************************************************/

 
#endif /* GPT_H_ */