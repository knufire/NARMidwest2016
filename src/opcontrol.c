#include <stdbool.h>
#include <stdlib.h>

#include "../include/API.h"
#include "../include/main.h"
#include "../include/math.h"

/*---------------------------------------------------
 * 					 CONSTANTS						|
 *---------------------------------------------------
 */
#define SHOOTER_SPEED_SHORT			1000	// Shooter RPM for close shot
#define SHOOTER_SPEED_MID			1500	// Shooter RPM for mid shot
#define SHOOTER_SPEED_LONG			2800	// Shooter RPM for far/fullcourt shot
#define SHOOTER_ENCODER_POLL_RATE 	100		// Rate in hz to poll shooter encoder
#define MOTOR_PORT_DRIVE_LEFT		8
#define MOTOR_PORT_DRIVE_RIGHT		3
#define MOTOR_PORT_DRIVE_BACK		2
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
unsigned long lastShooterSpeedLoopStopTime;

//Storage for rising edge of buttons
bool intakeInLastVal = false;
bool intakeOutLastVal = false;
bool conveyorInLastVal = false;
bool conveyorOutLastVal = false;
bool shooterOnLastVal = false;
bool shooterOffLastVal = false;

/*------------------------------------------------------
 * 				  Utility Functions					   |
 *------------------------------------------------------
 */

/**
 * Coerces the value between the minimum and the maximum. If the value is outside either limit,
 * it will be set to that limit
 *
 * @param value The value to coerce
 * @param min The minimum of the value's range
 * @param max The maximum of the values range
 * @return value The coerced value
 */
float limit(float value, float min, float max) {
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	}
	return value;
}

/**
 * A flag that acts like a 3 position toggle swtich. The high input will set the flag to +1,
 * and the low input will set the flag to -1. If the input is triggered when the flag is
 * already set to the input's respective state, the flag will shut off.
 *
 * @param highInput The boolean to toggle between forward and off
 * @param lowInput The boolean to toggle between backwards and off
 * @return flag	The value of the flag.
 */

int latchFlag(bool highInput, bool lowInput, int flag) {
	if (highInput) {
		if (flag == 1) {
			return 0;
		} else {
			return 1;
		}
	} else if (lowInput) {
		if (flag == -1) {
			return 0;
		} else {
			return -1;
		}
	} else {
		return flag;
	}
}

/**
 * A standard bang-bang controller. Full power forward if the process variable is above the
 * setpoint, off if the variable is below the setpoint, and half power if the variable and
 * setpoint are equal.
 *
 * @param setpoint The goal operating point
 * @param var The process variable
 */

float bangBangController(int setpoint, int var) {
	if (var > setpoint) {
		return 1.0;
	} else if (var < setpoint) {
		return 0;
	} else {
		return 0.5;
	}
}

/**
 * A standard PID controller.
 *
 * @param Kp Proportional gain
 * @param Ki Integral gain
 * @param Kd Derivative gain
 * @param setpoint Goal operating state of the mechanism
 * @param processVar Sensor value measuring what you're trying to control
 * @return output Motor power between -1 and 1
 */

float errorSum = 0;
float lastErr = 0;
float pidController(float Kp, float Ki, float Kd, float setpoint,
		float processVar) {
	float error = setpoint - processVar;

	float p = Kp * error;
	float i = Ki * errorSum;
	float d = Kd * error - lastErr;

	lastErr = error;
	errorSum += error;

	float output = p + i + d;
	output = limit(output, -1, 1);

	return output;
}

/*-----------------------------------------------------
 * 		   			Background Tasks				  |
 *-----------------------------------------------------
 */

/**
 * Updates the shooter speed global variable. This uses PROS tasks to ensure that this runs
 * precisely every 10ms; much more precisely than the general teleop loop. The even dt is
 * critical to make sure that the derivative doesn't add too much noise.
 */
void updateShooterSpeedTask(void *ignore) {
	int encoderTicks = encoderGet(shooterEncoder);
	//shooterSpeed = ((encoderTicks / 360.0) / (1 / (SHOOTER_ENCODER_POLL_RATE) * 60));
	//encoderReset(shooterEncoder);
	printf("Shooter speed: %d \n", encoderTicks);
	taskDelayUntil(&lastShooterSpeedLoopStopTime,
			(1 / SHOOTER_ENCODER_POLL_RATE) * 1000);
}

/*---------------------------------------------------
 * 					Teleop Functions				|
 * --------------------------------------------------
 */

void updateDriveTask(void *ignore) {
	//Grab Joystick Values
	float transX = joystickGetAnalog(1, 4) / 127;
	float transY = joystickGetAnalog(1, 3) / 127;
	float rotation = joystickGetAnalog(1, 2) / 127;

	//All the constants are based on drive geometry.
	float mult = (127-fabs( joystickGetAnalog(1, 2) /2 )) / 127;
	float wheel1 = (.160458 * transX + 0.57735 * transY) * mult + rotation;
	float wheel2 = (.160458 * transX - 0.57735 * transY) * mult + rotation;
	float wheel3 = 0.83954 * transX * mult - rotation;

	wheel1 = limit(wheel1, -1, 1);
	wheel2 = limit(wheel2, -1, 1);
	wheel3 = limit(wheel3, -1, 1);

	motorSet(MOTOR_PORT_DRIVE_LEFT, wheel1*127);
	motorSet(MOTOR_PORT_DRIVE_RIGHT, wheel2*127);
	motorSet(MOTOR_PORT_DRIVE_BACK, wheel3*127);
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
	lastShooterSpeedLoopStopTime = millis();
	while (1) {
		taskCreate(updateShooterSpeedTask, TASK_DEFAULT_STACK_SIZE, NULL,
				TASK_PRIORITY_DEFAULT);
		taskCreate(updateDriveTask, TASK_DEFAULT_STACK_SIZE, NULL,
				TASK_PRIORITY_DEFAULT);
		taskCreate(updateIntakeTask, TASK_DEFAULT_STACK_SIZE, NULL,
				TASK_PRIORITY_DEFAULT);
		taskCreate(updateShooterTask, TASK_DEFAULT_STACK_SIZE, NULL,
				TASK_PRIORITY_DEFAULT);
		delay(5);
	}
}
