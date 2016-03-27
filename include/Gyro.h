/*
 * Gyro.h
 *
 *  Created on: Mar 24, 2016
 *      Author: jaxon
 */

#ifndef INCLUDE_GYRO_H_
#define INCLUDE_GYRO_H_

double JoyGyroTurn();

int GyroCorrect(int x);

int GetOffset();

void SetHeading();

void RefreshGyro();

#endif /* INCLUDE_GYRO_H_ */
