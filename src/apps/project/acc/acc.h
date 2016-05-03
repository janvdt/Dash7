/*
Accelerometer LIS3DH

*/

#include "types.h"
#include "link_c.h"

#define ACC_ADDRESS 0x1D
#define I2C_WRITE (0xFE)
#define I2C_READ  (0x01)
#define LIS3DH_STATUS_REG_AUX 0x07
#define LIS3DH_OUT_ADC1_L 0x08
#define LIS3DH_OUT_ADC1_H 0x09
#define LIS3DH_OUT_ADC2_L 0x0A
#define LIS3DH_OUT_ADC2_H 0x0B
#define LIS3DH_OUT_ADC3_L 0x0C
#define LIS3DH_OUT_ADC3_H 0x0D
#define LIS3DH_INT_COUNTER_REG 0x0E
#define LIS3DH_WHO_AM_I 0x0F
#define LIS3DH_TEMP_CFG_REG 0x1F
#define LIS3DH_CTRL_REG1 0x20
#define LIS3DH_CTRL_REG2 0x21
#define LIS3DH_CTRL_REG3 0x22
#define LIS3DH_CTRL_REG4 0x23
#define LIS3DH_CTRL_REG5 0x24
#define LIS3DH_CTRL_REG6 0x25
#define LIS3DH_REFERENCE 0x26
#define LIS3DH_STATUS_REG 0x27
#define LIS3DH_OUT_X_L 0x28
#define LIS3DH_OUT_X_H 0x29
#define LIS3DH_OUT_Y_L 0x2A
#define LIS3DH_OUT_Y_H 0x2B
#define LIS3DH_OUT_Z_L 0x2C
#define LIS3DH_OUT_Z_H 0x2D
#define LIS3DH_FIFO_CTRL_REG 0x2E
#define LIS3DH_FIFO_SRC_REG 0x2F
#define LIS3DH_INT1_CFG 0x30



__LINK_C void init_accelero();

__LINK_C void accelero_wake_up();

__LINK_C void accelero_read();


