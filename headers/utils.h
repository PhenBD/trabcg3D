#ifndef UTILS_H
#define	UTILS_H

#include <cmath>
#include <GL/gl.h>

typedef struct
{
    //Vertex coordinate
    double X;
    double Y;
    double Z;
    
    //Vertex normal 
    double nX;
    double nY;
    double nZ;
    
    //Vertex texture coordinate
    double U;
    double V;
} VERTICES;

typedef struct
{
    VERTICES * vtx;
    int numVtx;
    double radius;
} SPHERE;

// Function to limit the number of decimal places of a float number.
GLfloat roundToDecimalPlaces(GLfloat value, int decimalPlaces);
GLfloat calculateArmAngle(GLfloat x, GLfloat y, GLfloat x_origin, GLfloat y_origin);
void normalizeVector3D(float &x, float &y, float &z);
void crossProduct3D(float ax, float ay, float az, 
    float bx, float by, float bz,
    float &cx, float &cy, float &cz);
void drawRectangle(float x1, float y1, float z1, 
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float normalX, float normalY, float normalZ,
    GLuint texture, int repeats);
SPHERE * CreateSphere (double R, double space);

#endif	/* UTILS_H */