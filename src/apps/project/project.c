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
#include "lora/lora.h"
#include "gps/gps.h"
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

void execute_sensor_measurement()
{
	//clear_fifo_gps();
	timer_post_task_delay(&execute_sensor_measurement, TIMER_TICKS_PER_SEC * 1);
}

void bootstrap()
{

    //initSensors();
    uart_init_gps();
    iM880A_Init();
    // Register callback functions for receive / send
    iM880A_RegisterRadioCallbacks(CbRxIndication, CbTxIndication);
    //uart_init_lora();

    //hw_gpio_configure_pin(C0,1,gpioModeWiredOr,1);
    //hw_gpio_set(C0);

    ubutton_register_callback(0, &userbutton_callback);
    ubutton_register_callback(1, &userbutton_callback);

    sched_register_task((&execute_sensor_measurement));
    timer_post_task_delay(&execute_sensor_measurement, TIMER_TICKS_PER_SEC * 1);

}

