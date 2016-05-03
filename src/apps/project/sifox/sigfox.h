/*
Sigfox

*/

void sendATmessage(char* data);

void sendAT_DBmessage(char* data, size_t length);

void sendAT_RFmessage(char* data, size_t length);

char * conv_to_hex (char * string);

void execute_send_data();

void request_reply();


	