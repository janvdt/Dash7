// acceleromter interface
// autho: Christophe VG <contact@christophe.vg>

#ifndef __ACCELEROMETER_H
#define __ACCELEROMETER_H

#include "link_c.h"
#include "types.h"
#include "platform.h"

typedef struct {
  int16_t AXIS_X;
  int16_t AXIS_Y;
  int16_t AXIS_Z;
} axes_data_t;

typedef void (*accelerometer_callback_t)();

/*! \brief Initialises the sensors
 * 			- accelerometer
 */
__LINK_C bool accelerometer_init();

__LINK_C bool accelerometer_enable_axis(bool x, bool y, bool z);

/*! \brief Set threshold to receive interrupt
 * 	threshold [0, 31]
 */
__LINK_C bool accelerometer_set_threshold(uint8_t threshold, bool x_high, bool x_low, bool y_high, bool y_low, bool z_high, bool z_low);

__LINK_C uint8_t accelerometer_read(axes_data_t *data);

__LINK_C bool accelerometer_register_callback(accelerometer_callback_t callback);

__LINK_C bool accelerometer_deregister_callback();

__LINK_C uint8_t accelerometer_get_register(uint8_t reg);

__LINK_C void acc_start();

__LINK_C uint8_t accelerometer_get_position();

__LINK_C void set_x(uint8_t new_x);

__LINK_C void set_y(uint8_t new_y);

__LINK_C uint8_t get_x();

__LINK_C uint8_t get_y();

__LINK_C void get_acc_data();

#endif
