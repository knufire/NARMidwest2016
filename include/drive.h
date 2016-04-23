/*
 * drive.h
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#ifndef INCLUDE_DRIVE_H_
#define INCLUDE_DRIVE_H_

#include "vector.h"

void driveDirection(float transX, float transY, float rotation);
void driveVector(Vector vec, float rotation);
void driveGyro(Vector translate, float rotation);
void driveGyro2(Vector translate, float rotation);
void driveMotor(float leftWheelPower, float rightWheelPower, float backWheelPower);


#endif /* INCLUDE_DRIVE_H_ */
