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
#include <stdbool.h>
#include "hwuart.h"
#include "fifo.h"
#include "assert.h"
#include "timer.h"
#include "IM880A_RadioInterface.h"
#include "hwlcd.h"
#include <string.h>
#include <ctype.h>


#define BUFFER_SIZE	256
#define BUFFER_SIZE_LORA 12
#define TX_LENGTH        12
#define TX_LENGTH2       2
#define INDENT_SPACES "  "

static uint8_t buffer[BUFFER_SIZE] = { 0 };
static fifo_t fifo_lora;

void uart_receive_lora(uint8_t byte);
void readout_fifo_lora();
void send_commando();

void init_fifo_lora()
{
	fifo_init(&fifo_lora, buffer, BUFFER_SIZE);
}

void uart_receive_lora(uint8_t byte)
{
	error_t err;
	err = fifo_put(&fifo_lora, &byte, 1); assert(err == SUCCESS);

	readout_fifo_lora();
	if(!sched_is_scheduled(&readout_fifo_lora))
		sched_post_task(&readout_fifo_lora);
}

void readout_fifo_lora()
{

}

void send_commando()
{
	//char* string = "TEST\r\n";
	//uart_send_string(uart_lora,string);
	//fill_buffer_lora();
	//char myArray[] = { 0x00, 0x11, 0x22, 0x00, 0x11, 0x22, 0x00, 0x11, 0x22, 0x00, 0x11, 0x22 };
	//iM880A_SendRadioTelegram(txBuffer, TX_LENGTH);
}






