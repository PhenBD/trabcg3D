#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <list>
#include <iostream>
#include "headers/tinyxml2.h"
#include "headers/player.h"
#include "headers/obstacle.h"
#include "headers/enemy.h"
#include "headers/arena.h"
#include "headers/object.h"
#include <random>
#include <ctime>

// Criar um gerador de números aleatórios
std::mt19937 rng(std::time(nullptr));

#define INC_KEY 1

// Camera
int camera = 3;
double camXYAngle=0;
double camXZAngle=0;
int camAngle = 60;
bool freeCam = false;

// Debug mode
bool debug = false;

// Font
void * font = GLUT_BITMAP_9_BY_15;

// Check end of the game
bool ended = false;
bool gameOver = false;
bool gameWin = false;

// Arena SVG file
char* arenaSVGFilename;

// Key status
int keyStatus[256];

// Mouse position
GLint mouseX = 0.0;
GLint mouseY = 0.0;
int lastMouseX = 0;
int lastMouseY = 0;

// Player arm movement
GLfloat armAngleXY = -90.0;
GLfloat armAngleXZ = 0.0;

// Window dimensions
const GLint Width = 500;
const GLint Height = 500;

//Components of the virtual world being modeled
Arena arena;
Player player;
std::list<Obstacle> obstacles;
std::list<Enemy> enemies;
std::list<Shoot> shoots;

using namespace tinyxml2;

void ReadSvg(const char* filename) {
    // Load the SVG file
    XMLDocument svgDoc;
    if (svgDoc.LoadFile(filename) != XML_SUCCESS) {
        std::cerr << "Erro ao carregar o arquivo SVG: " << svgDoc.ErrorStr() << std::endl;
        return;
    }

    // Get the root element
    XMLElement* root = svgDoc.RootElement();
    if (root == nullptr) {
        std::cerr << "Erro: Elemento raiz não encontrado no SVG." << std::endl;
        return;
    }

    // Loop through the children elements
    for (XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        // Check if it is a rectangle
        if (strcmp(child->Name(), "rect") == 0) {
            // Get the attributes
            const char* x = child->Attribute("x");
            const char* y = child->Attribute("y");
            const char* width = child->Attribute("width");
            const char* height = child->Attribute("height");
            const char* fill = child->Attribute("fill");

            // Check the fill color
            // If it is blue, it is the arena
            if (strcmp(child->Attribute("fill"), "blue") == 0) {
                arena = Arena(atof(x), atof(y), 0, atof(width), atof(height), atof(height)/2);
                break;
            } 
        }
    }

    // Loop through the children elements
    for (XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        // Check if it is a rectangle
        if (strcmp(child->Name(), "rect") == 0) {
            // Get the attributes
            const char* x = child->Attribute("x");
            const char* y = child->Attribute("y");
            const char* width = child->Attribute("width");
            const char* height = child->Attribute("height");
            const char* fill = child->Attribute("fill");

            // Check the fill color
            // If it is black, it is an obstacle
            if (strcmp(child->Attribute("fill"), "black") == 0) {
                Obstacle obstacle(atof(x), atof(y), 0, atof(width), atof(height), arena.getHeight()/2);
                obstacles.push_back(obstacle);
            }
        // Check if it is a circle
        } else if (strcmp(child->Name(), "circle") == 0) {
            // Get the attributes
            const char* cx = child->Attribute("cx");
            const char* cy = child->Attribute("cy");
            const char* r = child->Attribute("r");
            const char* fill = child->Attribute("fill");
            
            // Check the fill color
            // If it is green, it is the player
            if (strcmp(child->Attribute("fill"), "green") == 0) {
                std::uniform_real_distribution<float> floatDistribution(atof(r), arena.getDepth());
                player = Player(atof(cx), atof(cy), floatDistribution(rng), atof(r));
            } // If it is red, it is an enemy
            else if (strcmp(child->Attribute("fill"), "red") == 0) {
                std::uniform_real_distribution<float> floatDistribution(atof(r), arena.getDepth());
                Enemy enemy(atof(cx), atof(cy), floatDistribution(rng), atof(r));
                enemies.push_back(enemy);
            }
        }
    }
}

