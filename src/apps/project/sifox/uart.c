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
#include <debug.h>
#include "hwuart.h"
#include "hwlcd.h"
#include "fifo.h"
#include "assert.h"
#include "timer.h"

#define BUFFER_SIZE	256
#define COMMAND_SIZE_BEGIN 25

static uart_handle_t* uart_gps;
static uart_handle_t* uart_pc;
static uart_handle_t* uart_sigfox;
uart_handle_t* o_uart;				//voor extern bereikbaar te maken

fifo_t fifo_sigfox;
uint8_t buffer[BUFFER_SIZE];

void readout_fifo_sigfox()
{

	//if(fifo_get_size(&fifo_sigfox) >= 50)
	//{
		uint8_t length = fifo_get_size(&fifo_sigfox);
		//uint8_t received_data[length];
		//fifo_pop(&fifo_sigfox, buffer, length);
		fifo_peek(&fifo_sigfox, buffer ,0, length);
		lcd_write_string(buffer);
		//TODO : make parser to read only message..not whole response
		
	//}
}

void clear_fifo_sigfox()
{
	fifo_clear(&fifo_sigfox);
}

void uart_receive(uint8_t byte)
{
	//uart_send_byte(uart_pc,byte);
	//char* c = &byte;
	fifo_put(&fifo_sigfox, &byte, 1); //assert(err == SUCCESS);

	//lcd_write_string(c);
	//lcd_write_string("End program");
	//readout_fifo_sigfox();
    //if(!sched_is_scheduled(&readout_fifo_sigfox))
        //sched_post_task(&readout_fifo_sigfox);
}

void uart_receive_pc(uint8_t byte)
{
	uart_send_byte(uart_pc,byte);
}

void uart_init_gps()
{
	uart_gps = uart_init(1, 115200, 4);
	uart_enable(uart_gps);
	uart_set_rx_interrupt_callback(uart_gps,uart_receive);
	uart_rx_interrupt_enable(uart_gps);
}


void uart_init_sigfox()
{
	fifo_init(&fifo_sigfox, buffer, BUFFER_SIZE);
	o_uart = uart_init(1, 9600, 4);
	uart_enable(o_uart);
	uart_set_rx_interrupt_callback(o_uart,uart_receive);
	uart_rx_interrupt_enable(o_uart);
}

void uart_init_pc()
{
	uart_pc = uart_init(0,9600,0);
	uart_enable(uart_pc);
	uart_set_rx_interrupt_callback(uart_pc,uart_receive_pc);
	uart_rx_interrupt_enable(uart_pc);
}






