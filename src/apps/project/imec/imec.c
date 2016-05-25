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

#include "imec.h"

#define refpH_el 131 //= combined refpH number and electrode number

uint32_t tempData_pH;
//uint32_t tempData_Cl;

void init_pH()
{
	//Setup ADC for input pin D5 (=ADCch5)
	adc_init(adcReference1V25,adcInputSingleCh5,1000);
}

/*void init_Cl()
{
	//Setup ADC for input pin D6 (=ADCch6)
	adc_init(adcReference1V25,adcInputSingleCh6,1000);
}*/

uint32_t get_pH_value(void)
{
	tempData_pH = adc_read_single(); //read pH value
	tempData_pH = (tempData_pH * 1250)/4096; // normalize to mV
	float constant; //values depending on electrode
	float rico;
	switch(refpH_el)
	{
		case 94:
			constant = 657;
			rico = 70.867;
			break;
		case 112:
			constant = 675.22;
			rico = 65.35;
			break;
		case 115:
			constant = 768.25;
			rico = 74.45;
			break;
		case 121:
			constant = 780.37;
			rico = 78.167;
			break;
		case 122:
			constant = 774.83;
			rico = 77.9;
			break;
		case 124:
			constant = 776.28;
			rico = 77.483;
			break;
		case 125:
			constant = 663.5;
			rico = 64.367;
			break;
		case 131:
			constant = 812.3;
			rico = 75.867;
			break;
		case 132:
			constant = 818.37;
			rico = 76.967;
			break;
		case 134:
			constant = 836.98;
			rico = 75.55;
			break;
		case 135:
			constant = 644.12;
			rico = 63.95;
			break;
	}
	uint8_t pH = -((tempData_pH - constant)/rico); //apply formula stated in datasheet*/
    return tempData_pH;
}

/*uint32_t get_Cl_value()
{
	tempData_Cl = adc_read_single();
	return tempData_Cl;
}*/
