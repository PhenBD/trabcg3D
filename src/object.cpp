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