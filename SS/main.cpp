#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <cstdio>

// -------------------- Window --------------------
int windowWidth = 800, windowHeight = 600;
//---btnproperty----
float btnWidth = 100;
float btnHeight = 40;
float centerX;
float startY;
float gap = 60;

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
void resetGame(int selectedLevel) {
    playerX = windowWidth / 2;
    playerY = 50;

    enemyX = windowWidth / 2;
    enemyY = 550;

    level = selectedLevel;

    // Increase difficulty per level
    enemySpeed = 2 + level;

    bullets.clear();
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
void drawButton(float x, float y, float width, float height, const char* label) {
    glColor3f(0.2f, 0.6f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(x - width/2, y - height/2);
        glVertex2f(x + width/2, y - height/2);
        glVertex2f(x + width/2, y + height/2);
        glVertex2f(x - width/2, y + height/2);
    glEnd();

    glColor3f(1, 1, 1);
    drawText(x - width/4, y - 5, label); // adjust -5 for vertical centering
}

void drawMenu() {
    glColor3f(1, 1, 1);
    drawText(320, 500, "SPACE SHOOTER");

    // Use globals
    centerX = windowWidth / 2;
    startY = windowHeight / 2 + 100;

    for(int i = 0; i < 5; i++) {
        float y = startY - i * gap;
        char label[10];
        sprintf(label, "Level %d", i + 1);
        drawButton(centerX, y, btnWidth, btnHeight, label);
    }
}


// -------------------- Draw Player ----------------
void drawPlayer() {
    // -------- Main Body --------
    glColor3f(0.2f, 0.8f, 0.2f); // dark green
    glBegin(GL_TRIANGLES);
        glVertex2f(playerX, playerY + 25);    // nose
        glVertex2f(playerX - 20, playerY - 20); // bottom-left
        glVertex2f(playerX + 20, playerY - 20); // bottom-right
    glEnd();

    // -------- Cockpit --------
    glColor3f(0.0f, 0.5f, 1.0f); // blue
    glBegin(GL_POLYGON);
        glVertex2f(playerX - 7, playerY + 5);
        glVertex2f(playerX + 7, playerY + 5);
        glVertex2f(playerX + 4, playerY - 5);
        glVertex2f(playerX - 4, playerY - 5);
    glEnd();

    // -------- Left Wing --------
    glColor3f(0.1f, 0.6f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(playerX - 20, playerY - 5);
        glVertex2f(playerX - 35, playerY - 15);
        glVertex2f(playerX - 30, playerY - 25);
        glVertex2f(playerX - 15, playerY - 15);
    glEnd();

    // -------- Right Wing --------
    glBegin(GL_QUADS);
        glVertex2f(playerX + 20, playerY - 5);
        glVertex2f(playerX + 35, playerY - 15);
        glVertex2f(playerX + 30, playerY - 25);
        glVertex2f(playerX + 15, playerY - 15);
    glEnd();

    // -------- Thrusters --------
    glColor3f(1.0f, 0.5f, 0.0f); // orange flame
    glBegin(GL_TRIANGLES);
        glVertex2f(playerX - 7, playerY - 20); // left thruster
        glVertex2f(playerX + 7, playerY - 20); // right thruster
        glVertex2f(playerX, playerY - 35);     // tip of flame
    glEnd();

    // -------- Wing details (lines) --------
    glColor3f(0.0f, 0.0f, 0.0f); // black
    glBegin(GL_LINES);
        glVertex2f(playerX - 20, playerY - 5);
        glVertex2f(playerX - 30, playerY - 25);

        glVertex2f(playerX + 20, playerY - 5);
        glVertex2f(playerX + 30, playerY - 25);

        glVertex2f(playerX - 7, playerY + 5);
        glVertex2f(playerX - 4, playerY - 5);

        glVertex2f(playerX + 7, playerY + 5);
        glVertex2f(playerX + 4, playerY - 5);
    glEnd();
}



// -------------------- Draw Bullets ---------------
void drawBullets() {
    for (auto &b : bullets) {
        // Main laser body (yellow)
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(b.x - 2, b.y);      // bottom-left
            glVertex2f(b.x + 2, b.y);      // bottom-right
            glVertex2f(b.x + 2, b.y + 10); // top-right
            glVertex2f(b.x - 2, b.y + 10); // top-left
        glEnd();

        // Laser tip (bright white)
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(b.x - 2, b.y + 10); // left base of tip
            glVertex2f(b.x + 2, b.y + 10); // right base of tip
            glVertex2f(b.x, b.y + 15);     // tip point
        glEnd();
    }
}


// -------------------- Draw Enemy -----------------
void drawEnemy() {
    // -------- Main Body (red triangle) --------
    glColor3f(0.8f, 0.0f, 0.0f); // red
    glBegin(GL_TRIANGLES);
        glVertex2f(enemyX, enemyY + 20);    // nose
        glVertex2f(enemyX - 15, enemyY - 20); // bottom-left
        glVertex2f(enemyX + 15, enemyY - 20); // bottom-right
    glEnd();

    // -------- Cockpit / Core (dark red) --------
    glColor3f(0.5f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(enemyX - 5, enemyY + 5);
        glVertex2f(enemyX + 5, enemyY + 5);
        glVertex2f(enemyX + 3, enemyY - 5);
        glVertex2f(enemyX - 3, enemyY - 5);
    glEnd();

    // -------- Left spike wing --------
    glColor3f(0.6f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(enemyX - 15, enemyY - 5);
        glVertex2f(enemyX - 25, enemyY - 15);
        glVertex2f(enemyX - 15, enemyY - 15);
    glEnd();

    // -------- Right spike wing --------
    glBegin(GL_TRIANGLES);
        glVertex2f(enemyX + 15, enemyY - 5);
        glVertex2f(enemyX + 25, enemyY - 15);
        glVertex2f(enemyX + 15, enemyY - 15);
    glEnd();

    // -------- Thruster flames (small, dark orange) --------
    glColor3f(1.0f, 0.3f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(enemyX - 5, enemyY - 20); // left thruster
        glVertex2f(enemyX + 5, enemyY - 20); // right thruster
        glVertex2f(enemyX, enemyY - 30);     // tip
    glEnd();

    // -------- Aggressive detailing (spikes) --------
    glColor3f(0.3f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex2f(enemyX - 15, enemyY - 5);
        glVertex2f(enemyX - 25, enemyY - 15);

        glVertex2f(enemyX + 15, enemyY - 5);
        glVertex2f(enemyX + 25, enemyY - 15);
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
//----------------------lvl---
void level1() { printf("Level 1 Started!\n"); }
void level2() { printf("Level 2 Started!\n"); }
void level3() { printf("Level 3 Started!\n"); }
void level4() { printf("Level 4 Started!\n"); }
void level5() { printf("Level 5 Started!\n"); }
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
    else if (gameState==4){
        //lvl
    }
    else if(gameState==-5){
        //lvl5
    }

    glFlush();
}

// -------------------- Keyboard -------------------
void keyPress(unsigned char key, int x, int y) {
    // Shoot
    if (gameState == 1 && key == ' ') {
        bullets.push_back({playerX, playerY + 20});
    }
    //backtomenu
    if (gameState == 1 && key == 27) {   // ESC key
        gameState = 0;
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
//--------------mourse handle
void mouseClick(int button, int state, int mouseX, int mouseY) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && gameState == 0) {

        int y = windowHeight - mouseY;

        for(int i = 0; i < 5; i++) {
            float btnY = startY - i * gap;
            float left   = centerX - btnWidth / 2;
            float right  = centerX + btnWidth / 2;
            float bottom = btnY - btnHeight / 2;
            float top    = btnY + btnHeight / 2;

            if(mouseX >= left && mouseX <= right && y >= bottom && y <= top) {
                resetGame(i + 1);  // Start game directly
            }
        }
    }
}

// -------------------- Init -----------------------
void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth, 0, windowHeight);
}

//----- force the window to original size
void reshape(int w, int h) {
    glutReshapeWindow(windowWidth, windowHeight);
}
// -------------------- Main -----------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE  | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    int screenWidth  = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition(
        (screenWidth - windowWidth) / 2,
        (screenHeight - windowHeight) / 2 -40
    );
    glutCreateWindow("Space Shooter");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyPress);
    glutSpecialFunc(keyDown);
    glutSpecialUpFunc(keyUp);
    glutMouseFunc(mouseClick);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
