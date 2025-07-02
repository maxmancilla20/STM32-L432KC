/**********************************************************************************************************
 * @file           : Adc.h
 * @author         : Maximiliano Mancilla
 * @date           : Jun 28, 2025
 * @brief          : The ADC Driver abstracts the access to the microcontroller's Analog-to-Digital Converter.
 *                   It provides initialization, configuration, and data acquisition functions for the AUTOSAR MCAL layer.
 **********************************************************************************************************/

#ifndef ADC_H_
#define ADC_H_

/******************************************************
 **                     Includes                     **
 ******************************************************/
#include "Std_Types.h"

/******************************************************
 **                DET Error Codes                   **
 ******************************************************/
// Define ADC-related error codes here if needed

/*******************************************************
 **                 Type definitions                  **
 *******************************************************/
// Add ADC type definitions here if needed

/******************************************************************
 **                      FUNCTIONS PROTOTYPES                    **
 ******************************************************************/
void pa0_adc_init(void);
void start_conversion(void);
uint32_t adc_read(void);

#endif /* ADC_H_ */