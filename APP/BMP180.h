#ifndef BMP180_H_
#define BMP180_H_

#include "../BSW/MCAL/I2c/I2c.h"
#include "../BSW/MCAL/I2c/I2c_Cfg.h"

typedef struct
{
	float TemperatureC;
	uint32_t PressurePa;
	float AltitudeM;
} BMP180_DataType;

void BMP180_Init(void);
uint8_t BMP180_ReadId(void);
uint8_t BMP180_ReadByte(uint8_t registerAddress);
uint8_t BMP180_ReadMeasurement(BMP180_DataType *data);

#endif /* BMP180_H_ */
