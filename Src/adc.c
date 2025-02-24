/*
 * adc.c
 *
 *  Created on: Jan 23, 2025
 *      Author: manci
 * 
 * The device embeds a successive approximation analog-to-digital converter with the
   following features:
   • 12-bit native resolution, with built-in calibration
   • 5.33 Msps maximum conversion rate with full resolution
    – Down to 18.75 ns sampling time
    – Increased conversion rate for lower resolution (up to 8.88 Msps for 6-bit
      resolution)
   • Up to 10 external channels.
   • 4 internal channels: internal reference voltage, temperature sensor, DAC1_OUT1 and
     DAC1_OUT2.
   • Single-ended and differential mode inputs
   • Low-power design
    – Capable of low-current operation at low conversion rate (consumption decreases
      linearly with speed)
    – Dual clock domain architecture: ADC speed independent from CPU frequency
   • Highly versatile digital interface
 */
//RCC_AHB2ENR
//ADC1_IN17 TEMP SENSOR
//PA0 ADC1_IN5
/* ADC is Connected to AHB2 bus*/
#include "stm32l4xx.h"


#define ADC1EN (1U<<13)
#define GPIOAEN (1U<<0)

#define ADC_CH5 (5U<<6)
#define ADC_SEQ_LEN_1 0x00 
#define CR_ADEN (1U<<0)

void pa0_adc_init(void)
{
  /*** Configure the GPIOA pin ***/
  
  /* Enable clock access to GPIOA */
  RCC->AHB2ENR |= GPIOAEN;

  /* Set the mode of PA0 analog input */
  GPIOA->MODER |= (1U<<0);
  GPIOA->MODER |= (1U<<1);

  /*** ADC1 configuration ***/
  
  /* Enable clock access to ADC1 */
  RCC->AHB2ENR |= ADC1EN;

  /* Configure ADC Parameters */
  /*Conversion sequence start*/
  ADC1->SQR1 = ADC_CH5;
  //ADC1->SQR1 |= ADC_SEQ_LEN_1;
  
  ADC1->CR |= CR_ADEN;
  /*Conversion sequence lenght*/
  /*Enable ADC Module*/
}

void start_conversion(void)
{
  /* Start ADC Conversion */
  ADC1->CR |= CR_ADEN;
}