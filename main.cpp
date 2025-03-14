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
    mouseX = x;
    mouseY = y;
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
    int collisonDirection = player.checkArenaCollision(arena);
    if (collisonDirection == DOWN)
        landedOnArena = true;
    else if (collisonDirection == RIGHT){
        ended = true;
        gameWin = true;
    }   

    // Check collision with obstacles
    for (Obstacle obs : obstacles) {
        // The function returns the direction that the player collided with the object
        if (player.checkCollision(obs) == DOWN)
            landedOnObstacle = true;
    }

    // Check collision with enemies
    for (Enemy enemy : enemies) {
        // The function returns the direction that the player collided with the object
        if (player.checkCollision(enemy) == DOWN)
            landedOnEnemy = true;
    }

    // std::cout << "landedOnArena: " << landedOnArena << std::endl;
    // std::cout << "landedOnObstacle: " << landedOnObstacle << std::endl;
    // std::cout << "landedOnEnemy: " << landedOnEnemy << std::endl;
    // std::cout << "On air: " << player.isOnAir() << std::endl;
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
        player.moveX(player.getWalkSpeed() * sin(player.getWalkingDirection() * M_PI / 180.0f), timeDiff);
        player.moveZ(player.getWalkSpeed() * cos(player.getWalkingDirection() * M_PI / 180.0f), timeDiff);
        player.setDirection(UP);
        player.setWalking(true);
        checkCollisionPlayer();
    }
    else if(keyStatus[(int)('s')])
    {
        player.moveX(-player.getWalkSpeed() * sin(player.getWalkingDirection() * M_PI / 180.0f), timeDiff);
        player.moveZ(-player.getWalkSpeed() * cos(player.getWalkingDirection() * M_PI / 180.0f), timeDiff);
        player.setDirection(DOWN);
        player.setWalking(true);
        checkCollisionPlayer();
    }
    if(keyStatus[(int)('a')] && !player.isOnAir())
    {
        player.rotateXZ(0.2 * timeDiff);
        player.setDirection(LEFT);
        player.setWalking(true);
        checkCollisionPlayer();
    }
    else if(keyStatus[(int)('d')] && !player.isOnAir())
    {
        player.rotateXZ(-0.2 * timeDiff);
        player.setDirection(RIGHT);
        player.setWalking(true);
        checkCollisionPlayer();
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
    
    // Gravity
    player.moveY((player.getJumpSpeed()/2), timeDiff);
    player.setDirection(DOWN);
    checkCollisionPlayer();

    // Flip the direction of the player based on the mouse position
    if (mouseX < 250 && player.getLookingDirection() == RIGHT) 
    {
        player.setLookingDirection(LEFT);
        // player.flipDirection();
    } 
    else if (mouseX > 250 && player.getLookingDirection() == LEFT)
    {
        player.setLookingDirection(RIGHT);
        // player.flipDirection();
    }

    // Move the arm of the player based on the mouse position
    int signal;
    if (player.getLookingDirection() == LEFT)
        signal = 1;
    else if (player.getLookingDirection() == RIGHT)
        signal = -1;

    GLfloat mouseWorldX = 0;
    GLfloat mouseWorldY = 0;

    getMouseWorldCoordinates(mouseX, mouseY, mouseWorldX, mouseWorldY);

    // Use the correct coordinates for the player's arm
    float armBaseX = player.getX() + (player.getWidth() / 2);
    float armBaseY = player.getY() + (player.getArmHeight());

    player.setThetaArm(calculateArmAngle(mouseWorldX, mouseWorldY, armBaseX, armBaseY) * signal);
}

void checkCollisonEnemy(Enemy &enemy) {
        int collisonDirection = -1;
        int collisonDirectionArena = -1;
        int collisonDirectionObstacle = -1;
        int collisonDirectionPlayer = -1;

        // Check collision with the arena
        collisonDirection = enemy.checkArenaCollision(arena);
        if (collisonDirection != -1)
            collisonDirectionArena = collisonDirection;

        // Check collision with obstacles
        for (Obstacle obs : obstacles) {
            collisonDirection = enemy.checkCollision(obs);
            if (collisonDirection != -1)
                collisonDirectionObstacle = collisonDirection;
            
            // Make the enemy stay on the platform
            if (collisonDirection == DOWN){
                if (enemy.getLeft() < obs.getLeft()){
                    enemy.setWalkingDirection(RIGHT);
                }   
                else if (enemy.getRight() > obs.getRight()){
                    enemy.setWalkingDirection(LEFT);
                }
            }
        }

        // Check collision with the player
        enemy.checkCollision(player);

        // Flip the direction of the enemy based on the collision direction
        if (collisonDirectionArena == LEFT || collisonDirectionObstacle == LEFT) {
            enemy.setWalkingDirection(RIGHT);
        }
        else if (collisonDirectionArena == RIGHT || collisonDirectionObstacle == RIGHT) {
            enemy.setWalkingDirection(LEFT);
        }
}

