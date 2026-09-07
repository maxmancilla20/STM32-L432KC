# STM32L432KC Bare-Metal Drivers

A bare-metal embedded software project focused on developing low-level drivers for the **STM32L432KC** microcontroller.

The project is structured following an automotive-oriented **Basic Software / MCAL architecture**, with the objective of implementing peripheral drivers directly at register level and minimizing dependencies on high-level frameworks.

The main goal is to understand and implement the STM32L432KC peripherals from the hardware level up, including initialization, register configuration, interrupt handling, peripheral communication, and hardware abstraction.

---

## Project Status

### Driver Development

| Driver / Module |   Status  |
| --------------- | :-------: |
| GPIO / DIO      |   ✅ Done  |
| USART           |   ✅ Done  |
| ADC             |   ✅ Done  |
| SysTick         |   ✅ Done  |
| Timers / GPT    |   ✅ Done  |
| CAN             |   ✅ Done  |
| I2C             |   ✅ Done  |
| LIN             | ⬜ Missing |
| SPI             | ⬜ Missing |

### Additional Features

| Feature    | Status |
| ---------- | :----: |
| Bootloader | ✅ Done |
| FreeRTOS   | ✅ Done |

> The driver list will continue to grow as additional STM32L432KC peripherals and low-level features are implemented.

---

## Architecture

The project follows a layered structure inspired by automotive embedded software architectures:

```text
STM32-L432KC
│
├── APP
│   └── Application layer
│
├── BSW
│   └── MCAL
│       ├── Adc
│       ├── Can
│       ├── Dio
│       ├── Gpt
│       ├── Lin
│       ├── Mcu
│       └── Uart
│
├── Inc
│   ├── chip_headers
│   │   └── CMSIS
│   └── Std_Types.h
│
├── Startup
│   └── Startup / interrupt vector configuration
│
└── STM32L432KCUX_FLASH.ld
    └── Linker script
```

The repository currently separates the **application layer (`APP`)** from the **Basic Software / MCAL layer (`BSW/MCAL`)**, making it easier to develop and test peripherals independently.

---

## Hardware

### Microcontroller

**STM32L432KC**

* ARM Cortex-M4
* Up to 80 MHz CPU frequency
* 256 KB Flash
* 64 KB SRAM
* STM32L4 low-power architecture

The STM32L432KC is a Cortex-M4 based microcontroller with 256 KB Flash and 64 KB SRAM.

### Development Board

**NUCLEO-L432KC**

The project targets the STM32L432KC device used on the NUCLEO-L432KC development platform.

---

## Implemented Drivers

### GPIO / DIO

Low-level digital input/output driver responsible for:

* GPIO initialization
* Digital input configuration
* Digital output configuration
* Pin state control
* Register-level GPIO access

**Status:** ✅ Done

---

### USART

Low-level serial communication driver.

The USART driver is intended to provide the basic functionality required for serial communication and debugging without relying on high-level middleware.

**Status:** ✅ Done

---

### ADC

Analog-to-Digital Converter driver.

The ADC implementation provides low-level access to the MCU's analog conversion capabilities.

**Status:** ✅ Done

---

### SysTick

System timing functionality based on the Cortex-M SysTick peripheral.

The SysTick functionality is used as a fundamental time base for delays and software timing mechanisms.

**Status:** ✅ Done

---

### Timers / GPT

General Purpose Timer functionality.

The timer driver provides the foundation for hardware-based timing and periodic events.

**Status:** ✅ Done

---

### CAN

Low-level Controller Area Network driver.

The CAN implementation is particularly relevant to the automotive focus of this project and provides the basis for communication between ECUs.

**Status:** ✅ Done

---

### I2C

Low-level Inter-Integrated Circuit communication driver.

The driver provides register-level control of the I2C peripheral for communication with external devices.

**Status:** ✅ Done

---

## Drivers Under Development

### LIN

Local Interconnect Network driver.

**Status:** ⬜ Missing

Planned functionality includes:

* LIN initialization
* Master / slave configuration
* Header generation
* Break field handling
* Synchronization
* Frame transmission
* Frame reception

---

### SPI

Serial Peripheral Interface driver.

**Status:** ⬜ Missing

Planned functionality includes:

