#include "../headers/object.h"

Object::Object(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat depth){
    this->x = roundToDecimalPlaces(x, 5);
    this->y = roundToDecimalPlaces(y, 5);
    this->z = roundToDecimalPlaces(z, 5);
    this->width = roundToDecimalPlaces(width, 5);
    this->height = roundToDecimalPlaces(height, 5);
    this->depth = roundToDecimalPlaces(depth, 5);
    right = x + width;
    left = x;
    top = y;
    bottom = y + height;
    back = z;
    front = z + depth;
};

void Object::drawAxis(){
    GLfloat color_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat color_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat color_b[] = { 0.0, 0.0, 1.0, 1.0 };

    glPushMatrix();
        glPushAttrib(GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);

            glTranslatef(x, y, z);
            //x axis - Vermelho
            glPushMatrix();
                glColor3fv(color_r);
                glScalef(3, 0.3, 0.3);
                glTranslatef(0.5, 0, 0); // put in one end
                glutSolidCube(1.0);
            glPopMatrix();

            //y axis - Verde
            glPushMatrix();
                glColor3fv(color_g);
                glRotatef(90,0,0,1);
                glScalef(3, 0.3, 0.3);
                glTranslatef(0.5, 0, 0); // put in one end
                glutSolidCube(1.0);
            glPopMatrix();

            //z axis - Azul
            glPushMatrix();
                glColor3fv(color_b);
                glRotatef(-90,0,1,0);
                glScalef(3, 0.3, 0.3);
                glTranslatef(0.5, 0, 0); // put in one end
                glutSolidCube(1.0);
            glPopMatrix();
        glPopAttrib();
    glPopMatrix();
};

void Object::setX(GLfloat x){
    this->x = x;
    right = x + width;
    left = x;
};

void Object::setY(GLfloat y){
    this->y = y;
    top = y;
    bottom = y + height;
};

void Object::setZ(GLfloat z){
    this->z = z;
    back = z;
    front = z + depth;
};