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
	shooterSpeed = ((encoderTicks / (360.0 * 4)) / 0.005)*-60;
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
//	if (shooterTargetSpeed != 0) {
//		float error = shooterSpeed - shooterTargetSpeed;
//		if (fabsf(error) < 100) {
//			setShooterMotor(89);
//			printf(".7 %f %d\n\r", shooterSpeed, shooterTargetSpeed);
//		} else if (fabsf(error) < 200) {
//			if (error < 0){
//				setShooterMotor(114);
//				printf(".9 %f %d\n\r", shooterSpeed, shooterTargetSpeed);
//			}
//			else {
//				setShooterMotor(63);
//				printf(".5 %f %d\n\r", shooterSpeed, shooterTargetSpeed);
//			}
//		} else {
//			if (error < 0) {
//				setShooterMotor(127);
//				printf("1 %f %d\n\r", shooterSpeed, shooterTargetSpeed);
//			}
//			else {
//				setShooterMotor(38);
//				printf(".3 %f %d\n\r", shooterSpeed, shooterTargetSpeed);
//			}
//		}
//	}
//	else {
//		setShooterMotor(0);
//	}
	if (shooterTargetSpeed != 0) {
		float error = (shooterSpeed - shooterTargetSpeed) * -1;
		float Kp = .00017;
		float feedForward = .59;
		float power = ((error * Kp) + feedForward) * 127;
		setShooterMotor(power);
		printf("%f %f %d\n\r", power, shooterSpeed, shooterTargetSpeed);
	}
	else {
		setShooterMotor(0);
	}

}

void setShooterRPM(int rpm) {
	shooterTargetSpeed = rpm;
}

