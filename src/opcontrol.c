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

#define MOTOR_PORT_INTAKE_REV		1
#define MOTOR_PORT_INTAKE_FWD		10


int intakeState = 0;
int lastIntakeState = 0;
float intakePulseStartTime = 0;

//Change to true to enable field-centric drive
bool fieldCentricDrive = false;


void updateDrive() {
	//Grab Joystick Values
	Vector vec;
	vec.x = joystickGetAnalog(1, 3) / 127.0;
	vec.y = joystickGetAnalog(1, 4) / -127.0;
	float rotation = joystickGetAnalog(1, 2) / 127.0;
	if (fieldCentricDrive) {
		vec = rotate(vec, abs(getCurrentHeading()-360));
	}
	driveGyro(vec, rotation);
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
			intakeState = 0;
		}
		else {
			motorSet(MOTOR_PORT_INTAKE_FWD, intakeState * 127);
			motorSet(MOTOR_PORT_INTAKE_REV, intakeState * -127);
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
