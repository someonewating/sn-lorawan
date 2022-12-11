#include "mbed.h"

class SerialGPS {
public:
  SerialGPS(PinName tx, PinName rx, int baud);

  int sample();

  float longitude;

  float latitude;

  int sats;

  float hdop;

  float alt;

  float geoid;

  int hour, min, sec;

  void location();

private:
  float trunc(float v);
  void getline();

  char _msg[256];
  float _time;
  BufferedSerial _gps;
};
