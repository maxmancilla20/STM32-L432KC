/*
 * ============================= DATA ======================================
 * User LD3: the green LED is a user LED connected to Arduino Nano signal D13
 * corresponding to the STM32 I/O PB3 (pin 26).
 * | AHB2 | 0x4800 0400 - 0x4800 07FF | 1 KB | GPIOB
 * PIN D4(5) PB7 AS INPUT ABH2 ALSO

 * main.c
 * 
 * This file contains the main function and UART2 initialization function for the STM32L4 microcontroller.
 * 
 * Author: Maximiliano Mancilla
 * 
 * ============================= DATA =======================================
 * The STM32L432xx devices have two embedded universal synchronous receiver transmitters (USART1 and USART2).
 * These interfaces provide asynchronous communication, IrDA SIR ENDEC support, multiprocessor communication mode, 
 * single-wire half-duplex communication mode and have LIN Master/Slave capability. They provide hardware management
 * of the CTS and RTS signals, and RS485 Driver Enable. They are able to communicate at speeds of up to 10Mbit/s.
 * ==========================================================================
 */

/*
Used registers
GPIOB->ODR |= USER_LED_PIN; // Turn on the LED
GPIOB->ODR &= ~USER_LED_PIN; // Turn off the LED
GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
*/
#include "Std_Types.h"
#include <stdio.h>
#include "Mcu.h"
#include "Mcu_Cfg.h"
#include "Uart.h"
#include "Uart_Cfg.h"
#include "Dio.h"
#include "Dio_Cfg.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Gpt.h"
#include "Gpt_Cfg.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "I2c.h"
#include "I2c_Cfg.h"
#include "BMP180.h"
#include "Spi.h"
#include "Spi_Cfg.h"
#include "DHT11.h"
#include "Global_Timers.h"
#include "Pwm_Driver.h"
#include "ServoSG90_Driver.h"
#include "HCSR04.h"

extern Mcu_ConfigType McuDriverConfiguration;
/*
 * main function
 * 
 * The main function initializes the system and enters an infinite loop.
 */

 uint32_t adc_value = 0;
 uint32_t timestamp = 0;

 /* CAN GLOBAL VARIABLES */
 uint8_t rx_data[10];
 uint8_t tx_data[8];

 tx_header.std_id = 0x244; // Set the standard ID for the CAN message
 tx_header.ide = CAN_ID_STD; // Set the identifier type to standard
 tx_header.rtr = 0; // Set the Remote Transmission Request to 0 (data frame)
 tx_header.dlc = 8; // Set the Data Length Code to 8 bytes
 tx_header.ext_id = 0; // Set the extended ID to 0 (not used for standard ID)
 tx_header.transmit_global_time = 0; // Disable transmit global time
 
 tx_data[0] = 0x01;
 tx_data[1] = 0x02;
 tx_data[2] = 0x03;
 tx_data[3] = 0x04;
 tx_data[4] = 0x05;
 tx_data[5] = 0x06;
 tx_data[6] = 0x07;
 tx_data[7] = 0x08;

 uint32_t tx_mailbox[3];

 can_rx_header_typedef rx_header;
 can_tx_header_typedef tx_header;
 uint8_t count = 0;
 BMP180_DataType bmp180_data;

 void CAN1_RX0_IRQHandler(void)
 {
    if((CAN1->RF0R & CAN_RF0R_FMP0) != 0)  /* Check if there is a message pending in FIFO 0 */
    {

        can_get_rx_message(CAN_RX_FIFO0, &rx_header, rx_data);
        count++;

    }
 }

