#include <stdbool.h>
#include <stdlib.h>

#include "../include/API.h"
#include "../include/gyro.h"
#include "../include/main.h"
#include "../include/math.h"
#include "../include/utils.h"
#include "drive.h"
#include "vector.h"
#include "shooter.h"

#define MOTOR_PORT_INTAKE			9
#define MOTOR_PORT_CONVEYOR_REV		1
#define MOTOR_PORT_CONVEYOR_FWD		10

int conveyorFlag = 0;
int intakeFlag = 0;


//Storage for rising edge of buttons
bool intakeInLastVal = false;
bool intakeOutLastVal = false;
bool conveyorInLastVal = false;
bool conveyorOutLastVal = false;
bool shooterOnLastVal = false;
bool shooterOffLastVal = false;


void updateDriveTask(void *ignore) {
	//Grab Joystick Values
	Vector vec;
	vec.x = joystickGetAnalog(1, 3) / 127.0;
	vec.y = joystickGetAnalog(1, 4) / -127.0;
	float rotation = joystickGetAnalog(1, 2) / 127.0;
	driveGyro(vec, rotation);
	taskDelay(20);
}

void updateIntakeTask(void *ignore) {
	//Grab Joystick values
	bool intakeIn = joystickGetDigital(1, 6, JOY_UP);
	bool intakeOut = joystickGetDigital(1, 6, JOY_DOWN);
	bool conveyorIn = joystickGetDigital(1, 5, JOY_UP);
	bool conveyorOut = joystickGetDigital(1, 5, JOY_DOWN);

	//Set state flags
	intakeFlag = latchFlag(intakeIn > intakeInLastVal,
			intakeOut > intakeOutLastVal, intakeFlag);
	conveyorFlag = latchFlag(conveyorIn > conveyorInLastVal,
			conveyorOut > conveyorOutLastVal, conveyorFlag);

	//Set motors based on
	motorSet(MOTOR_PORT_INTAKE, intakeFlag * -127);
	motorSet(MOTOR_PORT_CONVEYOR_FWD, conveyorFlag * 127);
	motorSet(MOTOR_PORT_CONVEYOR_REV, conveyorFlag * -127);

	//Store states to determine rising edges on next loop
	intakeInLastVal = intakeIn;
	intakeOutLastVal = intakeOut;
	conveyorInLastVal = conveyorIn;
	conveyorOutLastVal = conveyorOut;
	taskDelay(20);
}

void updateShooterState(void *ignore) {
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
	taskDelay(20);
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
	taskRunLoop(updateShooterSpeedTask, 5);
	taskRunLoop(RefreshGyro, 10);
	taskRunLoop(runShooter, 5);
	while (1) {
		taskCreate(updateDriveTask, TASK_DEFAULT_STACK_SIZE, NULL,
		TASK_PRIORITY_DEFAULT);
		taskCreate(updateIntakeTask, TASK_DEFAULT_STACK_SIZE, NULL,
		TASK_PRIORITY_DEFAULT);
		taskCreate(updateShooterState, TASK_DEFAULT_STACK_SIZE, NULL,
		TASK_PRIORITY_DEFAULT);
		delay(10);
	}
}
