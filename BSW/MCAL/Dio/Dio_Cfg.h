/********************************************************************************
 * @file           : Dio_Cfg.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : 
 *******************************************************************************/

#ifndef DIO_CFG_H_
#define DIO_CFG_H_


/* Port ID*/
#define PORTA_ID                (0U)
#define PORTB_ID                (1U)
#define PORTC_ID                (2U)
#define PORTD_ID                (3U)
#define PORTE_ID                (4U)
#define PORTH_ID                (7U)

#define CHANEL_ID_0             (16u)
#define CHANEL_ID_1             (17u)
#define CHANEL_ID_2             (18u)
#define CHANEL_ID_3             (8u)

#define PORT_A_PIN_0            (0U)          /* ADC1/5    */
#define PORT_A_PIN_1            (1U)
#define PORT_A_PIN_2            (2U)          /* LIN       */
#define PORT_A_PIN_3            (3U)          /* LIN       */
#define PORT_A_PIN_4            (4U)          /* SPI1_SSEL */
#define PORT_A_PIN_5            (5U)          /* SPI1_SCLK */
#define PORT_A_PIN_6            (6U)          /* SPI1_MISO */
#define PORT_A_PIN_7            (7U)          /* SPI1_MOSI */
#define PORT_A_PIN_8            (8U)          /* PWM1/1    */
#define PORT_A_PIN_9            (9U)          /* UART1_TX  */
#define PORT_A_PIN_10           (10U)         /* UART1_RX  */
#define PORT_A_PIN_11           (11U)         /* CAN_RD    */
#define PORT_A_PIN_12           (12U)         /* CAN_TD    */

#define PORT_B_PIN_0            (13U)         /* ADC1/15   */
#define PORT_B_PIN_1            (14U)         /* PWM1/3N   */
#define PORT_B_PIN_2            (15U)
#define PORT_B_PIN_3            (16U)         /* I/O       */
#define PORT_B_PIN_4            (17U)         /* I/O       */
#define PORT_B_PIN_5            (18U)         /* I/O       */
#define PORT_B_PIN_6            (19U)         /* I2C_SCL   */
#define PORT_B_PIN_7            (20U)         /* I2C_SDA   */
#define PORT_B_PIN_8            (21U)

#define DIO_CHANNEL_ID_0        PORT_A_PIN_0
#define DIO_CHANNEL_ID_1        PORT_A_PIN_1
#define DIO_CHANNEL_ID_2        PORT_A_PIN_2
#define DIO_CHANNEL_ID_3        PORT_A_PIN_3
#define DIO_CHANNEL_ID_4        PORT_A_PIN_4
#define DIO_CHANNEL_ID_5        PORT_A_PIN_5
#define DIO_CHANNEL_ID_6        PORT_A_PIN_6
#define DIO_CHANNEL_ID_7        PORT_A_PIN_7
#define DIO_CHANNEL_ID_8        PORT_A_PIN_8
#define DIO_CHANNEL_ID_9        PORT_A_PIN_9
#define DIO_CHANNEL_ID_10       PORT_A_PIN_10
#define DIO_CHANNEL_ID_11       PORT_A_PIN_11
#define DIO_CHANNEL_ID_12       PORT_A_PIN_12
#define DIO_CHANNEL_ID_13       PORT_B_PIN_0
#define DIO_CHANNEL_ID_14       PORT_B_PIN_1
#define DIO_CHANNEL_ID_15       PORT_B_PIN_2
#define DIO_CHANNEL_ID_16       PORT_B_PIN_3
#define DIO_CHANNEL_ID_17       PORT_B_PIN_4
#define DIO_CHANNEL_ID_18       PORT_B_PIN_5
#define DIO_CHANNEL_ID_19       PORT_B_PIN_6
#define DIO_CHANNEL_ID_20       PORT_B_PIN_7
#define DIO_CHANNEL_ID_21       PORT_B_PIN_8

typedef struct {
    
}Dio_ConfigType;



#endif /* DIO_CFG_H_ */
