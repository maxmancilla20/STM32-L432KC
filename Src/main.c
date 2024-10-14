/*
 * Maximiliano Mancilla
 * ============================= DATA ======================================
 * User LD3: the green LED is a user LED connected to Arduino Nano signal D13
 * corresponding to the STM32 I/O PB3 (pin 26).
 * | AHB2 | 0x4800 0400 - 0x4800 07FF | 1 KB | GPIOB
 * ==========================================================================
 * */
#include <stdint.h>

#define PERIPH_BASE 		(0x40000000UL)
/*CLOCK BUS ADDRESS, BASED ON THE BLOCK DIAGRAM*/
#define AHB1PERIPH_OFFSET	(0x00020000UL)
#define AHB2PERIPH_OFFSET	(0x08000000UL)
#define AHB2PERIPH_BASE 	(PERIPH_BASE + AHB2PERIPH_OFFSET)
#define AHB1PERIPH_BASE 	(PERIPH_BASE + AHB1PERIPH_OFFSET)
/* GPIOS BASE ADDRESSES, BASED ON MEMORY MAP */
#define GPIOB_OFFSET 		(0x0400UL)
#define GPIOA_OFFSET 		(0x0000UL)
#define GPIOB_BASE 			(AHB2PERIPH_BASE + GPIOB_OFFSET)
#define GPIOA_BASE 			(AHB1PERIPH_BASE + GPIOA_OFFSET)
/* RCC BASE, PLACED ON AHB1 CLOCK BUS, THIS IS NEEDED TO ENABLE THE CLOCK BUSES*/
#define RCC_OFFSET			(0x1000UL)
#define RCC_BASE			(AHB1PERIPH_BASE + RCC_OFFSET)
#define AHB2ENR_OFFSET      (0x4CUL)
#define AHB1ENR_OFFSET      (0x48UL)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)
/* "DIRECTION REGISTER" (MODER), USED TO CONFIGURE THE PIN MODE */
#define MODER_OFFSET        (0x0UL)
/*FOR CONFIGURE PB3, WE WILL MODIFY MODER3*/
#define PB3_OUTPUT          ((1U<<6) | (~(1U<<7)))   
/* CONFIGURE THE DATA REGISTER, ODR(output) or IDR(input)*/     
#define GPIOX_ODR_OFFSET    (0x14UL)
#define PIN3                (1U<<3) 
#define USER_LED_PIN        (PIN3)

#define __IO volatile

