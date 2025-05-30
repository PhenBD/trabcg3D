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
#include "headers/imageloader.h"
#include <random>
#include <ctime>

// Criar um gerador de números aleatórios
std::mt19937 rng(std::time(nullptr));

#define INC_KEY 1

// Camera
int camera = 3;
int cameraBefore = 3;
double camXYAngle=0;
double camXZAngle=0;
int camAngle = 90;
bool freeCam = false;

// Night mode
bool nightMode = false;

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

// Textures
GLuint textureWall;
GLuint textureObstacle;
GLuint textureCamuflage;
GLuint textureMetal;
GLuint textureJeans;
GLuint textureStars;
GLuint textureSun;

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

GLuint LoadTextureRAW( const char * filename )
{

    GLuint texture;
    
    Image* image = loadBMP(filename);

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                             0,                            //0 for now
                             GL_RGB,                       //Format OpenGL uses for image
                             image->width, image->height,  //Width and height
                             0,                            //The border of the image
                             GL_RGB, //GL_RGB, because pixels are stored in RGB format
                             GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                               //as unsigned numbers
                             image->pixels);               //The actual pixel data
    delete image;

    return texture;
}

// Mouse passive position callback
void passiveMotionCallback(int x, int y) {
    if (freeCam) {
        camXZAngle += x - lastMouseX;
        camXYAngle += y - lastMouseY;
        
        camXZAngle = (int)camXZAngle % 360;
        if (camXYAngle > 60)
            camXYAngle = 60;
        else if (camXYAngle < -60)
            camXYAngle = -60;
    }

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
    
    // Copiar comportamento do passiveMotionCallback
    if (freeCam) {
        camXZAngle += x - lastMouseX;
        camXYAngle += y - lastMouseY;
        
        camXZAngle = (int)camXZAngle % 360;
        if (camXYAngle > 60)
            camXYAngle = 60;
        else if (camXYAngle < -60)
            camXYAngle = -60;
    }

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

// Mouse callback
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            cameraBefore = camera;
            camera = 2;
        }
        if (state == GLUT_UP) {
            camera = cameraBefore;
        }
    }
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

void changeCamera(int angle, int w, int h)
{
    glMatrixMode (GL_PROJECTION);

    glLoadIdentity ();

    gluPerspective (angle, 
            (GLfloat)w / (GLfloat)h, 0.1f, 400.0f);

    glMatrixMode (GL_MODELVIEW);
}

