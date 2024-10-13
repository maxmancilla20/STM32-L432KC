/*
 * Maximiliano Mancilla
 * ============================= DATA ======================================
 * User LD3: the green LED is a user LED connected to Arduino Nano signal D13
 * corresponding to the STM32 I/O PB3 (pin 26).
 * | AHB2 | 0x4800 0400 - 0x4800 07FF | 1 KB | GPIOB
 * ==========================================================================
 * */

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
#define RCC_AHB2ENR         (*(volatile unsigned int *) (RCC_BASE + AHB2ENR_OFFSET))/*To use the registers properly, we need to cast them volatile*/
#define RCC_AHB1ENR         (*(volatile unsigned int *) (RCC_BASE + AHB1ENR_OFFSET))/*and pointers, and delete the reference to the pointer*/
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)
/* "DIRECTION REGISTER" (MODER), USED TO CONFIGURE THE PIN MODE */
#define MODER_OFFSET        (0x0UL)
#define GPIOA_MODER         (*(volatile unsigned int *) (GPIOA_BASE + MODER_OFFSET))
#define GPIOB_MODER         (*(volatile unsigned int *) (GPIOB_BASE + MODER_OFFSET))
/*FOR CONFIGURE PB3, WE WILL MODIFY MODER3*/
#define PB3_OUTPUT          ((1U<<6) | (~(1U<<7)))   
/* CONFIGURE THE DATA REGISTER, ODR(output) or IDR(input)*/     
#define GPIOX_ODR_OFFSET    (0x14UL)
#define GPIOA_ODR           (*(volatile unsigned int *) (GPIOA_BASE + GPIOX_ODR_OFFSET))
#define GPIOB_ODR           (*(volatile unsigned int *) (GPIOB_BASE + GPIOX_ODR_OFFSET))
#define PIN3                (1U<<3) 
#define USER_LED_PIN        (PIN3)


int main (void)
{
    /* Enable clock access to GPIOB*/
    RCC_AHB2ENR |= GPIOBEN;
    /* Set PB3 AS OUTPUT*/
    GPIOB_MODER |= (1U<<6);
    GPIOB_MODER &=~ (1U<<7);

   while(1)
   {
        /*Set PB3 HIGH*/
        //GPIOB_ODR |= USER_LED_PIN;

        /*Make it toogle*/
        GPIOB_ODR ^= USER_LED_PIN;

        for(int i = 0; i < 100000; i++){}

   }

    return 0;
}
