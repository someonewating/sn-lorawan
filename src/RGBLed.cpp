#include "RGBLed.h"
#include "mbed.h"

RGBLed::RGBLed(PinName red, PinName green, PinName blue)
    : _output(red, green, blue) {}

void RGBLed::set_color(int color) { _output.write(color); }