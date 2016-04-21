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
#include "gyro.h"

#define MOTOR_PORT_DRIVE_LEFT		8
#define MOTOR_PORT_DRIVE_RIGHT		3
#define MOTOR_PORT_DRIVE_BACK_FWD	2
#define MOTOR_PORT_DRIVE_BACK_REV	9
#define RAD_CONVERSION 				M_PI / 180
#define TURNING_SPEED				1

#define leftWheelVector  			fromAngle(30 * RAD_CONVERSION)
#define rightWheelVector 			fromAngle(150 * RAD_CONVERSION)
#define backWheelVector				fromAngle(270 * RAD_CONVERSION)

void driveDirection(float transX, float transY, float rotation) {
	float leftWheelPower = (.160458 * transX) + (0.57735 * transY)
			+ (0.42 * rotation);
	float rightWheelPower = (.160458 * transX) - (0.57735 * transY)
			+ (0.42 * rotation);
	float backWheelPower = 0.83954 * transX - (0.42 * rotation);

	driveMotor(leftWheelPower, rightWheelPower, backWheelPower);
}

void driveVector(Vector translate, float rotation) {

	//Calculate motor power by projecting the desired translation
	//vector onto each wheel

	float leftWheelPower = scalarProjection(translate,
	leftWheelVector) + rotation * TURNING_SPEED;
	float rightWheelPower = scalarProjection(translate,
	rightWheelVector) + rotation * TURNING_SPEED;
	float backWheelPower = scalarProjection(translate,
	backWheelVector) + rotation * TURNING_SPEED;

	driveMotor(leftWheelPower, rightWheelPower, backWheelPower);
}

void driveGyro(Vector translate, float rotation) {
	//TODO: On falling edge of rotation stick, reset the integral term in the gyro correction PID loop

	if (fabs(rotation) > 0.05) {// If we're rotating, ignore gyro and just do
		driveVector(translate, rotation);	// normal open loop driving.
		setDesiredHeading();
	} else {
		//Calculate gyro correction from PID loop and drive motor power like normal
		float gyroCorrection = getGyroCorrection();
		float leftWheelPower = scalarProjection(translate,
		leftWheelVector) + rotation * TURNING_SPEED;
		float rightWheelPower = scalarProjection(translate,
		rightWheelVector) + rotation * TURNING_SPEED;
		float backWheelPower = scalarProjection(translate,
		backWheelVector) + rotation * TURNING_SPEED;

		//This is how 1114 did their gyro correction. Don't really understand why,
		//but it worked for them.
		float maxOut = fmax(fmax(leftWheelPower, rightWheelPower),
				backWheelPower);
		float strafeMagnitude = magnitude(translate);
		if (maxOut != 0) {
			leftWheelPower /= maxOut;
			rightWheelPower /= maxOut;
			backWheelPower /= maxOut;
		}

		leftWheelPower = leftWheelPower * strafeMagnitude + gyroCorrection;
		rightWheelPower = rightWheelPower * strafeMagnitude + gyroCorrection;
		backWheelPower = backWheelPower * strafeMagnitude + gyroCorrection;
		driveMotor(leftWheelPower, rightWheelPower, backWheelPower);
	}
}

void driveMotor(float leftWheelPower, float rightWheelPower,
		float backWheelPower) {

	leftWheelPower = limit(leftWheelPower, -1, 1);
	rightWheelPower = limit(rightWheelPower, -1, 1);
	backWheelPower = limit(backWheelPower, -1, 1);


	int wheelLeft = leftWheelPower * 127;
	int wheelRight = rightWheelPower * 127;
	int wheelBack = backWheelPower * 127;

	motorSet(MOTOR_PORT_DRIVE_LEFT, wheelLeft);
	motorSet(MOTOR_PORT_DRIVE_RIGHT, wheelRight);
	motorSet(MOTOR_PORT_DRIVE_BACK_FWD, wheelBack);
	motorSet(MOTOR_PORT_DRIVE_BACK_REV, -wheelBack);
}
