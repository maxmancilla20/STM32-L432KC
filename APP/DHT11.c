/****************************************************************************************************
 * @file           : DHT11.c
 * @author         : Maximiliano Mancilla
 * @date           : Sep 12, 2026
 * @brief          : DHT11 driver using the project timer and PB4 as the data line.
 *                   Correct protocol interpretation: start pulse + 80us response + 40 data bits.
 ****************************************************************************************************/

#include "DHT11.h"
#include "Gpt.h"
#include "stm32l4xx.h"
#include "Uart.h"

#define DHT11_PORT          GPIOB
#define DHT11_PIN_MASK      (1U << DHT11_PIN_POS)

extern uint32_t SystemCoreClock;

static void DHT11_Pin_Output(void)
{
    /* Configure PBx as push-pull output for driving the DHT11 data line. */
    DHT11_PORT->MODER &= ~(3U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->MODER |=  (1U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->OTYPER |=  (1U << DHT11_PIN_POS);
    DHT11_PORT->PUPDR &= ~(3U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->PUPDR |=  (1U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->OSPEEDR &= ~(3U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->OSPEEDR |=  (2U << (DHT11_PIN_POS * 2U));
}

static void DHT11_Pin_Input(void)
{
    /* Reconfigure PBx as input with pull-up to read the sensor response. */
    DHT11_PORT->MODER &= ~(3U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->OTYPER |=  (1U << DHT11_PIN_POS);
    DHT11_PORT->PUPDR &= ~(3U << (DHT11_PIN_POS * 2U));
    DHT11_PORT->PUPDR |=  (1U << (DHT11_PIN_POS * 2U));
}

static void DHT11_SetLow(void)
{
    /* Drive the data line low for the DHT11 start pulse. */
    DHT11_Pin_Output();
    DHT11_PORT->ODR &= ~(1U << DHT11_PIN_POS);
}

static void DHT11_SetHigh(void)
{
    /* Release the line so the sensor can pull it high/low. */
    DHT11_Pin_Input();
}

static void DHT11_Timer1us_Init(void)
{
    /* TIM2 is used as a 1 us time base for DHT11 timing measurements. */
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = (uint16_t)((SystemCoreClock / 1000000U) - 1U);
    TIM2->ARR = 0xFFFFU;
    TIM2->CNT = 0U;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

static void DHT11_DelayUs(uint32_t us)
{
    /* Delay using TIM2 to create a microsecond-scale waiting window. */
    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < us)
    {
    }
}

void DHT11_Init(void)
{
    /* Enable GPIOB clock and put the data pin in a safe idle state. */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    DHT11_Pin_Output();
    DHT11_SetHigh();
    DHT11_Timer1us_Init();
}

uint8_t DHT11_Read(DHT11_DataType *data)
{
    /* DHT11 sends 5 bytes: RH integer, RH decimal, T integer, T decimal, checksum. */
    uint8_t raw[5] = {0U};
    uint8_t byteIndex;
    uint8_t bitIndex;
    uint8_t bitValue;
    uint8_t checksum;
    uint8_t computedChecksum;
    uint32_t timeout;

    if (data == 0U)
    {
        return 0U;
    }

    data->Humidity = 0U;
    data->Temperature = 0U;
    data->Checksum = 0U;

    /* Step 1: Generate the DHT11 start pulse.
     * MCU pulls DATA LOW for 18 ms and then releases it. */
    DHT11_SetLow();
    SystickDelay_Ms(18U);
    DHT11_SetHigh();
    DHT11_DelayUs(20U);

    /* Step 2: Switch to input mode to let the DHT11 respond. */
    DHT11_Pin_Input();

    /* The sensor acknowledges by pulling the line LOW for ~80 us. */
    timeout = 1000U;
    while ((DHT11_PORT->IDR & DHT11_PIN_MASK) != 0U)
    {
        if (--timeout == 0U)
        {
            return 0U;
        }
    }

    /* After that, the sensor releases the line and drives it HIGH for ~80 us. */
    DHT11_DelayUs(80U);

    if ((DHT11_PORT->IDR & DHT11_PIN_MASK) == 0U)
    {
        return 0U;
    }

    /* Now read 40 data bits. Each bit begins with a LOW pulse and then a HIGH pulse.
     * If the HIGH pulse is longer than 40 us, it represents a logical 1; otherwise 0. */
    timeout = 1000U;
    while ((DHT11_PORT->IDR & DHT11_PIN_MASK) != 0U)
    {
        if (--timeout == 0U)
        {
            return 0U;
        }
    }

    for (byteIndex = 0U; byteIndex < 5U; byteIndex++)
    {
        raw[byteIndex] = 0U;
        for (bitIndex = 0U; bitIndex < 8U; bitIndex++)
        {
            timeout = 1000U;
            while ((DHT11_PORT->IDR & DHT11_PIN_MASK) == 0U)
            {
                if (--timeout == 0U)
                {
                    return 0U;
                }
            }

            /* Measure the width of the HIGH pulse, which determines the bit value. */
            DHT11_DelayUs(40U);

            bitValue = ((DHT11_PORT->IDR & DHT11_PIN_MASK) != 0U) ? 1U : 0U;
            raw[byteIndex] = (uint8_t)((raw[byteIndex] << 1U) | bitValue);

            timeout = 1000U;
            while ((DHT11_PORT->IDR & DHT11_PIN_MASK) != 0U)
            {
                if (--timeout == 0U)
                {
                    return 0U;
                }
            }
        }
    }

    checksum = raw[4];
    computedChecksum = (uint8_t)(raw[0] + raw[1] + raw[2] + raw[3]);

    /* DHT11 checksum: sum of the first four bytes must match the fifth byte. */
    if (checksum != computedChecksum)
    {
        return 0U;
    }

    data->Humidity = raw[0];
    data->Temperature = raw[2];
    data->Checksum = raw[4];

    return 1U;
}
