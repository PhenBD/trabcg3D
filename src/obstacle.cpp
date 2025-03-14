#include "../headers/obstacle.h"

void Obstacle::draw() {
    glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(x, y, z);  // Translada para a posição do obstáculo
    
        // Face frontal
        glBegin(GL_QUADS);
            glVertex3f(0, 0, 0);                // Ponto inferior esquerdo frontal
            glVertex3f(width, 0, 0);            // Ponto inferior direito frontal
            glVertex3f(width, height, 0);       // Ponto superior direito frontal
            glVertex3f(0, height, 0);           // Ponto superior esquerdo frontal
        glEnd();
        
        // Face traseira
        glBegin(GL_QUADS);
            glVertex3f(0, 0, depth);            // Ponto inferior esquerdo traseiro
            glVertex3f(width, 0, depth);        // Ponto inferior direito traseiro
            glVertex3f(width, height, depth);   // Ponto superior direito traseiro
            glVertex3f(0, height, depth);       // Ponto superior esquerdo traseiro
        glEnd();
        
        // Face superior
        glBegin(GL_QUADS);
            glVertex3f(0, height, 0);           // Ponto superior esquerdo frontal
            glVertex3f(width, height, 0);       // Ponto superior direito frontal
            glVertex3f(width, height, depth);   // Ponto superior direito traseiro
            glVertex3f(0, height, depth);       // Ponto superior esquerdo traseiro
        glEnd();
        
        // Face inferior
        glBegin(GL_QUADS);
            glVertex3f(0, 0, 0);                // Ponto inferior esquerdo frontal
            glVertex3f(width, 0, 0);            // Ponto inferior direito frontal
            glVertex3f(width, 0, depth);        // Ponto inferior direito traseiro
            glVertex3f(0, 0, depth);            // Ponto inferior esquerdo traseiro
        glEnd();
        
        // Face lateral esquerda
        glBegin(GL_QUADS);
            glVertex3f(0, 0, 0);                // Ponto inferior esquerdo frontal
            glVertex3f(0, height, 0);           // Ponto superior esquerdo frontal
            glVertex3f(0, height, depth);       // Ponto superior esquerdo traseiro
            glVertex3f(0, 0, depth);            // Ponto inferior esquerdo traseiro
        glEnd();
        
        // Face lateral direita
        glBegin(GL_QUADS);
            glVertex3f(width, 0, 0);            // Ponto inferior direito frontal
            glVertex3f(width, height, 0);       // Ponto superior direito frontal
            glVertex3f(width, height, depth);   // Ponto superior direito traseiro
            glVertex3f(width, 0, depth);        // Ponto inferior direito traseiro
        glEnd();
    glPopMatrix();
}