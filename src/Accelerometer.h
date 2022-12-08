#include "mbed.h"

#define MMA8451_DEFAULT_ADDR 0x1D
#define MMA8451_REG_OUT_X_MSB 0x01
#define MMA8451_REG_OUT_X_LSB 0x02
#define MMA8451_REG_OUT_Y_MSB 0x03
#define MMA8451_REG_OUT_Y_LSB 0x04
#define MMA8451_REG_OUT_Z_MSB 0x05
#define MMA8451_REG_OUT_Z_LSB 0x06
#define MMA8451_REG_CTRL_REG1 0x2A
#define MMA8451_REG_CTRL_REG2 0x2B
#define MMA8451_REG_CTRL_REG4 0x2D
#define MMA8451_REG_CTRL_REG5 0x2E
#define MMA8451_REG_XYZ_DATA_CFG 0x11
#define MMA8451_REG_PL_CFG 0x11
#define MMA8451_REG_WHOAMI 0x0D
#define UINT14_MAX_VALUE 16383

#define STANDART_GRAVITY 9.8

class Accelerometer {
public:
  Accelerometer(PinName sda, PinName scl);
  void read(float *x, float *y, float *z);
  float getX();
  float getY();
  float getZ();

private:
  I2C _i2c;
  int _i2caddr;
  int16_t get_axis(uint8_t addr);
};