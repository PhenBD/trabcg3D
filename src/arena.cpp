#include "../headers/arena.h"

void Arena::draw() {
    glColor3f(0.0, 0.0, 0.7);  // Um azul mais escuro
    
    // Face frontal
    glBegin(GL_QUADS);
        glVertex3f(x, y, z);
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x, y + height, z);
    glEnd();
    
    // Face traseira
    glBegin(GL_QUADS);
        glVertex3f(x, y, z + depth);
        glVertex3f(x + width, y, z + depth);
        glVertex3f(x + width, y + height, z + depth);
        glVertex3f(x, y + height, z + depth);
    glEnd();
    
    // Face superior
    glColor3f(0.0, 0.0, 0.8);  // Um azul um pouco diferente para distinguir
    glBegin(GL_QUADS);
        glVertex3f(x, y + height, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x + width, y + height, z + depth);
        glVertex3f(x, y + height, z + depth);
    glEnd();
    
    // Face inferior
    glBegin(GL_QUADS);
        glVertex3f(x, y, z);
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y, z + depth);
        glVertex3f(x, y, z + depth);
    glEnd();
    
    // Face lateral esquerda
    glColor3f(0.0, 0.0, 0.6);  // Um azul ainda mais escuro
    glBegin(GL_QUADS);
        glVertex3f(x, y, z);
        glVertex3f(x, y + height, z);
        glVertex3f(x, y + height, z + depth);
        glVertex3f(x, y, z + depth);
    glEnd();
    
    // Face lateral direita
    glBegin(GL_QUADS);
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x + width, y + height, z + depth);
        glVertex3f(x + width, y, z + depth);
    glEnd();
}