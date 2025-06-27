/*
 * adc.h
 *
 *  Created on: Jan 23, 2025
 *      Author: manci
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void pa0_adc_init(void);
void start_conversion(void);
uint32_t adc_read(void);

#endif /* ADC_H_ */
