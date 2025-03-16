#ifndef SHOOT_H
#define	SHOOT_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "object.h"
#include "arena.h"
#include "utils.h"
#include <iostream>

// Forward declaration - apenas declara que a classe Character existe
class Character;
class Player;
class Enemy;

class Shoot : public Object{
private:
    GLfloat dirX, dirY, dirZ;
    GLfloat speed;
    GLfloat radius;
    bool isPlayerShoot = false;

    void drawSphere(GLfloat radius, GLfloat R, GLfloat G, GLfloat B);
public:
    Shoot(GLfloat x, GLfloat y, GLfloat z, GLfloat dirX, GLfloat dirY, GLfloat dirZ, GLfloat speed, GLfloat radius, bool isPlayerShoot) : Object(x, y, z, 2*radius, 2*radius, 2*radius) {
        this->dirX = dirX;
        this->dirY = dirY;
        this->dirZ = dirZ;
        this->speed = speed;
        this->radius = radius;
        this->isPlayerShoot = isPlayerShoot;
    };

    void draw();
    void move(GLdouble timeDiff);
    bool checkCollisionCharacter(Character& character);
    bool checkCollisionPlayer(Player& player);
    bool checkCollisionEnemy(Enemy& enemy); 
    bool checkCollisionObstacle(Object obj);
    bool checkCollisionArena(Arena arena);
    bool operator==(const Shoot& other) const {
        return this->x == other.x && this->y == other.y;
    }

    bool isPlayer(){
        return isPlayerShoot;
    };
};

#endif	/* SHOOT_H */

