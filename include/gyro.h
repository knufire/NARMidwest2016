/*
 * Gyro.h
 *
 *  Created on: Mar 24, 2016
 *      Author: jaxon
 */

#ifndef INCLUDE_GYRO_H_
#define INCLUDE_GYRO_H_

void gyroCorrectionPID();

int correctGyroAngle(int x);

int getHeadingError();

void setDesiredHeading();

void refreshGyroTask();

float getGyroCorrection();

int getCurrentHeading();

#endif /* INCLUDE_GYRO_H_ */
