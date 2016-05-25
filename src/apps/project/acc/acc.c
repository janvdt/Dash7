#include "hwgpio.h"
#include "hwadc.h"
#include "hwatomic.h"
#include "scheduler.h"
#include "hwi2c.h"
#include "lis3dh_driver.h"
#include <string.h>
#include <assert.h>

#include "em_gpio.h"

#include "acc.h"

static i2c_handle_t* i2c;
uint8_t x;
uint8_t y;

bool _init_sensor();
void set_x(uint8_t new_x);
void set_y(uint8_t new_y);

void acc_start()
{
	i2c = i2c_init(0, 4);
	lis3dh_init_i2c(i2c);
	if(!accelerometer_init())
	{
		lcd_write_string("Init failed \r\n");
	}
}

bool accelerometer_init() {
  if( ! _init_sensor() ) { return false; }
}

bool _init_sensor() {
	if( ! LIS3DH_SetODR(LIS3DH_ODR_400Hz)  ) { return false; }
	if( ! LIS3DH_SetMode(LIS3DH_LOW_POWER) ) { return false; }
	return LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2);
}

bool accelerometer_enable_axis(bool x, bool y, bool z) {
	LIS3DH_Axis_t axis;
	if(x) { axis |= LIS3DH_X_ENABLE; }
	if(y) { axis |= LIS3DH_Y_ENABLE; }
	if(z) { axis |= LIS3DH_Z_ENABLE; }
	return LIS3DH_SetAxis(axis) == MEMS_SUCCESS;
}

bool accelerometer_set_threshold(uint8_t threshold,
                                 bool x_high, bool x_low,
                                 bool y_high, bool y_low,
                                 bool z_high, bool z_low)
{
	if( ! LIS3DH_SetInt1Threshold(threshold) ) { return false; }

	LIS3DH_Int1Conf_t config;
	if(x_high) { config |= LIS3DH_INT1_XHIE_ENABLE; }
	if(y_high) { config |= LIS3DH_INT1_YHIE_ENABLE; }
	if(z_high) { config |= LIS3DH_INT1_ZHIE_ENABLE; }
	if(x_low)  { config |= LIS3DH_INT1_XLIE_ENABLE; }
	if(y_low)  { config |= LIS3DH_INT1_YLIE_ENABLE; }
	if(z_low)  { config |= LIS3DH_INT1_ZLIE_ENABLE; }

	if( ! LIS3DH_SetIntConfiguration(config) ) { return false; }
	if( ! LIS3DH_SetIntMode(LIS3DH_INT_MODE_6D_MOVEMENT) ) { return false; }
  return LIS3DH_SetInt6D4DConfiguration(LIS3DH_INT1_6D_ENABLE) == MEMS_SUCCESS;
}

uint8_t accelerometer_read(axes_data_t *data) {
  return LIS3DH_GetAccAxesRaw((AxesRaw_t*) data);
}

uint8_t accelerometer_get_position() {
  uint8_t position;
  if( ! LIS3DH_Get6DPosition(&position) ) {
    return false;
  }
  return position;
}

void get_acc_data()
{
	axes_data_t frame;
	accelerometer_read(&frame);
	set_x((frame.AXIS_X/180) + 100);
	set_y((frame.AXIS_Y/180) + 100);
}


uint8_t accelerometer_get_register(uint8_t reg) {
  uint8_t data;
  LIS3DH_ReadReg(reg, &data);
  return data;
}

void set_x(uint8_t new_x){
	x = new_x;
}

void set_y(uint8_t new_y){
	y = new_y;
}

uint8_t get_x(){
	return x;
}

uint8_t get_y(){
	return y;
}
