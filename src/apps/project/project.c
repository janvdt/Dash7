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
#include "gps/gps.h"
#include "uart/uart.h"
#include "hwuart.h"
#include "hwgpio.h"
#include "em_gpio.h"
#include "lora/IM880A_RadioInterface.h"



#if (!defined PLATFORM_EFM32GG_STK3700 && !defined PLATFORM_EFM32HG_STK3400 && !defined PLATFORM_EZR32LG_WSTK6200A)
	#error Mismatch between the configured platform and the actual platform.
#endif

#include "userbutton.h"
#include "platform_sensors.h"
#include "platform_lcd.h"

#define SENSOR_FILE_ID           0x40
#define SENSOR_FILE_SIZE         8
#define ACTION_FILE_ID           0x41
#define TX_LENGTH               13


#define APP_MODE_LEDS		1
#define APP_MODE_LCD		1 << 1
#define APP_MODE_CONSOLE	1 << 2


typedef enum
{
    SEND_MSG                        = 0x0001,
    POWER_UP                        = 0x0002,
    MSG_RECEIVED                    = 0x0004,

}TMainEventCode;

uint8_t app_mode_status = 0xFF;
uint8_t app_mode_status_changed = 0x00;
uint8_t app_mode = 0;
uint8_t mainEvent = 0;

double sensor_values[96];	//ben niet zeker van datatype..of als string doorsturen?
							//max van 96 bits doorsturen.
							
/*
Message structure 96 bits
Place of bits 	(x:x) & possible value (xx)
-Control bits 	(1:8) : Water in tophalf (0/1), Battery Charching (0/1) , Percentage of battery in 5% steps (from 0 to 20)(00000/10100) , one bit left
- Water temp 	(9:16) : from -50 to 125 °C --> 8 bits (1100 1100 to 0111 1101 )
- CL			(17:25) : Reserve for CL (can be changed) --> can be used for temperature (1 getal na de komma erbij --> 4 bits) + 4 bits vrij
- pH 			(25:32) : pH value from 3,9 to 10,9 pH (8 bits)
- Accelerometer	(33:40) :  calculate the G force as an directionless measurement --> 1 byte of data:double g = Math.sqrt(x * x + y * y + z * z);
- Flow meter	(41:50) : from 0,00 to 6,00L (10 0101 1000)
- Latitude(GPS) (51:70) : -90,00000 to + 90,00000° --> 20 bits (0001 0101 1111 1001 0000 to 1001 0101 1111 1001 0000)
- Longitude(GPS)(71:94) :  -180,0000 to + 180,0000° --> 24 bits (0001 1011 0111 0111 0100 0000? tot 1001 1011 0111 0111 0100 0000)
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

static void CbRxIndication(uint8_t* rxMsg,uint8_t length, TRadioFlags rxFlags)
{
    if(rxFlags == trx_RXDone)
    {
        mainEvent |= MSG_RECEIVED;  // Radio Msg received
    }
}

static void CbTxIndication(TRadioMsg* txMsg, TRadioFlags txFlags)
{
    if(txFlags == trx_TXDone)
    {
        // TX was successfull
    }
    else
    {
        //  error
    }
}

void execute_flow_pump_measurement()
{
	start_pump();
	timer_post_task_delay(&execute_flow_pump_measurement, TIMER_TICKS_PER_SEC * 12);
}

void execute_hourly_measurement()
{
	//get_flow_meter_value();
	//accelero_read();
	timer_post_task_delay(&execute_hourly_measurement, TIMER_TICKS_PER_SEC * 1);
}

void bootstrap()
{

	uart_init_all();
    // Register callback functions for receive / send
    iM880A_RegisterRadioCallbacks(CbRxIndication, CbTxIndication);
    //init_accelero();

    //hw_gpio_configure_pin(C0,1,gpioModeWiredOr,1);
    //hw_gpio_set(C0);
    //ubutton_register_callback(0, &userbutton_callback);
    //ubutton_register_callback(1, &userbutton_callback);
    sched_register_task((&execute_hourly_measurement));
    sched_register_task((&execute_flow_pump_measurement));
    sched_register_task((&stop_pump));
    timer_post_task_delay(&execute_hourly_measurement, TIMER_TICKS_PER_SEC * 1);
    timer_post_task_delay(&execute_flow_pump_measurement, TIMER_TICKS_PER_SEC * 1);

}

