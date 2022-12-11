#include "ColorSensor.h"
#include "PinNames.h"
#include "RGBLed.h"
#include "mbed.h"
#include <cstdio>
#include <string>

ColorSensor::ColorSensor(PinName sda, PinName scl) : _i2c(sda, scl) {
  _i2cadrr = (TCS34725_DEFAULT_ADDR << 1);
  char cmd[2];

  // // ATIME 101 ms
  // cmd[0] = TCS34725_REG_ATIME | 0x80;
  // cmd[1] = 0xD5;
  // _i2c.write(_i2cadrr, cmd, 2);

  // // 1x Gain
  // cmd[0] = TCS34725_REG_CONTROL | 0x80;
  // cmd[1] = 0x00;
  // _i2c.write(_i2cadrr, cmd, 2);
  // wait_us(4000);

  // Enable with PON
  cmd[0] = TCS34725_REG_ENABLE | 0x80;
  cmd[1] = 0x01;
  _i2c.write(_i2cadrr, cmd, 2);
  wait_us(4000);

  // active AEN
  cmd[1] = 0x01 | 0x02;
  _i2c.write(_i2cadrr, cmd, 2);
  wait_us(4000);
}

void ColorSensor::read_raw_RGB(uint16_t *clear, uint16_t *red, uint16_t *green,
                               uint16_t *blue) {
  char cmd[1] = {TCS34725_REG_CDATAL | 0x80};
  char buf[8];

  _i2c.write(_i2cadrr, cmd, 1);
  _i2c.read(_i2cadrr, buf, 8);

  *clear = (uint16_t(buf[1]) << 8) | (uint16_t(buf[0]) & 0xFF);
  *red = (uint16_t(buf[3]) << 8) | (uint16_t(buf[2]) & 0xFF);
  *green = (uint16_t(buf[5]) << 8) | (uint16_t(buf[4]) & 0xFF);
  *blue = (uint16_t(buf[7]) << 8) | (uint16_t(buf[6]) & 0xFF);

  //   char cmd[1] = {TCS34725_REG_CDATAL | 0x80};
  //   char buf[2];

  //   _i2c.write(_i2cadrr, cmd, 1);
  //   _i2c.read(_i2cadrr, buf, 2);
  //   c = (buf[1] << 8 | buf[0]);
  //   printf("C %d\n", c);

  //   cmd[0] = TCS34725_REG_RDATAL | 0x80;
  //   _i2c.write(_i2cadrr, cmd, 1);
  //   _i2c.read(_i2cadrr, buf, 2);
  //   r = (buf[1] << 8 | buf[0]);
  //   printf("C %d\n", r);

  //   cmd[0] = TCS34725_REG_GDATAL | 0x80;
  //   _i2c.write(_i2cadrr, cmd, 1);
  //   _i2c.read(_i2cadrr, buf, 2);
  //   g = (buf[1] << 8 | buf[0]);
  //   printf("C %d\n", g);

  //   cmd[0] = TCS34725_REG_BDATAL | 0x80;
  //   _i2c.write(_i2cadrr, cmd, 1);
  //   _i2c.read(_i2cadrr, buf, 2);
  //   b = (buf[1] << 8 | buf[0]);
  //   printf("C %d\n", b);

  wait_us(3000);
}

string ColorSensor::get_dominant_color(uint16_t red, uint16_t green,
                                       uint16_t blue, int *RGB_led_value) {
  if (red > green && red > blue) {
    *RGB_led_value = RED;
    return "RED";
  } else if (green > red && green > blue) {
    *RGB_led_value = GREEN;
    return "GREEN";
  } else {
    *RGB_led_value = BLUE;
    return "BLUE";
  }
}

void ColorSensor::read_RGB(float *red, float *green, float *blue) {
  uint16_t r, g, b, c;
  read_raw_RGB(&c, &r, &g, &b);

  uint32_t sum = c;
  if (c == 0) {
    *red = *green = *blue = 0;
    return;
  }

  *red = (float)r / sum * 255.0;
  *green = (float)g / sum * 255.0;
  *blue = (float)b / sum * 255.0;

  return;
}
