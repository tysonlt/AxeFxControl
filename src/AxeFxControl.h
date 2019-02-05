#pragma once

#define AXE_DEBUG
#define AXE_DEBUG_SYSEX
#define DEBUGGER 													Serial

#define REFRESH_INTERVAL 									5000
#define REFRESH_THROTTLE 									1000
#define PRESET_CHANGE_THROTTLE 						200
#define DEFAULT_DEBOUNCE									25
#define BUTTON_HOLD_MILLIS 								800
#define BOOT_DELAY 												1000
#define TIME_LIMIT_SYSEX									REFRESH_INTERVAL + 3000 // after this, axe will be considered disconnected (needs realtime sysex!)
#define TIME_LIMIT_TUNER									250 // after this, we will assume the tuner was disabled from the front panel (needs realtime sysex!)

#define PRESET_CHANGING_MESSAGE 					"Changing..."

#include "axe/AxeSystem.h"