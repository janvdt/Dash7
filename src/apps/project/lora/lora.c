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

static uart_handle_t* uart_lora;
//static fifo_t fifo_lora;

static uint8_t arr[12];
uint8_t txBuffer[TX_LENGTH] = {'"','H','e','l','l','o',' ','W','o','l','d','"'};
uint8_t txBuffer2[TX_LENGTH2] = {'O','K'};

static void uart_receive_lora(uint8_t byte);
void fill_buffer_lora();
void send_commando();


void uart_init_lora()
{
	uart_lora = uart_init(0,9600,0);
	uart_enable(uart_lora);
	uart_set_rx_interrupt_callback(uart_lora,uart_receive_lora);
	uart_rx_interrupt_enable(uart_lora);
}

static void uart_receive_lora(uint8_t byte)
{
	//uart_send_byte(uart_pc,byte);
}

void fill_buffer_lora()
{
	for(int i=0;i<sizeof(arr);i++)
	{
	  arr[i] = i; // Initializing each element seperately
	}

}

void send_commando()
{
	char* string = "TEST\r\n";
	uart_send_string(uart_lora,string);
	fill_buffer_lora();
	char myArray[] = { 0x00, 0x11, 0x22, 0x00, 0x11, 0x22, 0x00, 0x11, 0x22, 0x00, 0x11, 0x22 };
	iM880A_SendRadioTelegram(txBuffer, TX_LENGTH);
}





