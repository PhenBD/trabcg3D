#ifndef ARENA_H
#define	ARENA_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "object.h"

class Arena : public Object {

public:
    Arena(){}; // Default constructor
    Arena(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth) : Object(x, y, z, width, height, depth) {};
    void draw();
};

#endif	/* ARENA_H */

