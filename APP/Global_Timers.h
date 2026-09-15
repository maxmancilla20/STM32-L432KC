/****************************************************************************************************
 * @file           : Global_Timers.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 13, 2026
 * @brief          : Application-level timer manager definitions.
 ****************************************************************************************************/

#ifndef GLOBAL_TIMERS_H_
#define GLOBAL_TIMERS_H_

#include "stm32l4xx.h"

#define GT_TIMER_COUNT  (10U)

typedef enum
{
    GT_0 = 0U,
    GT_1,
    GT_2,
    GT_3,
    GT_4,
    GT_5,
    GT_6,
    GT_7,
    GT_8,
    GT_9
} GlobalTimerIndex_t;

typedef enum
{
    GT_STATE_STOPPED = 0U,
    GT_STATE_RUNNING,
    GT_STATE_WAITING,
    GT_STATE_EXPIRED
} GlobalTimerState_t;

typedef struct
{
    uint32_t start_ms;
    uint32_t timeout_ms;
    GlobalTimerState_t state;
    uint8_t enabled;
} GlobalTimer_t;

void Gt_InitTimers(void);
void Gt_StartTimer(uint8_t timerIndex, uint32_t timeout_ms);
uint8_t Gt_CheckTimer(uint8_t timerIndex);
void Gt_StopTimer(uint8_t timerIndex);
void Gt_WaitTimer(uint8_t timerIndex);
void Gt_ResetTimer(uint8_t timerIndex);
void Gt_TickIncrement(void);

#endif /* GLOBAL_TIMERS_H_ */
