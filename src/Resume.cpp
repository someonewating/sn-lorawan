#include "Resume.h"
#include "float.h"

Resume::Resume() {
  _sum = 0;
  _max_value = DBL_MIN;
  _min_value = DBL_MAX;
  _counter = 0;
}

void Resume::add_value(double value) {
  _sum += value;
  if (value > _max_value) {
    _max_value = value;
  }
  if (value < _min_value) {
    _min_value = value;
  }
  _counter += 1;
}

double Resume::get_mean() {
  if (_counter == 0) {
    return 0;
  }
  return _sum / _counter;
}

double Resume::get_max() { return _max_value; }

double Resume::get_min() { return _min_value; }