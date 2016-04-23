#include <stdbool.h>
#include <stdlib.h>

#include "API.h"
#include "gyro.h"
#include "main.h"
#include "math.h"
#include "utils.h"
#include "drive.h"
#include "vector.h"
#include "shooter.h"

#define MOTOR_PORT_INTAKE_REV		10
#define MOTOR_PORT_INTAKE_FWD		1


int intakeState = 0;
int lastIntakeState = 0;
float intakePulseStartTime = 0;

//Change to true to enable field-centric drive
bool fieldCentricDrive = false;

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

#define PI 3.14159265
#define WHEELSIZE 3.25

double val = PI / 180;
double left;
double middle;
double right;
double mult;
double transMult;
double rotMult;
double Cos60;
double Cos30;
double Sin60;


#define C1LX joystickGetAnalog(1, 4)
#define C1LY joystickGetAnalog(1, 3)
#define C1RX joystickGetAnalog(1, 2)

void InitializeDrive()
{
	Cos60 = cos(60 * val);	//Cos 60
	Sin60 = sin(60 * val);	//Cos 30
	Cos30 = cos(30 * val);	//Cos 30
}

void Drive(int x, int y, int z) // Left Middle Right
{
	motorSet(8,x);
	motorSet(3,z);
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


	int leftAngle = 60;
	int rightAngle = 300;
	int backAngle = 0;

	left = ( cos(leftAngle * val) * -x * 0.75) + ( sin(leftAngle * val) * y );
	right = ( cos(rightAngle * val) * -x * 0.75 ) + ( sin(rightAngle * val) * y);
	middle = cos(backAngle * val) * x;//adjust multiplyer to taste

	left *= transMult;
	right *= transMult;
	middle *= transMult;

	left += rot * rotMult;
	right += rot * rotMult;
	middle += rot * rotMult;//adjust multiplyer to taste

	Drive( (int)round(left), (int)round(middle), (int)round(right) );
	//printf("CalcDrive -- Left %f  middle %f  Right %f\n\r ", left, Middle, Right);
	//printf("CalcDrive -- x:%d  y:%d  rot:%d  tansMult:%f  RotMult:%f\n\r ", x, y, rot, transMult, rotMult);
}

void updateDrive() {
//	//Grab Joystick Values
//	Vector vec;
//	vec.x = joystickGetAnalog(1, 3) / 127.0;
//	vec.y = joystickGetAnalog(1, 4) / -127.0;
//	float rotation = joystickGetAnalog(1, 2) / 127.0;
//	if (fieldCentricDrive) {
//		vec = rotate(vec, abs(getCurrentHeading()-360));
//	}
//	driveGyro2(vec, rotation);

	Vector vec;
	vec.x = C1LX;
	vec.y = -C1LY;
	vec = rotate(vec, getCurrentHeading() * val);

//	switch( DecideDrive(vec.x, vec.y, C1RX, true) )//Switch to calcDrive 2
//			{
//				case 1:
//					CalcDrive2(vec.x, vec.y, getGyroCorrection());
//					break;
//
//				case 2:
//					CalcDrive2(vec.x, vec.y, C1RX);
//					setDesiredHeading();
//					break;
//
//				case 3:
//					CalcDrive2(0, 0, getGyroCorrection());
//					break;
//
//				case 4:
//					CalcDrive2(0, 0, 0);
//					break;
//			}

	Drive(joystickGetAnalog(1,3),0,joystickGetAnalog(1,2)*-1);
}

void updateIntake() {
	//Grab Joystick values
	bool intakeIn = joystickGetDigital(1, 6, JOY_UP);
	bool intakeOut = joystickGetDigital(1, 6, JOY_DOWN);
	bool intakeOff = joystickGetDigital(1, 5, JOY_UP);
	bool intakePulse = joystickGetDigital(1, 5, JOY_DOWN);


	//Set state flags
	if (intakeIn) {
		intakeState = 1;
	}
	else if (intakeOut) {
		intakeState = -1;
	}
	else if (intakeOff) {
		intakeState = 0;
	}
	else if (intakePulse) {
		intakeState = 2;
	}

	//Set motors based on
	if (intakeState < 2) {
		motorSet(MOTOR_PORT_INTAKE_FWD, intakeState * 127);
		motorSet(MOTOR_PORT_INTAKE_REV, intakeState * -127);
	}

	//Pulse conveyor backwards to free flywheel
	else {
		if (lastIntakeState != 2) {
			intakePulseStartTime = millis();
		}
		if (millis() > intakePulseStartTime + 100) {
			motorStop(MOTOR_PORT_INTAKE_FWD);
			motorStop(MOTOR_PORT_INTAKE_REV);
		}
		else {
			motorSet(MOTOR_PORT_INTAKE_FWD, intakeState * -127);
			motorSet(MOTOR_PORT_INTAKE_REV, intakeState * 127);
		}
	}


	lastIntakeState = intakeState;
}


void updateShooterState() {
	bool shooterMid = joystickGetDigital(1, 8, JOY_UP);
	bool shooterOff = joystickGetDigital(1, 8, JOY_DOWN);
	bool shooterLong = joystickGetDigital(1, 8, JOY_RIGHT);
	bool shooterShort = joystickGetDigital(1, 8, JOY_LEFT);

	if (shooterOff) {
		setShooterRPM(0);
	}
	else if (shooterShort) {
		setShooterRPM(SHOOTER_SPEED_SHORT);
	}
	else if (shooterMid) {
		setShooterRPM(SHOOTER_SPEED_MID);
	}
	else if (shooterLong) {
		setShooterRPM(SHOOTER_SPEED_LONG);
	}
}

void updateHmiTask() {
	updateDrive();
	updateIntake();
	updateShooterState();
}

void cockShooter() {
	intakeState = 2;
}

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */

void operatorControl() {
	taskRunLoop(refreshGyroTask, 10);
	taskRunLoop(shooterClosedLoopTask, 20);
	taskRunLoop(updateHmiTask, 20);
	while (1) {
		delay(20);
	}
}
