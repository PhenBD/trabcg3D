#ifndef UTILS_H
#define	UTILS_H

#include <cmath>
#include <GL/gl.h>

// Function to limit the number of decimal places of a float number.
GLfloat roundToDecimalPlaces(GLfloat value, int decimalPlaces);
GLfloat calculateArmAngle(GLfloat x, GLfloat y, GLfloat x_origin, GLfloat y_origin);

#endif	/* UTILS_H */