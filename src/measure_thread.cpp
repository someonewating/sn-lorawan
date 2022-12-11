// #include "measure_thread.h"
// #include "Accelerometer.h"
// #include "ColorSensor.h"
// #include "LightSensor.h"
// #include "PinNames.h"
// #include "RGBLed.h"
// #include "Resume.h"
// #include "SerialGPS.h"
// #include "SoilSensor.h"
// #include "TempHumSensor.h"
// #include "Timeout.h"
// #include "main.h"

// #include "mbed.h"
// #include <cstdint>
// #include <cstdio>
// #include <map>
// #include <queue>
// #include <string>

// #define ALERT_DELAY_TIME 500000

// LightSensor light_sensor(PA_4);
// TempHumSensor temp_hum(PB_9, PB_8);
// ColorSensor color_sensor(PB_9, PB_8);
// RGBLed rgb_led(PH_0, PH_1, PB_13);
// SoilSensor soil_sensor(PA_0);
// Accelerometer accel(PB_9, PB_8);
// SerialGPS gps(PA_9, PA_10, 9600);

// int color_value = OFF;
// float soil_value;
// float light_value;
// uint16_t clear, red, green, blue;
// string dominant;
// float hum, temp;
// float x, y, z;

// // detect if active the normal mode on the first time
// bool first_normal_mode = true;
// bool first_advance_mode = true;

// // resumes
// map<string, long> colors_counter;
// Resume temp_resume;
// Resume hum_resume;
// Resume light_resume;
// Resume soil_resume;
// Resume x_resume;
// Resume y_resume;
// Resume z_resume;

// // alert queue
// queue<int> alert_queue;
// Timeout alert_timeout;
// bool on_blink_alert = false;

// void init_resume() {
//   colors_counter = {{"RED", 0}, {"GREEN", 0}, {"BLUE", 0}};
//   temp_resume = Resume();
//   hum_resume = Resume();
//   light_resume = Resume();
//   soil_resume = Resume();
//   x_resume = Resume();
//   y_resume = Resume();
//   z_resume = Resume();
// }

// void prepare_queue_task(queue<int> &alert_queue, int iter) {
//   for (int i = 0; i < 10; i++) {
//     queue<int> temp = alert_queue;
//     while (!temp.empty()) {
//       alert_queue.push(temp.front());
//       temp.pop();
//     }
//   }
// }

// void alert() {
//   if (system_mode == ADVANCE_MODE && !alert_queue.empty()) {
//     on_blink_alert = true;
//     alert_timeout.detach();
//     alert_timeout.attach(&alert, 500ms);
//   }
// }

// void measure_thread() {

//   while (true) {
//     if (on_measure) {

//       soil_value = soil_sensor.read();
//       light_value = light_sensor.read();
//       color_sensor.read_raw_RGB(&clear, &red, &green, &blue);
//       dominant =
//           color_sensor.get_dominant_color(red, green, blue, &color_value);
//       hum = temp_hum.read_humidity();
//       temp = temp_hum.read_temperature();

//       x = accel.getX();
//       y = accel.getY();
//       z = accel.getZ();

//       gps.sample();

//       switch (system_mode) {
//       case TEST_MODE:
//         first_normal_mode = true;
//         first_advance_mode = true;
//         rgb_led.set_color(color_value);

//         break;
//       case NORMAL_MODE:
//         first_advance_mode = true;
//         if (first_normal_mode) {
//           init_resume();

//           first_normal_mode = false;
//         }

//         soil_resume.add_value(soil_value);
//         light_resume.add_value(light_value);
//         temp_resume.add_value(temp);
//         hum_resume.add_value(hum);
//         x_resume.add_value(x);
//         y_resume.add_value(y);
//         z_resume.add_value(z);
//         colors_counter[dominant] = colors_counter[dominant] + 1;

//         // limits
//         rgb_led.set_color(OFF);
//         if (temp > TEMP_UPPER || temp < TEMP_LOWER) {
//           rgb_led.set_color(RED);
//         }
//         if (hum > HUM_UPPER || hum < HUM_LOWER) {
//           rgb_led.set_color(BLUE);
//         }
//         if (light_value > LIGHT_UPPER || light_value < LIGHT_LOWER) {
//           rgb_led.set_color(WHITE);
//         }
//         if (soil_value > SOIL_UPPER || soil_value < SOIL_LOWER) {
//           rgb_led.set_color(GREEN);
//         }
//         if (red > RED_UPPER || red < RED_LOWER || green > GREEN_UPPER ||
//             green < GREEN_LOWER || blue > BLUE_UPPER || blue < BLUE_LOWER) {
//           rgb_led.set_color(VIOLET);
//         }
//         if (x > X_UPPER || x < X_LOWER || y > Y_UPPER || y < Y_LOWER ||
//             z > Z_UPPER || z < Z_LOWER) {
//           rgb_led.set_color(CYAN);
//         }
//         break;
//       case ADVANCE_MODE:
//         first_normal_mode = true;
//         if (first_advance_mode) {
//           init_resume();

//           first_advance_mode = false;
//         }

//         soil_resume.add_value(soil_value);
//         light_resume.add_value(light_value);
//         temp_resume.add_value(temp);
//         hum_resume.add_value(hum);
//         x_resume.add_value(x);
//         y_resume.add_value(y);
//         z_resume.add_value(z);
//         colors_counter[dominant] = colors_counter[dominant] + 1;

