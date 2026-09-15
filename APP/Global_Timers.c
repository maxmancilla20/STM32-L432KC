/****************************************************************************************************
 * @file           : Global_Timers.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 13, 2026
 * @brief          : Global timer manager for application-level timing.
 *                   Provides 10 software timers indexed as GT_0 ... GT_9.
 ****************************************************************************************************/

#include "stm32l4xx.h"
#include "Global_Timers.h"

#define GT_TIMER_COUNT  (10U)

volatile uint32_t g_gt_tick_ms = 0U;
static GlobalTimer_t g_gt_timer_pool[GT_TIMER_COUNT];

static void Gt_ClearTimer(GlobalTimer_t *timer)
{
    timer->start_ms = 0U;
    timer->timeout_ms = 0U;
    timer->state = GT_STATE_STOPPED;
    timer->enabled = 0U;
}

void Gt_InitTimers(void)
{
    uint8_t index;

    for (index = 0U; index < GT_TIMER_COUNT; index++)
    {
        Gt_ClearTimer(&g_gt_timer_pool[index]);
    }
}

void Gt_StartTimer(uint8_t timerIndex, uint32_t timeout_ms)
{
    if (timerIndex >= GT_TIMER_COUNT)
    {
        return;
    }

    g_gt_timer_pool[timerIndex].start_ms = g_gt_tick_ms;
    g_gt_timer_pool[timerIndex].timeout_ms = timeout_ms;
    g_gt_timer_pool[timerIndex].state = GT_STATE_RUNNING;
    g_gt_timer_pool[timerIndex].enabled = 1U;
}

uint8_t Gt_CheckTimer(uint8_t timerIndex)
{
    GlobalTimer_t *timer;

    if (timerIndex >= GT_TIMER_COUNT)
    {
        return 0U;
    }

    timer = &g_gt_timer_pool[timerIndex];

    if (timer->enabled == 0U)
    {
        return 0U;
    }

    if ((g_gt_tick_ms - timer->start_ms) >= timer->timeout_ms)
    {
        timer->state = GT_STATE_EXPIRED;
        return 1U;
    }

    return 0U;
}

void Gt_StopTimer(uint8_t timerIndex)
{
    if (timerIndex >= GT_TIMER_COUNT)
    {
        return;
    }

    Gt_ClearTimer(&g_gt_timer_pool[timerIndex]);
}

void Gt_WaitTimer(uint8_t timerIndex)
{
    if (timerIndex >= GT_TIMER_COUNT)
    {
        return;
    }

    g_gt_timer_pool[timerIndex].state = GT_STATE_WAITING;
    g_gt_timer_pool[timerIndex].enabled = 0U;
}

void Gt_ResetTimer(uint8_t timerIndex)
{
    if (timerIndex >= GT_TIMER_COUNT)
    {
        return;
    }

    g_gt_timer_pool[timerIndex].start_ms = g_gt_tick_ms;
    g_gt_timer_pool[timerIndex].state = GT_STATE_RUNNING;
    g_gt_timer_pool[timerIndex].enabled = 1U;
}
