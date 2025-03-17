#include "../headers/utils.h"
#include <iostream>
#include "../headers/enemy.h"

GLfloat roundToDecimalPlaces(GLfloat value, int decimalPlaces) {
    GLfloat scale = std::pow(10.0f, decimalPlaces);
    return std::round(value * scale) / scale;
}

GLfloat calculateArmAngle(GLfloat x, GLfloat y, GLfloat x_origin, GLfloat y_origin) {
    // Difference in coordinates
    GLfloat dx = x - x_origin;
    GLfloat dy = y - y_origin;

    // Calculate the angle in radians
    GLfloat angleRadians = atan2(dy, dx);

    // Convert to degrees
    GLfloat angleDegrees = angleRadians * (180.0 / M_PI);

    // Ensure the angle is between 0 and 360 degrees
    if (angleDegrees < 0) {
        angleDegrees += 360.0;
    }

    angleDegrees = abs(angleDegrees - 90.0);

    // Adjust the angle to the range [0, 180]
    if (angleDegrees > 180.0) {
        angleDegrees = 360.0 - angleDegrees;
    }

    // Limit the angle to the range [45, 135]
    if (angleDegrees < 45.0) {
        return 45.0;  // Adjust to the lower limit
    } else if (angleDegrees > 135.0) {
        return 135.0; // Adjust to the upper limit
    }

    // Return the angle within the range
    return angleDegrees;
}

// Normaliza um vetor 3D
void normalizeVector3D(float &x, float &y, float &z) {
    float length = sqrt(x*x + y*y + z*z);
    
    // Evite divisão por zero
    if (length > 0.0001f) {
        x /= length;
        y /= length;
        z /= length;
    }
}

// Calcula o produto vetorial de dois vetores 3D
// (ax, ay, az) × (bx, by, bz) = (cx, cy, cz)
void crossProduct3D(float ax, float ay, float az, 
                    float bx, float by, float bz,
                    float &cx, float &cy, float &cz) {
    cx = ay * bz - az * by;
    cy = az * bx - ax * bz;
    cz = ax * by - ay * bx;
}

// Função auxiliar para criar um retângulo subdividido com GL_TRIANGLE_STRIP
void drawRectangle(float x1, float y1, float z1, 
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float normalX, float normalY, float normalZ,
    GLuint texture, int repeats) {
        
    // Definir a normal da face
    glNormal3f(normalX, normalY, normalZ);

    float width = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
    float height = sqrt(pow(x4 - x1, 2) + pow(y4 - y1, 2) + pow(z4 - z1, 2));
    
    int divisions = log(width * height) * 4;
    if (divisions < 1) divisions = 1;

    float stepX1 = (x2 - x1) / divisions;
    float stepY1 = (y2 - y1) / divisions;
    float stepZ1 = (z2 - z1) / divisions;
    
    float stepX2 = (x4 - x1) / divisions;
    float stepY2 = (y4 - y1) / divisions;
    float stepZ2 = (z4 - z1) / divisions;

    // Configurar textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    double textureS = repeats; // Fator de repetição da textura
    float texStep = textureS / divisions;

    glNormal3f(normalX, normalY, normalZ);

    for (int i = 0; i < divisions; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = divisions; j >= 0; j--) {
            float x0 = x1 + i * stepX1 + j * stepX2;
            float y0 = y1 + i * stepY1 + j * stepY2;
            float z0 = z1 + i * stepZ1 + j * stepZ2;

            float x1_ = x1 + (i + 1) * stepX1 + j * stepX2;
            float y1_ = y1 + (i + 1) * stepY1 + j * stepY2;
            float z1_ = z1 + (i + 1) * stepZ1 + j * stepZ2;

            float tx0 = i * texStep;
            float tx1 = (i + 1) * texStep;
            float tz = j * texStep;

            glTexCoord2f(tx0, tz);
            glVertex3f(x0, y0, z0);

            glTexCoord2f(tx1, tz);
            glVertex3f(x1_, y1_, z1_);
        }
        glEnd();
    }

    // // Desenhar as bordas dos triângulos
    // glDisable(GL_LIGHTING);  // Desativar iluminação para as linhas
    // glColor3f(0.0f, 0.0f, 0.0f);  // Cor preta para as linhas
    // glLineWidth(1.0f);  // Espessura da linha

    // for (int i = 0; i < subdivY; i++) {
    //     glBegin(GL_LINE_STRIP);
    //     for (int j = 0; j <= subdivX; j++) {
    //         float px1 = x1 + j * dx1 + i * dx2;
    //         float py1 = y1 + j * dy1 + i * dy2;
    //         float pz1 = z1 + j * dz1 + i * dz2;
            
    //         float px2 = x1 + j * dx1 + (i + 1) * dx2;
    //         float py2 = y1 + j * dy1 + (i + 1) * dy2;
    //         float pz2 = z1 + j * dz1 + (i + 1) * dz2;

    //         glVertex3f(px1, py1, pz1);
    //         glVertex3f(px2, py2, pz2);
    //     }
    //     glEnd();
    // }

    // // Desenhar as linhas horizontais para completar os triângulos
    // for (int j = 0; j <= subdivX; j++) {
    //     glBegin(GL_LINE_STRIP);
    //     for (int i = 0; i <= subdivY; i++) {
    //         float px = x1 + j * dx1 + i * dx2;
    //         float py = y1 + j * dy1 + i * dy2;
    //         float pz = z1 + j * dz1 + i * dz2;
    //         glVertex3f(px, py, pz);
    //     }
    //     glEnd();
    // }
}

