#pragma once
#include "DxLr30Board.h"
#include <helpers/radiolib/CustomSX1262Wrapper.h>
#include <helpers/ArduinoHelpers.h>
#include <helpers/sensors/EnvironmentSensorManager.h>

extern DxLr30Board board;
extern WRAPPER_CLASS radio_driver;
extern VolatileRTCClock rtc_clock;
extern EnvironmentSensorManager sensors;

bool radio_init();
mesh::LocalIdentity radio_new_identity();

