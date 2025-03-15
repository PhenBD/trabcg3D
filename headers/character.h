#ifndef CHARACTER_H
#define	CHARACTER_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "object.h"
#include "obstacle.h"
#include "arena.h"
#include "shoot.h"
#include <list>

// Enum for directions of movement
enum Direction {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

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
    GLfloat directionAngle = 90.0f;
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
    Character(GLfloat x, GLfloat y, GLfloat z, GLfloat r, bool player) : Object(x - r, y - r, z, r/2, 2*r, r/2) {
        this->player = player;
    };
    void draw(GLfloat R, GLfloat G, GLfloat B);
    void moveX(GLfloat dx, GLdouble timeDiff);
    void moveY(GLfloat dy, GLdouble timeDiff);
    void moveZ(GLfloat dz, GLdouble timeDiff);
    void rotateXZ(GLfloat angle);
    int checkCollisionObstacle(Obstacle obs);
    int checkCollisionArena(Arena arena);
    int checkCollisionCharacter(Character other);
    void flipDirection();
    void shoot(std::list<Shoot> &shoots);
    void drawCollisonBox();

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
    void setDirectionAngle(int direction){
        directionAngle = direction;
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
    GLfloat getDirectionAngle(){
        return directionAngle;
    };
    bool isPlayer(){
        return player;
    };
};

#endif	/* CHARACTER_H */