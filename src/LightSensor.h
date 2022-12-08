#include "AnalogIn.h"
#include "PinNames.h"
#include "mbed.h"

class LightSensor {
public:
  LightSensor(PinName input);
  float read();

private:
  AnalogIn _input;
};