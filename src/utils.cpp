#include "../headers/utils.h"
#include <iostream>
#include "../headers/enemy.h"

GLfloat roundToDecimalPlaces(GLfloat value, int decimalPlaces) {
    GLfloat scale = std::pow(10.0f, decimalPlaces);
    return std::round(value * scale) / scale;
}

GLfloat calculateArmAngle(GLfloat x, GLfloat y, GLfloat x_origin, GLfloat y_origin) {
    // Difference in coordinates
    GLfloat dx = x - x_origin;
    GLfloat dy = y - y_origin;

    // Calculate the angle in radians
    GLfloat angleRadians = atan2(dy, dx);

    // Convert to degrees
    GLfloat angleDegrees = angleRadians * (180.0 / M_PI);

    // Ensure the angle is between 0 and 360 degrees
    if (angleDegrees < 0) {
        angleDegrees += 360.0;
    }

    angleDegrees = abs(angleDegrees - 90.0);

    // Adjust the angle to the range [0, 180]
    if (angleDegrees > 180.0) {
        angleDegrees = 360.0 - angleDegrees;
    }

    // Limit the angle to the range [45, 135]
    if (angleDegrees < 45.0) {
        return 45.0;  // Adjust to the lower limit
    } else if (angleDegrees > 135.0) {
        return 135.0; // Adjust to the upper limit
    }

    // Return the angle within the range
    return angleDegrees;
}