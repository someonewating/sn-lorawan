/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdint>
#include <cstdio>

#include "mbed_version.h"

#include "events/EventQueue.h"
#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"

// Application helpers
#include "lora_radio_helper.h"
#include "trace_helper.h"

#include <string>

#include "Accelerometer.h"
#include "ColorSensor.h"
#include "LightSensor.h"
#include "RGBLed.h"
#include "SerialGPS.h"
#include "SoilSensor.h"
#include "TempHumSensor.h"


using namespace events;
using namespace std::chrono_literals;

// using namespace std;

// Max payload size can be LORAMAC_PHY_MAXPAYLOAD.
// This example only communicates with much shorter messages (<30 bytes).
// If longer messages are used, these buffers must be changed accordingly.
uint8_t tx_buffer[30];
uint8_t rx_buffer[30];

/*
 * Sets up an application dependent transmission timer in ms. Used only when
 * Duty Cycling is off for testing
 */
#define TX_TIMER 10s

/**
 * Maximum number of events for the event queue.
 * 10 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS 10

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER 3

LightSensor light_sensor(PA_4);
TempHumSensor temp_hum(PB_9, PB_8);
ColorSensor color_sensor(PB_9, PB_8);
RGBLed rgb_led(PH_0, PH_1, PB_13);
SoilSensor soil_sensor(PA_0);
Accelerometer accel(PB_9, PB_8);
SerialGPS gps(PA_9, PA_10, 9600);

/**
 * This event queue is the global event queue for both the
 * application and stack. To conserve memory, the stack is designed to run
 * in the same thread as the application and the application is responsible for
 * providing an event queue to the stack that will be used for ISR deferment as
 * well as application information event queuing.
 */
static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS *EVENTS_EVENT_SIZE);

/**
 * Event handler.
 *
 * This will be passed to the LoRaWAN stack to queue events for the
 * application which in turn drive the application.
 */
static void lora_event_handler(lorawan_event_t event);

/**
 * Constructing Mbed LoRaWANInterface and passing it the radio object from
 * lora_radio_helper.
 */
static LoRaWANInterface lorawan(radio);

/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;

/**
 * Default and configured device EUI, application EUI and application key
 */
static const uint8_t DEFAULT_DEV_EUI[] = {0x40, 0x39, 0x32, 0x35,
                                          0x59, 0x37, 0x91, 0x94};
static uint8_t DEV_EUI[] = {0x7e, 0x39, 0x32, 0x35, 0x59, 0x37, 0x91, 0x94};
static uint8_t APP_EUI[] = {0x70, 0xb3, 0xd5, 0x7e, 0xd0, 0x00, 0xfc, 0x4d};
static uint8_t APP_KEY[] = {0xf3, 0x1c, 0x2e, 0x8b, 0xc6, 0x71, 0x28, 0x1d,
                            0x51, 0x16, 0xf0, 0x8f, 0xf0, 0xb7, 0x92, 0x8f};

/**
 * Entry point for application
 */
int main(void) {
  rgb_led.set_color(OFF);
  printf("\r\n*** Sensor Networks @ ETSIST, UPM ***\r\n"
         "   Mbed (v%d.%d.%d) LoRaWAN example\r\n",
         MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

  printf("\r\n DEV_EUI: ");
  for (int i = 0; i < sizeof(DEV_EUI); ++i)
    printf("%02x", DEV_EUI[i]);
  printf("\r\n APP_EUI: ");
  for (int i = 0; i < sizeof(APP_EUI); ++i)
    printf("%02x", APP_EUI[i]);
  printf("\r\n APP_KEY: ");
  for (int i = 0; i < sizeof(APP_KEY); ++i)
    printf("%02x", APP_KEY[i]);
  printf("\r\n");

  if (!memcmp(DEV_EUI, DEFAULT_DEV_EUI, sizeof(DEV_EUI))) {
    printf("\r\n *** You are using the default device EUI value!!! *** \r\n");
    printf("Please, change it to ensure that the device EUI is unique \r\n");
    return -1;
  }

  // setup tracing
  setup_trace();

  // stores the status of a call to LoRaWAN protocol
  lorawan_status_t retcode;

  // Initialize LoRaWAN stack
  if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
    printf("\r\n LoRa initialization failed! \r\n");
    return -1;
  }

  printf("\r\n Mbed LoRaWANStack initialized \r\n");

  // prepare application callbacks
  callbacks.events = mbed::callback(lora_event_handler);
  lorawan.add_app_callbacks(&callbacks);

  // Set number of retries in case of CONFIRMED messages
  if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER) !=
      LORAWAN_STATUS_OK) {
    printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
    return -1;
  }

  printf("\r\n CONFIRMED message retries : %d \r\n",
         CONFIRMED_MSG_RETRY_COUNTER);

  // Enable adaptive data rate
  if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
    printf("\r\n enable_adaptive_datarate failed! \r\n");
    return -1;
  }

  printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");

  lorawan_connect_t connect_params;
  connect_params.connect_type = LORAWAN_CONNECTION_OTAA;
  connect_params.connection_u.otaa.dev_eui = DEV_EUI;
  connect_params.connection_u.otaa.app_eui = APP_EUI;
  connect_params.connection_u.otaa.app_key = APP_KEY;
  connect_params.connection_u.otaa.nb_trials = 3;

  retcode = lorawan.connect(connect_params);

  if (retcode == LORAWAN_STATUS_OK ||
      retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
  } else {
    printf("\r\n Connection error, code = %d \r\n", retcode);
    return -1;
  }

  printf("\r\n Connection - In Progress ...\r\n");

  // make your event queue dispatching events forever
  ev_queue.dispatch_forever();

  return 0;
}

