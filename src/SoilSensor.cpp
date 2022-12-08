#include "SoilSensor.h"
#include "PinNames.h"
#include "mbed.h"

SoilSensor::SoilSensor(PinName input) : _input(input) {}

float SoilSensor::read() { return (_input / 0.88) * 100; }
