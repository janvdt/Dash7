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
#define COMMAND_SIZE_BEGIN 70
#define ASCII_G_SIGN 71
#define ASCII_DOLLAR_SIGN 36
#define ASCII_CARRIAGE_RETURN 13



//Handles
static uart_handle_t* uart_gps;
static fifo_t fifo_gps;
static uint8_t buffer[BUFFER_SIZE];

/* New */

const char* MESSAGE_RATE 	= "$PMTK220,1000*1F\r\n";
const char* BAUD_RATE 		= "$PMTK251,9600*17\r\n";
const char* STOP_MODE 		= "$PMTK161,0*28\r\n";
const char* GNRMC_ONLY		= "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
const char* COLD_RESTART 	= "$PMTK103*30\r\n";
const char* WARM_RESTART    = "$PMTK101*32\r\n";
const char* GLONASS 		= "$PMTK353,1,1,0,0,0*2B\r\n";

uint32_t latitude;
uint32_t longitude;
bool gps_fix;
bool start_decoding;
int byte_counter;
int previous_byte;
int fifo_size;

void uart_receive_gps(uint8_t byte);
void readout_fifo_gps();
void clear_fifo_gps();
void set_latitude(uint32_t new_latitude);
void set_longitude(uint32_t new_longitude);
void shut_down_gps();
void gps_fix_failed();

void init_gps()
{
	fifo_init(&fifo_gps, buffer, BUFFER_SIZE);
	uart_gps = uart_init(1,9600,4);
	uart_enable(uart_gps);
	uart_set_rx_interrupt_callback(uart_gps, &uart_receive_gps);
	uart_rx_interrupt_enable(uart_gps);

}

void enable_gps()
{
	gps_fix = false;
	uart_send_string(uart_gps, WARM_RESTART);
	uart_send_string(uart_gps, GNRMC_ONLY);
	uart_send_string(uart_gps, GLONASS);
}

void uart_receive_gps(uint8_t byte)
{
	    led_toggle(1);

	    /*
	    error_t err;
		err = fifo_put(&fifo_gps, &byte, 1); assert(err == SUCCESS);

		readout_fifo_gps();
		if(!sched_is_scheduled(&readout_fifo_gps))
			sched_post_task(&readout_fifo_gps);
		*/

	    if(!gps_fix)
	    {
			// a $GNRMC sentence always starts with a $-sign followed by a G-sign
			if(byte == ASCII_G_SIGN && previous_byte == ASCII_DOLLAR_SIGN)
			{
				// we can now put bytes in the fifo and start counting
				byte_counter = 0;
				start_decoding = true;
			}

			if(start_decoding){
				// if we start a new sentence
				// we need to prepend the $ sign
				if(byte_counter == 0){
					// ASCII dollar sign
					uint8_t dollar_byte = 36;
					error_t fifo_status = fifo_put(&fifo_gps, &dollar_byte, 1);
					assert(fifo_status == SUCCESS);
				}
				error_t fifo_status = fifo_put(&fifo_gps, &byte, 1);
				assert(fifo_status == SUCCESS);
			}

			// we can start decoding when we reach the end of a sentence
			// indicated by a carriage return and a line feed
			if(byte == ASCII_CARRIAGE_RETURN)
			{
				// we need to count the $ too
				fifo_size = byte_counter + 1;

				readout_fifo_gps();

				start_decoding = false;
			}
	    }

	    	byte_counter++;
	    	previous_byte = byte;

}

void readout_fifo_gps()
{

			uint8_t received_data_command[fifo_size];
			fifo_pop(&fifo_gps, received_data_command, fifo_size);
			char *p = received_data_command;
			if(minmea_sentence_id(p, false) == MINMEA_SENTENCE_RMC)
			{
				struct minmea_sentence_rmc frame;
				if (minmea_parse_rmc(&frame, p))
				{
					// rescale and set to fit in the forseen 3 bytes per coordinate
					set_latitude(minmea_rescale(&frame.latitude, 1000)); // latitude 5 decimals
					set_longitude(minmea_rescale(&frame.longitude, 100)); // longitude 4 decimals
				}
			}
			clear_fifo_gps();
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
	clear_fifo_gps();
	uart_send_string(uart_gps, STOP_MODE);
}

void set_latitude(uint32_t new_latitude){
	latitude = new_latitude + 900000;
}

void set_longitude(uint32_t new_longitude){
	longitude = new_longitude + 1800000;
}

bool gps_is_fixed(){
	return gps_fix;
}

uint32_t get_latitude(){
	return latitude;
}

uint32_t get_longitude(){
	return longitude;
}





