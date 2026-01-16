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
int level = 0;

// -------------------- Player --------------------
float playerX = 400, playerY = 50;
float playerSpeed = 10;

// -------------------- Bullet --------------------
struct Bullet {
    float x, y;
};
std::vector<Bullet> bullets;

//---star for backgrond
struct Star {
    float x, y;
};

std::vector<Star> stars;

// -------------------- Enemy ---------------------
float enemyX = 400, enemyY = 550;
float enemySpeed = 3;
float enemyDX = 2.5f;
float enemyDY = 2.0f;
int bossMaxHP = 200;
int bossHP    = 200;
float bossWidth  = 120;   // sprite width
float bossHeight = 140;   // sprite height

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

//--------background----
void drawBackground() {

    // -------- Space gradient --------
    glBegin(GL_QUADS);
        glColor3f(0.02f, 0.02f, 0.08f); // dark blue bottom
        glVertex2f(0, 0);
        glVertex2f(windowWidth, 0);

        glColor3f(0.0f, 0.0f, 0.2f);   // deep space top
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0, windowHeight);
    glEnd();

    // -------- Stars --------
    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f);
        for (auto &s : stars) {
            glVertex2f(s.x, s.y);
        }
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

        if(level<4){
            // Enemy movement
            enemyY -= enemySpeed;
            if (enemyY < 0 ) {
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
        if(level==4){}
        if (level == 5) {
            // Move enemy
            enemyX += enemyDX;
            enemyY += enemyDY;

            // Window bounds (consider boss size)
            float minX = bossWidth / 2;
            float maxX = windowWidth - bossWidth / 2;

            float minY = 300 + bossHeight / 2;  // boss stays above y=300
            float maxY = 600 - bossHeight / 2;  // boss stays below y=600


            if (enemyX <= minX || enemyX >= maxX) {
                enemyDX = (rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
                // Make sure enemy is inside
                if (enemyX < minX) enemyX = minX;
                if (enemyX > maxX) enemyX = maxX;
            }

            // Bounce & randomize direction on Y bounds
            if (enemyY <= minY || enemyY >= maxY) {
                enemyDY = (rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
                // Make sure enemy is inside
                if (enemyY < minY) enemyY = minY;
                if (enemyY > maxY) enemyY = maxY;
            }

                // Bullet collision
            for (int i = 0; i < bullets.size(); i++) {
                if (bullets[i].x >= enemyX - bossWidth / 2 &&
                        bullets[i].x <= enemyX + bossWidth / 2 &&
                        bullets[i].y >= enemyY - bossHeight / 2 &&
                        bullets[i].y <= enemyY + bossHeight / 2)
                    {
                        bullets.erase(bullets.begin() + i);
                        bossHP--;

                        if (bossHP <= 0) {
                            level++;
                            enemyX = -1000;
                            enemyY = -1000;
                    }
                        break;
                }
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
//-----------------lvl-4-----------------------
void drawlvl4Enemy(){

}
void level4() {
     printf("Level 4 Started!\n");
}
//-----------------lvl-5-----------------------
void drawCircle(double cx, double cy, double r, int num_segments = 50) {
    glBegin(GL_POLYGON); // or GL_LINE_LOOP if you want just the outline
    for(int i = 0; i < num_segments; i++) {
        double theta = 2.0 * 3.1415926 * double(i) / double(num_segments); // angle
        double x = r * cos(theta); // x offset
        double y = r * sin(theta); // y offset
        glVertex2d(cx + x, cy + y);
    }
    glEnd();
}

//color plate
void bodyRed()    { glColor3f(0.65f, 0.1f, 0.1f); }
void armorDark()  { glColor3f(0.35f, 0.05f, 0.05f); }
void metalBlue()  { glColor3f(0.1f, 0.2f, 0.6f); }
void glowGreen()  { glColor3f(0.2f, 1.0f, 0.3f); }
void eyeYellow()  { glColor3f(1.0f, 0.9f, 0.2f); }
void shadow()     { glColor3f(0.08f, 0.08f, 0.08f); }

void drawBossLifeBar(float x, float y)
{
    float barWidth  = 60.0f;
    float barHeight = 6.0f;

    float hpRatio = (float)bossHP / bossMaxHP;
    float hpWidth = barWidth * hpRatio;

    // Background (red)
    glColor3f(0.6, 0.0, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(x - barWidth/2, y);
        glVertex2f(x + barWidth/2, y);
        glVertex2f(x + barWidth/2, y + barHeight);
        glVertex2f(x - barWidth/2, y + barHeight);
    glEnd();

    // HP (green)
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(x - barWidth/2, y);
        glVertex2f(x - barWidth/2 + hpWidth, y);
        glVertex2f(x - barWidth/2 + hpWidth, y + barHeight);
        glVertex2f(x - barWidth/2, y + barHeight);
    glEnd();
}


void drawlvl5Enemy()
{
    int bx = enemyX,by =enemyY;
    //left arm
    glPushMatrix();
    glTranslated(bx, by, 0);   // move to position
    glScalef(4, 4, 0); // scale ×2
    glTranslated(-bx, -by, 0); // move back
    //ru1
    metalBlue();
    glBegin(GL_POLYGON);
        glVertex2d(bx+15,by-2);
        glVertex2d(bx+15,by-3);
        glVertex2d(bx+10,by-10);
        glVertex2d(bx+7,by-6);
        glVertex2d(bx+9,by+1);
        glVertex2d(bx+11,by-1);
        glVertex2d(bx+15,by-2);
    glEnd();
    bodyRed();
    //--la
    glBegin(GL_POLYGON);
        glVertex2d(bx-10,by+1);
        glVertex2d(bx-8.5,by+2);
        glVertex2d(bx-6,by-2);
        glVertex2d(bx-11,by-5);
        glVertex2d(bx-14,by-5);
        glVertex2d(bx-16,by-4);
        glVertex2d(bx-21,by);
        glVertex2d(bx-21,by+3);
        glVertex2d(bx-23,by+4);
        glVertex2d(bx-20,by+8);
        glVertex2d(bx-19,by+7);
        glVertex2d(bx-17,by+6);
        glVertex2d(bx-16,by+6);
    glEnd();
    //-antenna
    glBegin(GL_POLYGON);
        glVertex2d(bx-13,by+15);
        glVertex2d(bx-12,by+16);
        glVertex2d(bx-10,by+16);
        glVertex2d(bx-4,by+11);
        glVertex2d(bx-8,by+5);
        glVertex2d(bx-12,by+10);
        glVertex2d(bx-13,by+13);
        glVertex2d(bx-13,by+15);
    glEnd();
    glBegin(GL_LINES);
        glVertex2d(bx-13,by+15);
        glVertex2d(bx-18,by+20);
    glEnd();
    drawCircle(bx-18, by+20, 1.0);

    //Circle Draw
    //R1
    //eye
    glColor3f(0,0,1);
    metalBlue();
    drawCircle(bx-5, by, 4.0);
    drawCircle(bx-3, by+5, 6.0);

    eyeYellow();
    drawCircle(bx-3, by+5, 5.0);

    bodyRed();
    drawCircle(bx-3, by+5, 4.0);
    glColor3f(0,0,0);
    drawCircle(bx-3, by+5, 3.0);

    bodyRed();
    //glowGreen();
    drawCircle(bx-6, by-5, 4.0);
    drawCircle(bx-6, by-10, 4.0);
    drawCircle(bx-1, by-10, 4.0);
    drawCircle(bx-1, by-4, 4.0);
    drawCircle(bx+6, by+5,4.5);
    drawCircle(bx+5, by+2,4.0);
    drawCircle(bx+3, by+1,4.0);
    drawCircle(bx+5, by-1,4.0);
    drawCircle(bx+6, by-2,4.0);
    drawCircle(bx+3, by-10,2.0);
    drawCircle(bx+3, by-8,3.0);
    drawCircle(bx-4, by-14,3.0);
    drawCircle(bx, by-12,4.0);
    //ru2
    bodyRed();
    glBegin(GL_POLYGON);
        glVertex2f(bx+7,by-6);
        glVertex2f(bx+4,by-6);
        glVertex2f(bx+13,by+3.5);
        glVertex2f(bx+15,by);
        glVertex2f(bx+9,by+1);
        glVertex2f(bx+7,by-6);
    glEnd();
    //ru3
    glBegin(GL_POLYGON);
        glVertex2f(bx+4,by-6);
        glVertex2f(bx+14,by+4);
        glVertex2f(bx+10,by+7);
        glVertex2f(bx+10,by+5);
        glVertex2f(bx+9,by+4);
        glVertex2f(bx+9,by+3);
        glVertex2f(bx+7,by+2);
        glVertex2f(bx+7,by+1);
        glVertex2f(bx+2,by-5);
        glVertex2f(bx,by-5);
        glVertex2f(bx+2,by-5);
        glVertex2f(bx+4,by-6);
    glEnd();


    //RA--
    //--ra
    glBegin(GL_POLYGON);
        glVertex2d(bx+12,by-11);
        glVertex2d(bx+12,by-16);
        glVertex2d(bx+20,by-15);
        glVertex2d(bx+20,by-3);
        glVertex2d(bx+18.8,by-2);
        glVertex2d(bx+19,by-1);
        glVertex2d(bx+18,by);
        glVertex2d(bx+17,by);
        glVertex2d(bx+4,by-9);
        glVertex2d(bx+5,by-10);
        glVertex2d(bx+6,by-11);
        glVertex2d(bx+12,by-11);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2d(bx+12,by-16);
        glVertex2d(bx,by-29);
        glVertex2d(bx+3,by-33);
        glVertex2d(bx+20,by-15);
        glVertex2d(bx+12,by-16);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2d(bx+2,by-29);
        glVertex2d(bx-1,by-30);
        glVertex2d(bx-4,by-30);
        glVertex2d(bx-9,by-28);
        glVertex2d(bx-5,by-31);
        glVertex2d(bx,by-33);
        glVertex2d(bx+3,by-33);
        glVertex2d(bx+2,by-29);
    glEnd();
    glPopMatrix();
}

void drawlvl5Player(){
    float px = playerX, py = playerY;
    //left arm
    glPushMatrix();
    glTranslated(px, py, 0);   // move to position
    glScalef(2, 2, 0); // scale ×2
    glTranslated(-px, -py, 0); // move back

    //top
    metalBlue();
    glColor3f(0.4f, 0.8f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(px,py+13);
        glVertex2f(px,py+22);
        glVertex2f(px+3,py+15);
    glEnd();
    drawCircle(px+2,py+13,3.0);
    drawCircle(px+2,py+4,4.0);
    drawCircle(px+8,py+2,4.0);
    drawCircle(px+2,py+7,4.5);

    //lid
    glColor3f(1,0,0);
    glColor3f(0.6f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(px+5,py+15);
        glVertex2f(px+13,py+7);
        glVertex2f(px+10,py+4);
        glVertex2f(px+5,py+4);
    glEnd();
    bodyRed();
    glColor3f(0.3f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(px+4,py+15);
        glVertex2f(px+10,py+9);
        glVertex2f(px+10,py+4);
        glVertex2f(px+4,py+4);
    glEnd();
    //bottom cir
    glowGreen();
    drawCircle(px+8,py-3,3.0);

    //lidddd
    bodyRed();
    glBegin(GL_POLYGON);
        glVertex2f(px+10,py+4);
        glVertex2f(px+10,py-1);
        glVertex2f(px+11,py-1);
        glVertex2f(px+13,py+1);
        glVertex2f(px+14,py+7);
        glVertex2f(px+13,py+7);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(px+18,py+1);
        glVertex2f(px+19,py);
        glVertex2f(px+21,py);
        glVertex2f(px+14,py+7);
        glVertex2f(px+13,py+1);
    glEnd();

    //teeth
    glBegin(GL_POLYGON);
        glVertex2f(px+3,py-5);
        glVertex2f(px+3,py-12);
        glVertex2f(px+7,py-12);
        glVertex2f(px+10,py-10);
        glVertex2f(px+10,py-7);
        glVertex2f(px+8,py-5);
        glVertex2f(px+3,py-5);
    glEnd();
    glowGreen();
    glBegin(GL_POLYGON);
        glVertex2f(px+3,py-11);
        glVertex2f(px+4,py-10);
        glVertex2f(px+8,py-10);
        glVertex2f(px+9,py-11);
        glVertex2f(px+7,py-12);
        glVertex2f(px+3,py-12);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(px+5,py-12);
        glVertex2f(px+5,py-11);
        glVertex2f(px+6,py-11);
        glVertex2f(px+7,py-12);
        glVertex2f(px+7,py-14);
        glVertex2f(px+6,py-14);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(px+7,py-14);
        glVertex2f(px+6,py-14);
        glVertex2f(px+6,py-16);
    glEnd();

    //btmsqr
    metalBlue();
    glBegin(GL_QUADS);
        glVertex2f(px,py-19);
        glVertex2f(px+3,py-16);
        glVertex2f(px+3,py-3);
        glVertex2f(px,py-3);
    glEnd();

    //mouth
    drawCircle(px+2,py,4.0);
    glColor3f(0,0,0);
    drawCircle(px+4,py,1.0);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glVertex2f(px,py-3);
        glVertex2f(px+1,py-2);
        glVertex2f(px+2,py-3);
        glVertex2f(px+1,py-5);
    glEnd();


    //after mirror
    //top
    metalBlue();
    glBegin(GL_TRIANGLES);
        glVertex2f(px,py+13);
        glVertex2f(px,py+22);
        glVertex2f(px-3,py+15);
    glEnd();
    drawCircle(px-2,py+13,3.0);
    drawCircle(px-2,py+4,4.0);
    drawCircle(px-8,py+2,4.0);
    drawCircle(px-2,py+7,4.5);

    //lid
    glColor3f(1,0,0);
    glBegin(GL_QUADS);
        glVertex2f(px-5,py+15);
        glVertex2f(px-13,py+7);
        glVertex2f(px-10,py+4);
        glVertex2f(px-5,py+4);
    glEnd();
    bodyRed();
    glBegin(GL_QUADS);
        glVertex2f(px-4,py+15);
        glVertex2f(px-10,py+9);
        glVertex2f(px-10,py+4);
        glVertex2f(px-4,py+4);
    glEnd();
    //bottom cir
    glowGreen();
    drawCircle(px-8,py-3,3.0);

    //lidddd
    bodyRed();
    glBegin(GL_POLYGON);
        glVertex2f(px-10,py+4);
        glVertex2f(px-10,py-1);
        glVertex2f(px-11,py-1);
        glVertex2f(px-13,py+1);
        glVertex2f(px-14,py+7);
        glVertex2f(px-13,py+7);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(px-18,py+1);
        glVertex2f(px-19,py);
        glVertex2f(px-21,py);
        glVertex2f(px-14,py+7);
        glVertex2f(px-13,py+1);
    glEnd();

    //teeth
    glBegin(GL_POLYGON);
        glVertex2f(px-3,py-5);
        glVertex2f(px-3,py-12);
        glVertex2f(px-7,py-12);
        glVertex2f(px-10,py-10);
        glVertex2f(px-10,py-7);
        glVertex2f(px-8,py-5);
        glVertex2f(px-3,py-5);
    glEnd();
    glowGreen();
    glBegin(GL_POLYGON);
        glVertex2f(px-3,py-11);
        glVertex2f(px-4,py-10);
        glVertex2f(px-8,py-10);
        glVertex2f(px-9,py-11);
        glVertex2f(px-7,py-12);
        glVertex2f(px-3,py-12);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(px-5,py-12);
        glVertex2f(px-5,py-11);
        glVertex2f(px-6,py-11);
        glVertex2f(px-7,py-12);
        glVertex2f(px-7,py-14);
        glVertex2f(px-6,py-14);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(px-7,py-14);
        glVertex2f(px-6,py-14);
        glVertex2f(px-6,py-16);
    glEnd();

    //btmsqr
    metalBlue();
    glBegin(GL_QUADS);
        glVertex2f(px,py-19);
        glVertex2f(px-3,py-16);
        glVertex2f(px-3,py-3);
        glVertex2f(px,py-3);
    glEnd();

    //mouth
    drawCircle(px-2,py,4.0);
    glColor3f(0,0,0);
    drawCircle(px-4,py,1.0);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glVertex2f(px,py-3);
        glVertex2f(px-1,py-2);
        glVertex2f(px-2,py-3);
        glVertex2f(px-1,py-5);
    glEnd();



    glPopMatrix();
}

void level5() {
    printf("Level 5 Started!\n");
    drawBullets();
    drawlvl5Enemy();
    // Life bar above boss head
    drawBossLifeBar(enemyX, enemyY + 50);
    drawlvl5Player();

}
// -------------------- Display --------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    if (gameState == 0) { // MENU
        drawMenu();
    }
    else if (gameState == 1 && level ==1) { // PLAYING
        drawPlayer();
        drawBullets();
        drawEnemy();
    }
    else if (gameState == 1 && level ==4){
        level4();
    }
    else if(gameState == 1 && level ==5){
        level5();
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

    // Create stars
    for (int i = 0; i < 150; i++) {
        stars.push_back({
            (float)(rand() % windowWidth),
            (float)(rand() % windowHeight)
        });
    }

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
