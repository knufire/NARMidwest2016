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

#define SHOOTER_SPEED_SHORT			1000	// Shooter RPM for close shot
#define SHOOTER_SPEED_MID			1500	// Shooter RPM for mid shot
#define SHOOTER_SPEED_LONG			2800	// Shooter RPM for far/fullcourt shot

int shooterFlag = 0;
int shooterTargetSpeed = 0;
float shooterSpeed = 0;
int shooterEncoderTicks = 0;
ShooterState state = OFF;

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
	setShooterMotor(controllerOutput);
	printf("%f %f %d\n\r", controllerOutput, shooterSpeed, shooterTargetSpeed);
}

void setShooterState(ShooterState newState) {
switch (state) {
	case (LONG):
		shooterTargetSpeed = SHOOTER_SPEED_LONG;
		break;
	case (MID):
		shooterTargetSpeed = SHOOTER_SPEED_MID;
		break;
	case (SHORT):
		shooterTargetSpeed = SHOOTER_SPEED_SHORT;
		break;
	case (OFF):
		shooterTargetSpeed = 0;
		break;
	}
}
