#pragma once

#define AXE_DEBUG
// #define AXE_DEBUG_SYSEX
#define DEBUGGER 								Serial

#define REFRESH_INTERVAL 				5000
#define REFRESH_THROTTLE 				1000
#define PRESET_CHANGE_THROTTLE 	200
#define TIME_LIMIT_SYSEX				REFRESH_INTERVAL + 3000 // after this, axe will be considered disconnected (needs realtime sysex!)
#define TIME_LIMIT_TUNER				250 // after this, we will assume the tuner was disabled from the front panel (needs realtime sysex!)

#define INIT_DELAY							1000 //Teesies seem to appreciate a delay before sending first sysex
// #define SYSEX_THROTTLE					1 //delay before sending each sysex

#include "axe/AxeSystem.h"