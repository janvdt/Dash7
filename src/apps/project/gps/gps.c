/*GPS
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
#include "hwlcd.h"
#include "minmea.h"
#include <string.h>
#include <ctype.h>
#include "../lora/IM880A_RadioInterface.h"

#define BUFFER_SIZE	256
#define COMMAND_SIZE_BEGIN 66

static fifo_t fifo_gps;
static uint8_t buffer[BUFFER_SIZE] = { 0 };

void uart_receive_gps(uint8_t byte);
void readout_fifo_gps();
void init_fifo_gps();

void init_fifo_gps()
{
	fifo_init(&fifo_gps, buffer, BUFFER_SIZE);
}

void uart_receive_gps(uint8_t byte)
{
	error_t err;
	err = fifo_put(&fifo_gps, &byte, 1); assert(err == SUCCESS);

	readout_fifo_gps();
    if(!sched_is_scheduled(&readout_fifo_gps))
        sched_post_task(&readout_fifo_gps);
}

void readout_fifo_gps()
{
	if(fifo_get_size(&fifo_gps) >= COMMAND_SIZE_BEGIN)
	{
		led_toggle(1);
		uint8_t length = fifo_get_size(&fifo_gps) > COMMAND_SIZE_BEGIN ? COMMAND_SIZE_BEGIN : fifo_get_size(&fifo_gps);
		uint8_t received_data_command[COMMAND_SIZE_BEGIN];
		fifo_pop(&fifo_gps, received_data_command, length);
		char *p = received_data_command;
		switch (minmea_sentence_id(p, false))
		{
			case MINMEA_SENTENCE_RMC:
			{
				struct minmea_sentence_rmc frame;
				if (minmea_parse_rmc(&frame, p))
				{
					//lcd_clear();
					//lcd_write_string("$xxRMC: raw coordinates and speed: (%d,%d) %d\n",minmea_rescale(&frame.latitude, 1000),minmea_rescale(&frame.longitude, 1000),minmea_rescale(&frame.speed, 1000));

					uint8_t send_data_command_gps[21];
					char slatitude[9];
					char slongitude[9];
					itoa(frame.latitude.value, slatitude, 10);
					itoa(frame.longitude.value, slongitude, 10);
					memcpy(send_data_command_gps,slatitude, sizeof(slatitude));
					memcpy(send_data_command_gps + strlen(slatitude),slongitude, sizeof(slongitude));
					iM880A_SendRadioTelegram(&send_data_command_gps,21);
				}
			}
			break;
			default:
				clear_fifo_gps();
			break;
		}

		//uart_send_bytes(uart_pc,received_data, length);
	}
}

void clear_fifo_gps()
{
	fifo_clear(&fifo_gps);
}





