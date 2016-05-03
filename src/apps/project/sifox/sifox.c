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
#include <string.h>
#include <stdlib.h>

#include "hwlcd.h"
#include "hwadc.h"
//#include "d7ap_stack.h"
//#include "fs.h"
#include "log.h"
#include "extsensor.h"
#include "uart.h"
#include "hwuart.h"
#include "fifo.h"
#include "em_i2c.h"
//#include "hwi2c.h"

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

#define AT_COMMAND_PREFIX 	"AT$ss="
#define AT_COMMAND_END		"\r"
#define AT_MAX_MESSAGE_SIZE	96
#define AT_DB_COMMAND		"ATS302="
#define AT_RF_BIT_COMMAND	"AT$SB="

uint8_t app_mode_status = 0xFF;
uint8_t app_mode_status_changed = 0x00;
uint8_t app_mode = 0;

extern uart_handle_t* o_uart;
uint8_t sensor_values[8];

void sendATmessage(char* data) //
{
	signed int msg_length = strlen(AT_COMMAND_PREFIX)+strlen(AT_COMMAND_END)+strlen(data);
	char f_data [msg_length];		//12*8bytes = 96 bits max!
	
	memcpy(f_data,AT_COMMAND_PREFIX, strlen(AT_COMMAND_PREFIX));
	memcpy(f_data+strlen(AT_COMMAND_PREFIX), data, strlen(data));
	memcpy(f_data+strlen(AT_COMMAND_PREFIX)+strlen(data),AT_COMMAND_END, strlen(AT_COMMAND_END));

	lcd_write_string("Sending... \n");
	uart_send_string(o_uart,f_data); 
	lcd_write_string("Data send! \n");
}

void sendAT_DBmessage(char* data, size_t length) //
{
	//uint8_t msg_length = strlen(AT_COMMAND_PREFIX)+strlen(AT_COMMAND_END)+strlen(data);
	char f_data [length];		//12*8bytes = 96 bits max!
	
	memcpy(f_data,AT_DB_COMMAND, strlen(AT_DB_COMMAND));
	memcpy(f_data+strlen(AT_DB_COMMAND), data, strlen(data));
	memcpy(f_data+strlen(AT_DB_COMMAND)+strlen(data),AT_COMMAND_END, strlen(AT_COMMAND_END));

	lcd_write_string("Sending... \n");
	uart_send_string(o_uart,f_data); 
	lcd_write_string("Data send! \n");
}

void sendAT_RFmessage(char* data, size_t length) //
{
	//uint8_t msg_length = strlen(AT_COMMAND_PREFIX)+strlen(AT_COMMAND_END)+strlen(data);
	char f_data [length];		//12*8bytes = 96 bits max!
	
	memcpy(f_data,AT_RF_BIT_COMMAND, strlen(AT_RF_BIT_COMMAND));
	memcpy(f_data+strlen(AT_RF_BIT_COMMAND), data, strlen(data));
	memcpy(f_data+strlen(AT_RF_BIT_COMMAND)+strlen(data),AT_COMMAND_END, strlen(AT_COMMAND_END));

	lcd_write_string("Sending... \n");
	lcd_write_string(f_data);
	uart_send_string(o_uart,f_data); 
	lcd_write_string("Data send! \n");
}

char * conv_to_hex (char * string) 
{
    int length = strlen(string);
	char * out_string = malloc(sizeof((length*2)+1));

    for (int i = 0; i < length; i++) 
	{
        sprintf ((out_string+(i*2)), "%02X", (unsigned char) (*(string+i)));
    }
    //lcd_write_string(out_string);
    return out_string;
}

void execute_send_data(){

	char* data = "AT\r";

	lcd_write_string("Sending... \n");
	uart_send_string(o_uart,data);
	lcd_write_string("Data send! \n");

	//timer_post_task_delay(&execute_send_data, TIMER_TICKS_PER_SEC * 1);
}

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

  lcd_write_string("Geck T: %d.%d C\n", (tData/1000), (tData%1000)/100);
  log_print_string("Temp: %d.%d C\n", (tData/1000), (tData%1000)/100);

  lcd_write_string("Geck H: %d.%d\n", (rhData/1000), (rhData%1000)/100);
  log_print_string("Hum: %d.%d %\n", (rhData/1000), (rhData%1000)/100);

  char sensorData[12];
  sprintf(sensorData,"%x%x%x",(int)internal_temp,(tData/1000),(rhData/1000));
  //sendATmessage(conv_to_hex(sensorData));
  sendATmessage((sensorData));
  //uint32_t vdd = hw_get_battery();

  //lcd_write_string("Batt %d mV\n", vdd);
  //log_print_string("Batt: %d mV\n", vdd);
  
  //TODO: put sensor values in array
	/*
  uint8_t sensor_values[8];
  uint16_t *pointer =  (uint16_t*) sensor_values;
  *pointer++ = (uint16_t) (internal_temp * 10);
  *pointer++ = (uint16_t) (external_temp * 10);
  *pointer++ = (uint16_t) (tData /100);
  *pointer++ = (uint16_t) (rhData /100);
  *pointer++ = (uint16_t) (vdd /10);
*/
  //fs_write_file(SENSOR_FILE_ID, 0, (uint8_t*)&sensor_values,8);
#endif

  //test uart
  //uint8_t a[5]={internal_temp,external_temp,tData,rhData,vdd};


  //timer_post_task_delay(&execute_sensor_measurement, TIMER_TICKS_PER_SEC * 10); //aanpassen naar 60
}

void request_reply(){
	char* data = "AT$sb=1,2,1";
	//uart_send_string(o_uart,"\n");
	//uart_send_string(o_uart, data);
	sendAT_RFmessage("1,2,1", 11);
	timer_post_task_delay(&readout_fifo_sigfox, TIMER_TICKS_PER_SEC * 40);
	
}


void userbutton_callback(button_id_t button_id)
{
	#ifdef PLATFORM_EFM32GG_STK3700
	lcd_write_string("Butt %d", button_id);
	#else
	  lcd_write_string("button: %d\n", button_id);
	#endif
}

void bootstrap()
{
    initSensors();
	ubutton_register_callback(0, &userbutton_callback);
    //ubutton_register_callback(1, &userbutton_callback);
	uart_init_sigfox();
	
	lcd_write_string("EFM32 Sensor2\n");
	//sendAT_DBmessage("14" , 96);
	//sendATmessage("36 1b 4d 39 c1 f4 71 14 7b 0f 09 9e" , 96);
	// byte lenght [2] = 0d 44
	// byte lenght [12] = 36 1b 4d 39 c1 f4 71 14 7b 0f 09 9e
	
	//
	sched_register_task((&readout_fifo_sigfox));
	sched_register_task((&request_reply));
	timer_post_task_delay((&request_reply), TIMER_TICKS_PER_SEC * 1);
	//while(1)
	//{
		
	//}
	
	
    //sched_register_task((&execute_sensor_measurement));
    //timer_post_task_delay(&execute_sensor_measurement, TIMER_TICKS_PER_SEC * 1);
	//sched_register_task((&readout_fifo_sigfox));
	//timer_post_task_delay(&readout_fifo_sigfox, TIMER_TICKS_PER_SEC * 40);
    //sched_register_task((&execute_send_data));
    //sched_register_task((&execute_send_data_sigfox));

    //timer_post_task_delay(&execute_send_data, TIMER_TICKS_PER_SEC * 1);
    //timer_post_task_delay(&execute_send_data_sigfox, TIMER_TICKS_PER_SEC * 1);

}

