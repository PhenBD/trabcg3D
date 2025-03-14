#include "../headers/shoot.h"

void Shoot::drawCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
        glColor3f(R, G, B);
        glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i+=20)
            {
                double radians = i * M_PI / 180.0;

                double x = radius * cos(radians);
                double y = radius * sin(radians);
                glVertex3f(x, y, 0);
            }
        glEnd();
    glPopMatrix();
}

void Shoot::draw()
{
    glPushMatrix();
        glTranslatef(x + radius, y + radius, 0.0);
        drawCirc(radius, 1.0, 0.7, 0.5);
    glPopMatrix();
}

void Shoot::move(GLdouble timeDiff)
{
    double radians = directionAng * M_PI / 180.0;

    double mx = roundToDecimalPlaces(timeDiff * speed * cos(radians), 5);
    double my = roundToDecimalPlaces(timeDiff * speed * sin(radians), 5);

    setX(getX() + mx);
    setY(getY() + my);
}

bool Shoot::checkCollision(Object obj) {
    if (getRight() > obj.getLeft() && getLeft() < obj.getRight() &&
        getBottom() > obj.getTop() && getTop() < obj.getBottom()) {
        return true;
    }

    return false;
}

bool Shoot::checkArenaCollision(Arena arena){
    if (getLeft() < arena.getLeft() || getRight() > arena.getRight() ||
        getTop() < arena.getTop() || getBottom() > arena.getBottom()) {
        return true;
    }

    return false;
}