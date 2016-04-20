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
#define MOTOR_PORT_DRIVE_BACK		2
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

double transMult;
double rotMult;
double left;
double middle;
double right;
#define PI 3.14159265
double val = PI / 180;

void Drive(int x, int y, int z) // Left Middle Right
{
	motorSet(MOTOR_PORT_DRIVE_LEFT,x);
	motorSet(MOTOR_PORT_DRIVE_BACK,y);
	motorSet(MOTOR_PORT_DRIVE_RIGHT,z);
}

void CalcDrive2(int x, int y, int rot)
{
	int min = 15;

	if ( abs(x) < min)
		x = 0;

	if ( abs(y) < min)
		y = 0;

	if ( abs(rot) < min)
		rot = 0;

	if (abs(x) > abs(y))
	{
		transMult = (double) (abs(x)  ) / ( abs(x) + abs(rot) );
		rotMult = (double) (abs(rot)  ) / ( abs(x) + abs(rot) );
	}
	else
	{
		transMult = (double) (abs(y)  ) / ( abs(y) + abs(rot) );
		rotMult = (double) (abs(rot)  ) / ( abs(y) + abs(rot) );
	}

	if ( (x == 0) && (y == 0) && (rot == 0) )
	{
		transMult = (double)0;
		rotMult = (double)0;
	}

	//transMult = 0.5;
	//rotMult = 0.5;

	left = ( cos(60 * val) * x ) + ( sin(60 * val) * y );
	right = ( cos(300 * val) * x ) + ( sin(300 * val) * y);
	middle = 1 * x;//adjust multiplyer to taste

	left *= transMult;
	right *= transMult;
	middle *= transMult;

	left += rot * rotMult;
	right += rot * rotMult;
	middle += rot * -rotMult;//adjust multiplyer to taste

	Drive( (int)round(left), (int)round(middle), (int)round(right) );
	//printf("CalcDrive -- Left %f  middle %f  Right %f\n\r ", left, Middle, Right);
	//printf("CalcDrive -- x:%d  y:%d  rot:%d  tansMult:%f  RotMult:%f\n\r ", x, y, rot, transMult, rotMult);
}

int DecideDrive (int x, int y, int r, bool GyroSwitch)

{
	if( ( fabs(y) > 10 ) || ( fabs(x) > 10 ) || ( fabs(r) > 10 ))
	{
		if (fabs(r) < 10 && GyroSwitch)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		if(GyroSwitch)
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}
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
	motorSet(MOTOR_PORT_DRIVE_BACK, wheelBack);
}



