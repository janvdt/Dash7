/*
Sigfox

*/

#include "types.h"
#include "link_c.h"

__LINK_C void sendATmessage(char* data);

__LINK_C void sendAT_DBmessage(char* data, size_t length);

__LINK_C void sendAT_RFmessage(char* data, size_t length);

__LINK_C char * conv_to_hex (char * string);

__LINK_C void execute_send_data();

__LINK_C void request_reply();

__LINK_C void init_fifo_sigfox();

__LINK_C void uart_receive_sigfox(uint8_t byte);

__LINK_C void init_sigfox();


	
