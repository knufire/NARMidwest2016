/*
 * Utils.h
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

float limit(float value, float min, float max);
int latchFlag(bool highInput, bool lowInput, int flag);
float bangBangController(int setpoint, int var);
float pidController(float Kp, float Ki, float Kd, float dt, float setpoint,
		float processVar);
void resetIntegral();

#endif /* INCLUDE_UTILS_H_ */
