#include "stop.h"


int Stop_run() {
	Devices_controlDevice(MOTOR_TAIL_, 82);
	Devices_controlDevice(MOTOR_LEFT_, 0);
	Devices_controlDevice(MOTOR_RIGHT_, 0);

	if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
		return LINE_TRACE_RUN;
	}
	return STOP;
}
