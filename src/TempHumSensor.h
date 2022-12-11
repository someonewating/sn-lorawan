#include "I2C.h"
#include "PinNames.h"
#include "mbed.h"

#define SI7021_DEFAULT_ADDR 0x40
#define SI7021_REG_NO_HOLD_HUMIDITY 0xF5
#define SI7021_REG_NO_HOLD_TEMPERATURE 0xF3

class TempHumSensor {

public:
  TempHumSensor(PinName sda, PinName scl);
  float read_temperature();
  float read_humidity();

private:
  I2C _i2c;
  int _i2caddr;
};