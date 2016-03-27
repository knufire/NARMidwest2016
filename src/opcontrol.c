#include <stdbool.h>
#include <stdlib.h>

#include "../include/API.h"
#include "../include/main.h"
#include "../include/math.h"
#include "../include/utils.h"
#include "drive.h"
#include "Gyro.h"
#include "vector.h"

/*---------------------------------------------------
 * 					 CONSTANTS						|
 *---------------------------------------------------
 */
#define SHOOTER_SPEED_SHORT			1000	// Shooter RPM for close shot
#define SHOOTER_SPEED_MID			1500	// Shooter RPM for mid shot
#define SHOOTER_SPEED_LONG			2800	// Shooter RPM for far/fullcourt shot
#define MOTOR_PORT_INTAKE			9
#define MOTOR_PORT_CONVEYOR_FWD		1
#define MOTOR_PORT_CONVEYOR_REV		10
#define MOTOR_PORT_SHOOTER_FWD1		4
#define MOTOR_PORT_SHOOTER_REV1		6
#define MOTOR_PORT_SHOOTER_FWD2		5
#define MOTOR_PORT_SHOOTER_REV2		7

/*----------------------------------------------------
 * 				Global Teleop Variables				 |
 *----------------------------------------------------
 */
int conveyorFlag = 0;
int intakeFlag = 0;
int shooterFlag = 0;
float shooterSpeed = 0;

//Storage for rising edge of buttons
bool intakeInLastVal = false;
bool intakeOutLastVal = false;
bool conveyorInLastVal = false;
bool conveyorOutLastVal = false;
bool shooterOnLastVal = false;
bool shooterOffLastVal = false;

/*-----------------------------------------------------
 * 		   			Background Tasks				  |
 *-----------------------------------------------------
 */

/**
 * Updates the shooter speed global variable. This uses PROS tasks to ensure that this runs
 * precisely every 10ms; much more precisely than the general teleop loop. The even dt is
 * critical to make sure that the derivative doesn't add too much noise.
 */
void updateShooterSpeedTask() {
	int encoderTicks = encoderGet(shooterEncoder);
	shooterSpeed = ((encoderTicks / (360.0 * 4)) / (0.01 / 60));
	encoderReset(shooterEncoder);
	printf("Shooter speed: %f\n\r", shooterSpeed);
}

/*---------------------------------------------------
 * 					Teleop Functions				|
 * --------------------------------------------------
 */

void updateDriveTask(void *ignore) {
	//Grab Joystick Values
	Vector vec;
	vec.x = joystickGetAnalog(1, 4) / 127;
	vec.y = joystickGetAnalog(1, 3) / 127;
	float rotation = joystickGetAnalog(1, 2) / 127;

	if ((magnitude(vec) > 0.05) || (fabs(rotation) > 0.05)) {
		if (((fabs(rotation)) < 0.05) && false) {
			//TODO: Uncomment line and take out false above once gyro works
			//driveVector(transX, transY, JoyGyroTurn());
		} else {
			driveVector(vec, rotation);
			SetHeading();
		}
	} else {
		driveDirection(0, 0, 0);
	}
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
	motorSet(MOTOR_PORT_CONVEYOR_FWD, conveyorFlag * -127);
	motorSet(MOTOR_PORT_CONVEYOR_REV, conveyorFlag * 127);

	//TODO: Change vertical conveyor to be hold-to-run for a fire button.

	//Store states to determine rising edges on next loop
	intakeInLastVal = intakeIn;
	intakeOutLastVal = intakeOut;
	conveyorInLastVal = conveyorIn;
	conveyorOutLastVal = conveyorOut;
	taskDelay(20);
}

void updateShooterTask(void *ignore) {
	bool shooterOn = joystickGetDigital(1, 8, JOY_UP);
	bool shooterOff = joystickGetDigital(1, 8, JOY_DOWN);
	bool shooterFull = joystickGetDigital(1, 8, JOY_RIGHT);

	//Scale output to current battery voltage to ensure constant voltage sent to shooter
	float shooterPower = 5800.0 / ((float) powerLevelMain());
	printf("Shooter power: %f\n\r", shooterPower);

	//TODO: Integrate bang-bang controller for flywheel speed.

	if (shooterOn) {
		motorSet(MOTOR_PORT_SHOOTER_FWD1, -shooterPower * 127);
		motorSet(MOTOR_PORT_SHOOTER_FWD2, -shooterPower * 127);
		motorSet(MOTOR_PORT_SHOOTER_REV1, shooterPower * 127);
		motorSet(MOTOR_PORT_SHOOTER_REV2, shooterPower * 127);
	} else if (shooterFull) {
		motorSet(MOTOR_PORT_SHOOTER_FWD1, -127);
		motorSet(MOTOR_PORT_SHOOTER_FWD2, -127);
		motorSet(MOTOR_PORT_SHOOTER_REV1, 127);
		motorSet(MOTOR_PORT_SHOOTER_REV2, 127);
	} else if (shooterOff) {
		motorStop(MOTOR_PORT_SHOOTER_FWD1);
		motorStop(MOTOR_PORT_SHOOTER_FWD2);
		motorStop(MOTOR_PORT_SHOOTER_REV1);
		motorStop(MOTOR_PORT_SHOOTER_REV2);
	}
	taskDelay(10);
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
	taskRunLoop(updateShooterSpeedTask, 10);
	taskRunLoop(RefreshGyro,20);
	while (1) {
		taskCreate(updateDriveTask, TASK_DEFAULT_STACK_SIZE, NULL,
		TASK_PRIORITY_DEFAULT);
		taskCreate(updateIntakeTask, TASK_DEFAULT_STACK_SIZE, NULL,
		TASK_PRIORITY_DEFAULT);
		taskCreate(updateShooterTask, TASK_DEFAULT_STACK_SIZE, NULL,
		TASK_PRIORITY_DEFAULT);
		delay(5);
	}
}
