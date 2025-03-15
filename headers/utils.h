#ifndef UTILS_H
#define	UTILS_H

#include <cmath>
#include <GL/gl.h>

// Function to limit the number of decimal places of a float number.
GLfloat roundToDecimalPlaces(GLfloat value, int decimalPlaces);
GLfloat calculateArmAngle(GLfloat x, GLfloat y, GLfloat x_origin, GLfloat y_origin);
void normalizeVector3D(float &x, float &y, float &z);
void crossProduct3D(float ax, float ay, float az, 
    float bx, float by, float bz,
    float &cx, float &cy, float &cz);

#endif	/* UTILS_H */