#ifndef ENEMY_H
#define	ENEMY_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "character.h"

class Enemy : public Character {
    static int shootTimer;

private:
    GLfloat walkSpeed = 0.01;
    int walkingDirection = RIGHT;

public:
    Enemy(){}; // Default constructor
    Enemy(GLfloat x, GLfloat y, GLfloat z, GLfloat r) : Character(x, y, z, r, false) {};
    void draw(){
        Character::draw(1.0, 0.5, 0.0);
    }
    bool operator==(const Enemy& other) const {
        return this->x == other.x && this->y == other.y;
    }

    void setWalkingDirection(int direction){
        walkingDirection = direction;
    };
    static void setShootTimer(int value){
        shootTimer = value;
    }
    static void addShootTimer(int value) {
        shootTimer += value;
    }

    GLfloat getWalkSpeed(){
        return walkSpeed;
    };
    GLfloat getWalkingDirection(){
        return walkingDirection;
    };
    static int getShootTimer() {
        return shootTimer;
    }
};

#endif	/* ENEMY_H */