// Mouse passive position callback
void passiveMotionCallback(int x, int y) {
    camXZAngle += x - lastMouseX;
    camXYAngle += y - lastMouseY;
    
    camXZAngle = (int)camXZAngle % 360;
    if (camXYAngle > 60)
        camXYAngle = 60;
    else if (camXYAngle < -60)
        camXYAngle = -60;

    armAngleXZ += (x - lastMouseX) * 0.25;
    armAngleXY += (y - lastMouseY) * 0.25;

    if (armAngleXZ < -45)
        armAngleXZ = -45;
    else if (armAngleXZ > 45)
        armAngleXZ = 45;
        
    if (armAngleXY > -45)
        armAngleXY = -45;
    else if (armAngleXY < -135)
        armAngleXY = -135;
    
    lastMouseX = x;
    lastMouseY = y;
}

// Mouse motion when clicked callback
void motionCallback(int x, int y) {
    mouseX = x;
    mouseY = y;
}

// Mouse callback
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (!ended)
             player.shoot(shoots);
        }
    }
    glutPostRedisplay();
}

void getMouseWorldCoordinates(int mouseX, int mouseY, float &worldX, float &worldY) {
    // Get the projection matrix and the modelview matrix
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];
    GLdouble winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Convert mouse coordinates to window coordinates
    winX = (double)mouseX;
    winY = (double)viewport[3] - (double)mouseY; // Invert the Y coordinate
    glReadPixels(mouseX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    // Use gluUnProject to convert window coordinates to world coordinates
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    worldX = (float)posX;
    worldY = (float)posY;
}

// Keyboard callback
void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '1':
            camera = 1;
            break;
        case '2':
            camera = 2;
            break;
        case '3':
            camera = 3;
            break;
        case ' ': // Tecla de espaço
            if (!player.isOnAir()) {
                player.setJumping(true);
            }
            break;
        case 'w':
        case 'W':
            keyStatus[(int)('w')] = 1; //Using keyStatus trick
            break;
        case 'a':
        case 'A':
            keyStatus[(int)('a')] = 1; //Using keyStatus trick
            break;
        case 's':
        case 'S':
            keyStatus[(int)('s')] = 1; //Using keyStatus trick
            break;
        case 'd':
        case 'D':
            keyStatus[(int)('d')] = 1; //Using keyStatus trick
            break;
        case 'r':
        case 'R':
            keyStatus[(int)('r')] = 1; //Using keyStatus trick
            break;
        case 'g':
        case 'G':
            debug = !debug;
            break;
        case 'x':
        case 'X':
            freeCam = !freeCam;
            camXZAngle = player.getDirectionAngle();
            camXYAngle = 0;
            break;
        case 27 :
             exit(0);
    }
    glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y)
{
    if (key == ' ') {
        player.setJumping(false);
    }

    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}

void ResetKeyStatus()
{
    int i;
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
       keyStatus[i] = 0; 
}

void checkCollisionPlayer() {
    bool landedOnArena = false;
    bool landedOnObstacle = false;
    bool landedOnEnemy = false;

    // Check collision with the arena
    // The function returns the direction that the player collided with the arena
    int collisonDirection = player.checkCollisionArena(arena);
    if (collisonDirection == DOWN)
        landedOnArena = true;
    else if (collisonDirection == RIGHT){
        ended = true;
        gameWin = true;
    }   

    // Check collision with obstacles
    for (Obstacle obs : obstacles) {
        // The function returns the direction that the player collided with the object
        if (player.checkCollisionObstacle(obs) == DOWN)
            landedOnObstacle = true;
    }

    // Check collision with enemies
    for (Enemy enemy : enemies) {
        // The function returns the direction that the player collided with the object
        if (player.checkCollisionCharacter(enemy) == DOWN)
            landedOnEnemy = true;
    }

    // If the player landed on anything, it is not on air
    if (landedOnArena || landedOnObstacle || landedOnEnemy) {
        player.setOnAir(false);
        player.setJumping(false);
        player.setJumpingTime(0);
    } 
    else {
        player.setOnAir(true);
    }
}

