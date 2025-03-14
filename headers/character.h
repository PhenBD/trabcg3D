#ifndef CHARACTER_H
#define	CHARACTER_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "object.h"
#include "arena.h"
#include "shoot.h"
#include <list>

// Enum for directions of movement
enum Direction {UP, DOWN, LEFT, RIGHT};

class Character : public Object {

protected:
    GLfloat thetaLeft1 = 0;
    GLfloat thetaLeft2 = 0;
    GLfloat thetaRight1 = 0;
    GLfloat thetaRight2 = 0;
    GLfloat thetaArm = -45;
    GLfloat armHeight = 0.4 * height;

private:
    int direction = RIGHT;
    int lookingDirection = RIGHT;
    GLfloat walkSpeed = 0.04;
    bool walking = false;
    GLfloat legsAnimation = walkSpeed * 3;
    bool player;

    void drawRect(GLfloat height, GLfloat width, GLfloat depth, GLfloat R, GLfloat G, GLfloat B);
    void drawCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B);
    void drawArm(GLfloat x, GLfloat y, GLfloat z, GLfloat theta);
    void drawLegs(GLfloat x, GLfloat y, GLfloat z);

public:
    Character(){}; // Default constructor
    Character(GLfloat x, GLfloat y, GLfloat z, GLfloat r, bool player) : Object(x - r, y - r, z, r/2, 2*r, r) {
        this->player = player;
    };
    void draw(GLfloat R, GLfloat G, GLfloat B);
    void moveX(GLfloat dx, GLdouble timeDiff);
    void moveY(GLfloat dy, GLdouble timeDiff);
    int checkCollision(Object obj);
    int checkArenaCollision(Arena arena);
    void flipDirection();
    void shoot(std::list<Shoot> &shoots);

    void setDirection(int direction){
        this->direction = direction;
    };
    void setLookingDirection(int direction){
        this->lookingDirection = direction;
    };
    void setThetaArm(GLfloat theta){
        this->thetaArm = theta;
    };
    void setWalking(bool walking){
        this->walking = walking;
    };

    int getDirection(){
        return direction;
    };
    int getLookingDirection(){
        return lookingDirection;
    };
    GLfloat getThetaArm(){
        return thetaArm;
    };  
    GLfloat getArmHeight(){
        return armHeight;
    };
    GLfloat getWalkSpeed(){
        return walkSpeed;
    };
    bool isPlayer(){
        return player;
    };
};

#endif	/* CHARACTER_H */