/**
 * Sends a message to the Network Server
 */
static void send_message() {
  uint16_t packet_len;
  int16_t retcode;
  int8_t light, soil;
  int8_t tem, hum;
  int8_t x, y, z;
  uint16_t clear, red, green, blue;
  float lat, lon;

  std::string space = " ";

  if (temp_hum.read_temperature() > 0) {
    light = light_sensor.read();

    soil = soil_sensor.read();

    tem = temp_hum.read_temperature();
    hum = temp_hum.read_humidity();

    x = accel.getX();
    y = accel.getY();
    z = accel.getZ();

    color_sensor.read_raw_RGB(&clear, &red, &green, &blue);

    gps.sample();
    lat = gps.latitude;
    lon = gps.longitude;
    if (lat == 0) {
        lon = -3.6290497507855557;
        lat = 40.389562770878904;
    }

    printf("Longitude: %6.4f, latitude: %6.4f \r\n", lon, lat);
    printf("Light Sensor Value = %d \r\n", light);
    printf("Soil Sensor Value = %d \r\n", soil);
    printf("Tem&Hum Sensor Value = %d, %d \r\n", tem, hum);
    printf("Accelerometer Sensor Value = %d, %d, %d \r\n", x, y, z);
    printf("Color Sensor Value = %d, %d, %d, %d \r\n", clear, red, green, blue);
  } else {
    printf("\r\n No sensor found \r\n");
    return;
  }

  packet_len =
      snprintf((char *)tx_buffer, sizeof(tx_buffer),
               "%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d", light, space.c_str(), soil,
               space.c_str(), tem, space.c_str(), hum, space.c_str(), clear,
               space.c_str(), red, space.c_str(), green, space.c_str(), blue);

  retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len,
                         MSG_UNCONFIRMED_FLAG);

  if (retcode < 0) {
    retcode == LORAWAN_STATUS_WOULD_BLOCK
        ? printf("send - WOULD BLOCK\r\n")
        : printf("\r\n send() - Error code %d \r\n", retcode);

    if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
      // retry in 3 seconds
      if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
        ev_queue.call_in(3s, send_message);
      }
    }
    return;
  }

  printf("\r\n %d bytes scheduled for transmission \r\n", retcode);
  printf("%s \n", tx_buffer);
  memset(tx_buffer, 0, sizeof(tx_buffer));
}

/**
 * Receive a message from the Network Server
 */
static void receive_message() {
  uint8_t port;
  int flags;
  int16_t retcode = lorawan.receive(rx_buffer, sizeof(rx_buffer), port, flags);
  uint8_t green[6], off[4], red[4];

  if (retcode < 0) {
    printf("\r\n receive() - Error code %d \r\n", retcode);
    return;
  }

  printf(" RX Data on port %u (%d bytes): ", port, retcode);
  for (uint8_t i = 0; i < retcode; i++) {
    printf("%02x ", rx_buffer[i]);
  }
  printf("\r\n");

  printf("%s \n", rx_buffer);

  snprintf((char *)green, sizeof(green), "%s", "Green");
  if (strcmp((const char *)rx_buffer, (const char *)green) == 0) {
    rgb_led.set_color(GREEN);
  }

  snprintf((char *)off, sizeof(off), "%s", "OFF");
  if (strcmp((const char *)rx_buffer, (const char *)off) == 0) {
    rgb_led.set_color(OFF);
  }

  snprintf((char *)red, sizeof(red), "%s", "Red");
  if (strcmp((const char *)rx_buffer, (const char *)red) == 0) {
    rgb_led.set_color(RED);
  }

  memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * Event handler
 */
static void lora_event_handler(lorawan_event_t event) {
  switch (event) {
  case CONNECTED:
    printf("\r\n Connection - Successful \r\n");
    if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
      send_message();
    } else {
      ev_queue.call_every(TX_TIMER, send_message);
    }

    break;
  case DISCONNECTED:
    ev_queue.break_dispatch();
    printf("\r\n Disconnected Successfully \r\n");
    break;
  case TX_DONE:
    printf("\r\n Message Sent to Network Server \r\n");
    if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
      send_message();
    }
    break;
  case TX_TIMEOUT:
  case TX_ERROR:
  case TX_CRYPTO_ERROR:
  case TX_SCHEDULING_ERROR:
    printf("\r\n Transmission Error - EventCode = %d \r\n", event);
    // try again
    if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
      send_message();
    }
    break;
  case RX_DONE:
    printf("\r\n Received message from Network Server \r\n");
    receive_message();
    break;
  case RX_TIMEOUT:
  case RX_ERROR:
    printf("\r\n Error in reception - Code = %d \r\n", event);
    break;
  case JOIN_FAILURE:
    printf("\r\n OTAA Failed - Check Keys \r\n");
    break;
  case UPLINK_REQUIRED:
    printf("\r\n Uplink required by NS \r\n");
    if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
      send_message();
    }
    break;
  default:
    MBED_ASSERT("Unknown Event");
  }
}

// EOF
