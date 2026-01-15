#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <cmath>

// -------------------- Window --------------------
int windowWidth = 800, windowHeight = 600;

// -------------------- Game State ----------------
// 0 = MENU
// 1 = PLAYING
int gameState = 0;

// -------------------- Level ---------------------
int level = 1;

// -------------------- Player --------------------
float playerX = 400, playerY = 50;
float playerSpeed = 10;

// -------------------- Bullet --------------------
struct Bullet {
    float x, y;
};
std::vector<Bullet> bullets;

// -------------------- Enemy ---------------------
float enemyX = 400, enemyY = 550;
float enemySpeed = 3;

// -------------------- Input ---------------------
bool keyLeft = false;
bool keyRight = false;

// -------------------- Reset Game ----------------
void resetGame() {
    playerX = windowWidth / 2;
    playerY = 50;

    enemyX = windowWidth / 2;
    enemyY = 550;
    enemySpeed = 3;

    bullets.clear();
    level = 1;

    gameState = 1; // PLAYING
}

// -------------------- Draw Text -----------------
void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// -------------------- Draw Menu -----------------
void drawMenu() {
    glColor3f(1, 1, 1);
    drawText(300, 400, "SPACE SHOOTER");
    drawText(260, 320, "Press N to Start New Game");
    drawText(280, 280, "Level starts from 1");
}

// -------------------- Draw Player ----------------
void drawPlayer() {
    glColor3f(0, 1, 0);
    glBegin(GL_TRIANGLES);
        glVertex2f(playerX, playerY + 20);
        glVertex2f(playerX - 15, playerY - 20);
        glVertex2f(playerX + 15, playerY - 20);
    glEnd();
}

// -------------------- Draw Bullets ---------------
void drawBullets() {
    glColor3f(1, 1, 0);
    for (auto &b : bullets) {
        glBegin(GL_QUADS);
            glVertex2f(b.x - 2, b.y);
            glVertex2f(b.x + 2, b.y);
            glVertex2f(b.x + 2, b.y + 10);
            glVertex2f(b.x - 2, b.y + 10);
        glEnd();
    }
}

// -------------------- Draw Enemy -----------------
void drawEnemy() {
    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
        glVertex2f(enemyX - 20, enemyY - 20);
        glVertex2f(enemyX + 20, enemyY - 20);
        glVertex2f(enemyX + 20, enemyY + 20);
        glVertex2f(enemyX - 20, enemyY + 20);
    glEnd();
}

// -------------------- Update Game ----------------
void update(int value) {

    if (gameState == 1) { // PLAYING

        // Player movement
        if (keyLeft && playerX > 20)
            playerX -= playerSpeed;

        if (keyRight && playerX < windowWidth - 20)
            playerX += playerSpeed;

        // Bullet movement
        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].y += 10;
            if (bullets[i].y > windowHeight) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }

        // Enemy movement
        enemyY -= enemySpeed;
        if (enemyY < 0) {
            enemyY = 550;
            enemyX = rand() % (windowWidth - 40) + 20;
        }

        // Collision detection
        for (int i = 0; i < bullets.size(); i++) {
            if (abs(bullets[i].x - enemyX) < 20 &&
                abs(bullets[i].y - enemyY) < 20) {

                bullets.erase(bullets.begin() + i);
                enemyY = 550;
                enemyX = rand() % (windowWidth - 40) + 20;
                break;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// -------------------- Display --------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameState == 0) { // MENU
        drawMenu();
    }
    else if (gameState == 1) { // PLAYING
        drawPlayer();
        drawBullets();
        drawEnemy();
    }

    glFlush();
}

// -------------------- Keyboard -------------------
void keyPress(unsigned char key, int x, int y) {

    // New Game
    if (gameState == 0 && (key == 'n' || key == 'N')) {
        resetGame();
    }

    // Shoot
    if (gameState == 1 && key == ' ') {
        bullets.push_back({playerX, playerY + 20});
    }
}

void keyDown(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) keyLeft = true;
    if (key == GLUT_KEY_RIGHT) keyRight = true;
}

void keyUp(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) keyLeft = false;
    if (key == GLUT_KEY_RIGHT) keyRight = false;
}

// -------------------- Init -----------------------
void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth, 0, windowHeight);
}

// -------------------- Main -----------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Space Shooter");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyPress);
    glutSpecialFunc(keyDown);
    glutSpecialUpFunc(keyUp);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
