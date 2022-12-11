#include "Accelerometer.h"
#include "mbed.h"

Accelerometer::Accelerometer(PinName sda, PinName scl) : _i2c(sda, scl) {
  _i2caddr = (MMA8451_DEFAULT_ADDR << 1);
  char cmd[2] = {MMA8451_REG_CTRL_REG1, 0x01};
  _i2c.write(_i2caddr, cmd, 2);

  //   cmd[0] = MMA8451_REG_CTRL_REG2;
  //   cmd[1] = 0x40;
  //   _i2c.write(_i2caddr, cmd, 2);

  //   do {
  //     _i2c.write(_i2caddr, cmd, 1);
  //     _i2c.read(_i2caddr, buf, 1);
  //   } while (buf[0] & 0x40);

  //   // enable 4G range
  //   cmd[0] = MMA8451_REG_XYZ_DATA_CFG;
  //   cmd[1] = 0b01;
  //   _i2c.write(_i2caddr, cmd, 2);

  // High res
  //   cmd[0] = MMA8451_REG_CTRL_REG2;
  //   cmd[1] = 0x02;
  //   _i2c.write(_i2caddr, cmd, 2);

  //   // DRDY on INT1
  //   cmd[0] = MMA8451_REG_CTRL_REG4;
  //   cmd[1] = 0x01;
  //   _i2c.write(_i2caddr, cmd, 2);
  //   cmd[0] = MMA8451_REG_CTRL_REG5;
  //   cmd[1] = 0x01;
  //   _i2c.write(_i2caddr, cmd, 2);

  //   // Turn on orientation config
  //   cmd[0] = MMA8451_REG_PL_CFG;
  //   cmd[1] = 0x40;
  //   _i2c.write(_i2caddr, cmd, 2);

  //     cmd[0] = MMA8451_REG_CTRL_REG1;
  //     cmd[1] = 0x01 | 0x04;
  //   _i2c.write(_i2caddr, cmd, 2);
}

void Accelerometer::read(float *x, float *y, float *z) {
  *x = getX();
  *y = getY();
  *z = getZ();

  //   cmd[0] = MMA8451_REG_OUT_X_MSB;
  //   _i2c.write(_i2caddr, cmd, 1);
  //   _i2c.read(_i2caddr, buf, 6);
  //   x_value = (buf[0] << 8 | buf[1] >> 2);
  //   y_value = (buf[2] << 8 | buf[3] >> 2);
  //   z_value = (buf[4] << 8 | buf[5] >> 2);

  //   *x = (float)x_value / 2048;
  //   *y = (float)y_value / 2048;
  //   *z = (float)z_value / 2048;
}

int16_t Accelerometer::get_axis(uint8_t addr) {
  char cmd[1] = {(char)addr};
  char buf[2];
  int16_t acc;
  _i2c.write(_i2caddr, cmd, 1, true);
  _i2c.read(_i2caddr, buf, 2);
  acc = (buf[0] << 6) | (buf[1] >> 2);
  if (acc > UINT14_MAX_VALUE / 2) {
    acc -= UINT14_MAX_VALUE;
  }
  return acc;
}

float Accelerometer::getX() {
  return (float(get_axis(MMA8451_REG_OUT_X_MSB)) / 4096.0);
}

float Accelerometer::getY() {
  return (float(get_axis(MMA8451_REG_OUT_Y_MSB)) / 4096.0);
}

float Accelerometer::getZ() {
  return (float(get_axis(MMA8451_REG_OUT_Z_MSB)) / 4096.0);
}