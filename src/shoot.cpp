#include "../headers/shoot.h"
#include "../headers/character.h" // Adicione esta linha!
#include "../headers/player.h"    // Para os métodos específicos
#include "../headers/enemy.h"     // Para os métodos específicos

void Shoot::drawSphere(GLfloat radius, GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
        glColor3f(R, G, B);
        GLUquadricObj *sphere = gluNewQuadric();
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluSphere(sphere, radius, 20, 20);
        gluDeleteQuadric(sphere);
    glPopMatrix();
}

void Shoot::draw()
{
    glPushMatrix();
        glTranslatef(x, y, z);
        drawSphere(radius, 1.0, 0.7, 0.5);
    glPopMatrix();
}

void Shoot::move(GLdouble timeDiff)
{
    double mx = roundToDecimalPlaces(timeDiff * speed * dirX, 5);
    double my = roundToDecimalPlaces(timeDiff * speed * dirY, 5);
    double mz = roundToDecimalPlaces(timeDiff * speed * dirZ, 5);

    setX(getX() + mx);
    setY(getY() + my);
    setZ(getZ() + mz);
}

bool Shoot::checkCollisionCharacter(Character& character)
{
    // Centro da esfera (tiro)
    double sx = this->getX();
    double sy = this->getY();
    double sz = this->getZ();

    // Centro da base do cilindro (personagem)
    double cx = character.getX() + character.getWidth()/2;
    double cy = character.getY();
    double cz = character.getZ() + character.getDepth()/2;
    double cylinderRadius = character.getCylinderRadius();
    double cylinderHeight = character.getHeight();

    // Projeção do centro da esfera no eixo Y do cilindro
    double projectionY = sy;

    // Restringir a projeção para estar dentro da altura do cilindro
    if (projectionY < cy) {
        projectionY = cy;
    }
    else if (projectionY > cy + cylinderHeight) {
        projectionY = cy + cylinderHeight;
    }

    // Calcular a distância no plano XZ entre o centro da esfera e o eixo do cilindro
    double dx = sx - cx;
    double dz = sz - cz;
    double distanceXZ = sqrt(dx * dx + dz * dz);

    // Determinar o ponto mais próximo na superfície do cilindro
    double nearestX, nearestZ;

    if (distanceXZ == 0) {
        // A esfera está diretamente acima ou abaixo do cilindro
        nearestX = cx;
        nearestZ = cz;
    } else {
        // O ponto na superfície do cilindro
        nearestX = cx + (dx * cylinderRadius / distanceXZ);
        nearestZ = cz + (dz * cylinderRadius / distanceXZ);
    }

    // Calcular a distância entre o centro da esfera e o ponto mais próximo
    double distX = sx - nearestX;
    double distY = sy - projectionY;
    double distZ = sz - nearestZ;
    double distance = sqrt(distX * distX + distY * distY + distZ * distZ);

    // Há colisão se a distância for menor ou igual ao raio da esfera
    return distance <= this->radius;
}

// Método específico para Player
bool Shoot::checkCollisionPlayer(Player& player) {
    return checkCollisionCharacter(player);
}

// Método específico para Enemy
bool Shoot::checkCollisionEnemy(Enemy& enemy) {
    return checkCollisionCharacter(enemy);
}

bool Shoot::checkCollisionObstacle(Object obj) {
    if (getRight() > obj.getLeft() && getLeft() < obj.getRight() &&
        getBottom() > obj.getTop() && getTop() < obj.getBottom()) {
        return true;
    }

    return false;
}

bool Shoot::checkCollisionArena(Arena arena){
    if (getLeft() < arena.getLeft() || getRight() > arena.getRight() ||
        getTop() < arena.getTop() || getBottom() > arena.getBottom()) {
        return true;
    }

    return false;
}