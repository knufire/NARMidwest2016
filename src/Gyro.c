/*
 * Gyro.c
 *
 *  Created on: Mar 24, 2016
 *      Author: jaxon
 */

#include "main.h"
#include "Gyro.h"
#include "math.h"

extern Gyro gyro;

int OffSet;
int HeadingSet;
int CurrentHeading;
double LastTime = 0;
double ErrSum, LastErr;
double kp, ki, kd;

double JoyGyroTurn() {
	int min = 30;
	int max = 127;
	kp = 5;
	ki = 0;
	kd = 20;

	double CurrentTime = (double) millis();
	double TimeChange = CurrentTime - LastTime;

	ErrSum += (OffSet * TimeChange / 1000);

	double dErr = (OffSet - LastErr) / (TimeChange / 1000);

	double y = kp * OffSet;
	y += ki * ErrSum;
	y += kd * dErr;

	if (y > max)
		y = max;

	if (fabs(y) < min)
		y = 0;

	if (y < -max)
		y = -max;

	//y = y * reverse;
	printf("Offset: %d  dErr: %f\n\r", OffSet, dErr);

	return y;
}

int GyroCorrect(int x) {
	while (x > 360) {
		x = x - 360;
	}

	while (x < 0) {
		x = x + 360;
	}
	return x;
}

int GetOffset() {
	int z = CurrentHeading - HeadingSet;
	if (z > 180)									//fix > 180
		z -= 360;

	if (z < -180)
		z += 360;

	OffSet = z;

	return z;
}

void SetHeading() {
	HeadingSet = GyroCorrect(gyroGet(gyro));
}

void RefreshGyro() {
	CurrentHeading = GyroCorrect(gyroGet(gyro));
	GetOffset();
}

