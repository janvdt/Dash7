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
#include "assert.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>

#include "hwlcd.h"
#include "hwadc.h"
#include "d7ap_stack.h"
#include "fs.h"
#include "log.h"
#include "extsensor.h"
#include "uart.h"
#include "hwuart.h"

#if (!defined PLATFORM_EFM32GG_STK3700 && !defined PLATFORM_EFM32HG_STK3400 && !defined PLATFORM_EZR32LG_WSTK6200A)
	#error Mismatch between the configured platform and the actual platform.
#endif

#include "userbutton.h"
#include "platform_sensors.h"
#include "platform_lcd.h"

#define SENSOR_FILE_ID           0x40
#define SENSOR_FILE_SIZE         8
#define ACTION_FILE_ID           0x41

#define APP_MODE_LEDS		1
#define APP_MODE_LCD		1 << 1
#define APP_MODE_CONSOLE	1 << 2

uint8_t app_mode_status = 0xFF;
uint8_t app_mode_status_changed = 0x00;
uint8_t app_mode = 0;

static uart_handle_t* uart;
static uart_handle_t* sigfox_uart;



void execute_sensor_measurement()
{
#ifdef PLATFORM_EFM32GG_STK3700
  float internal_temp = hw_get_internal_temperature();
  lcd_write_temperature(internal_temp*10, 1);

  uint32_t vdd = hw_get_battery();

#endif

#if (defined PLATFORM_EFM32HG_STK3400  || defined PLATFORM_EZR32LG_WSTK6200A)
  lcd_clear();
  float internal_temp = hw_get_internal_temperature();
  lcd_write_string("Int T: %2d.%d C\n", (int)internal_temp, (int)(internal_temp*10)%10);
  log_print_string("Int T: %2d.%d C\n", (int)internal_temp, (int)(internal_temp*10)%10);

  float external_temp = get_external_temperature();
  lcd_write_string("Ext s T: %2d.%d C\n", (int)external_temp, (int)(external_temp*10)%10);
  log_print_string("Ext s T: %2d.%d C\n", (int)external_temp, (int)(external_temp*10)%10);

  uint32_t rhData;
  uint32_t tData;
  getHumidityAndTemperature(&rhData, &tData);

  lcd_write_string("Ext T: %d.%d C\n", (tData/1000), (tData%1000)/100);
  log_print_string("Temp: %d.%d C\n", (tData/1000), (tData%1000)/100);

  lcd_write_string("Ext H: %d.%d\n", (rhData/1000), (rhData%1000)/100);
  log_print_string("Hum: %d.%d\n", (rhData/1000), (rhData%1000)/100);

  uint32_t vdd = hw_get_battery();

  lcd_write_string("Batt %d mV\n", vdd);
  log_print_string("Batt: %d mV\n", vdd);
  
  lcd_write_string("ATZ2");

  //TODO: put sensor values in array

  uint8_t sensor_values[8];
  uint16_t *pointer =  (uint16_t*) sensor_values;
  *pointer++ = (uint16_t) (internal_temp * 10);
  *pointer++ = (uint16_t) (external_temp * 10);
  *pointer++ = (uint16_t) (tData /100);
  *pointer++ = (uint16_t) (rhData /100);
  *pointer++ = (uint16_t) (vdd /10);

  //fs_write_file(SENSOR_FILE_ID, 0, (uint8_t*)&sensor_values,8);
#endif

  //test uart
  //uint8_t a[5]={internal_temp,external_temp,tData,rhData,vdd};


  timer_post_task_delay(&execute_sensor_measurement, TIMER_TICKS_PER_SEC * 10); //aanpassen naar 60
  //timer_post_task_delay(sendATmessage(), TIMER_TICKS_PER_SEC * 10);
}

void sendATmessage()
{
	char* TMcommand = "AT";
	lcd_write_string("Ready to send\n");
	uart_send_string(sigfox_uart,TMcommand); //LCD valt uit dan..
	lcd_write_string("Sending...\n");
	timer_post_task_delay(&sendATmessage, TIMER_TICKS_PER_SEC * 1);
}

void bootstrap()
{
	log_print_string("Device booted\n");
	lcd_enable(true);
    initSensors();		//onboard sensors
    uart_init_sigfox();
	 
    //sched_register_task((&execute_sensor_measurement));
	sched_register_task((&sendATmessage));
    //timer_post_task_delay(&execute_sensor_measurement, TIMER_TICKS_PER_SEC * 1);
	timer_post_task_delay(&sendATmessage, TIMER_TICKS_PER_SEC * 1);

    lcd_write_string("EFM32 Sensor\n");
	
	//sendATmessage();
	
}

