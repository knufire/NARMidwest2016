/*
 * shooter.h
 *
 *  Created on: Mar 27, 2016
 *      Author: Rahul
 */

#ifndef INCLUDE_SHOOTER_H_
#define INCLUDE_SHOOTER_H_

typedef enum ShooterState {
	LONG = 0,
	MID,
	SHORT,
	OFF
}ShooterState;

void shooterEncoderInterruptHandler (unsigned char pin);
void updateShooterSpeedTask();
void setShooterMotor (int power);
void runShooter();
void setShooterState(ShooterState state);



#endif /* INCLUDE_SHOOTER_H_ */
