#pragma once

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DRIVER_NUM_HUMIDITY 0x60001

// check if humidity sensor exists
bool humidity_exists(void);

// units: humidity in hundredths of percent.

// function to be called when the humidity measurement is finished
//
// callback       - pointer to function to be called
// callback_args  - pointer to data provided to the callback
int humidity_set_callback (subscribe_upcall callback, void* callback_args);

// initiate an humidity measurement used both for synchronous and asynchronous readings
int humidity_read(void);

// initiate a synchronous humidity measurement
//
// humi           - pointer/address where the result of the humidity reading should be stored
int humidity_read_sync (unsigned* humi);

#ifdef __cplusplus
}
#endif