typedef struct /*The house of GPIO*/
{
    __IO uint32_t MODER;   /* GPIO mode register                        Offset: 0x00*/
    __IO uint32_t OTYPER;  /* GPIO output type register                 Offset: 0x04*/
    __IO uint32_t OSPEEDR; /* GPIO output speed register                Offset: 0x08*/
    __IO uint32_t PUPDR;   /* GPIO pull-up/pull-down register           Offset: 0x0C*/
    __IO uint32_t IDR;     /* GPIO input data register                  Offset: 0x10*/
    __IO uint32_t ODR;     /* GPIO output data register                 Offset: 0x14*/
    __IO uint32_t BSRR;    /* GPIO bit set/reset register               Offset: 0x18*/
    __IO uint32_t LCKR;    /* GPIO configuration lock register          Offset: 0x1C*/
    __IO uint32_t AFRL;    /* GPIO alternate function low register      Offset: 0x20*/
    __IO uint32_t AFRH;    /* GPIO alternate function high register     Offset: 0x24*/
    __IO uint32_t BRR;     /* GPIO port bit reset register              Offset: 0x28*/
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t CR;           // Clock control register                          Offset: 0x00
    __IO uint32_t ICSCR;        // Internal clock sources calibration register     Offset: 0x04
    __IO uint32_t CFGR;         // Clock configuration register                    Offset: 0x08
    __IO uint32_t PLLCFGR;      // PLL configuration register                      Offset: 0x0C
    __IO uint32_t PLLSAI1CFGR;  // PLL SAI1 configuration register                 Offset: 0x10
    __IO uint32_t PLLSAI2CFGR;  // PLL SAI2 configuration register                 Offset: 0x14
    __IO uint32_t CIER;         // Clock interrupt enable register                 Offset: 0x18
    __IO uint32_t CIFR;         // Clock interrupt flag register                   Offset: 0x1C
    __IO uint32_t CICR;         // Clock interrupt clear register                  Offset: 0x20
    uint32_t      RESERVED0;    // Reserved                                        Ofsset: 0x24
    __IO uint32_t AHB1RSTR;     // AHB1 peripheral reset register                  Offset: 0x28
    __IO uint32_t AHB2RSTR;     // AHB2 peripheral reset register                  Offset: 0x2C
    __IO uint32_t AHB3RSTR;     // AHB3 peripheral reset register                  Offset: 0x30
    uint32_t      RESERVED1;    // Reserved                                        Offset: 0x34
    __IO uint32_t APB1RSTR1;    // APB1 peripheral reset register 1                Offset: 0x38
    __IO uint32_t APB1RSTR2;    // APB1 peripheral reset register 2                Offset: 0x3C
    __IO uint32_t APB2RSTR;     // APB2 peripheral reset register                  Offset: 0x40
    uint32_t      RESERVED2;    // Reserved                                        Offset: 0x44
    __IO uint32_t AHB1ENR;      // AHB1 peripheral clock enable register           Offset: 0x48
    __IO uint32_t AHB2ENR;      // AHB2 peripheral clock enable register           Offset: 0x4C
    __IO uint32_t AHB3ENR;      // AHB3 peripheral clock enable register           Offset: 0x50
    uint32_t      RESERVED3;    // Reserved                                        Offset: 0x54
    __IO uint32_t APB1ENR1;     // APB1 peripheral clock enable register 1         Offset: 0x58
    __IO uint32_t APB1ENR2;     // APB1 peripheral clock enable register 2         Offset: 0x5C
    __IO uint32_t APB2ENR;      // APB2 peripheral clock enable register           Offset: 0x60
    __IO uint32_t AHB1SMENR;    // AHB1 peripheral clocks enable in Sleep/Stop     Offset: 0x68
    __IO uint32_t AHB2SMENR;    // AHB2 peripheral clocks enable in Sleep/Stop     Offset: 0x6C
    __IO uint32_t AHB3SMENR;    // AHB3 peripheral clocks enable in Sleep/Stop     Offset: 0x70
    uint32_t      RESERVED4;    // Reserved                                        Offset: 0x74
    __IO uint32_t APB1SMENR1;   // APB1 peripheral clocks enable in Sleep/Stop 1   Offset: 0x78
    __IO uint32_t APB1SMENR2;   // APB1 peripheral clocks enable in Sleep/Stop 2   Offset: 0x7C
    __IO uint32_t APB2SMENR;    // APB2 peripheral clocks enable in Sleep/Stop     Offset: 0x80
    __IO uint32_t CCIPR;        // Peripherals independent clock config register   Offset: 0x88
    __IO uint32_t BDCR;         // Backup domain control register                  Offset: 0x90
    __IO uint32_t CSR;          // Control/status register                         Offset: 0x94
    __IO uint32_t CRRCR;        // Clock recovery RC register                      Offset: 0x98
    __IO uint32_t CCIPR2;       // Peripherals independent clock config register 2 Offset: 0x9C
    __IO uint32_t DLYCFGR;      // OCTOSPI delay configuration register            Offset: 0xA0
} RCC_TypeDef;

/*To use those Typedef, we need to typecast the base address*/
#define RCC     ((RCC_TypeDef*) RCC_BASE)
#define GPIOA   ((GPIO_TypeDef*) GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef*) GPIOB_BASE)

int main (void)
{
    /* Enable clock access to GPIOB*/
    RCC->AHB2ENR |= GPIOBEN;
    /* Set PB3 AS OUTPUT*/
    GPIOB->MODER |= (1U<<6);
    GPIOB->MODER &=~ (1U<<7);
    

   while(1)
   {
        /*Make it toogle*/
        GPIOB->ODR ^= USER_LED_PIN;

        for(int i = 0; i < 100000; i++){}
   }

    return 0;
}
