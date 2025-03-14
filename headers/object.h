#ifndef OBJECT_H
#define	OBJECT_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "utils.h"
#include <iostream>

class Object {

protected:
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat height;
    GLfloat width;
    GLfloat depth;
    GLfloat right;
    GLfloat left;
    GLfloat top;
    GLfloat bottom;
    GLfloat back;
    GLfloat front;

public:
    Object(){
        x = 0;
        y = 0;
        height = 0;
        width = 0;
        right = 0;
        left = 0;
        top = 0;
        bottom = 0;
    }

    Object(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth);

    void setX(GLfloat x);
    void setY(GLfloat y);
    void setZ(GLfloat z);

    GLfloat getX(){
        return x;
    };
    GLfloat getY(){
        return y;
    };
    GLfloat getZ(){
        return z;
    };
    GLfloat getHeight(){
        return height;
    };
    GLfloat getWidth(){
        return width;
    };
    GLfloat getDepth(){
        return depth;
    };
    GLfloat getRight(){
        return right;
    };
    GLfloat getLeft(){
        return left;
    };
    GLfloat getTop(){
        return top;
    };
    GLfloat getBottom(){
        return bottom;
    };
    GLfloat getBack(){
        return back;
    };
    GLfloat getFront(){
        return front;
    };
};

#endif	/* OBJECT_H */