/*
Sigfox Lib
*/
#include "scheduler.h"
#include "timer.h"
#include "assert.h"
#include "platform.h"
#include "hwlcd.h"
#include "hwadc.h"

#include sigfox.h
#include "extsensor.h"
#include "uart.h"
#include "hwuart.h"
#include "fifo.h"
#include "em_i2c.h"

#define AT_COMMAND_PREFIX 	"AT$ss="
#define AT_COMMAND_END		"\r"
#define AT_MAX_MESSAGE_SIZE	96
#define AT_DB_COMMAND		"ATS302="
#define AT_RF_BIT_COMMAND	"AT$SB="

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

void request_reply(){
	char* data = "AT$sb=1,2,1";
	//uart_send_string(o_uart,"\n");
	//uart_send_string(o_uart, data);
	sendAT_RFmessage("1,2,1", 11);
	timer_post_task_delay(&readout_fifo_sigfox, TIMER_TICKS_PER_SEC * 40);
	
}