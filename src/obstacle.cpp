#include "../headers/obstacle.h"



void Obstacle::draw(GLuint texture) {
    glPushMatrix();
        glColor3f(0, 0, 0);
        // glTranslatef(x + width/2, y + height/2, z + depth/2);  // Translada para a posição do meio do obstáculo

        GLfloat materialEmission[] = { 0.0, 0.0, 0.0, 1};
        GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
        GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
        GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1};
        GLfloat mat_shininess[] = { 100.0 };

        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialColorA);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColorD);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

        // glScalef(width, height, depth);  // Escala o cubo para as dimensões do obstáculo
        // glutSolidCube(1);  // Desenha um cubo com as dimensões do obstáculo

        // Faces com subdivisões (4x4 por padrão)
        // Face frontal
        drawRectangle(x, y, z + depth,                    // v1
                x + width, y, z + depth,            // v2
                x + width, y + height, z + depth,   // v3
                x, y + height, z + depth,           // v4
                0.0f, 0.0f, 1.0f,
                texture, 5);         // normal

        // Face traseira
        drawRectangle(x, y, z,            // v1
                x + width, y, z,    // v2
                x + width, y + height, z, // v3
                x, y + height, z ,   // v4
                0.0f, 0.0f, -1.0f,
                texture, 5);        // normal

        // Face superior
        drawRectangle(x, y + height, z,           // v1
                x + width, y + height, z,   // v2
                x + width, y + height, z + depth, // v3
                x, y + height, z + depth,   // v4
                0.0f, 1.0f, 0.0f,
                texture, 5);        // normal

        // Face inferior
        drawRectangle(x, y, z,                    // v1
                x + width, y, z,            // v2
                x + width, y, z + depth,    // v3
                x, y, z + depth,            // v4
                0.0f, -1.0f, 0.0f,
                texture, 5);         // normal

        // Face lateral esquerda
        drawRectangle(x, y, z,                    // v1
                x, y + height, z,           // v2
                x, y + height, z + depth,   // v3
                x, y, z + depth,            // v4
                -1.0f, 0.0f, 0.0f,
                texture, 5);         // normal

        // Face lateral direita
        drawRectangle(x + width, y, z,            // v1
                x + width, y + height, z,   // v2
                x + width, y + height, z + depth, // v3
                x + width, y, z + depth,    // v4
                1.0f, 0.0f, 0.0f,
                texture, 5);        // normal

    glPopMatrix();
}