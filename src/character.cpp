#include "../headers/character.h"

void Character::drawRect(GLfloat height, GLfloat width, GLfloat depth, GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
        glColor3f(R, G, B);
        
        // Face frontal
        glBegin(GL_QUADS);
            glVertex3f(-width/2, 0, depth/2);
            glVertex3f(width/2, 0, depth/2);
            glVertex3f(width/2, height, depth/2);
            glVertex3f(-width/2, height, depth/2);
        glEnd();
        
        // Face traseira
        glBegin(GL_QUADS);
            glVertex3f(-width/2, 0, -depth/2);
            glVertex3f(width/2, 0, -depth/2);
            glVertex3f(width/2, height, -depth/2);
            glVertex3f(-width/2, height, -depth/2);
        glEnd();
        
        // Face superior
        glBegin(GL_QUADS);
            glVertex3f(-width/2, height, depth/2);
            glVertex3f(width/2, height, depth/2);
            glVertex3f(width/2, height, -depth/2);
            glVertex3f(-width/2, height, -depth/2);
        glEnd();
        
        // Face inferior
        glBegin(GL_QUADS);
            glVertex3f(-width/2, 0, depth/2);
            glVertex3f(width/2, 0, depth/2);
            glVertex3f(width/2, 0, -depth/2);
            glVertex3f(-width/2, 0, -depth/2);
        glEnd();
        
        // Face lateral esquerda
        glBegin(GL_QUADS);
            glVertex3f(-width/2, 0, depth/2);
            glVertex3f(-width/2, height, depth/2);
            glVertex3f(-width/2, height, -depth/2);
            glVertex3f(-width/2, 0, -depth/2);
        glEnd();
        
        // Face lateral direita
        glBegin(GL_QUADS);
            glVertex3f(width/2, 0, depth/2);
            glVertex3f(width/2, height, depth/2);
            glVertex3f(width/2, height, -depth/2);
            glVertex3f(width/2, 0, -depth/2);
        glEnd();
    glPopMatrix();
}

void Character::drawCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
        glColor3f(R, G, B);

        GLUquadricObj *sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluSphere(sphere, radius, 20, 20);  // radius, slices, stacks
        gluDeleteQuadric(sphere);
    glPopMatrix();
}

void Character::drawArm(GLfloat x, GLfloat y, GLfloat z, GLfloat theta)
{
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(theta, 0, 0, 1);
        drawRect(height * 0.25, height * 0.05, depth * 0.2, 1.0, 1.0, 0.0);
    glPopMatrix();
}

