#include "../headers/character.h"
#include <algorithm>

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
        glRotatef(directionAngle - 90, 0, 1, 0);
        drawCirc((0.15 * height), R, G, B);
        glTranslatef(0, (0.15 * height), 0);
        drawRect(0.3 * height, width, depth, R, G, B);
        drawArm(0, 0.1 * height, 0, thetaArm);
        drawLegs(0, 0.3 * height, 0);
    glPopMatrix();
}

int Character::checkCollisionObstacle(Obstacle obs) {
    if (getRight() > obs.getLeft() && getLeft() < obs.getRight() &&
        getBottom() > obs.getTop() && getTop() < obs.getBottom()) {

        // Handle collision based on Character's direction
        switch (getDirection()) {
            case RIGHT:
                setX(obs.getLeft() - getWidth());
                return RIGHT;
            case LEFT:
                setX(obs.getRight());
                return LEFT;
            case UP:
                setY(obs.getBottom());
                return UP;
            case DOWN:
                setY(obs.getTop() - getHeight());
                return DOWN;
            default:
                break;
        }
    }

    return -1;
}

int Character::checkCollisionArena(Arena arena){
    // X axis
    if (getLeft() < arena.getLeft())
    {
        setX(arena.getLeft());
    }
    else if (getRight() > arena.getRight()) 
    {
        setX(arena.getRight() - getWidth());
        return RIGHT;
    }
    // Z axis
    if (getFront() > arena.getFront())
    {
        setZ(arena.getFront() - getDepth());
    }
    else if (getBack() < arena.getBack()) 
    {
        setZ(arena.getBack());
    }
    // Y axis
    if (getTop() < arena.getTop())
    {
        setY(arena.getTop());
    }
    else if (getBottom() > arena.getBottom()) 
    {
        setY(arena.getBottom() - getHeight());
        return DOWN;
    }

    return -1;
}

int Character::checkCollisionCharacter(Character other) {
    // Calculate cylinder centers
    float xp = x + width/2;
    float zp = z + depth/2;
    
    float xo = other.getX() + other.getWidth()/2;
    float zo = other.getZ() + other.getDepth()/2;
    
    // Calculate vector between centers (in XZ plane for cylinder collision)
    float dx = xo - xp;
    float dz = zo - zp;
    
    // Calculate distance between centers
    float distance = sqrt(dx*dx + dz*dz);
    
    // Get radius of both characters
    float radiusP = cylinderRadius;
    float radiusO = other.getCylinderRadius();


    if (getBottom() > other.getTop() && getTop() < other.getTop() &&
    distance < radiusP + radiusO && getDirection() == DOWN) {
        setY(other.getTop() - getHeight());
        return DOWN;
    }
    
    // Horizontal collision case
    if (getBottom() > other.getTop() && getTop() < other.getBottom() &&
        distance < radiusP + radiusO) {
        // Normalize the direction vector
        if (distance > 0) {
            dx /= distance;
            dz /= distance;
        } else {
            // Centers are exactly at the same position, choose arbitrary direction
            dx = 1.0f;
            dz = 0.0f;
        }
        
        // Calculate overlap amount
        float overlap = (radiusP + radiusO) - distance;
        
        // Move this character away from the collision
        setX(x - dx * overlap * 1.05);
        setZ(z - dz * overlap * 1.05);
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
    directionAngle += angle;

    if (directionAngle >= 360.0f)
        directionAngle -= 360.0f;
    else if (directionAngle < 0.0f)
        directionAngle += 360.0f;
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

void Character::drawCollisonBox() {
    glPushMatrix();
        // Salvar estados de atributos
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
            
            // Mover para o centro do personagem
            // O cilindro deve estar centrado no plano XZ
            glTranslatef(x + (width/2), y, z + (depth/2));
            
            // Desabilitar iluminação para melhor visualização do wireframe
            glDisable(GL_LIGHTING);
            
            // Definir cor (vermelho) e estilo de linha
            glColor3f(1.0, 0.0, 0.0);
            glLineWidth(2.0);
            
            // Rotacionar para alinhar com o eixo Y
            // gluCylinder desenha o cilindro ao longo do eixo Z por padrão
            // então rotacionamos -90 graus em torno do eixo X para alinhá-lo com Y
            glRotatef(-90, 1.0, 0.0, 0.0);
            
            // Para cilindros, desenhar um wireframe customizado
            GLUquadricObj *cylinder = gluNewQuadric();
            gluQuadricDrawStyle(cylinder, GLU_LINE);
            
            // Desenha o cilindro (agora alinhado com o eixo Y após a rotação)
            gluCylinder(cylinder, cylinderRadius, cylinderRadius, height, 16, 5);
            gluDeleteQuadric(cylinder);
            
            // Desenha os círculos das extremidades
            GLUquadricObj *disk = gluNewQuadric();
            gluQuadricDrawStyle(disk, GLU_LINE);
            
            // Círculo da base (já estamos posicionados na base)
            gluDisk(disk, 0, cylinderRadius, 16, 1);
            
            // Círculo do topo (translada para o topo do cilindro)
            glTranslatef(0, 0, height);
            gluDisk(disk, 0, cylinderRadius, 16, 1);
            gluDeleteQuadric(disk);
        
        // Restaurar estados de atributos
        glPopAttrib();
    glPopMatrix();
}


