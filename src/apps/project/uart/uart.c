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
#include "../gps/gps.h"
#include "../lora/lora.h"
#include "../sigfox/sigfox.h"
#include "../lora/IM880A_RadioInterface.h"


static uart_handle_t* uart_gps;
static uart_handle_t* uart_sigfox;


void uart_init_all()
{
	/*
	//UART GPS
	uart_gps = uart_init(1, 9600, 4);
	uart_set_rx_interrupt_callback(uart_gps, &uart_receive_gps);
	uart_enable(uart_gps);
	uart_rx_interrupt_enable(uart_gps);
	init_fifo_gps();
	*/

	//UART LORA
	iM880A_Init();
	/*
	//UART SIGFOX
	uart_sigfox = uart_init(0, 9600, 4);
	uart_enable(uart_sigfox);
	uart_set_rx_interrupt_callback(uart_sigfox,&uart_receive_sigfox);
	uart_rx_interrupt_enable(uart_sigfox);
	init_fifo_sigfox();
	*/
}




