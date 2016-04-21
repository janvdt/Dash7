/*
Accelerometer LIS3DH

*/

#include "hwi2c.h"
#define CONFIG_RG1 0b01000111
#define STATUS_RG 0x07
#define RG1 0x20
static i2c_handle_t* acc;

void init_accelero()
{
	acc = i2c_init(0,5);
	i2c_write(acc,RG1,(uint8_t*)CONFIG_RG1,1);
}

void accelero_wake_up()
{
	//i2cwrite naar module om uit slaap te halen
}

void accelero_read()
{
	uint8_t status[10];
	i2c_read(acc,STATUS_RG,status,1);
	lcd_write_string("ACC: %d\n",status);
}
