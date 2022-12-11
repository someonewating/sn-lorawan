#include "LightSensor.h"
#include "PinNames.h"
#include "mbed.h"

LightSensor::LightSensor(PinName input) : _input(input) {}

float LightSensor::read() { return _input * 100; }
