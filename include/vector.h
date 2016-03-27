/*
 * vector.h
 *
 *  Created on: Mar 26, 2016
 *      Author: Rahul
 */

#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

typedef struct Vector {
	float x;
	float y;
} Vector;

Vector add(Vector vec1, Vector vec2);
Vector subtract(Vector vec1, Vector vec2);
float magnitude(Vector vec);
float dotProduct(Vector vec1, Vector vec2);
Vector scalarProduct(Vector vec, float scalar);
Vector fromAngle(float c);
Vector unit (Vector vec);
float scalarProjection(Vector vec1, Vector vec2);
Vector projection(Vector vec1, Vector vec2);
Vector rotate (Vector vec, float angle);

#endif /* INCLUDE_VECTOR_H_ */
