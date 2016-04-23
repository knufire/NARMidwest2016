/*
 * vector.c
 *
 * 	Basic vector library. Ported from FRC1114's 2015 code.
 * 	Original (Java) author: Jason
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#include "vector.h"
#include "math.h"

Vector add(Vector vec1, Vector vec2) {
	Vector result;
	result.x = vec1.x + vec2.x;
	result.y = vec1.y + vec2.y;
	return result;
}

Vector subtract(Vector vec1, Vector vec2) {
	Vector result;
	result.x = vec1.x - vec2.x;
	result.y = vec1.y - vec2.y;
	return result;
}

float magnitude(Vector vec) {
	return sqrt(magSq(vec));
}

float dotProduct(Vector vec1, Vector vec2) {
	return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

Vector scalarProduct(Vector vec, float scalar) {
	Vector result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	return result;
}

Vector fromAngle(float c) {
	Vector result;
	result.x = cos(c);
	result.y = sin(c);
	return result;
}

Vector unit(Vector vec) {
	float mag = magnitude(vec);
	Vector result;
	result.x = vec.x / mag;
	result.y = vec.y / mag;
	return result;
}

float magSq (Vector vec) {
	return dotProduct(vec, vec);
}

float scalarProjection(Vector vec1, Vector vec2) {
	return dotProduct(vec1, vec2) / magnitude(vec2);
}

Vector projection(Vector vec1, Vector vec2) {
	return scalarProduct(vec2,
			(dotProduct(vec1, vec2) / dotProduct(vec2, vec2)));
}

Vector rotate(Vector vec, float angle) {
	Vector result;
	result.x = vec.x * cos(angle) - vec.y * sin(angle);
	result.y = vec.x * sin(angle) - vec.y * cos(angle);
	return result;
}