// Keyboard callback
void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '1':
            camera = 1;
            changeCamera(90, 
                glutGet(GLUT_WINDOW_WIDTH), 
                glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case '2':
            camera = 2;
            changeCamera(90, 
                glutGet(GLUT_WINDOW_WIDTH), 
                glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case '3':
            camera = 3;
            break;
        case ' ': // Tecla de espaço
        {
            if (!player.isOnAir()) {
                player.setJumping(true);
            }
            break;
        }
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
            break;
        case 'n':
        case 'N':
            nightMode = !nightMode;
            break;
        case '-':
        {
            if (camera != 3)
                break;
            int inc = camAngle >= 180 ? 0 : 1;
            camAngle += inc;
            changeCamera(camAngle, 
                    glutGet(GLUT_WINDOW_WIDTH), 
                    glutGet(GLUT_WINDOW_HEIGHT));
            break;
        }
        case '+':
        {
            if (camera != 3)
                break;
            int inc = camAngle <= 5 ? 0 : 1;
            camAngle -= inc;
            changeCamera(camAngle, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            break;
        }
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
    // std::cout << "x: " << player.getX() << " y: " << player.getY() << " z: " << player.getZ() << std::endl;
    bool landedOnArena = false;
    bool landedOnObstacle = false;
    bool landedOnEnemy = false;

    // Check collision with obstacles
    for (Obstacle obs : obstacles) {
        // The function returns the direction that the player collided with the object
        if (player.checkCollisionObstacle(obs) == DOWN)
            landedOnObstacle = true;
    }

    // Check collision with the arena
    // The function returns the direction that the player collided with the arena
    int collisonDirection = player.checkCollisionArena(arena);
    if (collisonDirection == DOWN)
        landedOnArena = true;
    else if (collisonDirection == RIGHT){
        ended = true;
        gameWin = true;
    }   

    if (landedOnArena && !landedOnObstacle) {
        // std::cout << "landedOnArena: " << landedOnArena << " landedOnObstacle: " << landedOnObstacle << std::endl;
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
        player.setWalking(true);

        player.moveX(player.getWalkSpeed() * sin(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);
        player.moveZ(player.getWalkSpeed() * cos(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);

        if (sin(player.getDirectionAngle() * M_PI / 180.0f) > 0){
            player.setDirection(RIGHT);
        }
        else if (sin(player.getDirectionAngle() * M_PI / 180.0f) < 0){
            player.setDirection(LEFT);
        }

        checkCollisionPlayer();
    }
    else if(keyStatus[(int)('s')])
    {
        player.setWalking(true);

        player.moveX(-player.getWalkSpeed() * sin(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);
        player.moveZ(-player.getWalkSpeed() * cos(player.getDirectionAngle() * M_PI / 180.0f), timeDiff);

        if (sin(player.getDirectionAngle() * M_PI / 180.0f) > 0){
            player.setDirection(LEFT);
        }
        else if (sin(player.getDirectionAngle() * M_PI / 180.0f) < 0){
            player.setDirection(RIGHT);
        }

        checkCollisionPlayer();
    }
    else {
        player.setWalking(false);
        player.moveX(0, timeDiff);
    }

    if(keyStatus[(int)('a')] && !player.isOnAir())
    {
        player.rotateXZ(-0.2 * timeDiff);
    }
    else if(keyStatus[(int)('d')] && !player.isOnAir())
    {
        player.rotateXZ(0.2 * timeDiff);
    }
    
    // Treat jumping
    if (player.getJumpingTime() <= 1000 && player.isJumping()) 
    {
        player.setWalking(true);
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
        player.setWalking(true);
        player.moveY((player.getJumpSpeed()), timeDiff);
        player.setWalking(false);
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

        // Check collision with obstacles
        for (Obstacle obs : obstacles) {
            collisonDirection = enemy.checkCollisionObstacle(obs);
            if (collisonDirection != -1)
                collisonDirectionObstacle = collisonDirection;
            
            // Make the enemy stay on the platform
            if (collisonDirection == DOWN){
                if (enemy.getLeft() < obs.getRight()){
                    enemy.setX(obs.getRight() - enemy.getWidth());
                }   
                else if (enemy.getRight() > obs.getLeft()){
                    enemy.setX(obs.getLeft());
                }
            }
        }

        // Check collision with the arena
        collisonDirection = enemy.checkCollisionArena(arena);
        if (collisonDirection != -1)
            collisonDirectionArena = collisonDirection;

        // Check collision with the player
        enemy.checkCollisionCharacter(player);
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
            enemy.setWalking(true);
            enemy.moveX(enemy.getWalkSpeed() * sin(enemy.getDirectionAngle() * M_PI / 180.0f), timeDiff);
            enemy.moveZ(enemy.getWalkSpeed() * cos(enemy.getDirectionAngle() * M_PI / 180.0f), timeDiff);
            if (sin(enemy.getDirectionAngle() * M_PI / 180.0f) > 0){
                enemy.setDirection(RIGHT);
            }
            else if (sin(enemy.getDirectionAngle() * M_PI / 180.0f) < 0){
                enemy.setDirection(LEFT);
            }
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
        if (Enemy::getShootTimer() >= 3000) {
            enemy.shoot(shoots);
        }
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
            if (it->checkCollisionObstacle(obs)) {
                it = shoots.erase(it); // Use erase to remove the element and get the next iterator
                break;
            }
        }

        if (it->isPlayer()) {
            for (Enemy &enemy : enemies) {
                if (it->checkCollisionEnemy(enemy)) {
                    it = shoots.erase(it); // Use erase to remove the element and get the next iterator
                    enemies.remove(enemy);
                    break;
                }
            }
        }

        if(it->checkCollisionPlayer(player)) {
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

    if (timeDiference > 7){
        timeDiference = 7;
    }

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

void PrintText(GLfloat x, GLfloat y, const char * text, double r, double g, double b)
{
    GLint matrixMode;

    // Salvar estados atuais
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
    
    // Desativar depth test para garantir que o texto seja desenhado na frente
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Configurar projeção ortográfica para coordenadas de tela
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, Width, Height, 0, -1, 1);
    
    // Resetar a matriz de modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Calcular largura do texto para centralização
    GLfloat textWidth = 0;
    const char* p = text;
    while (*p) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_9_BY_15, *p);
        p++;
    }
    
    // Desenhar o texto centralizado
    glColor3f(r, g, b);
    glRasterPos2f(x - textWidth/2, y);
    
    // Renderizar caracteres
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *text);
        text++;
    }
    
    // Restaurar estados
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    
    glMatrixMode(matrixMode);
}

// Render function
void renderScene(void) {
    // Clear the screen.
    glClearColor (0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
            float shoulderX = (player.getX() + player.getWidth()/2) + sin(player.getDirectionAngle() * M_PI / 180.0f) * (player.getBodyWidth()/2);
            float shoulderY = player.getY() + player.getHeight() * 0.325f;
            float shoulderZ = (player.getZ() + player.getDepth()/2) + cos(player.getDirectionAngle() * M_PI / 180.0f) * (player.getBodyDepth()/2);
            
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

            break;
        }
        case 3:
        {
            // Câmera em terceira pessoa (atrás do jogador)
            float angle, heightAbove;
            angle = camXZAngle * M_PI / 180.0f;
            heightAbove = (-player.getHeight() * 1.5) + (player.getHeight() * (camXYAngle * M_PI / 180.0f));  

            float distanceBehind = player.getHeight() * 1.5; // Distância atrás do jogador
            
            
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

    // Configurar a lanterna no modo noturno
    if (nightMode) {
        // Posição do ombro (base do braço)
        float shoulderX = (player.getX() + player.getWidth()/2) + sin(player.getDirectionAngle() * M_PI / 180.0f) * (player.getBodyWidth()/2);
        float shoulderY = player.getY() + player.getHeight() * 0.325f;
        float shoulderZ = (player.getZ() + player.getDepth()/2) + cos(player.getDirectionAngle() * M_PI / 180.0f) * (player.getBodyDepth()/2);

        // Direção do braço
        float dirX = sin((armAngleXZ + player.getDirectionAngle()) * M_PI / 180.0f);
        float dirY = sin((armAngleXY + 90) * M_PI / 180.0f);
        float dirZ = cos((armAngleXZ + player.getDirectionAngle()) * M_PI / 180.0f);

        // Posição da lanterna (próxima à mão)
        float armLength = player.getArmHeight() * 0.9f; // Ajuste para estar perto da mão
        float flashlightX = shoulderX + dirX * armLength;
        float flashlightY = shoulderY + dirY * armLength;
        float flashlightZ = shoulderZ + dirZ * armLength;

        // Configurar luz spotlight
        GLfloat light_position[] = {flashlightX, flashlightY, flashlightZ, 1.0f}; // Posição da luz
        GLfloat light_direction[] = {dirX, dirY, dirZ}; // Direção da luz
        GLfloat light_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Luz ambiente fraca
        GLfloat light_diffuse[] = {2.0f, 2.0f, 1.9f, 1.0f}; // Luz difusa (levemente amarelada)
        GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Luz especular

        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

        // Configurar propriedades de spotlight
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 40.0f); // Ângulo de corte (cone de luz estreito)
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 30.0f); // Foco da luz (quanto maior, mais concentrado)
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f); // Atenuação constante
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f); // Atenuação linear
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f); // Atenuação quadrática

        glEnable(GL_LIGHT0); // Ativar a lanterna
    } else {
        glDisable(GL_LIGHT0); // Desativar a lanterna fora do modo noturno
    }

    // Draw elements
    arena.draw(nightMode, textureWall);

    player.draw(camera, nightMode, textureCamuflage, textureCamuflage, textureJeans, textureStars);
    if (debug){
        player.drawAxis();
        player.drawCollisonBox();
    }

    for (Obstacle obs : obstacles) {
        obs.draw(textureObstacle);
        if (debug)
            obs.drawAxis();
    }
    for (Enemy enemy : enemies) {
        enemy.draw(camera, nightMode, textureCamuflage, textureCamuflage, textureJeans, textureSun);
        if (debug){
            enemy.drawAxis();
            enemy.drawCollisonBox();
        }
    }
    for (Shoot shoot : shoots) {
        shoot.draw(textureMetal);
        if (debug)
            shoot.drawAxis();
    }

    // Draw texts
    if (ended){
        if (gameOver)
            PrintText(Width/2, Height/2 - 20, "Game Over", 1.0f, 0.0f, 0.0f);
        else if (gameWin)
            PrintText(Width/2, Height/2 - 20, "You Win!", 0.0f, 1.0f, 0.0f);
        
        PrintText(Width/2, Height/2, "Press 'R' to restart", 1.0f, 1.0f, 1.0f);
    }

    glutSwapBuffers(); // Draw the new frame of the game.
}

