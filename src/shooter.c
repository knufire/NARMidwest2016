/*
 * shooter.c
 *
 *  Created on: Mar 27, 2016
 *      Author: Rahul
 */

#include "../include/API.h"
#include "../include/main.h"
#include "../include/math.h"
#include "../include/utils.h"
#include "shooter.h"

#define MOTOR_PORT_SHOOTER_REV1		4
#define MOTOR_PORT_SHOOTER_FWD1		6
#define MOTOR_PORT_SHOOTER_FWD2		5
#define MOTOR_PORT_SHOOTER_REV2		7

int shooterFlag = 0;
int shooterTargetSpeed = 0;
float shooterSpeed = 0;
int shooterEncoderTicks = 0;

void shooterEncoderInterruptHandler (unsigned char pin) {
	shooterEncoderTicks++;
}

/**
 * Updates the shooter speed global variable. This uses PROS tasks to ensure that this runs
 * precisely every 10ms; much more precisely than the general teleop loop. The even dt is
 * critical to make sure that the derivative doesn't add too much noise.
 */
void updateShooterSpeedTask() {
	int encoderTicks = encoderGet(shooterEncoder);
	shooterSpeed = ((encoderTicks / (360.0 * 4)) / 0.01)*-60;
	encoderReset(shooterEncoder);
//	shooterSpeed = ((shooterEncoderTicks / (360.0)) / (0.01 / 60));
//	shooterEncoderTicks = 0;

}

void setShooterMotor (int power) {
	motorSet(MOTOR_PORT_SHOOTER_FWD1, power);
	motorSet(MOTOR_PORT_SHOOTER_FWD2, power);
	motorSet(MOTOR_PORT_SHOOTER_REV1, -power);
	motorSet(MOTOR_PORT_SHOOTER_REV2, -power);
}

void runShooter() {
	float controllerOutput = bangBangController(shooterTargetSpeed, shooterSpeed);
	setShooterMotor(controllerOutput * 127);
	printf("%f %f %d\n\r", controllerOutput, shooterSpeed, shooterTargetSpeed);
}

void setShooterRPM(int rpm) {
	shooterTargetSpeed = rpm;
}

