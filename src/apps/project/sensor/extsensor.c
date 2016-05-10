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
#include "timer.h"
#include "em_gpio.h"
#include <debug.h>

uint8_t flow_frequency;
void stop_pump();

void counter_flow()
{
  flow_frequency++;
}

void start_pump()
{
	//Pin to start pump
	hw_gpio_set(C2);

	//initialize flow meter
	hw_gpio_configure_pin(C0, true, gpioModeInput, 0);
	error_t err;
	err = hw_gpio_configure_interrupt(C0,counter_flow,GPIO_RISING_EDGE);assert(err == SUCCESS);
	hw_gpio_enable_interrupt(C0);

	timer_post_task_delay(&stop_pump, TIMER_TICKS_PER_SEC * 10);
}

void stop_pump()
{
	get_flow_meter_value();
	hw_gpio_clr(C2);
}

void get_flow_meter_value()
{
	uint8_t flow_value = (flow_frequency / 7.5);
	lcd_write_string("Flow meter: %d\n",flow_value);
	flow_frequency = 0;
}

void start_temp_sensor()
{
	//initialize temp sensor
	hw_gpio_configure_pin(D4,true, gpioModePushPull, 0);
	adc_init(adcReference1V25,adcInputSingleCh4,1000);

}

uint32_t get_temp_sensor_value()
{
	uint32_t tempData_temperature = adc_read_single();
	tempData_temperature = ((tempData_temperature*125)/4096)-44;
	return tempData_temperature;
}