// Função de inicialização
void init() {
    glutIgnoreKeyRepeat(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glShadeModel (GL_SMOOTH);
    
    // Ativar normalização automática de normais
    glEnable(GL_NORMALIZE);

    textureWall = LoadTextureRAW("textures/pedregulho.bmp");
    textureObstacle = LoadTextureRAW("textures/madeira.bmp");
    textureCamuflage = LoadTextureRAW("textures/camuflagem.bmp");
    textureJeans = LoadTextureRAW("textures/tecido.bmp");
    textureMetal = LoadTextureRAW("textures/metal.bmp");
    textureStars = LoadTextureRAW("textures/stars.bmp");
    textureSun = LoadTextureRAW("textures/sun.bmp");

    ResetKeyStatus();
    ReadSvg(arenaSVGFilename);

    camXZAngle = player.getDirectionAngle();
    camXYAngle = 0;

    // The color the windows will redraw. Its done to erase the previous frame.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).
 
    glMatrixMode(GL_PROJECTION); // Select the projection matrix   

    glLoadIdentity();
    // Use perspective projection instead of orthographic
    gluPerspective(90.0f,                           // Field of view angle
                  (GLfloat)Width/(GLfloat)Height,   // Aspect ratio
                  0.1f,                             // Near clipping plane
                  400.0f);                          // Far clipping plane

    glMatrixMode(GL_MODELVIEW); // Select the modelview matrix
    glLoadIdentity();
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

