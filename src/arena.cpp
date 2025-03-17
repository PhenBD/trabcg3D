#include "../headers/arena.h"

void Arena::displayPlane(GLuint texture)
{
    int divisions = log(width * height) * 4;
    glBindTexture(GL_TEXTURE_2D, texture);
    double textureS = 1; // Controla a repetição da textura
    float step = 1.0f / divisions;
    float texStep = textureS / divisions;

    for (int i = 0; i < divisions; i++)
    {
        glBegin(GL_TRIANGLE_STRIP);
            for (int j = divisions; j >= 0; j--)
            {
                float x0 = -0.5f + i * step;
                float x1 = x0 + step;
                float z = -0.5f + j * step;

                float tx0 = i * texStep;
                float tx1 = tx0 + texStep;
                float tz = j * texStep;

                glNormal3f(0, 1, 0);
                glTexCoord2f(tx0, tz);
                glVertex3f(x0, 0, z);

                glTexCoord2f(tx1, tz);
                glVertex3f(x1, 0, z);
            }
        glEnd();
    }
}

void Arena::draw(bool nightMode, GLuint texture) {
    // Configurar 5 luzes espalhadas uniformemente pela arena
    if (!nightMode) {
        // Calcular as posições para 5 luzes distribuídas uniformemente
        float spacing = width / 6.0f; // Divide em 6 partes para ter 5 pontos intermediários
        
        for (int i = 0; i < 5; i++) {
            // Configurar posição da luz
            GLfloat center_position[] = {x + spacing * (i + 1), y + height * 0.1f, z + depth/2, 1.0f};
            
            // Cores diferentes para cada luz (opcional)
            GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
            GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; 
            GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 0.0f};
            
            // Configurar propriedades da luz
            glLightfv(GL_LIGHT0 + i, GL_POSITION, center_position);
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);

            // Configurar atenuação para simular decaimento realista da luz
            glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0.3f);   // Menor valor = luz mais forte
            glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.002f);   // Decaimento com distância
            glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.0005f); // Decaimento quadrático

            // Ativar a luz
            glEnable(GL_LIGHT0 + i);
            
            // Desenhar uma pequena esfera para representar visualmente a luz
            glPushMatrix();
                glDisable(GL_LIGHTING);  // Desabilitar iluminação para a esfera da luz
                glTranslatef(center_position[0], center_position[1], center_position[2]);
                glColor3f(light_diffuse[0], light_diffuse[1], light_diffuse[2]);
                
                GLUquadricObj *sphere = gluNewQuadric();
                gluQuadricNormals(sphere, GLU_SMOOTH);
                gluSphere(sphere, 1.0f, 10, 10);  // Esfera pequena
                gluDeleteQuadric(sphere);
                
                glEnable(GL_LIGHTING);  // Reabilitar iluminação
            glPopMatrix();
        }
    } else {
        // No modo noturno, desativar todas as luzes da arena
        for (int i = 0; i < 5; i++) {
            glDisable(GL_LIGHT0 + i);
        }
    }

    GLfloat materialEmission[] = { 0.0, 0.0, 0.0, 1};
    GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
    GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1};
    GLfloat mat_shininess[] = { 100.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    // Faces com subdivisões (4x4 por padrão)
    // Face frontal
    drawRectangle(x, y, z + depth,                    // v1
        x + width, y, z + depth,            // v2
        x + width, y + height, z + depth,   // v3
        x, y + height, z + depth,           // v4
        0.0f, 0.0f, -1.0f,
        texture, 3);         // normal

    // Face traseira
    drawRectangle(x, y, z,            // v1
            x + width, y, z,    // v2
            x + width, y + height, z, // v3
            x, y + height, z ,   // v4
            0.0f, 0.0f, 1.0f,
            texture, 3);        // normal

    // Face inferior
    drawRectangle(x, y + height, z,           // v1
            x + width, y + height, z,   // v2
            x + width, y + height, z + depth, // v3
            x, y + height, z + depth,   // v4
            0.0f, -1.0f, 0.0f,
            texture, 3);        // normal

    // Face superior
    drawRectangle(x, y, z,                    // v1
            x + width, y, z,            // v2
            x + width, y, z + depth,    // v3
            x, y, z + depth,            // v4
            0.0f, 1.0f, 0.0f,
            texture, 3);         // normal

    // Face lateral esquerda
    drawRectangle(x, y, z,                    // v1
            x, y + height, z,           // v2
            x, y + height, z + depth,   // v3
            x, y, z + depth,            // v4
            1.0f, 0.0f, 0.0f,
            texture, 1);         // normal

    // Face lateral direita
    drawRectangle(x + width, y, z,            // v1
            x + width, y + height, z,   // v2
            x + width, y + height, z + depth, // v3
            x + width, y, z + depth,    // v4
            -1.0f, 0.0f, 0.0f,
            texture, 1);        // normal
}

