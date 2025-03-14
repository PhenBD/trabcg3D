#ifndef SHOOT_H
#define	SHOOT_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "object.h"
#include "arena.h"
#include "utils.h"
#include <iostream>

class Shoot : public Object{
private:
    GLfloat directionAng;
    GLfloat speed;
    GLfloat radius;
    bool isPlayerShoot = false;

    void drawCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B);
public:
    Shoot(GLfloat x, GLfloat y, GLfloat directionAng, GLfloat speed, GLfloat radius, bool isPlayerShoot) : Object(x - radius, y - radius, 0.0f, 2*radius, 2*radius, 2*radius) {
        this->directionAng = directionAng; 
        this->speed = speed;
        this->radius = radius;
        this->isPlayerShoot = isPlayerShoot;
    };

    void draw();
    void move(GLdouble timeDiff);
    bool checkCollision(Object obj);
    bool checkArenaCollision(Arena arena);
    bool operator==(const Shoot& other) const {
        return this->x == other.x && this->y == other.y;
    }

    bool isPlayer(){
        return isPlayerShoot;
    };
};

#endif	/* SHOOT_H */

