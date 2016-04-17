/*
 * Utils.c
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#include "main.h"

/*------------------------------------------------------
 * 				  Utility Functions					   |
 *------------------------------------------------------
 */

/**
 * Coerces the value between the minimum and the maximum. If the value is outside either limit,
 * it will be set to that limit
 *
 * @param value The value to coerce
 * @param min The minimum of the value's range
 * @param max The maximum of the values range
 * @return value The coerced value
 */
float limit(float value, float min, float max) {
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	}
	return value;
}

/**
 * A flag that acts like a 3 position toggle swtich. The high input will set the flag to +1,
 * and the low input will set the flag to -1. If the input is triggered when the flag is
 * already set to the input's respective state, the flag will shut off.
 *
 * @param highInput The boolean to toggle between forward and off
 * @param lowInput The boolean to toggle between backwards and off
 * @return flag	The value of the flag.
 */

int latchFlag(bool highInput, bool lowInput, int flag) {
	if (highInput) {
		if (flag == 1) {
			return 0;
		} else {
			return 1;
		}
	} else if (lowInput) {
		if (flag == -1) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return flag;
	}
}

/**
 * A standard bang-bang controller. Full power forward if the process variable is above the
 * setpoint, off if the variable is below the setpoint, and half power if the variable and
 * setpoint are equal.
 *
 * @param setpoint The goal operating point
 * @param var The process variable
 */

float bangBangController(int setpoint, float var) {
	if (var < setpoint) {
		return 1.0;
	} else if (var > setpoint) {
		return 0;
	} else {
		return 0.5;
	}
}

/**
 * A standard PID controller.
 *
 * @param Kp Proportional gain
 * @param Ki Integral gain
 * @param Kd Derivative gain
 * @param setpoint Goal operating state of the mechanism
 * @param processVar Sensor value measuring what you're trying to control
 * @return output Motor power between -1 and 1
 */

float errorSum = 0;
float lastErr = 0;
float pidController(float Kp, float Ki, float Kd, float dt, float setpoint,
		float processVar) {
	float error = setpoint - processVar;

	float p = Kp * error;
	float i = Ki * errorSum * dt / 1000;
	float d = Kd * (error - lastErr) * dt / 1000;

	lastErr = error;
	errorSum += error;

	float output = p + i + d;
	output = limit(output, -1, 1);

	return output;
}

void resetIntegral() {
	errorSum = 0;
}

