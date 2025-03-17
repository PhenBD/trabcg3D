#ifndef CHARACTER_H
#define	CHARACTER_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "object.h"
#include "obstacle.h"
#include "arena.h"
#include "shoot.h"
#include "utils.h"
#include <list>
#include <cmath>

// Enum for directions of movement
enum Direction {UP, DOWN, LEFT, RIGHT, FRONT, BACK};

class Character : public Object {

protected:
    GLfloat thetaLeft1 = 0;
    GLfloat thetaLeft2 = 0;
    GLfloat thetaRight1 = 0;
    GLfloat thetaRight2 = 0;
    GLfloat thetaArmXY = -45;
    GLfloat thetaArmXZ = 0;
    GLfloat armHeight = 0.2 * height;

private:
    int direction = RIGHT;
    GLfloat directionAngle = 90.0f;
    int lookingDirection = RIGHT;
    GLfloat walkSpeed = 0.04;
    bool walking = false;
    GLfloat legsAnimation = walkSpeed * 3;
    bool player;
    float cylinderRadius;
    float bodyWidth, bodyDepth;

    void drawBody(GLuint texture);
    void drawHead(GLfloat radius, GLuint texture);
    void drawArm(GLfloat x, GLfloat y, GLfloat z, GLfloat thetaXY, GLfloat thetaXZ, GLuint texture);
    void drawLegs(GLfloat x, GLfloat y, GLfloat z, GLuint texture);
    void drawLegPart(GLuint texture);

public:
    Character(){}; // Default constructor
    Character(GLfloat x, GLfloat y, GLfloat z, GLfloat r, bool player) : Object(x - r, y - r, z, r/2, 2*r, r/2) {
        this->player = player;
        this->cylinderRadius = sqrt(width*width + depth*depth) / 2;
        this->bodyWidth = width;
        this->bodyDepth = depth;
        this->width = 2 * cylinderRadius;
        this->right = x + this->width;
        this->depth = 2 * cylinderRadius;
        this->front = z + this->depth;
    };
    void draw(GLfloat R, GLfloat G, GLfloat B, int camera, bool nightMode, GLuint textureArm, GLuint textureBody, GLuint textureLegs, GLuint textureHead);
    void moveX(GLfloat dx, GLdouble timeDiff);
    void moveY(GLfloat dy, GLdouble timeDiff);
    void moveZ(GLfloat dz, GLdouble timeDiff);
    void rotateXZ(GLfloat angle);
    int checkCollisionObstacle(Obstacle obs);
    int checkCollisionArena(Arena arena);
    int checkCollisionCharacter(Character other);
    void shoot(std::list<Shoot> &shoots);
    void drawCollisonBox();

    void setDirection(int direction){
        this->direction = direction;
    };
    void setLookingDirection(int direction){
        this->lookingDirection = direction;
    };
    void setThetaArmXY(GLfloat theta){
        this->thetaArmXY = theta;
    };
    void setThetaArmXZ(GLfloat theta){
        this->thetaArmXZ = theta;
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
    GLfloat getThetaArmXY(){
        return thetaArmXY;
    };  
    GLfloat getThetaArmXZ(){
        return thetaArmXZ;
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
    float getCylinderRadius(){
        return cylinderRadius;
    };
    float getBodyWidth(){
        return bodyWidth;
    };
    float getBodyDepth(){
        return bodyDepth;
    };
};

#endif	/* CHARACTER_H */