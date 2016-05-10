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

#include "hwleds.h"
#include "hwsystem.h"
#include "scheduler.h"
#include "timer.h"
#include "platform.h"
#include "hwlcd.h"
#include "hwadc.h"
//#include "d7ap_stack.h"
//#include "fs.h"
#include "log.h"
#include "sensor/extsensor.h"
#include "acc/acc.h"
#include "lora/lora.h"
#include "sigfox/sigfox.h"
#include "gps/gps.h"
#include "hwuart.h"
#include "hwgpio.h"
#include "em_gpio.h"
#include "lora/IM880A_RadioInterface.h"
#include "userbutton.h"
#include "platform_sensors.h"
#include "platform_lcd.h"

enum sensor_tasks {
	GPS_DATA,
	PARSE_GPS,
	PUMP_FLOW,
	TEMPERATURE,
	PH,
	GET_DATA_SEND,
	SEND,
	SLEEP
} current_task = GPS_DATA;

enum choose_send_method {
	DASH7,
	SIGFOX,
	LORA
} current_send_method = LORA;

uint8_t app_mode_status = 0xFF;
uint8_t app_mode_status_changed = 0x00;
uint8_t app_mode = 0;
uint8_t mainEvent = 0;
int gps_fix_counter = 0;

double sensor_values[96];	//ben niet zeker van datatype..of als string doorsturen?
							//max van 96 bits doorsturen.
							
/*
Message structure 96 bits
Place of bits 	(x:x) & possible value (xx)
-Control bits 	(1:8) : Water in tophalf (0/1), Battery Charching (0/1) , Percentage of battery in 5% steps (from 0 to 20)(00000/10100) , one bit left
- Water temp 	(9:16) : from -50 to 125 �C --> 8 bits (1100 1100 to 0111 1101 )
- CL			(17:25) : Reserve for CL (can be changed) --> can be used for temperature (1 getal na de komma erbij --> 4 bits) + 4 bits vrij
- pH 			(25:32) : pH value from 3,9 to 10,9 pH (8 bits)
- Accelerometer	(33:40) :  calculate the G force as an directionless measurement --> 1 byte of data:double g = Math.sqrt(x * x + y * y + z * z);
- Flow meter	(41:50) : from 0,00 to 6,00L (10 0101 1000)
- Latitude(GPS) (51:70) : -90,00000 to + 90,00000� --> 20 bits (0001 0101 1111 1001 0000 to 1001 0101 1111 1001 0000)
- Longitude(GPS)(71:94) :  -180,0000 to + 180,0000� --> 24 bits (0001 1011 0111 0111 0100 0000? tot 1001 1011 0111 0111 0100 0000)
 - open			(95:96) : can be used
*/

// Toggle different operational modes
void userbutton_callback(button_id_t button_id)
{
	#ifdef PLATFORM_EFM32GG_STK3700
	//lcd_write_string("Butt %d", button_id);
	#else
	#endif
}

void set_send_method()
{
	switch(current_send_method)
	{
		case DASH7:
			//TO DO
			break;

		case SIGFOX:
			//init_sigfox();
			break;

		case LORA:
			//init_lora();
			break;
	}
}

void send_data(uint8_t* data)
{
	switch(current_send_method)
	{
		case DASH7:
			// TO DO
			break;

		case SIGFOX:
			//sendATmessage(data);
			break;

		case LORA:
			//lora_send(data,sizeof(data));
			break;
	}
}

void get_measurements(uint8_t* data){

	//Control Bits
	data[0] = 0x00;
	//Water Temp
	data[1] = 0x00;
	//CL
	data[2] = 0x00;
	//Ph
	data[3] = 0x00;
	//Accelerometer
	data[4] = 0x00;
	//Flow meter
	data[5] = get_flowvalue();
	//Longitude
	data[6] = get_longitude();
	data[7] = get_latitude();
}

void init_measurement_structure()
{
	set_send_method();
	uint8_t data[12];
	get_measurements(data);
	send_data(data);
}

void sensor_measurement()
{
	switch(current_task){
			case GPS_DATA:
			{
				lcd_write_string("Getting GPS \r\n");
				enable_gps();
				current_task = PARSE_GPS;
				timer_tick_t stop_time = timer_get_counter_value() + 40000;
				if(!sched_is_scheduled(sensor_measurement)){
					error_t gps_timer_task = timer_post_task(sensor_measurement, stop_time);
				}

			} break;

			case PARSE_GPS:
			{
				if(gps_is_fixed())
				{
					lcd_write_string("Gps is fixed \r\n");
					lcd_write_string("%d - %d \r\n", get_latitude(), get_longitude());
					current_task = PUMP_FLOW;
					sensor_measurement();
				}
				else
				{
					lcd_write_string("Gps fix failed \r\n");
					gps_fix_failed();
					current_task = PUMP_FLOW;
					sensor_measurement();
				}

			} break;


			case PUMP_FLOW:
			{
				lcd_write_string("Starting pump \r\n");
				start_pump();
				timer_tick_t stop_time_flow = timer_get_counter_value() + 10000;
				if(!sched_is_scheduled(sensor_measurement)){
					error_t flow_timer_task = timer_post_task(sensor_measurement, stop_time_flow);
				}

				EMU_EnterEM2(true);

				stop_pump();
				current_task = TEMPERATURE;

			} break;
			case TEMPERATURE:
			{
				lcd_write_string("Water temperature\r\n");
				init_temp_sensor();
				timer_tick_t stop_time_temp = timer_get_counter_value() + 1000;
				if(!sched_is_scheduled(sensor_measurement)){
					error_t temp_timer_task = timer_post_task(sensor_measurement, stop_time_temp);
				}

				EMU_EnterEM2(true);
				lcd_write_string("temperature : %d \r\n",get_tempvalue());

				current_task = PH;

			} break;
			case PH:
			{
				lcd_write_string("PH quality \r\n");
				timer_tick_t stop_time_ph = timer_get_counter_value() + 1000;
				if(!sched_is_scheduled(sensor_measurement)){
					error_t ph_timer_task = timer_post_task(sensor_measurement, stop_time_ph);
				}

				EMU_EnterEM2(true);

				current_task = GET_DATA_SEND;

			} break;
			case GET_DATA_SEND:
			{
				lcd_write_string("Collect all data \r\n");
				init_measurement_structure();

				timer_tick_t stop_time_send = timer_get_counter_value() + 10000;
				if(!sched_is_scheduled(sensor_measurement)){
					error_t send_timer_task = timer_post_task(sensor_measurement, stop_time_send);
				}

				lcd_write_string("data send! \r\n");
				EMU_EnterEM2(true);
				current_task = GPS_DATA;
				sensor_measurement();


			} break;
			case SLEEP:
			{


			} break;
	}
}

void bootstrap()
{
	init_gps();
	sched_register_task(sensor_measurement);
	sensor_measurement();
}