int main(void)
{
    // Initialization code here
    Mcu_Init(&McuDriverConfiguration);
    Uart2_RXTX_Init(); // Initialize UART2 for transmission and reception
    Dio_Init(); // Initialize GPIO
    pa0_adc_init();
    start_conversion();
    /* Configure TIMERS  */
    tim1_init(); /* TIM1 FOR PWM */
    tim2_1us_Init(); /* Timer for HCSR04 and DHT11 */

    /* CAN INIT FUNCTIONS */
    can_gpio_init(); // Initialize CAN GPIO pins
    can_params_init(CAN_MODE_LOOPBACK); // Initialize CAN parameters in loopback mode
    can_filter_config(0x244); // Configure CAN filter for standard ID 0x123
    can_start(); // Start the CAN peripheral
    I2c_Init(); // Initialize I2C peripheral
    BMP180_Init(); // Load BMP180 factory calibration coefficients
    spi1_gpio_init(); // Initialize SPI1 GPIO pins
    spi1_config(); // Configure SPI1 peripheral
    DHT11_Init(); // Initialize DHT11 on PB4
    HCSR04_Init(); // Initialize HCSR04 ultrasonic sensor
    Gt_InitTimers();
    Gpt_Init();
    ServoSG90_Init();

    Gt_StartTimer(GT_0, 500U);
    Gt_StartTimer(GT_1, 1000U);
    Gt_StartTimer(GT_2, 2000U);

    /*
    CAN1_TX_IRQHandler 
    CAN1_RX0_IRQHandler
    CAN1_RX1_IRQHandler
    CAN1_SCE_IRQHandler
    */

    while(1)
    {
        if (Gt_CheckTimer(GT_0))
        {
            GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED every 500 ms

            HCSR04_ReadDistance(&timestamp); // Read distance from HCSR04 ultrasonic sensor
            printf("Distance: %lu mm\n\r", timestamp); // Print the distance in millimeters

            Gt_StartTimer(GT_0, 500U);
        }   

        if (Gt_CheckTimer(GT_1))
        {
            static uint8_t PositionPosControl = 0U;

            if (PositionPosControl >= 180U)
            {
                PositionPosControl = 0U;
            }
            else
            {
                PositionPosControl += 20U;
            }

            ServoSG90_SetAngle(PositionPosControl);
            //Pwm_SetPulseUs(3000U);
            Gt_StartTimer(GT_1, 300U);
        }


        if(Gt_CheckTimer(GT_2))
        {
            /* I2C Test */
            // BMP180_ReadByte(0xD0); // Read the chip ID from the BMP180 sensor
            BMP180_ReadMeasurement(&bmp180_data); // Read temperature and pressure data from the BMP180 sensor
            {
                int32_t temperatureHundredths =
                    (int32_t)(bmp180_data.TemperatureC * 100.0F);
                int32_t temperatureWhole = temperatureHundredths / 100;
                int32_t temperatureFraction = temperatureHundredths % 100;
                int32_t altitudeHundredths =
                    (int32_t)(bmp180_data.AltitudeM * 100.0F);
                int32_t altitudeWhole = altitudeHundredths / 100;
                int32_t altitudeFraction = altitudeHundredths % 100;

                if (temperatureFraction < 0)
                {
                    temperatureFraction = -temperatureFraction;
                }

                if (altitudeFraction < 0)
                {
                    altitudeFraction = -altitudeFraction;
                }

                printf("BMP180: \n\r     Temperature: %ld.%02ld C,\n\r     Pressure: %lu Pa, \n\r     Altitude: %ld.%02ld m\n\r\n\r",
                    (long)temperatureWhole,
                    (long)temperatureFraction,
                    (unsigned long)bmp180_data.PressurePa,
                    (long)altitudeWhole,
                    (long)altitudeFraction);
            }


            /* SPI TEST */
            //spi1_loopback_test();

            /* DHT11 TEST */
            {
                DHT11_DataType dht11_data;
                uint8_t status = DHT11_Read(&dht11_data);
                if (status == 1U)
                {


                    printf("DHT11: \n\r     H=%u%% \n\r     T=%uC\r\n\n\r",
                        (unsigned int)dht11_data.Humidity,
                        (unsigned int)dht11_data.Temperature);
                }
                else
                {
                    printf("DHT11: NO DATA\r\n");
                }
            }

            Gt_StartTimer(GT_2, 2000U);
        
        }

        
    }
    return 0;
}


/* OLD TEST CODE BEFORE WHILE*/

    //tim2_output_pb3_compare_1hz(); // Initialize Timer 2 for output compare on PB3


/* OLD TEST CODE IN WHILE*/

        //GPIOB->ODR |= USER_LED_PIN;  /* Turn on the LED */
        //GPIOB->ODR &= ~USER_LED_PIN; /* Turn off the LED */
        //GPIOB->ODR ^= USER_LED_PIN;  /* Toggle the LED */
        //for(int i = 0; i < 100000; i++){} /* Delay */
        //key = read_uart2(); // Read the key pressed
        
        /* Read ADC value */
        //adc_value = adc_read();
        //printf("ADC Value: %lu\n\r", adc_value);
        //SystickDelay_Ms(1000);

        //tim2_1hz_delay(); // Wait for 1 second (1Hz)
        //GPIOB->ODR ^= USER_LED_PIN; // Toggle the LED
        
        //printf("LED toggled\n\r");

        /* Wait until edge is captured */
        //while(!(TIM1->SR & SR_CC1IF)) {} // Wait for the capture/compare interrupt flag
        /* Read captured value */
        //timestamp = TIM1->CCR1; // Read the captured value from channel 1

        /* CAN SEND MESSAGE CONFIGURATION */
        //can_add_tx_message(&tx_header, &tx_data[0], tx_mailbox); // Send the CAN message
        //SystickDelay_Ms(250); // Delay for 1 second



