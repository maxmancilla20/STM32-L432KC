/****************************************************************************************************
 * @file           : DHT11.h
 * @author         : Maximiliano Mancilla
 * @date           : Sep 12, 2026
 * @brief          : DHT11 driver for STM32L432 using direct register access
 ****************************************************************************************************/

#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>

#define DHT11_PIN_POS     4U
#define DHT11_PIN_MASK    (1U << DHT11_PIN_POS)

typedef struct
{
    uint8_t Humidity;
    uint8_t Temperature;
    uint8_t Checksum;
} DHT11_DataType;

void DHT11_Init(void);
uint8_t DHT11_Read(DHT11_DataType *data);

#endif /* DHT11_H_ */
