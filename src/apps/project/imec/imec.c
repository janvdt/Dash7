/* 
IMEC pH Sensor Lib
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

#include <imec.h>

uint32_t tempData_pH;
uint32_t tempData_Cl;

void init_pH()
{
	//Setup ADC for input pin D5 (=ADCch5)
	adc_init(adcReference1V25,adcInputSingleCh5,1000);
}

void init_Cl()
{
	//Setup ADC for input pin D6 (=ADCch6)
	adc_init(adcReference1V25,adcInputSingleCh6,1000);
}

uint32_t get_pH_value()
{
	tempData_pH = adc_read_single();
    return tempData_pH;
}

uint32_t get_Cl_value()
{
	tempData_Cl = adc_read_single();
	return tempData_Cl;
}



