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

uint8_t flow_frequency;

void counter_flow()
{
  flow_frequency++;
}

void init_flow_meter()
{
	hw_gpio_configure_pin(C0, true, gpioModeInput, 0);
	error_t err;
	    err = hw_gpio_configure_interrupt(C0,counter_flow,GPIO_RISING_EDGE);assert(err == SUCCESS);
	hw_gpio_enable_interrupt(C0);
}

void get_flow_meter_value()
{
	uint8_t flow_value = (flow_frequency / 7.5);
	lcd_write_string("Flow meter: %d\n",flow_value);
	flow_frequency = 0;
}



