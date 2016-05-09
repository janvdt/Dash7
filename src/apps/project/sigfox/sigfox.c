/*
Sigfox Lib
*/
#include "scheduler.h"
#include "timer.h"
#include "assert.h"
#include "platform.h"
#include "hwlcd.h"
#include "hwadc.h"
#include "sigfox.h"
#include "hwuart.h"
#include "fifo.h"
#include "em_i2c.h"

#define BUFFER_SIZE	256
#define COMMAND_SIZE_BEGIN 25
#define AT_COMMAND_PREFIX 	"AT$ss="
#define AT_COMMAND_END		"\r"
#define AT_MAX_MESSAGE_SIZE	96
#define AT_DB_COMMAND		"ATS302="
#define AT_RF_BIT_COMMAND	"AT$SB="

static uart_handle_t* uart_sigfox;
//uint8_t sensor_values[8];

fifo_t fifo_sigfox;
uint8_t buffer[BUFFER_SIZE];

void uart_receive_sigfox(uint8_t byte);
void readout_fifo_sigfox();
void init_fifo_sigfox();
void init_sigfox();

void init_sigfox()
{
	//UART SIGFOX
	uart_sigfox = uart_init(0, 9600, 4);
	uart_enable(uart_sigfox);
	uart_set_rx_interrupt_callback(uart_sigfox,&uart_receive_sigfox);
	uart_rx_interrupt_enable(uart_sigfox);
	init_fifo_sigfox();
}

void init_fifo_sigfox()
{
	fifo_init(&fifo_sigfox, buffer, BUFFER_SIZE);
}

void uart_receive_sigfox(uint8_t byte)
{
	error_t err;
	err = fifo_put(&fifo_sigfox, &byte, 1); assert(err == SUCCESS);

	readout_fifo_sigfox();
    if(!sched_is_scheduled(&readout_fifo_sigfox))
        sched_post_task(&readout_fifo_sigfox);
}

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

void sendATmessage(char* data) //
{
	signed int msg_length = strlen(AT_COMMAND_PREFIX)+strlen(AT_COMMAND_END)+strlen(data);
	char f_data [msg_length];		//12*8bytes = 96 bits max!
	
	memcpy(f_data,AT_COMMAND_PREFIX, strlen(AT_COMMAND_PREFIX));
	memcpy(f_data+strlen(AT_COMMAND_PREFIX), data, strlen(data));
	memcpy(f_data+strlen(AT_COMMAND_PREFIX)+strlen(data),AT_COMMAND_END, strlen(AT_COMMAND_END));

	lcd_write_string("Sending... \n");
	uart_send_string(uart_sigfox,f_data);
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
	uart_send_string(uart_sigfox,f_data);
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
	uart_send_string(uart_sigfox,f_data);
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
	uart_send_string(uart_sigfox,data);
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
