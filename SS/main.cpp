#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <cstdio>

// -------------------- Window --------------------
int windowWidth = 800, windowHeight = 600;
float btnWidth = 100;
float btnHeight = 40;
float centerX;
float startY;
float gap = 60;
bool keyLeft = false,keyRight = false;

// -------------------- Game State ----------------
int gameState = 0;
int level = 0;

// -------------------- Player --------------------
float playerX = 400, playerY = 50;
int playerWidth = 90 , playerHeight = 50 ;
float playerSpeed = 10;
int playerMaxHP = 30, playerHP = 30, playerLives = 3;
const int fireDelay = 150 ; //150 ms wait
int lastFireTime = 0 ;

// Level 5 Timer (5 minutes = 300 seconds)
int level5Timer = 300;  // in seconds
int lastTimerUpdate = 0; // stores last GLUT time for decrement
bool timerActive = false; // to control timer

struct Bullet {
    float x, y;
};
std::vector<Bullet> bullets;

//---star for backgrond
struct Star {
    float x, y;
};
std::vector<Star> stars;

//---Coin
int coinCount = 0;
int coinTimer = 0;

struct Coin {
    float x, y;
    float speed;
};
std::vector<Coin> coins;

// -------------------- Enemy ---------------------
float enemyX = 400, enemyY = 500;
float enemySpeed = 3;
float enemyDX = 2.5f, enemyDY = 2.0f;
int bossMaxHP = 200;
int bossHP    = 200;

float bossWidth  = 120, bossHeight = 140;
int bossFireCooldown = 0 , bossBombCooldown = 300;
struct BossBomb {
    float x, y;
    float speedY;
    bool exploded;
};
std::vector<BossBomb> bossBombs;

struct BossBullet {
    float x, y;
    float vx, vy;
};
std::vector<BossBullet> bossBullets;

struct EnemyBullet {
    float x, y;
    float speedY;
};
std::vector<EnemyBullet> enemyBullets;


