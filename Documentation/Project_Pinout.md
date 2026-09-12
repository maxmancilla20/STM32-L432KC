# STM32L432KC - Pin mapping summary

> This mapping reflects the current project configuration and the drivers already in use. Some pins have alternate functions and can be reused depending on the active peripheral.

| Free or Used | Pin | Current use / function |
| --- | --- | --- |
| <span style="color: red;">Used</span> | PA0 | ADC1_IN5 |
| <span style="color: red;">Used</span> | PA4 | SPI1_NSS / SPI1_SSEL |
| <span style="color: red;">Used</span> | PA5 | SPI1_SCK |
| <span style="color: red;">Used</span> | PA6 | SPI1_MISO |
| <span style="color: red;">Used</span> | PA7 | SPI1_MOSI |
| <span style="color: green;">Free</span> | PA8 | GPIO / PWM (not assigned in the base configuration) |
| <span style="color: red;">Used</span> | PA9 | UART1_TX / I2C1_SCL (alternate depending on usage) |
| <span style="color: red;">Used</span> | PA10 | UART1_RX / I2C1_SDA (alternate depending on usage) |
| <span style="color: red;">Used</span> | PA11 | CAN_RX |
| <span style="color: red;">Used</span> | PA12 | CAN_TX |
| <span style="color: green;">Free</span> | PA15 | GPIO / SPI1 alternate NSS available as AF |
| <span style="color: red;">Used</span> | PB0 | ADC1_IN15 |
| <span style="color: green;">Free</span> | PB1 | GPIO / PWM (general use) |
| <span style="color: green;">Free</span> | PB2 | GPIO free |
| <span style="color: green;">Free</span> | PB3 | GPIO general |
| <span style="color: green;">Free</span> | PB4 | GPIO general |
| <span style="color: green;">Free</span> | PB5 | GPIO general |
| <span style="color: red;">Used</span> | PB6 | I2C_SCL (documented in Dio_Cfg.h) |
| <span style="color: red;">Used</span> | PB7 | I2C_SDA (documented in Dio_Cfg.h) |
| <span style="color: green;">Free</span> | PB8 | GPIO general |
| <span style="color: green;">Free</span> | PC0 | GPIO / ADC possible (if used in another configuration) |
| <span style="color: green;">Free</span> | PC1 | GPIO / ADC possible |
| <span style="color: green;">Free</span> | PC2 | GPIO / ADC possible |
| <span style="color: green;">Free</span> | PC3 | GPIO / ADC possible |

## Peripheral summary

| Peripheral | Pins used |
| --- | --- |
| ADC | PA0, PB0 |
| SPI1 | PA4, PA5, PA6, PA7 |
| UART1 | PA9, PA10 |
| I2C1 | PA9, PA10 or PB6, PB7 (depending on implementation) |
| CAN | PA11, PA12 |
| GPIO | PA8, PA15, PB1, PB2, PB3, PB4, PB5, PB8 |

## Important note

The configuration in `Dio_Cfg.h` and the implementation in `I2c.c` mark certain functions slightly differently:

- `Dio_Cfg.h` documents `PB6/PB7` as I2C.
- `I2c.c` configures `PA9/PA10` as I2C1_SCL/SDA.

This means the project currently has a pin-conflict area depending on which implementation is active; it is best to define one group as active at a time.