//         queue<int> empty;
//         swap(alert_queue, empty);
//         on_blink_alert = false;

//         alert_timeout.detach();

//         // limits
//         rgb_led.set_color(OFF);
//         if (temp > TEMP_UPPER || temp < TEMP_LOWER) {
//           rgb_led.set_color(RED);
//           alert_queue.push(RED);
//         }
//         if (hum > HUM_UPPER || hum < HUM_LOWER) {
//           rgb_led.set_color(BLUE);
//           alert_queue.push(BLUE);
//         }
//         if (light_value > LIGHT_UPPER || light_value < LIGHT_LOWER) {
//           rgb_led.set_color(WHITE);
//           alert_queue.push(WHITE);
//         }
//         if (soil_value > SOIL_UPPER || soil_value < SOIL_LOWER) {
//           rgb_led.set_color(GREEN);
//           alert_queue.push(GREEN);
//         }
//         if (red > RED_UPPER || red < RED_LOWER || green > GREEN_UPPER ||
//             green < GREEN_LOWER || blue > BLUE_UPPER || blue < BLUE_LOWER) {
//           rgb_led.set_color(VIOLET);
//           alert_queue.push(VIOLET);
//         }
//         if (x > X_UPPER || x < X_LOWER || y > Y_UPPER || y < Y_LOWER ||
//             z > Z_UPPER || z < Z_LOWER) {
//           rgb_led.set_color(CYAN);
//           alert_queue.push(CYAN);
//         }

//         int sizeToFill = 6 - alert_queue.size();
//         for (int i = 0; i < sizeToFill; i++) {
//           alert_queue.push(OFF);
//         }

//         queue<int> alert_queue_copy = alert_queue;
//         for (int i = 0; i < 9; i++) {
//           queue<int> temp = alert_queue_copy;
//           while (!temp.empty()) {
//             alert_queue.push(temp.front());
//             temp.pop();
//           }
//         }

//         rgb_led.set_color(alert_queue.front());
//         alert_queue.pop();

//         alert_timeout.attach(&alert, 500ms);
//         bool on_blink_alert = true;

//         break;
//       }

//       printf("SOIL MOISTURE: %.1f%%\n", soil_value);
//       printf("LIGHT: %.1f%%\n", light_value);

//       printf("GPS: #Sats: %.0d, Lat(UTC): %f, Long(UTC): %f, Altitude: %.1fm, "
//              "time: %d:%d:%d\n",
//              gps.sats, gps.latitude, gps.longitude, gps.alt, gps.hour + 2,
//              gps.min, gps.sec);

//       printf("COLOR SENSOR: Clear: %i, Red: %i, Green: %i, Blue: %d -- "
//              "Dominant color: %s\n",
//              clear, red, green, blue, dominant.c_str());

//       printf("ACCELEROMETER: X_axis: %f, Y_axis: %f, Z_axis: %f\n", x, y, z);

//       printf("TEMP/HUMIDITY Temperature: %.1f C, Humidity: %.1f%%  \n\n", temp,
//              hum);

//       // clear data
//       soil_value = NAN;
//       light_value = NAN;
//       clear = NAN;
//       red = NAN;
//       green = NAN;
//       blue = NAN;
//       dominant = NAN;
//       hum = NAN;
//       temp = NAN;
//       x = NAN;
//       y = NAN;
//       z = NAN;

//       on_measure = false;
//     }

//     if (on_resume) {
//       // send resume
//       string dominat;
//       if (colors_counter["RED"] > colors_counter["GREEN"] &&
//           colors_counter["RED"] > colors_counter["BLUE"]) {
//         dominant = "RED";
//       } else if (colors_counter["GREEN"] > colors_counter["RED"] &&
//                  colors_counter["GREEN"] > colors_counter["BLUE"]) {
//         dominant = "GREEN";
//       } else {
//         dominant = "BLUE";
//       }

//       printf("----RESUMES----\n");
//       printf("TEMP: Mean: %f, Max: %f, Min: %f\n", temp_resume.get_mean(),
//              temp_resume.get_max(), temp_resume.get_min());
//       printf("HUM: Mean: %f, Max: %f, Min: %f\n", hum_resume.get_mean(),
//              hum_resume.get_max(), hum_resume.get_min());
//       printf("LIGHT: Mean: %f, Max: %f, Min: %f\n", light_resume.get_mean(),
//              light_resume.get_max(), light_resume.get_min());
//       printf("SOIL: Mean: %f, Max: %f, Min: %f\n", soil_resume.get_mean(),
//              soil_resume.get_max(), soil_resume.get_min());
//       printf("DOMINANT COLOR: %s\n", dominant.c_str());
//       printf("ACCELEROMETER:\n");
//       printf("X: Max: %f, Min: %f\n", x_resume.get_max(), x_resume.get_min());
//       printf("Y: Max: %f, Min: %f\n", y_resume.get_max(), y_resume.get_min());
//       printf("Z: Max: %f, Min: %f\n\n", z_resume.get_max(), z_resume.get_min());

//       // reset resume
//       init_resume();
//       on_resume = false;
//     }

//     if (on_blink_alert) {
//       if (!alert_queue.empty()) {
//         rgb_led.set_color(alert_queue.front());
//         alert_queue.pop();
//       }
//       on_blink_alert = false;
//     }
//   }
// }