void Character::drawLegs(GLfloat x, GLfloat y, GLfloat z){
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(thetaLeft1, 0, 0, 1);
        drawRect(height * 0.22, height * 0.05, depth * 0.2, 1.0, 0.0, 0.0);
        glTranslatef(0, height * 0.22, 0);
        glRotatef(thetaLeft2, 0, 0, 1);
        drawRect(height * 0.22, height * 0.05, depth * 0.2, 1.0, 0.0, 0.0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(thetaRight1, 0, 0, 1);
        drawRect(height * 0.22, height * 0.05, depth * 0.2, 1.0, 0.0, 0.0);
        glTranslatef(0, height * 0.22, 0);
        glRotatef(thetaRight2, 0, 0, 1);
        drawRect(height * 0.22, height * 0.05, depth * 0.2, 1.0, 0.0, 0.0);
    glPopMatrix();
}

void Character::draw(GLfloat R, GLfloat G, GLfloat B) {
    glPushMatrix();
        glTranslatef(x + (width/2), y + (0.15 * height), z + (depth/2));
        glRotatef(walkingDirection - 90, 0, 1, 0);
        drawCirc((0.15 * height), R, G, B);
        glTranslatef(0, (0.15 * height), 0);
        drawRect(0.3 * height, width, depth, R, G, B);
        drawArm(0, 0.1 * height, 0, thetaArm);
        drawLegs(0, 0.3 * height, 0);
    glPopMatrix();
}

int Character::checkCollision(Object obj) {
    // Check collision on X and Y axes
    if (getRight() > obj.getLeft() && getLeft() < obj.getRight() &&
        getBottom() > obj.getTop() && getTop() < obj.getBottom()) {
        
        // Handle collision based on Character's direction
        switch (getDirection()) {
            case RIGHT:
                setX(obj.getLeft() - getWidth());
                return RIGHT;
            case LEFT:
                setX(obj.getRight());
                return LEFT;
            case UP:
                setY(obj.getBottom());
                return UP;
            case DOWN:
                setY(obj.getTop() - getHeight());
                return DOWN;
            default:
                break;
        }
    }

    return -1;
}

int Character::checkArenaCollision(Arena arena){
    // X axis
    if (getLeft() < arena.getLeft())
    {
        setX(arena.getLeft());
        return LEFT;
    }
    else if (getRight() > arena.getRight()) 
    {
        setX(arena.getRight() - getWidth());
        return RIGHT;
    }
    // Y axis
    if (getTop() < arena.getTop())
    {
        setY(arena.getTop());
        return UP;
    }
    else if (getBottom() > arena.getBottom()) 
    {
        setY(arena.getBottom() - getHeight());
        return DOWN;
    }
    // Z axis
    if (getFront() > arena.getFront())
    {
        setZ(arena.getFront() - getDepth());
        return FRONT;
    }
    else if (getBack() < arena.getBack()) 
    {
        setZ(arena.getBack());
        return BACK;
    }

    return -1;
}

void Character::flipDirection() {
    thetaArm *= -1;
    thetaLeft1 *= -1;
    thetaLeft2 *= -1;
    thetaRight1 *= -1;
    thetaRight2 *= -1;
}

void Character::moveX(GLfloat dx, GLdouble timeDiff) {
    if (walking){
        if (lookingDirection == LEFT){
            thetaLeft1 += -legsAnimation * timeDiff;
            thetaLeft2 += -legsAnimation / 1.5 * timeDiff;
            thetaRight1 += legsAnimation * timeDiff;
            thetaRight2 += -legsAnimation / 1.5 * timeDiff;
        }
        else if (lookingDirection == RIGHT){
            thetaLeft1 += legsAnimation * timeDiff;
            thetaLeft2 += legsAnimation / 1.5 * timeDiff;
            thetaRight1 += -legsAnimation * timeDiff;
            thetaRight2 += legsAnimation / 1.5 * timeDiff;
        }

        if (thetaLeft1 > 45 || thetaLeft1 < -45 || thetaRight2 > 45 || thetaRight2 < -45){
            legsAnimation *= -1;
        }
        if ((thetaLeft1 > 0 || thetaLeft2 > 0 || thetaRight1 < 0 || thetaRight2 > 0) && lookingDirection == LEFT){
            legsAnimation *= -1;
        }
        else if ((thetaLeft1 < 0 || thetaLeft2 < 0 || thetaRight1 > 0 || thetaRight2 < 0) && lookingDirection == RIGHT){
            legsAnimation *= -1;
        }
    }
    else {
        thetaLeft1 = 0;
        thetaLeft2 = 0;
        thetaRight1 = 0;
        thetaRight2 = 0;
    }
    
    setX(x + dx * timeDiff);
}

void Character::moveY(GLfloat dy, GLdouble timeDiff) {
    setY(y + dy * timeDiff);
}

void Character::moveZ(GLfloat dz, GLdouble timeDiff) {
    setZ(z + dz * timeDiff);
}

void Character::rotateXZ(GLfloat angle) {
    walkingDirection += angle;

    if (walkingDirection >= 360.0f)
        walkingDirection -= 360.0f;
    else if (walkingDirection < 0.0f)
        walkingDirection += 360.0f;
}

void Character::shoot(std::list<Shoot> &shoots){
    int ySignal = 1, xSignal = 1;
    if (getThetaArm() < -90 || getThetaArm() > 90)
        ySignal = -1;
    if (getThetaArm() > 0)
        xSignal = -1;

    GLfloat xs = getX() + (width / 2) + (xSignal * height * 0.25 * abs(sin(getThetaArm() * (M_PI / 180.0))));
    GLfloat ys = getY() + (0.4 * height) + (ySignal * height * 0.25 * abs(cos(getThetaArm() * (M_PI / 180.0))));
    Shoot shoot(xs, ys, getThetaArm() + 90, walkSpeed * 2, height * 0.07, player);
    shoots.push_back(shoot);
}