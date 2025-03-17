#ifndef OBSTACLE_H
#define	OBSTACLE_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "object.h"

class Obstacle : public Object {

public:
    Obstacle(){};
    Obstacle(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth) : Object(x, y, z, width, height, depth) {};
    void draw(GLuint texture);
};

#endif	/* OBSTACLE_H */