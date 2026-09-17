# STM32L432KC - Current project pinout

> This pinout reflects the active configuration used in the project at the time of the current debugging and sensor integration work. It is intentionally practical and hardware-oriented, not a generic board map.

## Active MCU assignments

| Pin | Function | Status |
| --- | --- | --- |
| PA0 | ADC input | <span style="color: red;">Used</span> |
| PA2 | UART2_TX | <span style="color: red;">Used</span> |
| PA3 | UART2_RX | <span style="color: red;">Used</span> |
| PA4 | SPI1_NSS / CS manual | <span style="color: red;">Used</span> |
| PA5 | SPI1_SCK | <span style="color: red;">Used</span> |
| PA6 | SPI1_MISO | <span style="color: red;">Used</span> |
| PA7 | SPI1_MOSI | <span style="color: red;">Used</span> |
| PA9 | I2C1_SCL | <span style="color: red;">Used</span> |
| PA10 | I2C1_SDA | <span style="color: red;">Used</span> |
| PA11 | CAN_RX | <span style="color: red;">Used</span> |
| PA12 | CAN_TX | <span style="color: red;">Used</span> |
| PB0 | RC522 reset / GPIO control | <span style="color: red;">Used</span> |
| PB1 | General GPIO / alternate USART3_RTS_DE | <span style="color: green;">Free</span> |
| PB4 | DHT11 DATA line | <span style="color: red;">Used</span> |
| PB6 | Reserved for I2C alternate usage | <span style="color: green;">Free</span> |
| PB7 | Reserved for I2C alternate usage | <span style="color: green;">Free</span> |
| PB3 | User GPIO / LED-related usage | <span style="color: green;">Free</span> |
| PB5 | General GPIO | <span style="color: green;">Free</span> |
| PB8 | General GPIO | <span style="color: green;">Free</span> |
| PB9 | General GPIO | <span style="color: green;">Free</span> |
| PA8 | PWM TIM1 function | <span style="color: red;">Used</span> |
| PA15 | General GPIO / alternate function | <span style="color: green;">Free</span> |

## Peripheral mapping

![Peripheral summary](stm32l432kc_pinout_proyect.png)

| Peripheral | MCU pins | Notes |
| --- | --- | --- |
| ADC | PA0 | ADC input used for project testing |
| I2C1 | PA9, PA10 | BMP180 connected here |
| UART2 | PA2, PA3 | Serial debug / printf |
| SPI1 | PA4, PA5, PA6, PA7 | Used for SPI peripheral work and RC522 experiments |
| DHT11 | PB4 | Data line for DHT11 sensor |
| CAN | PA11, PA12 | CAN RX/TX |

## Sensor-specific wiring currently in use

| Sensor / Module | Pin used | Notes |
| --- | --- | --- |
| BMP180 | PA9 = SCL, PA10 = SDA | I2C1; mounted on the board and working |
| DHT11 | PB4 | Data pin; 1-wire protocol |
| SPI device | PA4-PA7 | SPI1 bus on MCU |
| UART debug | PA2-PA3 | UART2 for serial log |

## Important warnings

- The DHT11 data line is not shared with UART2. PB4 is intentionally selected to avoid PA2/PA3 conflict.
- The project has multiple configurations for I2C and SPI, but the current active mapping is:
  - I2C1 on PA9/PA10
  - SPI1 on PA4/PA5/PA6/PA7
- If a different peripheral is enabled on the same pins, it must be reconfigured before reuse.
- GPIOs such as PB0, PB3, PB4, PB5, PB8, and PB9 are convenient for lab experiments, but they should be assigned intentionally to avoid collisions.

## Recommended board reference

The current practical pin map for this project is:

- BMP180: PA9 / PA10 (I2C1)
- DHT11: PB4
- SPI bus: PA4 / PA5 / PA6 / PA7
- UART debug: PA2 / PA3
- RC522 reset/control: PB0