//------------helper function
void drawCircle(double cx, double cy, double r, int num_segments = 50) {
    glBegin(GL_POLYGON);
    for(int i = 0; i < num_segments; i++) {
        double theta = 2.0 * 3.1415926 * double(i) / double(num_segments);
        double x = r * cos(theta); // x offset
        double y = r * sin(theta); // y offset
        glVertex2d(cx + x, cy + y);
    }
    glEnd();
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

//color plate
void bodyRed()    { glColor3f(0.65f, 0.1f, 0.1f); }
void armorDark()  { glColor3f(0.35f, 0.05f, 0.05f); }
void metalBlue()  { glColor3f(0.1f, 0.2f, 0.6f); }
void glowGreen()  { glColor3f(0.2f, 1.0f, 0.3f); }
void eyeYellow()  { glColor3f(1.0f, 0.9f, 0.2f); }
void shadow()     { glColor3f(0.08f, 0.08f, 0.08f); }
// -------------------- Reset Game ----------------
void resetGame(int selectedLevel) {
    playerX = windowWidth / 2;
    playerY = 50;

    enemyX = windowWidth / 2;
    enemyY = 500;

    level = selectedLevel;

    bossMaxHP = (level==4?100:200) ;
    bossHP    = (level==4?100:200) ;
    playerMaxHP = (level==4?50:30) ;
    playerHP = (level==4?50:30) ;
    playerLives = (level==4?3:2) ;

    bullets.clear();

    if(level == 5){
        level5Timer = 300; // reset timer
        lastTimerUpdate = glutGet(GLUT_ELAPSED_TIME);
        timerActive = true;
    } else {
        timerActive = false;
    }

    gameState = 1; // PLAYIN
}

void drawCoins() {
    for (auto &c : coins) {
        glColor3f(1.0f, 0.85f, 0.0f);
        drawCircle(c.x, c.y, 6.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawCircle(c.x, c.y, 3.0);
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

void drawHUD() {
    char text[50];

    // Lives (top-left)
    sprintf(text, "Lives: %d", playerLives);
    drawText(20, windowHeight - 30, text);

    // Coins (top-right)
    sprintf(text, "Coins: %d", coinCount);
    drawText(windowWidth - 150, windowHeight - 30, text);

    // Level (top-center)
    sprintf(text, "Level: %d", level);
    drawText(windowWidth / 2 - 40, windowHeight - 30, text);

    if(level == 5 && timerActive) {
        int minutes = level5Timer / 60;
        int seconds = level5Timer % 60;
        char timeText[20];
        sprintf(timeText, "Time: %02d:%02d", minutes, seconds);
        drawText(windowWidth / 2 - 40, windowHeight - 50, timeText);
    }

}

//--------background----
void drawBackground() {
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

// -------------------- Draw Player ----------------
void drawlvl1Player() {

    float px = playerX, py = playerY;

    glPushMatrix();

    glTranslated(px, py, 0);   // move to position
    glScalef(2.5, 2.5, 0); // scale ×2
    glTranslated(-px, -py, 0); // move back

    //glColor3f(0.149f, 0.255f, 0.235f);

 glColor3f(0.157f, 0.349f, 0.263f);

    glBegin(GL_TRIANGLES);
        glVertex2f(px, py + 15);
        glVertex2f(px , py );
        glVertex2f(px + 10, py );
    glEnd();
//glColor3f(0.149f, 0.255f, 0.235f);

 glColor3f(0.561f, 0.737f, 0.580f);
     glBegin(GL_TRIANGLES);
        glVertex2f(px, py + 15);
        glVertex2f(px , py );
        glVertex2f(px - 10, py );
    glEnd();

    //back
   //   glColor3f(0.561f, 0.737f, 0.580f);
glColor3f(0.149f, 0.255f, 0.235f);
     glBegin(GL_QUADS);
        glVertex2f(px, py);
        glVertex2f(px+5 , py );
        glVertex2f(px , py-5 );
        glVertex2f(px+5 , py-10 );

    glEnd();
 //glColor3f(0.561f, 0.737f, 0.580f);
glColor3f(0.149f, 0.255f, 0.235f);
     glBegin(GL_QUADS);
        glVertex2f(px, py);
        glVertex2f(px-5 , py );
        glVertex2f(px , py-5 );
        glVertex2f(px-5 , py-10 );

    glEnd();


//wings

 //glColor3f(0.561f, 0.737f, 0.580f);
glColor3f(0.149f, 0.255f, 0.235f);
     glBegin(GL_QUADS);
        glVertex2f(px+5, py);
        glVertex2f(px+10 , py );
        glVertex2f(px+15 , py-5 );
        glVertex2f(px+10 , py-5 );

    glEnd();

  //  glColor3f(0.561f, 0.737f, 0.580f);
glColor3f(0.149f, 0.255f, 0.235f);
     glBegin(GL_QUADS);
        glVertex2f(px-5, py);
        glVertex2f(px-10 , py );
        glVertex2f(px-15 , py-5 );
        glVertex2f(px-10 , py-5 );

    glEnd();


  glColor3f(0.149f, 0.255f, 0.235f);
     glBegin(GL_TRIANGLES);
        glVertex2f(px, py+5);
        glVertex2f(px+5 , py );
        glVertex2f(px , py );


    glEnd();



  glColor3f(0.149f, 0.255f, 0.235f);
     glBegin(GL_TRIANGLES);
        glVertex2f(px, py+5);
        glVertex2f(px-5 , py );
        glVertex2f(px , py );


    glEnd();




    glPopMatrix();
}

void drawlvl2Player()
{
     float px = playerX, py = playerY;

    glPushMatrix();

    glTranslated(px, py, 0);   // move to position
    glScalef(2.5, 2.5, 0); // scale ×2
    glTranslated(-px, -py, 0); // move back

    //glColor3f(0.149f, 0.255f, 0.235f);
glColor3f(0.690f, 0.071f, 0.255f);


    glBegin(GL_TRIANGLES);
        glVertex2f(px, py + 15);
        glVertex2f(px , py );
        glVertex2f(px + 15, py );
    glEnd();

glColor3f(0.690f, 0.071f, 0.255f);


    glBegin(GL_TRIANGLES);
        glVertex2f(px, py + 15);
        glVertex2f(px , py );
        glVertex2f(px - 15, py );
    glEnd();

     glColor3f(0.157f, 0.349f, 0.263f);

    glBegin(GL_QUADS);

        glVertex2f(px , py );
        glVertex2f(px +10, py );
        glVertex2f(px +10, py-5 );
  glVertex2f(px, py -5);
    glEnd();

glColor3f(0.157f, 0.349f, 0.263f);

    glBegin(GL_QUADS);

        glVertex2f(px , py );
        glVertex2f(px -10, py );
        glVertex2f(px -10, py-5 );
  glVertex2f(px, py -5);
    glEnd();



    ////
    glColor3f(0.988f, 0.004f, 0.004f);

     glBegin(GL_QUADS);

        glVertex2f(px , py-5 );
        glVertex2f(px +5, py-5 );
        glVertex2f(px +5, py-10 );
  glVertex2f(px, py -10);
    glEnd();
////
glColor3f(0.545f, 0.0f, 0.294f);


     glBegin(GL_QUADS);

        glVertex2f(px , py-5 );
        glVertex2f(px -5, py-5 );
        glVertex2f(px -5, py-10 );
  glVertex2f(px, py -10);
    glEnd();


    glPopMatrix();

}

void drawlvl4Player(){
    float px = playerX, py = playerY;
    //left arm
    glPushMatrix();
    glTranslated(px, py, 0);   // move to position
    glScalef(2.7, 2.7, 0); // scale ×2
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

void drawlvl5Player(){
    float px = playerX , py = playerY ;

    glPushMatrix();
    glTranslated(px, py, 0);
    glScalef(4, 4, 0);
    glTranslated(-px, -py, 0);

    glColor3f(1,1,1);
    glColor3f(107/255.0f, 120/255.0f, 78/255.0f);

    glBegin(GL_POLYGON);
        glVertex2f(px,py+13);
        glVertex2f(px-30,py-6);
        glVertex2f(px-28,py-9);
        glVertex2f(px-15,py-2);
        glVertex2f(px-8,py-8);
        glVertex2f(px-5,py-5);
        glVertex2f(px,py-10);
    glEnd();

    glColor3f(0,0,0);
    //glColor3f(60/255.0f, 60/255.0f, 60/255.0f);

    glBegin(GL_LINES);
        glVertex2f(px,py+12.2);
        glVertex2f(px-29,py-6);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-29,py-6);
        glVertex2f(px-27.5,py-8);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-27.5,py-8);
        glVertex2f(px-15,py-1);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-15,py-1);
        glVertex2f(px-8,py-7);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-8,py-7);
        glVertex2f(px-5,py-4);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-5,py-4);
        glVertex2f(px,py-9);
    glEnd();


    glBegin(GL_LINES);
        glVertex2f(px,py-6);
        glVertex2f(px-2,py-6);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-2,py-6);
        glVertex2f(px-3,py-7);
    glEnd();


    glBegin(GL_LINES);
        glVertex2f(px-3,py-6);
        glVertex2f(px-3,py+2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-7,py-6);
        glVertex2f(px-7,py+2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-7,py-3);
        glVertex2f(px-5,py-2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-5,py-2);
        glVertex2f(px-3,py-3);
    glEnd();


    glBegin(GL_TRIANGLES);
        glVertex2f(px-7,py+2);
        glVertex2f(px-7,py+3);
        glVertex2f(px-5,py+2);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(px-5,py+2);
        glVertex2f(px-3,py+2);
        glVertex2f(px-3,py+3);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2f(px-1,py+7);

        glVertex2f(px-1,py+10);
        glVertex2f(px-2,py+9);
        glVertex2f(px-3,py+7);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px-3,py+3);
        glVertex2f(px-7,py+3);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px-1,py+10);
        glVertex2f(px,py+11);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-3,py+7);
        glVertex2f(px-3,py+2);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px-12,py+5);
        glVertex2f(px-10,py+3);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-10,py+3);
        glVertex2f(px-10,py-2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-10,py-2);
        glVertex2f(px-11,py-3);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px-9,py-5);
        glVertex2f(px-16,py+1);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-14,py+2.1);
        glVertex2f(px-27,py-6);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px-27,py-6);
        glVertex2f(px-27.5,py-5.2);
    glEnd();

    glColor3f(1,1,1);
    glColor3f(107/255.0f, 120/255.0f, 78/255.0f);

    glBegin(GL_POLYGON);
        glVertex2f(px,py+13);
        glVertex2f(px+30,py-6);
        glVertex2f(px+28,py-9);
        glVertex2f(px+15,py-2);
        glVertex2f(px+8,py-8);
        glVertex2f(px+5,py-5);
        glVertex2f(px,py-10);
    glEnd();

    glColor3f(0,0,0);

    glBegin(GL_LINES);
        glVertex2f(px,py+12.2);
        glVertex2f(px+29,py-6);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+29,py-6);
        glVertex2f(px+27.5,py-8);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+27.5,py-8);
        glVertex2f(px+15,py-1);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+15,py-1);
        glVertex2f(px+8,py-7);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+8,py-7);
        glVertex2f(px+5,py-4);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+5,py-4);
        glVertex2f(px,py-9);
    glEnd();


    glBegin(GL_LINES);
        glVertex2f(px,py-6);
        glVertex2f(px+2,py-6);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+2,py-6);
        glVertex2f(px+3,py-7);
    glEnd();


    glBegin(GL_LINES);
        glVertex2f(px+3,py-6);
        glVertex2f(px+3,py+2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+7,py-6);
        glVertex2f(px+7,py+2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+7,py-3);
        glVertex2f(px+5,py-2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+5,py-2);
        glVertex2f(px+3,py-3);
    glEnd();


    glBegin(GL_TRIANGLES);
        glVertex2f(px+7,py+2);
        glVertex2f(px+7,py+3);
        glVertex2f(px+5,py+2);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(px+5,py+2);
        glVertex2f(px+3,py+2);
        glVertex2f(px+3,py+3);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2f(px+1,py+7);

        glVertex2f(px+1,py+10);
        glVertex2f(px+2,py+9);
        glVertex2f(px+3,py+7);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px+3,py+3);
        glVertex2f(px+7,py+3);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px+1,py+10);
        glVertex2f(px,py+11);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+3,py+7);
        glVertex2f(px+3,py+2);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px+12,py+5);
        glVertex2f(px+10,py+3);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+10,py+3);
        glVertex2f(px+10,py-2);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+10,py-2);
        glVertex2f(px+11,py-3);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(px+9,py-5);
        glVertex2f(px+16,py+1);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+14,py+2.1);
        glVertex2f(px+27,py-6);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(px+27,py-6);
        glVertex2f(px+27.5,py-5.2);
    glEnd();



    glPopMatrix();
}

