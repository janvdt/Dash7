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
#include "hwlcd.h"
#include "em_gpio.h"
#include <debug.h>
#include "../lora/IM880A_RadioInterface.h"

uint8_t flow_frequency;
int flow;

void stop_pump();
void get_flow_meter_value();
void start_pump();
void set_flowvalue(int new_flowvalue);
int get_flowvalue();

void counter_flow()
{
  flow_frequency++;
}

void start_pump()
{
	//Initialize pin
	hw_gpio_configure_pin(C2,1,gpioModeWiredOr,1);
	//Pin to start pump
	hw_gpio_set(C2);

	//initialize flow meter
	hw_gpio_configure_pin(C0, true, gpioModeInput, 0);
	error_t err;
	err = hw_gpio_configure_interrupt(C0,counter_flow,GPIO_RISING_EDGE);assert(err == SUCCESS);
	hw_gpio_enable_interrupt(C0);
}

void stop_pump()
{
	get_flow_meter_value();
	hw_gpio_clr(C2);
	lcd_write_string("Pump stopped \r\n");
}

void get_flow_meter_value()
{
	uint8_t send_data[1];
	set_flowvalue(flow_frequency / 7.5);
	/*
	char flow_value = (flow_frequency / 7.5);
	send_data[0] = flow_value;
	lcd_write_string("Flow meter: %d\n",flow_value);
	iM880A_SendRadioTelegram(&send_data,sizeof(send_data));
	flow_frequency = 0;
	*/
}

void set_flowvalue(int new_flowvalue){
	flow = new_flowvalue;
}

int get_flowvalue(){
	return flow;
}




