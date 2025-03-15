#ifndef PLAYER_H
#define	PLAYER_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "character.h"
#include <math.h>

class Player : public Character {

private:
    GLfloat jumpSpeed;
    bool onAir = false;
    bool jumping = false;
    int jumpingTime = 0;

public:
    Player(){}; // Default constructor
    Player(GLfloat x, GLfloat y, GLfloat z, GLfloat r) : Character(x, y, z, r, true) {
        jumpSpeed = ((height * 3) / 1000);
    };
    void draw(){
        Character::draw(0.0, 1.0, 0.0);
    }

    void setJumping(bool jumping){
        this->jumping = jumping;
    };
    void setJumpingTime(int time){
        jumpingTime = time;
    };
    void addJumpingTime(int time){
        jumpingTime += time;
    };
    void setOnAir(bool onAir){
        this->onAir = onAir;
    };

    GLfloat getJumpSpeed(){
        return jumpSpeed;
    };
    bool isJumping(){
        return jumping;
    };
    int getJumpingTime(){
        return jumpingTime;
    };
    bool isOnAir(){
        return onAir;
    };
};

#endif	/* PLAYER_H */