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
#include "em_emu.h"
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
#include "acc/lis3dh_driver.h"
#include "userbutton.h"
#include "platform_sensors.h"
#include "platform_lcd.h"

enum sensor_tasks {
	WAKEUP,
	GPS_DATA,
	PARSE_GPS,
	START_PUMP_FLOW,
	STOP_PUMP_FLOW,
	INIT_TEMPERATURE,
	GET_TEMPERATURE,
	PH,
	ACC,
	GET_DATA_SEND,
	SEND,
	SLEEP
} current_task = WAKEUP;

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
void hourly_measurement();
double sensor_values[96];	//ben niet zeker van datatype..of als string doorsturen?
							//max van 96 bits doorsturen.
							
/*
Message structure 96 bits
Place of bits 	(x:x)  - data[x] & possible value (xx)
-Control bits 	(1:8)  - data[0]: Water in tophalf (0/1), Battery Charching (0/1) , Percentage of battery in 5% steps (from 0 to 20)(00000/10100) , one bit left
- Water temp 	(9:16) - data[1] : from -50 to 125 �C --> 8 bits (1100 1100 to 0111 1101 )
- pH 			(17:25)- data[2] : pH value from 3,9 to 10,9 pH (8 bits)
- Latitude(GPS) (26:49)- data[3] : -90,00000 to + 90,00000� --> 20 bits (0001 0101 1111 1001 0000 to 1001 0101 1111 1001 0000)
					   - data[4]
					   - data[5]
- Longitude(GPS)(50:72)- data[6] : -180,0000 to + 180,0000� --> 24 bits (0001 1011 0111 0111 0100 0000? tot 1001 1011 0111 0111 0100 0000)
						 data[7]
						 data[8]
- Flow meter	(73:80)- data[9] : from 0,00 to 6,00L (10 0101 1000)
- Accelerometer	(81:96)- data[10] :  calculate the G force as an directionless measurement --> 1 byte of data:double g = Math.sqrt(x * x + y * y + z * z);
					   - data[11]

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
			init_lora();
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

		//SizeOF not working!
		case LORA:
			lora_send(data,12);
			break;
	}
}

void get_measurements(uint8_t* data){

	//Control Bits
	data[0] = 20;
	//Water Temp
	uint8_t temp = get_tempvalue();
	data[1] = temp;
	//PH
	data[2] = 7;
	//Latitude + 900000
	uint32_t latitude = get_latitude();
	data[3] = (uint8_t) (latitude >> 16) & 255;
	data[4] = (uint8_t) (latitude >> 8) & 255;
	data[5] = (uint8_t) (latitude) & 255;
	//Longitude + 1800000
	uint32_t longitude = get_longitude();
	data[6] = (uint8_t) (longitude >> 16) & 255;
	data[7] = (uint8_t) (longitude >> 8) & 255;
	data[8] = (uint8_t) (longitude) & 255;
	//Flow
	uint8_t flow = get_flowvalue();
	data[9] = flow;
	//Accelerometer
	uint8_t x = get_x();
	uint8_t y = get_y();
	data[10] = x;
	data[11] = y;
}

void init_measurement_structure()
{
	uint8_t data[12];
	get_measurements(data);
	send_data(data);
}

void sensor_measurement()
{
	switch(current_task){

			//Switch on Gps and look for fix! wait ... secs
			case WAKEUP:
			{
				//hw_enter_lowpower_mode(0);
				current_task = GPS_DATA;
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);


			} break;
			case GPS_DATA:
			{
				//hw_enter_lowpower_mode(0);
				lcd_write_string("Getting GPS \r\n");
				enable_gps();
				current_task = PARSE_GPS;

				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 20);


			} break;

			//Parse the gps data Fix or no fix?
			case PARSE_GPS:
			{
				lcd_write_string("Gps is fixed \r\n");
				lcd_write_string("%d - %d \r\n", get_latitude(), get_longitude());
				shut_down_gps();
				current_task = START_PUMP_FLOW;
				sensor_measurement();
				/*
				if(gps_is_fixed())
				{
					lcd_write_string("Gps is fixed \r\n");
					lcd_write_string("%d - %d \r\n", get_latitude(), get_longitude());
					current_task = START_PUMP_FLOW;
					sensor_measurement();
				}
				else
				{
					lcd_write_string("Gps fix failed \r\n");
					gps_fix_failed();
					current_task = START_PUMP_FLOW;
					sensor_measurement();
				}
				*/

			} break;

			//Start pump and initialize flow meter
			case START_PUMP_FLOW:
			{
				lcd_write_string("Starting pump \r\n");
				start_pump();
				current_task = STOP_PUMP_FLOW;
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);


			} break;
			//Stop pump
			case STOP_PUMP_FLOW:
			{
				stop_pump();
				current_task = INIT_TEMPERATURE;
				lcd_write_string("Flow value %d \r\n",get_flowvalue());
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);

			} break;
			case INIT_TEMPERATURE:
			{
				lcd_write_string("Water temperature\r\n");
				init_temp_sensor();
				current_task = GET_TEMPERATURE;
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);

			} break;
			case GET_TEMPERATURE:
			{
				get_temp_sensor_value();
				lcd_write_string("TEMP : %d \r\n",get_temp_sensor_value());
				lcd_write_string("temperature : %d \r\n",get_tempvalue());
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);
				current_task = PH;

			} break;
			case PH:
			{
				lcd_write_string("PH quality \r\n");
				current_task = ACC;
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);

			} break;
			case ACC:
			{
				lcd_write_string("ACC \r\n");
				current_task = GET_DATA_SEND;
				get_acc_data();
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);

			} break;
			case GET_DATA_SEND:
			{
				lcd_write_string("Collect all data \r\n");
				init_measurement_structure();
				lcd_write_string("data send! \r\n");
				current_task = SLEEP;
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 1);


			} break;
			//TODO: add case for waterdetection
			case SLEEP:
			{
				lcd_write_string("SLEEP MODE \r\n");
				current_task = WAKEUP;
				hw_enter_lowpower_mode(1);
				timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 5);
			} break;
	}
}

void hourly_measurement()
{
	lcd_write_string("Execute cycle! \r\n");
	timer_post_task_delay(&sensor_measurement, TIMER_TICKS_PER_SEC * 10);
}
void read_acc()
{
	//uint8_t test = accelerometer_get_register(LIS3DH_WHO_AM_I);
	//lcd_write_string("Who am i : %d \r\n",accelerometer_get_register(LIS3DH_WHO_AM_I));

	//accelerometer_get_position();
	axes_data_t frame;
	accelerometer_read(&frame);
	//lcd_write_string("Position : %d \r\n",accelerometer_get_position());
	lcd_write_string("X : %d \r\n",frame.AXIS_X/180);
	lcd_write_string("Y : %d \r\n",frame.AXIS_Y/180);
	//lcd_write_string("Z : %d \r\n",frame.AXIS_Z);

	timer_post_task_delay(&read_acc, TIMER_TICKS_PER_SEC * 1);
}

void bootstrap()
{
	init_gps();
	set_send_method();
	acc_start();
	if(accelerometer_enable_axis(true,true,false))
	{
		lcd_write_string("Axiss success \n\r");
	}
	if(accelerometer_set_threshold(1,true,true,true,true,true,true))
	{
		lcd_write_string("Treshold set success \n\r");
	}
	sched_register_task(sensor_measurement);
	sensor_measurement();
}