void updateEnemies(GLdouble timeDiff) {
    Enemy::addShootTimer(timeDiff);

    for (Enemy &enemy : enemies) {
        // Gravity
        enemy.moveY((player.getJumpSpeed()/2), timeDiff);
        enemy.setDirection(DOWN);
        checkCollisonEnemy(enemy);

        // Move the enemy
        if (enemy.getWalkingDirection() == LEFT){
            enemy.moveX(-enemy.getWalkSpeed(), timeDiff);
            enemy.setWalking(true);
            enemy.setDirection(LEFT);
        }
        else if (enemy.getWalkingDirection() == RIGHT){
            enemy.moveX(enemy.getWalkSpeed(), timeDiff);
            enemy.setWalking(true);
            enemy.setDirection(RIGHT);
        }
        checkCollisonEnemy(enemy);

        // Flip the looking direction of the enemy based on the player position
        if (player.getX() < enemy.getX() && enemy.getLookingDirection() == RIGHT) {
            enemy.flipDirection();
            enemy.setLookingDirection(LEFT);
        } 
        else if (player.getX() > enemy.getX() && enemy.getLookingDirection() == LEFT) {
            enemy.flipDirection();
            enemy.setLookingDirection(RIGHT);
        }

        // Move the arm of the enemy based on the player position
        int signal;
        if (enemy.getLookingDirection() == LEFT)
            signal = 1;
        else if (enemy.getLookingDirection() == RIGHT)
            signal = -1;
        
        enemy.setThetaArm(calculateArmAngle(player.getX(), player.getY(), enemy.getX(), enemy.getY()) * signal);
        
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

        if (it->checkArenaCollision(arena)) {
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
// ver dps
void rasterChars(GLfloat x, GLfloat y, GLfloat z, const char * text, double r, double g, double b)
{
    //Push to recover original attributes
    glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //Draw text in the x, y, z position
        glColor3f(r,g,b);
        glRasterPos3f(x, y, z);
        const char* tmpStr;
        tmpStr = text;
        while( *tmpStr ){
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tmpStr);
            tmpStr++;
        }
    glPopAttrib();
}
void printText(GLfloat x, GLfloat y, const char * text, double r, double g, double b)
{
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode); // Save the current matrix mode

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // Save the current projection matrix
        glLoadIdentity();
        glOrtho(0, Width, Height, 0, -1, 1);

        // glMatrixMode(GL_MODELVIEW);
        // glPushMatrix(); // Save the current model-view matrix
            // glLoadIdentity();
            GLfloat w = 0;
            const char* textAux = text;
            while (*textAux) {
                w += glutBitmapWidth(font, *textAux);
                textAux++;
            }

            //Define a posicao onde vai comecar a imprimir
            int xw = x - w/2;

            rasterChars(xw, y, 0, text, r, g, b);   

        // glPopMatrix(); // Restore the previous model-view matrix
        // glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // Restore the previous projection matrix
    glMatrixMode(matrixMode); // Restore the previous matrix mode
}
//
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
        if (enemies.size() > 0)
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

    // Configure the camera
    GLfloat eyeX = player.getX();
    GLfloat eyeY = arena.getY() + arena.getHeight()/2;
    GLfloat eyeZ = arena.getZ() + arena.getDepth()/2;

    GLfloat centerX = player.getX();
    GLfloat centerY = player.getY();
    GLfloat centerZ = player.getZ(); 

    GLfloat upX = 0.0f;
    GLfloat upY = -1.0f;
    GLfloat upZ = 0.0f;

    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);

    // Draw elements
    arena.draw();

    player.draw();
    if (debug)
        player.drawAxis();

    for (Obstacle obs : obstacles) {
        obs.draw();
        if (debug)
            obs.drawAxis();
    }
    for (Enemy enemy : enemies) {
        enemy.draw();
        if (debug)
            enemy.drawAxis();
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
    gluPerspective(60.0f,                           // Field of view angle
                  (GLfloat)Width/(GLfloat)Height,   // Aspect ratio
                  1.0f,                             // Near clipping plane
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
