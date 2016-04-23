/*
 * shooter.h
 *
 *  Created on: Mar 27, 2016
 *      Author: Rahul
 */

#ifndef INCLUDE_SHOOTER_H_
#define INCLUDE_SHOOTER_H_

void shooterEncoderInterruptHandler (unsigned char pin);
void updateShooterSpeed();
void setShooterMotor (int power);
void runShooterControlLoop();
void setShooterRPM(int rpm);
void shooterClosedLoopTask();

#define SHOOTER_SPEED_SHORT			5000	// Shooter RPM for close shot
#define SHOOTER_SPEED_MID			2800	// Shooter RPM for mid shot
#define SHOOTER_SPEED_LONG			3350	// Shooter RPM for far/fullcourt shot

#endif /* INCLUDE_SHOOTER_H_ */
