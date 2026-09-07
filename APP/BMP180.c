#include "BMP180.h"
#include <math.h>

#define BMP180_ADDRESS             (0x77U)
#define BMP180_REG_ID              (0xD0U)
#define BMP180_REG_CONTROL         (0xF4U)
#define BMP180_REG_DATA            (0xF6U)
#define BMP180_CMD_TEMPERATURE     (0x2EU)
#define BMP180_CMD_PRESSURE        (0x34U)
#define BMP180_MODE_STANDARD       (0U)
#define BMP180_REFERENCE_PRESSURE  (102560.0F)
#define BMP180_ALTITUDE_EXPONENT   (0.19029495F)

static int16_t calibrationAc1;
static int16_t calibrationAc2;
static int16_t calibrationAc3;
static uint16_t calibrationAc4;
static uint16_t calibrationAc5;
static uint16_t calibrationAc6;
static int16_t calibrationB1;
static int16_t calibrationB2;
static int16_t calibrationMb;
static int16_t calibrationMc;
static int16_t calibrationMd;
static int32_t calibrationB5;
static uint8_t bmp180Initialized;

static float bmp180_pressure_to_altitude(uint32_t pressurePa)
{
    return 44330.0F *
           (1.0F - powf((float)pressurePa / BMP180_REFERENCE_PRESSURE,
                        BMP180_ALTITUDE_EXPONENT));
}

static uint16_t bmp180_read_u16(uint8_t registerAddress)
{
    uint8_t buffer[2];

    I2c_burstRead(BMP180_ADDRESS, registerAddress, 2U, buffer);
    return ((uint16_t)buffer[0] << 8U) | buffer[1];
}

static int16_t bmp180_read_s16(uint8_t registerAddress)
{
    return (int16_t)bmp180_read_u16(registerAddress);
}

static uint32_t bmp180_read_pressure(uint8_t oversampling)
{
    uint8_t buffer[3];
    uint32_t rawPressure;
    int32_t x1;
    int32_t x2;
    int32_t x3;
    int32_t b3;
    int32_t b6;
    int32_t b7;
    int32_t pressure;
    uint32_t divisor;

    I2c_burstWrite(BMP180_ADDRESS, BMP180_REG_CONTROL, 1U,
                   (const uint8_t[]){(uint8_t)(BMP180_CMD_PRESSURE + (oversampling << 6U))});

    for (volatile uint32_t delay = 0U; delay < 30000U; delay++)
    {
        /* Conversion delay for standard pressure mode */
    }

    I2c_burstRead(BMP180_ADDRESS, BMP180_REG_DATA, 3U, buffer);
    rawPressure = (((uint32_t)buffer[0] << 16U) |
                   ((uint32_t)buffer[1] << 8U) |
                   buffer[2]) >> (8U - oversampling);

    b6 = calibrationB5 - 4000;
    x1 = (calibrationB2 * ((b6 * b6) >> 12)) >> 11;
    x2 = (calibrationAc2 * b6) >> 13;
    x3 = x1 + x2;
    b3 = ((((int32_t)calibrationAc1 * 4 + x3) << oversampling) + 2) >> 2;
    x1 = (calibrationAc3 * b6) >> 13;
    x2 = (calibrationB1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    divisor = (uint32_t)(((calibrationAc4 * (uint32_t)(x3 + 32768)) >> 15));
    b7 = ((uint32_t)rawPressure - (uint32_t)b3) * (50000U >> oversampling);

    if (b7 < 0x80000000U)
    {
        pressure = (int32_t)((b7 * 2U) / divisor);
    }
    else
    {
        pressure = (int32_t)((b7 / divisor) * 2U);
    }

    x1 = (pressure >> 8) * (pressure >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * pressure) >> 16;
    return (uint32_t)(pressure + ((x1 + x2 + 3791) >> 4));
}


void BMP180_Init(void)
{
    calibrationAc1 = bmp180_read_s16(0xAAU);
    calibrationAc2 = bmp180_read_s16(0xACU);
    calibrationAc3 = bmp180_read_s16(0xAEU);
    calibrationAc4 = bmp180_read_u16(0xB0U);
    calibrationAc5 = bmp180_read_u16(0xB2U);
    calibrationAc6 = bmp180_read_u16(0xB4U);
    calibrationB1 = bmp180_read_s16(0xB6U);
    calibrationB2 = bmp180_read_s16(0xB8U);
    calibrationMb = bmp180_read_s16(0xBAU);
    calibrationMc = bmp180_read_s16(0xBCU);
    calibrationMd = bmp180_read_s16(0xBEU);
    bmp180Initialized = 1U;
}

uint8_t BMP180_ReadId(void)
{
    return BMP180_ReadByte(BMP180_REG_ID);
}

uint8_t BMP180_ReadByte(uint8_t registerAddress)
{
    uint8_t data = 0U;

    I2c_burstRead(BMP180_ADDRESS, registerAddress, 1U, &data);
    return data;
}

uint8_t BMP180_ReadMeasurement(BMP180_DataType *data)
{
    uint8_t temperatureBuffer[2];
    uint16_t rawTemperature;
    int32_t x1;
    int32_t x2;

    if ((data == 0) || (bmp180Initialized == 0U))
    {
        return 0U;
    }

    I2c_burstWrite(BMP180_ADDRESS, BMP180_REG_CONTROL, 1U,
                   (const uint8_t[]){BMP180_CMD_TEMPERATURE});

    for (volatile uint32_t delay = 0U; delay < 10000U; delay++)
    {
        /* Conversion delay for temperature */
    }

    I2c_burstRead(BMP180_ADDRESS, BMP180_REG_DATA, 2U, temperatureBuffer);
    rawTemperature = ((uint16_t)temperatureBuffer[0] << 8U) | temperatureBuffer[1];

    x1 = (((int32_t)rawTemperature - calibrationAc6) * calibrationAc5) >> 15;
    x2 = ((int32_t)calibrationMc << 11) / (x1 + calibrationMd);
    calibrationB5 = x1 + x2;
    data->TemperatureC = (float)((calibrationB5 + 8) >> 4) / 10.0F;
    data->PressurePa = bmp180_read_pressure(BMP180_MODE_STANDARD);
    data->AltitudeM = bmp180_pressure_to_altitude(data->PressurePa);

    return 1U;
}

