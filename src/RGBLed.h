#include "BusOut.h"
#include "PinNames.h"
#include "PwmOut.h"
#include "mbed.h"
#include <cstdint>

#define OFF 7
#define WHITE 0
#define RED 6
#define BLUE 3
#define GREEN 5
#define YELLOW 4
#define VIOLET 2
#define CYAN 1

class RGBLed {
public:
  RGBLed(PinName red, PinName green, PinName blue);
  void set_color(int color);

private:
  BusOut _output;
};