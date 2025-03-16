#ifndef ENEMY_H
#define	ENEMY_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "character.h"

class Enemy : public Character {
    static int shootTimer;

private:
    GLfloat walkSpeed = 0.01;
    int walkTimer = 0;
    int maxWalkTimer = 1500;
    int idleTimer = 0;
    int maxIdleTimer = 7000;
    int nextDirectionAngle = 0;

public:
    Enemy(){}; // Default constructor
    Enemy(GLfloat x, GLfloat y, GLfloat z, GLfloat r) : Character(x, y, z, r, false) {};
    void draw(int camera){
        Character::draw(1.0, 0.5, 0.0, camera);
    }
    bool operator==(const Enemy& other) const {
        return this->x == other.x && this->y == other.y;
    }

    static void setShootTimer(int value){
        shootTimer = value;
    }
    static void addShootTimer(int value) {
        shootTimer += value;
    }
    void addWalkTimer(GLfloat value){
        walkTimer += value;
    }
    void resetWalkTimer(){
        walkTimer = 0;
    }
    void setNextDirectionAngle(int value){
        nextDirectionAngle = value;
    }
    void addIdleTimer(int value){
        idleTimer += value;
    }
    void resetIdleTimer(){
        idleTimer = 0;
    }

    GLfloat getWalkSpeed(){
        return walkSpeed;
    };
    static int getShootTimer() {
        return shootTimer;
    }
    int getWalkTimer(){
        return walkTimer;
    }
    int getMaxWalkTimer(){
        return maxWalkTimer;
    }
    int getNextDirectionAngle(){
        return nextDirectionAngle;
    }
    int getIdleTimer(){
        return idleTimer;
    }
    int getMaxIdleTimer(){
        return maxIdleTimer;
    }
};

#endif	/* ENEMY_H */

