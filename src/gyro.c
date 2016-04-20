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
	float kp = .05;
	float ki = 0;
	float kd = 1;

	float output = pidController(kp, ki, kd, 10, 0, headingError);

	if (fabs(output) < .2) {
		if (output > 0) {
			output += .2;
		}
		else {
			output -= .2;
		}
	}
	if (fabs(headingError) < 2) {
		output = 0;
	}


	gyroCorrection = output;
}

int correctGyroAngle(int x) { //JR Speak: GyroCorrect
	while (x > 360) {
		x = x - 360;
	}

	while (x < 0) {
		x = x + 360;
	}
	return x;
}

int getHeadingError() { //JR Speak: GetOffset
	int z = currentHeading - desiredHeading;
	if (z > 180)
		z -= 360;
	if (z < -180)
		z += 360;
	headingError = z;
	return z;
}

void setDesiredHeading() { //JR Speak: set heading
	desiredHeading = correctGyroAngle(gyroGet(gyro));
	headingError = 0;

}

float getGyroCorrection() {
	return gyroCorrection;
}

void RefreshGyro() {
	currentHeading = correctGyroAngle(gyroGet(gyro));
	getHeadingError();
	gyroCorrectionPID();
//	printf("Heading: %d\n\r", currentHeading);
//	printf("Offset: %d\n\r", headingError);
//	printf("Correction: %f\n\r", gyroCorrection);

}

