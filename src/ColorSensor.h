#include "I2C.h"
#include "PinNames.h"
#include "mbed.h"
#include <cstdint>

#define TCS34725_DEFAULT_ADDR 0x29
#define TCS34725_REG_ENABLE 0x00
#define TCS34725_REG_CDATAL 0x14
#define TCS34725_REG_RDATAL 0x16
#define TCS34725_REG_GDATAL 0x18
#define TCS34725_REG_BDATAL 0x1A
#define TCS34725_REG_ATIME 0x01
#define TCS34725_REG_WTIME 0x03
#define TCS34725_REG_CONTROL 0x0F
#define TCS34725_REG_ID 0x12

class ColorSensor {
public:
  ColorSensor(PinName sda, PinName scl);
  void read_raw_RGB(uint16_t *clear, uint16_t *red, uint16_t *green,
                    uint16_t *blue);
  void read_RGB(float *red, float *green, float *blue);
  string get_dominant_color(uint16_t red, uint16_t green, uint16_t blue,
                            int *RGB_led_value);

private:
  I2C _i2c;
  int _i2cadrr;
};