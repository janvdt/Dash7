/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2015 University of Antwerp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hwsystem.h"
#include "hwadc.h"
#include "em_system.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "hwgpio.h"
#include "em_gpio.h"
#include <debug.h>

uint32_t tempData_pH;
uint32_t tempData_Cl;
static volatile bool updateMeasurementpH = true;
static volatile bool updateMeasurementCl = true;
static volatile bool adcConversionCompletepH = false;
static volatile bool adcConversionCompleteCl = false;

void ADCConfig(void)
{
	ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
	  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

	  /* Init common settings for both single conversion and scan mode */
	  init.timebase = ADC_TimebaseCalc(0);
	  /* Might as well finish conversion as quickly as possibly since polling */
	  /* for completion. */
	  /* Set ADC clock to 7 MHz, use default HFPERCLK */
	  init.prescale = ADC_PrescaleCalc(7000000, 0);

	  /* WARMUPMODE must be set to Normal according to ref manual before */
	  /* entering EM2. In this example, the warmup time is not a big problem */
	  /* due to relatively infrequent polling. Leave at default NORMAL, */

	  ADC_Init(ADC5, &init);
	  ADC_Init(ADC6, &init);

}
void init_pH()
{
	hw_gpio_configure_pin(D5, true, gpioModeInput, 0);
}

void init_Cl()
{
	hw_gpio_configure_pin(D6, true, gpioModeInput, 0);
}

uint32_t get_pH_value(void)
{
	adcConversionComplete = false;
	ADC_Start(ADC5, adcStartSingle);
    while (!adcConversionComplete){};
    tempData_pH = ADC_DataSingleGet( ADC5 );
    return tempData_pH;
}

uint32_t get_Cl_value(void)
{
	adcConversionComplete = false;
	ADC_Start(ADC6, adcStartSingle);
	while (!adcConversionComplete){};
	tempData_Cl = ADC_DataSingleGet( ADC6 );
	return tempData_Cl;
}



