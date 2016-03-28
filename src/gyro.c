/*
 * Gyro.c
 *
 *  Created on: Mar 24, 2016
 *      Author: jaxon
 */

#include "main.h"
#include "math.h"
#include "../include/gyro.h"
#include "utils.h"

int headingError;
int desiredHeading;
int currentHeading;
float gyroCorrection;

void gyroCorrectionPID() {
	float kp = 5 / 127;
	float ki = 0;
	float kd = 20 / 127;

	float output = pidController(kp, ki, kd, 10, currentHeading,
			desiredHeading);

	if (fabs(output) < 0.1) {
		output = 0;
	}

	gyroCorrection = output;
}

int correctGyroAngle(int x) {
	while (x > 360) {
		x = x - 360;
	}

	while (x < 0) {
		x = x + 360;
	}
	return x;
}

int getHeadingError() {
	int z = currentHeading - desiredHeading;
	if (z > 180)
		z -= 360;
	if (z < -180)
		z += 360;
	headingError = z;
	return z;
}

void setDesiredHeading() {
	desiredHeading = correctGyroAngle(gyroGet(gyro));
}

float getGyroCorrection() {
	return gyroCorrection;
}

void RefreshGyro() {
//	currentHeading = correctGyroAngle(gyroGet(gyro));
//	getHeadingError();
//	gyroCorrectionPID();
//	printf("Heading: %d\n\r", currentHeading);
//	printf("Offset: %d\n\r", headingError);

	printf( "%d\n\r",gyroGet(gyro));
}

