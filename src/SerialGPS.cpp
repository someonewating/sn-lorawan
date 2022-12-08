#include "SerialGPS.h"
#include "mbed.h"
#include "string.h"
#include <cstdio>
#include <string>

SerialGPS::SerialGPS(PinName tx, PinName rx, int baud) : _gps(tx, rx, baud) {
  longitude = 0.0;
  latitude = 0.0;
}

int SerialGPS::sample() {
  char ns, ew, unit;
  int lock;

  while (1) {
    getline();

    if (sscanf(_msg, "GPGGA,%f,%f,%c,%f,%c,%d,%d,%f,%f,%c,%f", &_time,
               &latitude, &ns, &longitude, &ew, &lock, &sats, &hdop, &alt,
               &unit, &geoid) >= 1) {
      if (!lock) {
        _time = 0.0;
        longitude = 0.0;
        latitude = 0.0;
        sats = 0;
        hdop = 0.0;
        alt = 0.0;
        geoid = 0.0;
        return 0;
      } else {

        if (ns == 'S') {
          latitude *= -1.0;
        }
        if (ew == 'W') {
          longitude *= -1.0;
        }
        float degrees = trunc(latitude / 100.0f);
        float minutes = latitude - (degrees * 100.0f);
        latitude = degrees + minutes / 60.0f;
        degrees = trunc(longitude / 100.0f * 0.01f);
        minutes = longitude - (degrees * 100.0f);
        longitude = degrees + minutes / 60.0f;

        string string_time = std::to_string((int)_time);
        sscanf(string_time.c_str(), "%2d%2d%2d", &hour, &min, &sec);

        return 1;
      }
    }
  }
}

float SerialGPS::trunc(float v) {
  if (v < 0.0) {
    v *= -1.0;
    v = floor(v);
    v *= -1.0;
  } else {
    v = floor(v);
  }
  return v;
}

void SerialGPS::getline() {
  char c[1];
  _gps.read(c, 1);
  while (c[0] != '$') {
    _gps.read(c, 1);
  }; // wait for the start of a line

  for (int i = 0; i < 256; i++) {
    _gps.read(c, 1);
    _msg[i] = c[0];
    if (_msg[i] == '\r') {
      _msg[i] = 0;
      return;
    }
  }
  error("Overflowed message limit");
}