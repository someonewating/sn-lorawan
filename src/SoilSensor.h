#include "AnalogIn.h"
#include "PinNames.h"
#include "mbed.h"

class SoilSensor {
public:
  SoilSensor(PinName input);
  float read();

private:
  AnalogIn _input;
};