void updatePlayer(GLdouble timeDiff) {
    // Treat keyPress
    if(keyStatus[(int)('w')])
    {
        player.moveX(player.getWalkSpeed() * sin(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);
        player.moveZ(player.getWalkSpeed() * cos(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);

        if (sin(player.getDirectionAngle() * M_PI / 180.0f) > 0){
            player.setDirection(RIGHT);
        }
        else if (sin(player.getDirectionAngle() * M_PI / 180.0f) < 0){
            player.setDirection(LEFT);
        }

        player.setWalking(true);
        checkCollisionPlayer();
    }
    else if(keyStatus[(int)('s')])
    {
        player.moveX(-player.getWalkSpeed() * sin(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);
        player.moveZ(-player.getWalkSpeed() * cos(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);

        if (sin(player.getDirectionAngle() * M_PI / 180.0f) > 0){
            player.setDirection(LEFT);
        }
        else if (sin(player.getDirectionAngle() * M_PI / 180.0f) < 0){
            player.setDirection(RIGHT);
        }

        player.setWalking(true);
        checkCollisionPlayer();
    }
    if(keyStatus[(int)('a')] && !player.isOnAir())
    {
        player.rotateXZ(-0.2 * timeDiff);
    }
    else if(keyStatus[(int)('d')] && !player.isOnAir())
    {
        player.rotateXZ(0.2 * timeDiff);
    }
    else {
        player.setWalking(false);
    }
    
    // Treat jumping
    if (player.getJumpingTime() <= 1000 && player.isJumping()) 
    {
        player.moveY(-player.getJumpSpeed(), timeDiff);
        player.setOnAir(true);
        player.addJumpingTime(timeDiff);
        player.setDirection(UP);
        checkCollisionPlayer();
    }
    else
    {
        player.setJumping(false);
        player.setJumpingTime(0);
    }
    
    // Gravity
    if (!player.isJumping()){
        player.moveY((player.getJumpSpeed()), timeDiff);
        player.setDirection(DOWN);
        checkCollisionPlayer();
    }

    player.setThetaArmXY(armAngleXY);
    player.setThetaArmXZ(-armAngleXZ);
}

void checkCollisonEnemy(Enemy &enemy) {
        int collisonDirection = -1;
        int collisonDirectionArena = -1;
        int collisonDirectionObstacle = -1;
        int collisonDirectionPlayer = -1;

        // Check collision with the arena
        collisonDirection = enemy.checkCollisionArena(arena);
        if (collisonDirection != -1)
            collisonDirectionArena = collisonDirection;

        // // Check collision with obstacles
        // for (Obstacle obs : obstacles) {
        //     collisonDirection = enemy.checkCollisionObstacle(obs);
        //     if (collisonDirection != -1)
        //         collisonDirectionObstacle = collisonDirection;
            
        //     // Make the enemy stay on the platform
        //     if (collisonDirection == DOWN){
        //         if (enemy.getLeft() < obs.getLeft()){
        //             enemy.setDirectionAngle(RIGHT);
        //         }   
        //         else if (enemy.getRight() > obs.getRight()){
        //             enemy.setDirectionAngle(LEFT);
        //         }
        //     }
        // }

        // // Check collision with the player
        // enemy.checkCollisionCharacter(player);

        // // Flip the direction of the enemy based on the collision direction
        // if (collisonDirectionArena == LEFT || collisonDirectionObstacle == LEFT) {
        //     enemy.setDirectionAngle(RIGHT);
        // }
        // else if (collisonDirectionArena == RIGHT || collisonDirectionObstacle == RIGHT) {
        //     enemy.setDirectionAngle(LEFT);
        // }
}

void updateEnemies(GLdouble timeDiff) {
    Enemy::addShootTimer(timeDiff);

    for (Enemy &enemy : enemies) {
        // Gravity
        enemy.moveY((player.getJumpSpeed()), timeDiff);
        enemy.setDirection(DOWN);
        checkCollisonEnemy(enemy);

        // Move the enemy
        if (enemy.getWalkTimer() >= enemy.getMaxWalkTimer() && enemy.getIdleTimer() >= enemy.getMaxIdleTimer()) {
            std::uniform_int_distribution<int> angleDistribution(0, 359);
            enemy.setNextDirectionAngle(angleDistribution(rng));
            enemy.resetWalkTimer();
            enemy.resetIdleTimer();
        }

        if (enemy.getDirectionAngle() < enemy.getNextDirectionAngle() && enemy.getWalkTimer() < enemy.getMaxWalkTimer()) {
            enemy.rotateXZ(1);
        }
        else if (enemy.getDirectionAngle() > enemy.getNextDirectionAngle() && enemy.getWalkTimer() < enemy.getMaxWalkTimer()) {
            enemy.rotateXZ(-1);
        }
        else if (enemy.getDirectionAngle() == enemy.getNextDirectionAngle() && enemy.getWalkTimer() < enemy.getMaxWalkTimer()) {
            enemy.addWalkTimer(timeDiff);
        }
        
        if (enemy.getDirectionAngle() == enemy.getNextDirectionAngle() && enemy.getWalkTimer() < enemy.getMaxWalkTimer()) {
            enemy.moveX(enemy.getWalkSpeed() * sin(enemy.getDirectionAngle() * M_PI / 180.0f), timeDiff);
            enemy.moveZ(enemy.getWalkSpeed() * cos(enemy.getDirectionAngle() * M_PI / 180.0f), timeDiff);
            checkCollisonEnemy(enemy);
        }

        // Rotate the direction of the enemy based on the player position
        if (enemy.getWalkTimer() >= enemy.getMaxWalkTimer()){
            enemy.addIdleTimer(timeDiff);

            float enemyX = enemy.getX() + enemy.getWidth()/2;
            float enemyZ = enemy.getZ() + enemy.getDepth()/2;
            
            float playerX = player.getX() + player.getWidth()/2;
            float playerZ = player.getZ() + player.getDepth()/2;
            
            // Calculate vector from enemy to player
            float dx = playerX - enemyX;
            float dz = playerZ - enemyZ;

            int targetAngle = floor(atan2(dx, dz) * 180.0f / M_PI);

            if (targetAngle < 0)
                targetAngle += 360.0f;

            if (enemy.getDirectionAngle() < targetAngle) {
                enemy.rotateXZ(1);
            }
            else if (enemy.getDirectionAngle() > targetAngle) {
                enemy.rotateXZ(-1);
            }
        }
        // Move the arm of the enemy based on the player position
        int signal;
        if (enemy.getLookingDirection() == LEFT)
            signal = 1;
        else if (enemy.getLookingDirection() == RIGHT)
            signal = -1;
        
        enemy.setThetaArmXY(calculateArmAngle(player.getX(), player.getY(), enemy.getX(), enemy.getY()) * signal);
        
        // Shoot periodically
        // if (Enemy::getShootTimer() >= 3000) {
        //     enemy.shoot(shoots);
        // }
    }

    if (Enemy::getShootTimer() >= 3000) {
        Enemy::setShootTimer(0);
    }
}

void updateShoots(GLdouble timeDiff) {
    for (auto it = shoots.begin(); it != shoots.end(); ) {
        it->move(timeDiff);

        if (it->checkCollisionArena(arena)) {
            it = shoots.erase(it); // Use erase to remove the element and get the next iterator
            continue;
        }

        for (Obstacle obs : obstacles) {
            if (it->checkCollision(obs)) {
                it = shoots.erase(it); // Use erase to remove the element and get the next iterator
                break;
            }
        }

        if (it->isPlayer()) {
            for (Enemy &enemy : enemies) {
                if (it->checkCollision(enemy)) {
                    it = shoots.erase(it); // Use erase to remove the element and get the next iterator
                    enemies.remove(enemy);
                    break;
                }
            }
        }

        if(it->checkCollision(player)) {
            if (!it->isPlayer()) {
                ended = true;
                gameOver = true;
                it = shoots.erase(it); // Use erase to remove the element and get the next iterator
            }
            break;
        }
        ++it;
    }
}

void restart() {
    ended = false;
    gameOver = false;
    gameWin = false;
    player = Player();
    obstacles.clear();
    enemies.clear();
    shoots.clear();
    ReadSvg(arenaSVGFilename);
}

void drawText(GLfloat x, GLfloat y, const char* text, GLfloat r, GLfloat g, GLfloat b) {
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode); // Save the current matrix mode

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // Save the current projection matrix
    glLoadIdentity();
    glOrtho(0, Width, Height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Save the current model-view matrix
    glLoadIdentity();

    glColor3f(r, g, b);

    GLfloat w = 0;
    const char* textAux = text;
    while (*textAux) {
        w += glutBitmapWidth(font, *textAux);
        textAux++;
    }

    //Define a posicao onde vai comecar a imprimir
    glRasterPos2f(x - w/2, y);

    //Imprime um caractere por vez
    while(*text){
        glutBitmapCharacter(font, *text);
        text++;
    }

    glPopMatrix(); // Restore the previous model-view matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // Restore the previous projection matrix
    glMatrixMode(matrixMode); // Restore the previous matrix mode
}

// Função auxiliar para verificar se um ponto está dentro de um obstáculo
bool isPointInsideObstacle(float x, float y, float z, Obstacle obs) {
    return (x >= obs.getLeft() && x <= obs.getRight() && 
            y >= obs.getY() && y <= obs.getY() + obs.getHeight() && 
            z >= obs.getBack() && z <= obs.getFront());
}

// Função auxiliar para verificar se um ponto está fora da arena
bool isPointOutsideArena(float x, float y, float z, Arena arena) {
    return (x < arena.getLeft() || x > arena.getRight() || 
            y < arena.getTop() || y > arena.getBottom() || 
            z < arena.getBack() || z > arena.getFront());
}

// Função para realizar o ray casting entre jogador e câmera
bool checkCameraCollision(float playerX, float playerY, float playerZ, 
                         float cameraX, float cameraY, float cameraZ, 
                         float& newCameraX, float& newCameraY, float& newCameraZ) {
    // Direção do ray
    float dirX = cameraX - playerX;
    float dirY = cameraY - playerY;
    float dirZ = cameraZ - playerZ;
    
    // Comprimento do ray (distância entre jogador e câmera)
    float rayLength = sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
    
    // Normalizar o vetor direção
    if (rayLength > 0) {
        dirX /= rayLength;
        dirY /= rayLength;
        dirZ /= rayLength;
    }
    
    // Número de passos para checar (quanto maior, mais preciso mas mais custoso)
    const int numSteps = 100;
    float stepSize = rayLength / numSteps;
    
    // Iniciar com a câmera na posição pretendida
    newCameraX = cameraX;
    newCameraY = cameraY;
    newCameraZ = cameraZ;
    
    // Verificar do jogador para a câmera
    for (int i = 1; i <= numSteps; i++) {
        float t = i * stepSize;
        float pointX = playerX + dirX * t;
        float pointY = playerY + dirY * t;
        float pointZ = playerZ + dirZ * t;
        
        // Verificar se o ponto está fora da arena
        if (isPointOutsideArena(pointX, pointY, pointZ, arena)) {
            // Recuar um pouco para garantir que ficamos dentro da arena
            float safeT = (i - 5) * stepSize;
            newCameraX = playerX + dirX * safeT;
            newCameraY = playerY + dirY * safeT;
            newCameraZ = playerZ + dirZ * safeT;
            return true;
        }
        
        // Verificar colisão com obstáculos
        for (Obstacle obs : obstacles) {
            if (isPointInsideObstacle(pointX, pointY, pointZ, obs)) {
                // Recuar um pouco para garantir que não ficamos dentro do obstáculo
                float safeT = (i - 5) * stepSize;
                newCameraX = playerX + dirX * safeT;
                newCameraY = playerY + dirY * safeT;
                newCameraZ = playerZ + dirZ * safeT;
                return true;
            }
        }
    }
    
    return false; // Não houve colisão
}

void idle(void)
{
    static GLdouble previousTime = 0;
    GLdouble currentTime;
    GLdouble timeDiference;
    
    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDiference = currentTime - previousTime; // Elapsed time from the previous frame.
    previousTime = currentTime; //Update previous time

    /*if (debug) {
        if (keyStatus[(int)('r')])
            restart();
    }
    else */if (keyStatus[(int)('r')] && ended) {
        restart();
    }

    if (!ended){
        updatePlayer(timeDiference);
        if (enemies.size() > 0 && !debug)
            updateEnemies(timeDiference);
        if (shoots.size() > 0)
            updateShoots(timeDiference);
    }
    
    glutPostRedisplay();
}

// Render function
void renderScene(void) {
    // Clear the screen.
    glClearColor (0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configure a câmera com base no valor de camera
    GLfloat eyeX, eyeY, eyeZ;           // Posição da câmera
    GLfloat centerX, centerY, centerZ;  // Ponto para onde a câmera olha
    GLfloat upX, upY, upZ;              // Vetor "up" da câmera
    
    // Ponto para onde a câmera olha (geralmente o jogador)
    centerX = player.getX() + player.getWidth()/2;
    centerY = player.getY() + player.getHeight()/2;
    centerZ = player.getZ() + player.getDepth()/2;
    
    // Vetor up padrão
    upX = 0.0f;
    upY = -1.0f;
    upZ = 0.0f;

    switch (camera) {
        case 1: 
        {
            // Câmera em primeira pessoa (olhos do jogador)
            float angle = player.getDirectionAngle() * M_PI / 180.0f;
            
            // Posicione a câmera na "cabeça" do personagem
            eyeX = centerX;
            eyeY = player.getY() + (player.getHeight() * 0.15f); // Altura dos "olhos"
            eyeZ = centerZ;
            
            // Olhe na direção em que o personagem está olhando
            centerX = eyeX + sin(angle);
            centerY = eyeY;  // Olhe na horizontal
            centerZ = eyeZ + cos(angle);
            
            // Ajuste o vetor "up" para ficar para cima
            upX = 0.0f;
            upY = -1.0f;
            upZ = 0.0f;
            break;
        }
        case 2:
        {
            // Calcule a posição base do braço (ombro)
            float shoulderX = player.getX() + player.getWidth()/2;
            float shoulderY = player.getY() + player.getHeight() * 0.3f;
            float shoulderZ = player.getZ() + player.getDepth()/2 + player.getDepth() * 0.2f;
            
            // Comprimento do braço para posicionar a câmera na mão
            float armLength = player.getArmHeight();
            
            // Calcule direção do braço usando os ângulos
            float dirX = sin((armAngleXZ + player.getDirectionAngle()) * M_PI / 180.0f);
            float dirY = sin((armAngleXY + 90) * M_PI / 180.0f);
            float dirZ = cos((armAngleXZ + player.getDirectionAngle()) * M_PI / 180.0f);

            // Posição da câmera um pouco acima do braço
            eyeX = shoulderX + dirX * armLength * 0.2f;
            eyeY = (shoulderY + dirY * armLength * 0.2f) - player.getHeight() * 0.05f;
            eyeZ = shoulderZ + dirZ * armLength * 0.2f;

            // Ponto para onde a câmera olha (na direção do braço)
            centerX = shoulderX + dirX * (armLength + 50.0f);  // Olha além do braço
            centerY = shoulderY + dirY * (armLength + 50.0f);
            centerZ = shoulderZ + dirZ * (armLength + 50.0f);

            // float x = eyeX, y = eyeY, z = eyeZ;
            // float rotatedX1, rotatedY1, rotatedZ1, rotatedX2, rotatedY2, rotatedZ2;
            // float angle1 = armAngleXZ * M_PI / 180.0f;
            // float angle2 = -(armAngleXY + 90) * M_PI / 180.0f;

            // std::cout << "angle1: " << armAngleXZ << " angle2: " << -(armAngleXY + 90) << std::endl;

            // // rotate around the Y axis
            // rotatedX1 = x * cos(angle1) - z * sin(angle1);
            // rotatedY1 = y;
            // rotatedZ1 = x * sin(angle1) + z * cos(angle1);

            // // rotate around the Z axis
            // rotatedX2 = rotatedX1 * cos(angle2) - rotatedY1 * sin(angle2);
            // rotatedY2 = rotatedX1 * sin(angle2) + rotatedY1 * cos(angle2);
            // rotatedZ2 = rotatedZ1;

            // normalizeVector3D(dirX, dirY, dirZ);
            // std::cout << "dirX: " << dirX << " dirY: " << dirY << " dirZ: " << dirZ << std::endl;

            // float zCamX, zCamY, zCamZ;
            // crossProduct3D(dirX, dirY, dirZ, rotatedX2 - shoulderX, rotatedY2 - (shoulderY + player.getHeight() * 0.05f), rotatedZ2 - shoulderZ, zCamX, zCamY, zCamZ);
            // normalizeVector3D(zCamX, zCamY, zCamZ);

            // crossProduct3D(zCamX, zCamY, zCamZ, dirX, dirY, dirZ, upX, upY, upZ);

            // std::cout << "upX: " << upX << " upY: " << upY << " upZ: " << upZ << std::endl;

            break;
        }
        case 3:
        {
            // Câmera em terceira pessoa (atrás do jogador)
            float angle, heightAbove;
            if (freeCam){
                angle = camXZAngle * M_PI / 180.0f;
                heightAbove = (-player.getHeight() * 2) + ((player.getHeight() * 2) * (camXYAngle * M_PI / 180.0f));  
            }
            else {
                angle = player.getDirectionAngle() * M_PI / 180.0f;
                heightAbove = -player.getHeight() * 2;     // Altura acima do jogador
            }
            float distanceBehind = player.getHeight() * 3; // Distância atrás do jogador
            
            
            // Posição ideal da câmera (antes da verificação de colisão)
            eyeX = centerX - distanceBehind * sin(angle);
            eyeY = centerY + heightAbove;
            eyeZ = centerZ - distanceBehind * cos(angle);
            
            // Verificar colisão da câmera e ajustar se necessário
            float newEyeX, newEyeY, newEyeZ;
            if (checkCameraCollision(centerX, centerY, centerZ, eyeX, eyeY, eyeZ, 
                                    newEyeX, newEyeY, newEyeZ)) {
                eyeX = newEyeX;
                eyeY = newEyeY;
                eyeZ = newEyeZ;
            }
            
            break;
        }
    }

    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);

    // Draw elements
    arena.draw();

    player.draw(camera);
    if (debug){
        player.drawAxis();
        player.drawCollisonBox();
    }

    for (Obstacle obs : obstacles) {
        obs.draw();
        if (debug)
            obs.drawAxis();
    }
    for (Enemy enemy : enemies) {
        enemy.draw(camera);
        if (debug){
            enemy.drawAxis();
            enemy.drawCollisonBox();
        }
    }
    for (Shoot shoot : shoots) {
        shoot.draw();
        if (debug)
            shoot.drawAxis();
    }

    // Draw texts
    if (ended){
        if (gameOver)
            drawText(Width/2, Height/2 - 20, "Game Over", 1.0f, 0.0f, 0.0f);
        else if (gameWin)
            drawText(Width/2, Height/2 - 20, "You Win!", 0.0f, 1.0f, 0.0f);
        
        drawText(Width/2, Height/2, "Press 'R' to restart", 1.0f, 1.0f, 1.0f);
    }

    glutSwapBuffers(); // Draw the new frame of the game.
}

// Função de inicialização
void init() {
    glutIgnoreKeyRepeat(true);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_TEXTURE_2D);
    // glEnable(GL_LIGHTING);
    // glShadeModel (GL_SMOOTH);

    // glDepthFunc(GL_LEQUAL);

    ResetKeyStatus();
    ReadSvg(arenaSVGFilename);

    // The color the windows will redraw. Its done to erase the previous frame.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).
 
    glMatrixMode(GL_PROJECTION); // Select the projection matrix   

    glLoadIdentity();
    // Use perspective projection instead of orthographic
    gluPerspective(75.0f,                           // Field of view angle
                  (GLfloat)Width/(GLfloat)Height,   // Aspect ratio
                  0.1f,                             // Near clipping plane
                  200.0f);                          // Far clipping plane

    glMatrixMode(GL_MODELVIEW); // Select the modelview matrix
    glLoadIdentity();

    // glEnable(GL_LIGHT0);
}

int main(int argc, char** argv) {
    // Check if the SVG file was passed as an argument
    if (argc < 2) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    // Get the SVG filename
    arenaSVGFilename = argv[1];

    // Initialize openGL with Double buffer and RGB color without transparency
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
 
    // Create the window
    glutInitWindowSize(Width, Height);
    glutInitWindowPosition(750,250);
    glutCreateWindow("Trabalho CG 2D");

    // Define callbacks

    // Display callbacks
    glutDisplayFunc(renderScene);
    glutIdleFunc(idle);

    // Keyboard callbacks
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);

    // Mouse callbacks
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(passiveMotionCallback);
    glutMotionFunc(motionCallback);
    
    init();
 
    glutMainLoop();
    return 0;
}
