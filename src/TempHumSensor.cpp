#include "TempHumSensor.h"
#include "PinNames.h"
#include "mbed.h"
using namespace std::chrono;

TempHumSensor::TempHumSensor(PinName sda, PinName scl) : _i2c(sda, scl) {
  _i2caddr = (SI7021_DEFAULT_ADDR << 1);
}

float TempHumSensor::read_humidity() {
  char cmd[1] = {SI7021_REG_NO_HOLD_HUMIDITY};
  char buf[3];
  if (_i2c.write(_i2caddr, cmd, 1) != 0)
    return NAN;

  //   wait_us(6000);

  Timer t;
  t.start();
  while (_i2c.read(_i2caddr, buf, 3) != 0) {
    if (duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count() >
        100)
      return NAN;
    wait_us(6000);
  }

  float humidity = (buf[0] << 8 | buf[1]) * 125;
  humidity /= 65536;
  humidity -= 6;
  return humidity;
}

float TempHumSensor::read_temperature() {
  char cmd[1] = {SI7021_REG_NO_HOLD_TEMPERATURE};
  char buf[3];
  if (_i2c.write(_i2caddr, cmd, 1) != 0)
    return NAN;

  //   wait_us(6000);

  Timer t;
  t.start();
  while (_i2c.read(_i2caddr, buf, 3) != 0) {
    if (duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count() >
        100)
      return NAN;
    wait_us(6000);
  }

  float temperature = buf[0] << 8 | buf[1];
  temperature *= 175.72;
  temperature /= 65536;
  temperature -= 46.85;

  return temperature;
}
