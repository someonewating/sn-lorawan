#include "float.h"
#include <cstdlib>

#define TEMP_UPPER 50
#define TEMP_LOWER -10
#define HUM_UPPER 75
#define HUM_LOWER 25
#define LIGHT_UPPER 80
#define LIGHT_LOWER 0
#define SOIL_UPPER 80
#define SOIL_LOWER 0
#define RED_UPPER FLT_MAX
#define RED_LOWER 10
#define GREEN_UPPER FLT_MAX
#define GREEN_LOWER 10
#define BLUE_UPPER FLT_MAX
#define BLUE_LOWER 10
#define X_UPPER 1.1
#define X_LOWER -1.1
#define Y_UPPER 1.1
#define Y_LOWER -1.0
#define Z_UPPER 1.1
#define Z_LOWER 0.5

extern int system_mode;
extern bool on_measure;
extern bool on_resume;

void measure_thread();