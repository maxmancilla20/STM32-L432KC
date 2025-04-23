/*
 * Maximiliano Mancilla
 * ============================= DATA ======================================
 * User LD3: the green LED is a user LED connected to Arduino Nano signal D13
 * corresponding to the STM32 I/O PB3 (pin 26).
 * | AHB2 | 0x4800 0400 - 0x4800 07FF | 1 KB | GPIOB
 * ==========================================================================
 * */
#include <stdint.h>
#include <stdio.h>
#include "stm32l4xx.h"
#include "Uart.h"
#include "GPIO.h"

#define PIN3                (1U<<3) 
#define USER_LED_PIN        (PIN3)
#define PIN7                (1U<<7)
#define GPIOAEN             (1U<<0)
#define GPIOBEN             (1U<<1)


/*Modules:
 * FPU
 * UART
 * TIMEBASE
 * GPIO (BSP)
 * ADC
 * */



#define  GPIOAEN		(1U<<0)
#define  PIN5			(1U<<5)
#define  LED_PIN		PIN5

#define BOOT_FUNC __attribute__((section(".boot_flash_block")))
#define APPLICATION_ADDRESS         0x08008000
#define MSP_VERIFY_MASK            0x2FFE0000
#define EMPTY_FLASH                0xFFFFFFFF

/*
#define VECT_TAB_BASE_ADDRESS           FLASH_BASE
#define VECT_TAB_OFFSET                 0x8000
*/


typedef void (*func_ptr)(void);
struct Boot_Common_Apis
{
    void (*GPIO_Init)(void);
    void (*GPIO_Toggle)(void);
    void (*GPIO_On)(void);
    void (*GPIO_Off)(void);
    void (*Uart2_RXTX_Init)(void);
    char (*read_uart2)(void);

};
struct Boot_Common_Apis Common_Apis __attribute__((section(".COMMON_CODE"))) = {
    GPIO_Init,
    GPIO_Toggle,
    GPIO_On,
    GPIO_Off,
    Uart2_RXTX_Init,
    read_uart2,
};

#define MEM_VERIFY_V2

void JmpToApplication(void)
{
    uint32_t app_start_address;
    func_ptr jump_to_app;

    printf("Bootloader Started...\n\r");
    for(int i = 0; i < 100000; i++){} /* Delay */
    for(int i = 0; i < 100000; i++){} /* Delay */
     
    /* Version 1 of validation */
#ifndef MEM_VERIFY_V1
    if(((*(uint32_t *)APPLICATION_ADDRESS) & MSP_VERIFY_MASK) == 0x20000000)
#endif

    /* Version 2 of validation */
#ifndef MEM_VERIFY_V2
    if((*(uint32_t *)APPLICATION_ADDRESS) != EMPTY_FLASH)
#endif
    {
        printf("MSP is valid, starting application...\n\r");
        app_start_address = *(uint32_t *)(APPLICATION_ADDRESS + 4);
        jump_to_app = (func_ptr)(app_start_address);
    
        /* Initialize the main stack pointer */
        __set_MSP(*(uint32_t *)APPLICATION_ADDRESS);
    
        /*Jump to the application */
        jump_to_app();

    }
    else
    {
        printf("No valid application found...\n\r");
        for(int i = 0; i < 100000; i++){} /* Delay */
        return;
    }

}

void BOOT_FUNC _bootloader(void)
{

    GPIOB->ODR ^= USER_LED_PIN;  /* Turn on the LED */
    for(int i = 0; i < 100000; i++){} /* Delay */
    
}

int main(void)
{
  //  /* Enable clock access to GPIOB */
  //  RCC->AHB2ENR |= GPIOBEN;
//
  //  /* Set PB3 AS OUTPUT MODE B7 = 0, B6 =  1*/
  //  GPIOB->MODER |= (1U<<6);
  //  GPIOB->MODER &=~ (1U<<7);
//
  //  /* Set PB7 as input B14 = 0, B15 = 0 */
  //  GPIOB->MODER &=~ (1U<<14);
  //  GPIOB->MODER &=~ (1U<<15);   
    // Initialization code here
    Uart2_RXTX_Init();

	JmpToApplication();
    while(1)
    {
        //GPIOB->ODR |= USER_LED_PIN;  /* Turn on the LED */
        //GPIOB->ODR &= ~USER_LED_PIN; /* Turn off the LED */
        //GPIOB->ODR ^= USER_LED_PIN;  /* Toggle the LED */
        //for(int i = 0; i < 100000; i++){} /* Delay */

        //_bootloader();
        //printf("Applciation 1 is running...\n");
        //for(int i = 0; i < 100000; i++){} /* Delay */
    }
    return 0;
}

/*
void SystemInit(void)
{
    SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
}
*/
