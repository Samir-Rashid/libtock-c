#pragma once

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DRIVER_NUM_TEMPERATURE 0x60000

// check if temperature sensor exists
bool temperature_exists(void);

// units: temperature in hundredths of degrees centigrade.

// function to be called when the temperature measurement is finished
//
// callback       - pointer to function to be called
// callback_args  - pointer to data provided to the callback
int temperature_set_callback (subscribe_upcall callback, void* callback_args);

// initiate an ambient temperature measurement used both for synchronous and asynchronous readings
int temperature_read(void);


// initiate a synchronous ambient temperature measurement
//
// temperature     - pointer/address where the result of the temperature reading should be stored
int temperature_read_sync (int* temperature);

#ifdef __cplusplus
}
#endif