void damagePlayer(int dmg)
{
    playerHP -= dmg;

    if (playerHP <= 0) {
        playerLives--;
        playerHP = playerMaxHP;   // reset HP for next life

        printf("Life lost! Remaining lives: %d\n", playerLives);

        // Reset player position
        playerX = windowWidth / 2;
        playerY = 50;
    }

    // Game over
    if (playerLives <= 0) {
        printf("GAME OVER\n");
        gameState = 0; // back to menu
    }
}

void drawPlayerLifeBar()
{
    float barWidth  = 120.0f;
    float barHeight = 10.0f;

    float marginX = 15.0f;
    float marginY = 15.0f;

    float hpRatio = (float)playerHP / playerMaxHP;
    float hpWidth = barWidth * hpRatio;

    float x = marginX;
    float y = marginY;

    // Background (red)
    glColor3f(0.6f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + barWidth, y);
        glVertex2f(x + barWidth, y + barHeight);
        glVertex2f(x, y + barHeight);
    glEnd();

    // HP (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + hpWidth, y);
        glVertex2f(x + hpWidth, y + barHeight);
        glVertex2f(x, y + barHeight);
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
void drawlvl1Enemy()
{
        float px = enemyX, py = enemyY ;

    glPushMatrix();

    glTranslated(px, py, 0);   // move to position
    glScalef(2, 2, 0); // scale ×2
    glTranslated(-px, -py, 0); // move back

    //glColor3f(0.149f, 0.255f, 0.235f);

glColor3f(0.545f, 0.0f, 0.294f);

    glBegin(GL_TRIANGLES);
        glVertex2f(px+5, py + 10);
        glVertex2f(px+5 , py );
        glVertex2f(px , py );
    glEnd();
glColor3f(0.545f, 0.0f, 0.294f);

      glBegin(GL_TRIANGLES);
        glVertex2f(px-5, py + 10);
        glVertex2f(px-5 , py );
        glVertex2f(px , py );
    glEnd();



glColor3f(0.659f, 0.318f, 0.431f);


    glBegin(GL_TRIANGLES);
        glVertex2f(px, py +20);
        glVertex2f(px+5 , py+10 );
        glVertex2f(px , py );
    glEnd();
    //glColor3f(0.545f, 0.0f, 0.294f);
glColor3f(0.153f, 0.0f, 0.082f);

      glBegin(GL_TRIANGLES);
        glVertex2f(px, py +20);
        glVertex2f(px-5 , py+10 );
        glVertex2f(px , py );
    glEnd();

  //  glColor3f(0.153f, 0.0f, 0.082f);
glColor3f(0.498f, 0.090f, 0.204f);


      glBegin(GL_POLYGON);
        glVertex2f(px+5 , py+10 );
        glVertex2f(px+10 , py+5 );
        glVertex2f(px+15 , py-10 );
        glVertex2f(px +10, py-5 );
        glVertex2f(px+5 , py-5 );

    glEnd();

  //  glColor3f(0.153f, 0.0f, 0.082f);
glColor3f(0.498f, 0.090f, 0.204f);


      glBegin(GL_POLYGON);
        glVertex2f(px-5 , py+10 );
        glVertex2f(px-10 , py+5 );
        glVertex2f(px-15 , py-10 );
        glVertex2f(px -10, py-5 );
        glVertex2f(px-5 , py-5 );

    glEnd();





   glPopMatrix();

}

void drawlvl2Enemy() {

 float px = enemyX, py = enemyY;

    glPushMatrix();

    glTranslated(px, py, 0);   // move to position
    glScalef(2, 2, 0); // scale ×2
    glTranslated(-px, -py, 0); // move back

    //glColor3f(0.149f, 0.255f, 0.235f);

glColor3f(0.0f, 0.427f, 0.467f);
    glBegin(GL_TRIANGLES);
        glVertex2f(px, py + 20);
        glVertex2f(px+10 , py );
        glVertex2f(px , py );
    glEnd();

glColor3f(0.0f, 0.427f, 0.467f);


    glBegin(GL_TRIANGLES);
        glVertex2f(px, py + 20);
        glVertex2f(px-10 , py );
        glVertex2f(px , py );
    glEnd();

glColor3f(0.369f, 0.392f, 0.447f);

    glBegin(GL_POLYGON);
       glVertex2f(px, py );
        glVertex2f(px+10 , py );

        glVertex2f(px+15 , py-10 );
         glVertex2f(px , py-5 );

    glEnd();

glColor3f(0.369f, 0.392f, 0.447f);

    glBegin(GL_POLYGON);
       glVertex2f(px, py );
        glVertex2f(px-10 , py );

        glVertex2f(px-15 , py-10 );
         glVertex2f(px , py-5 );

        glEnd();

        //////////////
glColor3f(0.120f, 0.1927f, 0.277f);

    glBegin(GL_QUADS);
       glVertex2f(px, py );
        glVertex2f(px+5 , py );

        glVertex2f(px+5 , py+5 );
         glVertex2f(px , py+5 );

        glEnd();


        //////////////
glColor3f(0.210f, 0.22f, 0.267f);

    glBegin(GL_QUADS);
       glVertex2f(px, py );
        glVertex2f(px-5 , py );

        glVertex2f(px-5 , py+5 );
         glVertex2f(px , py+5 );
    glEnd();
    glPopMatrix();
}


void drawlvl4Enemy(){
   float ex = enemyX , ey = enemyY ;

    glPushMatrix();
    glTranslated(ex, ey, 0);
    glScalef(4, 4, 0);
    glTranslated(-ex, -ey, 0);

    glColor3f(0.43f, 0.45f, 0.47f);
    glBegin(GL_QUADS);
        glVertex2f(ex,ey+20);
        glVertex2f(ex+2,ey+20);
        glVertex2f(ex+5,ey+15);
        glVertex2f(ex,ey+10);
    glEnd();

    glColor3f(0.47f, 0.08f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(ex+5,ey+15);
        glVertex2f(ex,ey+10);
        glVertex2f(ex,ey-5);
        glVertex2f(ex+12,ey-5);
        glVertex2f(ex+15,ey+5);
        glVertex2f(ex+15,ey+10);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_QUADS);
        glVertex2f(ex,ey);
        glVertex2f(ex,ey-1);
        glVertex2f(ex+8,ey-1);
        glVertex2f(ex+8,ey);
    glEnd();

    drawCircle(ex+5,ey+5,2.0);

    glColor3f(1,1,1);
    glBegin(GL_TRIANGLES);
        glVertex2f(ex,ey-5);
        glVertex2f(ex,ey-10);
        glVertex2f(ex+3,ey-5);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(ex+3,ey-5);
        glVertex2f(ex+5,ey-10);
        glVertex2f(ex+8,ey-5);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(ex+8,ey-5);
        glVertex2f(ex+12,ey-5);
        glVertex2f(ex+10,ey-10);
    glEnd();


    //--hand right
    glColor3f(0.35f, 0.06f, 0.07f);
    glBegin(GL_POLYGON);
        glVertex2f(ex+16,ey+10);
        glVertex2f(ex+15,ey+10);
        glVertex2f(ex+15,ey+5);
        glVertex2f(ex+20,ey-10);
        glVertex2f(ex+23,ey-10);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(ex+15,ey-15);
        glVertex2f(ex+20,ey-10);
        glVertex2f(ex+23,ey-10);
    glEnd();
    //--hand left
    glBegin(GL_POLYGON);
        glVertex2f(ex-16,ey+10);
        glVertex2f(ex-15,ey+10);
        glVertex2f(ex-15,ey+5);
        glVertex2f(ex-20,ey-10);
        glVertex2f(ex-23,ey-10);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(ex-15,ey-15);
        glVertex2f(ex-20,ey-10);
        glVertex2f(ex-23,ey-10);
    glEnd();



    //---left
    glColor3f(0.43f, 0.45f, 0.47f);
    glBegin(GL_QUADS);
        glVertex2f(ex,ey+20);
        glVertex2f(ex-2,ey+20);
        glVertex2f(ex-5,ey+15);
        glVertex2f(ex,ey+10);
    glEnd();

    glColor3f(0.47f, 0.08f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(ex-5,ey+15);
        glVertex2f(ex,ey+10);
        glVertex2f(ex,ey-5);
        glVertex2f(ex-12,ey-5);
        glVertex2f(ex-15,ey+5);
        glVertex2f(ex-15,ey+10);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_QUADS);
        glVertex2f(ex,ey);
        glVertex2f(ex,ey-1);
        glVertex2f(ex-8,ey-1);
        glVertex2f(ex-8,ey);
    glEnd();

    drawCircle(ex-5,ey+5,2.0);

    glColor3f(1,1,1);
    glBegin(GL_TRIANGLES);
        glVertex2f(ex,ey-5);
        glVertex2f(ex,ey-10);
        glVertex2f(ex-3,ey-5);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(ex-3,ey-5);
        glVertex2f(ex-5,ey-10);
        glVertex2f(ex-8,ey-5);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(ex-8,ey-5);
        glVertex2f(ex-12,ey-5);
        glVertex2f(ex-10,ey-10);
    glEnd();

    glPopMatrix();
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

void updateLevel5Timer() {
    if(!timerActive) return;

    int currentTime = glutGet(GLUT_ELAPSED_TIME); // milliseconds
    if(currentTime - lastTimerUpdate >= 1000) {  // 1 second elapsed
        level5Timer--;   // reduce 1 second
        lastTimerUpdate = currentTime;
    }

    // Time up -> Game Over
    if(level5Timer <= 0) {
        timerActive = false;
        printf("TIME UP! Game Over\n");
        playerLives--;
        playerHP = playerMaxHP;
        gameState = 0; // go back to menu
    }
}

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

void drawBossBullets() {
    for (auto &b : bossBullets) {
        glColor3f(1.0f, 0.2f, 0.2f); // red bullet
        glBegin(GL_QUADS);
            glVertex2f(b.x - 3, b.y);
            glVertex2f(b.x + 3, b.y);
            glVertex2f(b.x + 3, b.y + 10);
            glVertex2f(b.x - 3, b.y + 10);
        glEnd();
    }
}

void drawBossBombs() {
    for(auto &b : bossBombs) {
        if(!b.exploded) {
            glColor3f(0.8f, 0.5f, 0.0f); // orange bomb
            drawCircle(b.x, b.y, 12.0);
        } else {
            // Draw explosion
            glColor3f(1.0f, 0.6f, 0.0f); // bright orange
            drawCircle(b.x, b.y, 18.0);
            glColor3f(1.0f, 0.2f, 0.0f); // red inner
            drawCircle(b.x, b.y, 13.0);
        }
    }

    for(int i = 0; i < bossBombs.size(); i++) {
        if(bossBombs[i].exploded) {
            bossBombs.erase(bossBombs.begin() + i);
            i--;
        }
    }
}

void fireBossBullet(float startX, float startY) {
    float targetX = playerX;
    float targetY = playerY;

    float dx =  targetX - startX;
    float dy = targetY - startY ;

    float length = sqrt(dx*dx + dy*dy);
    if (length == 0) length = 1;

    float speed = 5.0f;

    BossBullet b;
    b.x = startX;
    b.y = startY;
    b.vx = (dx / length) * speed;
    b.vy = (dy / length) * speed;

    bossBullets.push_back(b);
}

// -------------------- Update Game ----------------
void update(int value) {
    if (gameState == 1) { // PLAYING
        coinTimer++;
        if (coinTimer > 300 && level >=4 ) { // ~5 seconds (16ms * 300)
            coins.push_back({
                (float)(rand() % (windowWidth - 40) + 20),
                (float)windowHeight + 10,   // start from top
                2.5f + rand()%3             // falling speed
            });
            coinTimer = 0;
        }

        for (int i = 0; i < coins.size(); i++) {
            // Fall down
            coins[i].y -= coins[i].speed;

            // Player collision
            /*
            bossBombs[i].x >= playerX - playerWidth / 2 &&
                        bossBombs[i].x <= playerX + playerWidth / 2 &&
                        bossBombs[i].y >= playerY - playerHeight / 2 &&
                        bossBombs[i].y <= playerY + playerHeight / 2
            */

            if (coins[i].x >= playerX - playerWidth / 2 &&
                        coins[i].x <= playerX + playerWidth / 2 &&
                        coins[i].y >= playerY - playerHeight / 2 &&
                        coins[i].y <= playerY + playerHeight / 2){

                coinCount++;
                coins.erase(coins.begin() + i);
                i--;
                continue;
            }

            if (coins[i].y < -10) coins.erase(coins.begin() + i--);
        }

        // Player movement
        if (keyLeft && playerX > 20) playerX -= playerSpeed;
        if (keyRight && playerX < windowWidth - 20) playerX += playerSpeed;

        // player Bullet movement
        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].y += 10;
            if (bullets[i].y > windowHeight) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }

        if(level==1 || level==2){
            // Collision detection
            for (int i = 0; i < bullets.size(); i++) {
                if (abs(bullets[i].x - enemyX) < 20 &&
                    abs(bullets[i].y - enemyY) < 20)
                    {

                    bullets.erase(bullets.begin() + i);
                        coins.push_back({
            enemyX,
            enemyY,
            2.0f   // falling speed
        });
                    enemyY = 450;
                    enemyX = rand() % (windowWidth - 40) + 20;

                    break;
                }
            }
            for (int i = 0; i < bullets.size(); i++) {
                if (abs(bullets[i].x - enemyX) < 20 &&
                    abs(bullets[i].y - enemyY) < 20)
                {
                    bullets.erase(bullets.begin() + i);
                    coins.push_back({enemyX, enemyY, 2.0f});
                    enemyY = 450;
                    enemyX = rand() % (windowWidth - 40) + 20;
                    break;
                }
            }

        // Enemy shooting logic for level 2
        if(level == 2){
            static int lastEnemyFireTime = 0;
            int currentTime = glutGet(GLUT_ELAPSED_TIME);
            int fireDelay = 2000; // fire every 2 seconds

            if(currentTime - lastEnemyFireTime > fireDelay){
                // Create new enemy bullet
                enemyBullets.push_back({enemyX, enemyY, -5.0f}); // speedY = -5 (downwards)
                lastEnemyFireTime = currentTime;
            }
        }
        }
        if(level == 2){
            for(int i = 0; i < enemyBullets.size(); i++){
                enemyBullets[i].y += enemyBullets[i].speedY;

                // Check collision with player
                if(abs(enemyBullets[i].x - playerX) < 15 &&
                abs(enemyBullets[i].y - playerY) < 15){
                    damagePlayer(5); // example damage
                    enemyBullets.erase(enemyBullets.begin() + i--);
                    continue;
                }

                // Remove if off-screen
                if(enemyBullets[i].y < 0){
                    enemyBullets.erase(enemyBullets.begin() + i--);
            }
        }

        }
        if (level == 5 || level==4) {
            // Move enemy
            if (level==5)updateLevel5Timer();
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

            bossFireCooldown--;
            if (bossFireCooldown <= 0) {
                fireBossBullet(enemyX, enemyY - bossHeight/2);
                bossFireCooldown = 60; // fire rate
            }


            // Move boss bullets
            for (int i = 0; i < bossBullets.size(); i++) {
                bossBullets[i].x += bossBullets[i].vx;
                bossBullets[i].y += bossBullets[i].vy;

                // Remove if off screen
                if (bossBullets[i].x < 0 || bossBullets[i].x > windowWidth ||
                    bossBullets[i].y < 0 || bossBullets[i].y > windowHeight) {
                    bossBullets.erase(bossBullets.begin() + i);
                    i--;
                    continue;
                }

                // Collision with player
                if (bossBullets[i].x >= playerX - playerWidth / 2 &&
                    bossBullets[i].x <= playerX + playerWidth / 2 &&
                    bossBullets[i].y >= playerY - playerHeight / 2 &&
                    bossBullets[i].y <= playerY + playerHeight / 2) {

                    damagePlayer(5);
                    printf("Player hit by boss!\n");

                    bossBullets.erase(bossBullets.begin() + i);
                    i--;
                }
            }


            // Boss dropping bombs
            bossBombCooldown--;
            if (bossBombCooldown <= 0) {
                bossBombs.push_back({enemyX, enemyY - 50, -4.0f, false}); // slower than bullets
                bossBombCooldown = 400 + rand()%100; // randomize cooldown for unpredictability
            }

            // Move bombs
            for(int i=0; i<bossBombs.size(); i++) {
                if(bossBombs[i].exploded) continue;

                bossBombs[i].y += bossBombs[i].speedY;

                // Check collision with player
                if(bossBombs[i].x >= playerX - playerWidth / 2 &&
                        bossBombs[i].x <= playerX + playerWidth / 2 &&
                        bossBombs[i].y >= playerY - playerHeight / 2 &&
                        bossBombs[i].y <= playerY + playerHeight / 2) {
                    bossBombs[i].exploded = true;
                    printf("Player hit by bomb!\n");
                    damagePlayer(20);
                    // Optional: reduce player life or reset player
                }

                // Check if bomb reaches bottom of screen
                if(bossBombs[i].y <= 0) {
                    bossBombs[i].exploded = true;
                }
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

//------------levels
void drawEnemyBullets(){
    for(auto &b : enemyBullets){
        glColor3f(1.0f, 0.0f, 0.0f); // red bullets
        glBegin(GL_QUADS);
            glVertex2f(b.x-2, b.y);
            glVertex2f(b.x+2, b.y);
            glVertex2f(b.x+2, b.y+10);
            glVertex2f(b.x-2, b.y+10);
        glEnd();
    }
}

void level1() {
    printf("Level 1 Started!\n");
    drawHUD();
    drawCoins();

    drawlvl1Player();
    drawBullets();
    drawlvl1Enemy();
    drawPlayerLifeBar();
    drawHUD();
}
void level2() {
    printf("Level 2 Started!\n");
    drawEnemyBullets();
    drawHUD();
    drawCoins();
    drawBullets();
    drawlvl2Player();
    drawlvl2Enemy();

    drawPlayerLifeBar();
}
void level3() { printf("Level 3 Started!\n"); }

void level4() {
     printf("Level 4 Started!\n");
    drawlvl4Enemy();
    drawBossLifeBar(enemyX, enemyY + 50);

    drawBullets();
    drawBossBullets();
    drawCoins();

    drawlvl4Player();

    drawPlayerLifeBar();
    drawHUD();
}

void level5() {
    printf("Level 5 Started!\n");
    drawlvl5Enemy();
    drawBossLifeBar(enemyX, enemyY + 50);

    drawBullets();
    drawBossBullets();
    drawlvl5Player();
    drawBossBombs();
    drawCoins();

    drawPlayerLifeBar();
    drawHUD();
}
// -------------------- Display --------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    if (!gameState) {
        drawMenu();
    }else{
        if(level == 1) level1();
        if(level == 2) level2();
        if(level == 3) level3();
        if(level == 4) level4();
        if(level == 5) level5();
    }

    glFlush();
}

// -------------------- Keyboard -------------------
void keyPress(unsigned char key, int x, int y) {
    if (key == ' ' && gameState==1) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);

        if (currentTime - lastFireTime >= fireDelay) {
            bullets.push_back({playerX, playerY + 20});
            lastFireTime = currentTime;
        }
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
