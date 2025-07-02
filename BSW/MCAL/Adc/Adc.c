/****************************************************************************************************
 * @file           : Adc.c
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The ADC Driver abstracts the access to the microcontroller's Analog-to-Digital Converter.
 *                   It provides initialization, configuration, and data acquisition functions for the AUTOSAR MCAL layer.
 *  ***************************************************************************************************/

#include "stm32l4xx.h"
#include "Adc.h"


#define ADC1EN (1U<<13)
#define GPIOAEN (1U<<0)

#define ADC_CH5 (5U<<6)
#define ADC_SEQ_LEN_1 0x00 
#define CR_ADEN (1U<<0)
#define ISR_EOC (1U<<2)
#define ISR_ADRDY (1U<<0)
#define CR_ADSTART (1U<<2)
#define IER_ADRDYIE (1U<<0)
#define IER_EOCIE (1U<<2)
#define CR_ADVREGEN (1U<<28)
#define CR_CONTINUOUS (1U<<1)
#define CFGR_CONT (1U<<13)
#define CR_DEEPPWD (1U<<29)


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

  ADC1_COMMON->CCR &= ~ADC_CCR_CKMODE_0;
  ADC1_COMMON->CCR |= ADC_CCR_CKMODE_1;

  ADC1->CFGR |= ADC_CFGR_CONT; // Set continuous mode

  ADC1->SQR1 |= ADC_SEQ_LEN_1;

  ADC1->SQR1 = (5U << ADC_SQR1_SQ1_Pos); 

  ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode
  ADC1->CR |= ADC_CR_ADVREGEN; // Enable voltage regulator for ADC

  ADC1->CR |= ADC_CR_ADCAL;

  ADC1->CR |= ADC_CR_ADEN; // Enable ADC

  while(!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait for ADC ready

  //ADC1->IER |= ADC_IER_ADRDYIE | ADC_IER_EOCIE;

  ADC1->ISR |= ADC_ISR_ADRDY;

  /*Conversion sequence lenght 21.4.9 */
  /*Enable ADC Module*/
}

void start_conversion(void)
{
  /* Enabe Continuos Conversion */
  ADC1->CFGR |= ADC_CFGR_CONT; 
  /* Start ADC Conversion */
  ADC1->CR |= ADC_CR_ADSTART;
}

uint32_t adc_read(void)
{
  /* Wait for conversion to complete */
  while (!(ADC1->ISR & ISR_EOC)); // Wait for EOC (End of Conversion)

  /* Clear the EOC flag */
  ADC1->ISR |= ISR_EOC;

  /* Read the converted value */
  uint32_t adc_value = ADC1->DR;

  return adc_value;
}
