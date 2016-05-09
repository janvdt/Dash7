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

//Define GPS
#define BUFFER_SIZE	256
#define COMMAND_SIZE_BEGIN 66

//Handles
static uart_handle_t* uart_gps;
static fifo_t fifo_gps;
static uint8_t buffer[BUFFER_SIZE];

/* New */

const char* MESSAGE_RATE 	= "$PMTK220,1000*1F\r\n";
const char* BAUD_RATE 		= "$PMTK251,9600*17\r\n";
const char* STOP_MODE 		= "$PMTK161,0*28\r\n";
const char* GNRMC_ONLY		= "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";

int latitude;
int longitude;
bool gps_fix;
int fifo_size;

void uart_receive_gps(uint8_t byte);
void readout_fifo_gps();
void clear_fifo_gps();
void set_latitude(int new_latitude);
void set_longitude(int new_longitude);
void shut_down_gps();
void gps_fix_failed();

void init_gps()
{
	fifo_init(&fifo_gps, buffer, BUFFER_SIZE);
	uart_gps = uart_init(1,9600,4);
	uart_enable(uart_gps);
}

void enable_gps()
{
	gps_fix = false;

	uart_send_string(uart_gps, BAUD_RATE);
	uart_send_string(uart_gps, MESSAGE_RATE);
	uart_send_string(uart_gps, GNRMC_ONLY);

	uart_set_rx_interrupt_callback(uart_gps, &uart_receive_gps);
	uart_rx_interrupt_enable(uart_gps);

}

void uart_receive_gps(uint8_t byte)
{
	    led_toggle(1);
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

					set_latitude(minmea_rescale(&frame.latitude, 1000)); // latitude 5 decimals
					set_longitude(minmea_rescale(&frame.longitude, 100)); // longitude 4 decimals
					gps_fix = true;
					shut_down_gps();
				}
			}
			break;
			default:
				clear_fifo_gps();
			break;
		}
	}
}

void clear_fifo_gps()
{
	fifo_clear(&fifo_gps);
}

void gps_fix_failed()
{
	gps_fix = false;
	set_longitude(0);
	set_latitude(0);
	shut_down_gps();
}

void disable_gps()
{
	uart_rx_interrupt_disable(uart_gps);
}

void shut_down_gps()
{
	disable_gps();
	uart_disable(uart_gps);
}

void set_latitude(int new_latitude){
	latitude = new_latitude;
}

void set_longitude(int new_longitude){
	longitude = new_longitude;
}

bool gps_is_fixed(){
	return gps_fix;
}

int get_latitude(){
	return latitude;
}

int get_longitude(){
	return longitude;
}





