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
int actualCurrentHeading;

void gyroCorrectionPID() {
	float kp = 1.4;
	float ki = 0;
	float kd = 0.4;

	float output = pidController(kp, ki, kd, 10, 0, headingError);

	if (fabs(headingError) < 2) {
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
	headingError = 0;

}

float getGyroCorrection() {
	return gyroCorrection;
}

void refreshGyroTask() {
	currentHeading = correctGyroAngle(gyroGet(gyro));
	actualCurrentHeading = gyroGet(gyro);
	getHeadingError();
	gyroCorrectionPID();
	//printf("%d,%d,%f \n\r", currentHeading, headingError, gyroCorrection);

}

int getCurrentHeading() {

	return actualCurrentHeading;
}


