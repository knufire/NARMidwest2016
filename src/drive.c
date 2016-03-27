/*
 * drive.c
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#include "main.h"
#include "math.h"
#include "drive.h"

#include "../include/utils.h"

#define MOTOR_PORT_DRIVE_LEFT		8
#define MOTOR_PORT_DRIVE_RIGHT		3
#define MOTOR_PORT_DRIVE_BACK		2

void driveVector(float transX, float transY, float rotation) {
	float wheelLeft = (.160458 * transX) + (0.57735 * transY)
			+ (0.42 * rotation);
	float wheelRight = (.160458 * transX) - (0.57735 * transY)
			+ (0.42 * rotation);
	float wheelBack = 0.83954 * transX - (0.42 * rotation);

	wheelLeft = limit(wheelLeft, -1, 1);
	wheelRight = limit(wheelRight, -1, 1);
	wheelBack = limit(wheelBack, -1, 1);

	wheelLeft *= 127;
	wheelRight *= 127;
	wheelBack *= 127;

	driveMotor(wheelLeft, wheelRight, wheelBack);
}

void driveMotor(int wheelLeft, int wheelRight, int wheelBack) {
	motorSet(MOTOR_PORT_DRIVE_LEFT, wheelLeft * 127);
	motorSet(MOTOR_PORT_DRIVE_RIGHT, wheelRight * 127);
	motorSet(MOTOR_PORT_DRIVE_BACK, wheelBack * 127);
}
