/*
 * drive.c
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#include "main.h"
#include "math.h"
#include "drive.h"
#include "vector.h"
#include "../include/utils.h"

#define MOTOR_PORT_DRIVE_LEFT		8
#define MOTOR_PORT_DRIVE_RIGHT		3
#define MOTOR_PORT_DRIVE_BACK		2
#define RAD_CONVERSION 				M_PI / 180
#define TURNING_SPEED				1

void driveDirection(float transX, float transY, float rotation) {
	float leftWheelPower = (.160458 * transX) + (0.57735 * transY)
			+ (0.42 * rotation);
	float rightWheelPower = (.160458 * transX) - (0.57735 * transY)
			+ (0.42 * rotation);
	float backWheelPower = 0.83954 * transX - (0.42 * rotation);

	leftWheelPower = limit(leftWheelPower, -1, 1);
	rightWheelPower = limit(rightWheelPower, -1, 1);
	backWheelPower = limit(backWheelPower, -1, 1);

	leftWheelPower *= 127;
	rightWheelPower *= 127;
	backWheelPower *= 127;

	driveMotor(leftWheelPower, rightWheelPower, backWheelPower);
}

void driveVector(Vector vec, float rot) {

	Vector leftWheelVector = fromAngle(30 * RAD_CONVERSION);
	Vector rightWheelVector = fromAngle(150 * RAD_CONVERSION);
	Vector backWheelVector = fromAngle(270 * RAD_CONVERSION);

	float leftWheelPower = scalarProjection(vec, leftWheelVector) + rot * TURNING_SPEED;
	float rightWheelPower = scalarProjection(vec, rightWheelVector) + rot * TURNING_SPEED;
	float backWheelPower = scalarProjection(vec, backWheelVector) + rot * TURNING_SPEED;

	leftWheelPower = limit(leftWheelPower, -1, 1);
	rightWheelPower = limit(rightWheelPower, -1, 1);
	backWheelPower = limit(backWheelPower, -1, 1);

	leftWheelPower *= 127;
	rightWheelPower *= 127;
	backWheelPower *= 127;

	driveMotor(leftWheelPower, rightWheelPower, backWheelPower);

}

void driveMotor(int wheelLeft, int wheelRight, int wheelBack) {
	motorSet(MOTOR_PORT_DRIVE_LEFT, wheelLeft * 127);
	motorSet(MOTOR_PORT_DRIVE_RIGHT, wheelRight * 127);
	motorSet(MOTOR_PORT_DRIVE_BACK, wheelBack * 127);
}
