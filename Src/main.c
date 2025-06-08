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

#define  PIN5			(1U<<5)
#define  LED_PIN		PIN5

#define BOOT_FUNC __attribute__((section(".boot_flash_block")))
#define MSP_VERIFY_MASK            0x2FFE0000
#define EMPTY_FLASH                0xFFFFFFFF

#define SECTOR0_BASE_ADDRESS      0x08000000 /* Bootloader */
#define SECTOR1_BASE_ADDRESS      0x08004000 /* Default Application */
#define SECTOR2_BASE_ADDRESS      0x08008000 /* APP 2 */
#define SECTOR3_BASE_ADDRESS      0x0800C000 /* Factory Application */

#define BOOTLOADER_ADDRESS         SECTOR0_BASE_ADDRESS
#define DEFAULT_APP_ADDRESS        SECTOR1_BASE_ADDRESS
#define APP2_ADDRESS               SECTOR2_BASE_ADDRESS
#define FACTORY_APP_ADDRESS        SECTOR3_BASE_ADDRESS

#define MEM_CHECK_V2    /* Enable memory check version 2 */


/* Global Variables */
volatile char g_app_key;
volatile uint8_t g_un_key;

typedef void (*func_ptr)(void);
struct Boot_Common_Apis
{
    void (*GPIO_Init)(void);
    void (*GPIO_Toggle)(void);
    void (*GPIO_On)(void);
    void (*GPIO_Off)(void);
    void (*Uart2_RXTX_Init)(void);
    char (*read_uart2)(void);
    uint8_t (*GPIO_Read)(void);
    void (*write_uart2)(uint8_t data);
};

struct Boot_Common_Apis Common_Apis __attribute__((section(".COMMON_APIS"))) = {
    GPIO_Init,
    GPIO_Toggle,
    GPIO_On,
    GPIO_Off,
    Uart2_RXTX_Init,
    read_uart2,
    GPIO_Read,
    write_uart2
};



typedef enum {
    APP2 = 1,
    FACTORY_APP,
} SYS_APPS;

/* Function Definitions */
static void process_btldr_cmds(SYS_APPS curr_app);

static void jump_to_app(uint32_t app_start_address)
{
    func_ptr jump_to_app;

     
    /* Version 1 of validation */
#ifndef MEM_CHECK_V1
    if(((*(uint32_t *)app_start_address) & MSP_VERIFY_MASK) == 0x20000000)
#endif

    /* Version 2 of validation */
#ifndef MEM_CHECK_V2
    if((*(uint32_t *)app_start_address) != EMPTY_FLASH)
#endif
    {
        printf("\n\rMSP is valid, starting application...\n\r");
        app_start_address = *(uint32_t *)(app_start_address + 4);
        jump_to_app = (func_ptr)(app_start_address);
    
        /* Initialize the main stack pointer */
        __set_MSP(*(uint32_t *)app_start_address);
    
        /*Jump to the application */
        jump_to_app();

    }
    else
    {
        printf("\n\r...No valid application found...\n\r");
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
  
    /* Hardware Initialization  Start */
    Uart2_RXTX_Init();
    GPIO_Init();
    /* Hardware Initialization End */

    if(GPIO_Read())
    {
        printf("\n\r=============================\n\r");
        printf("\n\r=====Button Pressed==========\n\r");
        printf("\n\r=============================\n\r");
        printf("\n\r");
        printf("\n\r=============================\n\r");
        printf("\n\r=Select App2 or Factory======\n\r");
        printf("\n\r=============================\n\r");
        printf("\n\r===Press '1' to App2=========\n\r"); 
        printf("\n\r=============================\n\r");
        printf("\n\r===Press '2' to Factory======\n\r");
        printf("\n\r=============================\n\r");
        printf("\n\r=Press any other for default=\n\r");
        printf("\n\r=============================\n\r");
        printf("\n\r");

        while(1)
        {
            process_btldr_cmds(g_un_key);
        }
    }
    else
    {
        printf("\n\r=============================\n\r");
        printf("\n\r====Button NOT Pressed=======\n\r");
        printf("\n\r=============================\n\r");
        printf("\n\r====Running Default App======\n\r");
        printf("\n\r=============================\n\r");
        for(int i = 0; i < 100000; i++){} /* Delay */
        jump_to_app(DEFAULT_APP_ADDRESS); /* Jump to the application */
    }


	
    while(1)
    {
        printf("\n\rERROR:...\n\r");
        for(int i = 0; i < 100000; i++){} /* Delay */

    }
    return 0;
}

/*
void SystemInit(void)
{
    SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
}
*/
static void process_btldr_cmds(SYS_APPS curr_app)
{
    switch(curr_app)
    {
        case APP2:
            jump_to_app(APP2_ADDRESS);
            break;
        case FACTORY_APP:
            jump_to_app(FACTORY_APP_ADDRESS);
            break;
        default:
            jump_to_app(DEFAULT_APP_ADDRESS);
            break;
    }
}

static void uart_callback(void)
{
    g_app_key =   read_uart2();

    if(g_app_key == '1')
    {   
        g_un_key = APP2; /* Set the application key */
        printf("\n\rKey 1 pressed, Running APP2...\n\r");

    }
    else if(g_app_key == '2')
    {   
        g_un_key = FACTORY_APP; /* Set the application key */
        printf("\n\rKey 2 pressed, Running Factory App...\n\r");

    }
    else 
    {   
        g_un_key = 0; /* Invalid key */
        printf("\n\rDefault App Selected...\n\r");
    }
}

void USART2_IRQHandler(void)
{
    if(USART2->ISR & CR1_RXFNEIE) /* Check if RXNE flag is set */
    {
        uart_callback(); /* Call the UART callback function */
    }
}