* SPI initialization
* Clock configuration
* Master communication
* Full-duplex transmission
* Full-duplex reception
* Chip-select handling
* Interrupt-based communication

---

## Additional Projects

### Bootloader

A bootloader implementation is included as part of the project.

**Status:** ✅ Done

The bootloader work provides practical experience with:

* MCU startup
* Flash memory handling
* Application programming
* Firmware execution flow
* Linker configuration
* Memory layout

---

### FreeRTOS

The repository also includes work with **FreeRTOS** on the STM32L432KC.

**Status:** ✅ Done

This provides a comparison between:

```text
Bare Metal
    │
    ├── Direct register access
    ├── Interrupts
    ├── SysTick
    └── Super-loop / scheduler
```

and:

```text
FreeRTOS
    │
    ├── Tasks
    ├── Scheduling
    ├── Queues
    ├── Semaphores
    └── Interrupt synchronization
```

The project therefore covers both **low-level bare-metal development** and **RTOS-based embedded software**.

---

## Development Approach

The main objective is to avoid hiding the hardware behind high-level frameworks.

The drivers are developed around:

* Memory-mapped registers
* Peripheral clock configuration
* Register bit manipulation
* Interrupt configuration
* NVIC
* Peripheral initialization
* Hardware state management
* Direct MCU peripheral control

This approach provides a deeper understanding of how the STM32L4 peripherals operate internally.

---

## Automotive-Oriented Structure

One of the goals of this project is to apply concepts commonly found in automotive embedded software.

The structure follows a simplified architecture:

```text
┌─────────────────────────────┐
│       Application           │
│            APP              │
├─────────────────────────────┤
│       Basic Software        │
│            BSW              │
├─────────────────────────────┤
│            MCAL             │
│                             │
│ GPIO | ADC | CAN | UART     │
│ I2C  | GPT | MCU | ...      │
├─────────────────────────────┤
│       STM32L432KC HW        │
└─────────────────────────────┘
```

This organization is intended to make the project easier to extend and closer to the concepts used in automotive embedded architectures.

---

## Tools

The project is developed using:

* **STM32L432KC**
* **STM32CubeIDE**
* **Embedded C**
* **ARM Cortex-M4**
* **Git / GitHub**
* **FreeRTOS**
* **CMSIS device headers**

The repository contains the MCU startup code, linker script, application layer and MCAL-oriented driver structure.

---

## Learning Objectives

This project is being developed to strengthen practical knowledge in:

* Embedded C
* ARM Cortex-M architecture
* STM32 peripheral registers
* Bare-metal programming
* Interrupt-driven programming
* Peripheral drivers
* MCU initialization
* Memory mapping
* Linker scripts
* Bootloaders
* CAN communication
* RTOS concepts
* Automotive software architecture
* MCAL concepts

---

## Roadmap

### Completed

* [x] GPIO / DIO
* [x] USART
* [x] ADC
* [x] SysTick
* [x] Timers / GPT
* [x] CAN
* [x] I2C
* [x] Bootloader
* [x] FreeRTOS

### Planned

* [ ] LIN Driver
* [ ] SPI Driver
* [ ] DMA
* [ ] PWM
* [ ] RTC
* [ ] Watchdog
* [ ] Low-power modes
* [ ] More comprehensive driver tests
* [ ] Driver documentation
* [ ] Unit testing

---

## Repository Structure

```text
STM32-L432KC/
│
├── APP/
│   ├── main.c
│   ├── syscalls.c
│   └── sysmem.c
│
├── BSW/
│   └── MCAL/
│       ├── Adc/
│       ├── Can/
│       ├── Dio/
│       ├── Gpt/
│       ├── Lin/
│       ├── Mcu/
│       └── Uart/
│
├── Inc/
│   ├── chip_headers/
│   │   └── CMSIS/
│   └── Std_Types.h
│
├── Startup/
│
├── Debug/
│
├── STM32L432KCUX_FLASH.ld
│
└── README.md
```

---

## Author

**Maximiliano Mancilla**

Embedded Software Engineer
Automotive Embedded Systems | AUTOSAR | Bare Metal | FreeRTOS | MATLAB/Simulink

[GitHub](https://github.com/maxmancilla20)

[LinkedIn](https://www.linkedin.com/in/mmancilla20)
