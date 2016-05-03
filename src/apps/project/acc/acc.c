/*
Accelerometer LIS3DH

*/

#include "hwi2c.h"
#include "acc.h"

static i2c_handle_t* acc;

#define LIS3DH_ADDRESS	(0x18<<1)

void init_accelero()
{
	acc = i2c_init(0,4);
	accelero_wake_up();
}

void accelero_wake_up()
{
	uint8_t i2c_write_data_reg1[1];
	uint8_t i2c_write_data_reg2[1];
	uint8_t i2c_write_data_reg3[1];
	uint8_t i2c_write_data_reg4[1];
	uint8_t i2c_write_data_reg5[1];
	uint8_t i2c_write_data_reg6[1];
	i2c_write_data_reg1[0] = 0x20;
	i2c_write_data_reg1[1] = 0x00;
	i2c_write_data_reg2[0] = 0x21;
	i2c_write_data_reg2[1] = 0x00;
	i2c_write_data_reg3[0] = 0x22;
	i2c_write_data_reg3[1] = 0x00;
	i2c_write_data_reg4[0] = 0x23;
	i2c_write_data_reg4[1] = 0x00;
	i2c_write_data_reg5[0] = 0x24;
	i2c_write_data_reg5[1] = 0x00;
	i2c_write_data_reg6[0] = 0x25;
	i2c_write(acc,0x19,i2c_write_data_reg1,2);
	i2c_write(acc,0x19,i2c_write_data_reg2,2);
	i2c_write(acc,0x19,i2c_write_data_reg3,2);
	i2c_write(acc,0x19,i2c_write_data_reg4,2);
	i2c_write(acc,0x19,i2c_write_data_reg5,2);
	i2c_write(acc,0x19,i2c_write_data_reg6,2);

	//i2cwrite naar module om uit slaap te halen
}

void accelero_read()
{
	uint8_t i2c_read_data[8];
	uint8_t i2c_write_data[1];
	uint8_t data = 0x0F;
	i2c_write_data[0] = LIS3DH_STATUS_REG;
	i2c_read(acc,(0x19 << 1),&data,1);
	//i2c_write_read(acc,0x19,i2c_write_data,1,i2c_read_data,8);
}
