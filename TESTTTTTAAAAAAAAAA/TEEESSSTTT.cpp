#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
// crediting"https://stackoverflow.com/questions/8804880/use-playsound-in-c-opengl-to-play-sound-in-background"
#include <windows.h>        // Required for PlaySound to access the core WIN32 sound function
#include <mmsystem.h>       // Required for PlaySound to access the core WIN32 sound function
#pragma comment(lib, "winmm.lib") // Links the audio library automatically via playsound() to enable processing at the system level 
#endif

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GL/glu.h>

bool hasPlayedEndSound = false;
// --- Global Variables ---
GLuint myTexture1, myTexture2, page2, page3,endPage;
char image1Path[] = "assets\\backgroundBMPSized.bmp";
char image2Path[] = "assets\\START_BACKGROUND.bmp";
//--------
char page2Path[] = "assets\\2_continue.bmp";
char page3Path[] = "assets\\3_continue.bmp";
char EndPagePath[] = "assets\\Ending.bmp";


char soundPath[] ="assets\\soundEffect.WAV";

GLint win_width = 500, win_hight = 500;
GLdouble PI = 3.14159265359;

// Game State: 0 = Menu, 1 = Playing, 2 = Finished
int gameState = 0;

// Ghost Properties
float ghostX = -0.8f;  // Start Bottom Left to Ep1
float ghostY = -1.0f;
float ghostScale = 0.1f;
float moveSpeed = 0.05f;
float ghostRadius = 0.06f; 

float goalX = 1.0f;
float goalY = 0.7f;
float thick = 0.015f;

struct Wall { //def the wall matrix
    float x1, y1, x2, y2, thickness;
};

//crediting "Dr/ Omniah H. Nagoor"
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius) {
    int triangleAmount = 20;
    GLfloat twicePi = 2.0f * PI;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= triangleAmount; i++) {
        glVertex2f(
            x + (radius * cos(i * twicePi / triangleAmount)),
            y + (radius * sin(i * twicePi / triangleAmount))
        );
    }
    glEnd();
}
// Ep1 Samaa walls
Wall walls[] = {
    // OBJ 1
     {-1.0f,  0.0f, -0.2f,  0.0f, thick}, // z -> A1
     {-0.2f,  0.0f, -0.2f, -0.2f, thick}, // A1 -> B1
     {-0.2f, -0.2f, -0.6f, -0.2f, thick}, // B1 -> C1
     {-0.6f, -0.2f, -0.6f, -0.4f, thick}, // C1 -> D1
     {-0.6f, -0.4f, -1.0f, -0.4f, thick}, // D1 -> W

     //  OBJ 2
     {-1.0f,  0.4f, -0.6f,  0.4f, thick}, // E -> F
     {-0.6f,  0.4f, -0.6f,  0.6f, thick}, // F -> G
     {-0.6f,  0.6f, -0.4f,  0.6f, thick}, // G -> H
     {-0.4f,  0.6f, -0.4f,  0.2f, thick}, // H -> I
     {-0.4f,  0.2f, -1.0f,  0.2f, thick}, // I -> J

     // OBJ 3
     {-0.9f,  1.0f, -0.9f,  0.5f, thick}, // K -> L
     {-0.9f,  0.5f, -0.7f,  0.5f, thick}, // L -> S
     {-0.7f,  0.5f, -0.7f,  0.6f, thick}, // S -> M
     {-0.7f,  0.6f, -0.8f,  0.6f, thick}, // M -> T
     {-0.8f,  0.6f, -0.8f,  0.8f, thick}, // T -> U
     {-0.8f,  0.8f, -0.7f,  0.8f, thick}, // U -> V
     {-0.7f,  0.8f, -0.7f,  1.0f, thick}, // V -> N

     // OBJ 4
     {-0.6f,  1.0f, -0.6f,  0.8f, thick}, // O -> Q
     {-0.6f,  0.8f, -0.2f,  0.8f, thick}, // Q -> R
     {-0.2f,  0.8f, -0.2f,  1.0f, thick}, // R -> P

     //OBJ 5 
     { 0.0f,  1.0f,  0.0f,  0.8f, thick}, // z1 -> O2
     { 0.0f,  0.8f,  0.2f,  0.8f, thick}, // O2 -> P2
     { 0.2f,  0.8f,  0.2f,  0.6f, thick}, // P2 -> Q2
     { 0.2f,  0.6f,  0.0f,  0.6f, thick}, // Q2 -> R2
     { 0.0f,  0.6f,  0.0f,  0.4f, thick}, // R2 -> A2
     { 0.0f,  0.4f, -0.2f,  0.4f, thick}, // A2 -> Q1
     {-0.2f,  0.4f, -0.2f,  0.2f, thick}, // Q1 -> R1
     {-0.2f,  0.2f,  0.0f,  0.2f, thick}, // R1 -> S1
     { 0.0f,  0.2f,  0.0f,  0.0f, thick}, // S1 -> L2
     { 0.0f,  0.0f,  0.2f,  0.0f, thick}, // L2 -> M2
     { 0.2f,  0.0f,  0.2f,  0.4f, thick}, // M2 -> V1
     { 0.2f,  0.4f,  0.8f,  0.4f, thick}, // V1 -> J1
     { 0.8f,  0.4f,  0.8f,  0.8f, thick}, // J1 -> I1
     { 0.8f,  0.8f,  1.0f,  0.8f, thick}, // I1 -> R1

     // OBJ 6 
     { 0.2f,  0.4f,  0.2f, -0.4f, thick}, // V1 -> W1 (GL_LINES)
     {-0.4f, -0.4f, -0.4f, -0.6f, thick}, // M1 -> P1 
     {-0.4f, -0.6f,  0.2f, -0.6f, thick}, // P1 -> S2
     { 0.2f, -0.6f,  0.2f, -0.8f, thick}, // S2 -> T2
     { 0.2f, -0.8f,  0.4f, -0.8f, thick}, // T2 -> U2
     { 0.4f, -0.8f,  0.4f, -0.4f, thick}, // U2 -> N1
     { 0.4f, -0.4f,  0.2f, -0.4f, thick}, // N1 -> W1
     { 0.2f, -0.4f,  0.2f, -0.2f, thick}, // W1 -> N2
     { 0.2f, -0.2f,  0.0f, -0.2f, thick}, // N2 -> T1
     { 0.0f, -0.2f,  0.0f, -0.4f, thick}, // T1 -> U1
     { 0.0f, -0.4f,  -0.4f, -0.4f, thick},// U1 -> M1



     // OBJ 7
     { 1.0f,  0.2f,  0.4f,  0.2f, thick}, // B2 -> C2
     { 0.4f,  0.2f,  0.4f,  0.0f, thick}, // C2 -> V2
     { 0.4f,  0.0f,  0.8f,  0.0f, thick}, // V2 -> Z2
     { 0.8f,  0.0f,  0.8f, -0.2f, thick}, // Z2 -> D2
     { 0.8f, -0.2f,  0.6f, -0.2f, thick}, // D2 -> E2
     { 0.6f, -0.2f,  0.6f, -0.4f, thick}, // E2 -> F2
     { 0.6f, -0.4f,  0.8f, -0.4f, thick}, // F2 -> G2
     { 0.8f, -0.4f,  0.8f, -0.6f, thick}, // G2 -> H2
     { 0.8f, -0.6f,  0.6f, -0.6f, thick}, // H2 -> I2
     { 0.6f, -0.6f,  0.6f, -0.8f, thick}, // I2 -> J2
     { 0.6f, -0.8f,  1.0f, -0.8f, thick}, // J2 -> K2
     { 1.0f, -0.8f,  1.0f,  0.2f, thick}, // K2 -> B2

     // OBJ 8
     {-1.0f, -0.6f, -0.6f, -0.6f, thick}, // E1 -> F1
     {-0.6f, -0.6f, -0.6f, -0.7f, thick}, // F1 -> G1
     {-0.6f, -0.7f, -1.0f, -0.7f, thick}, // G1 -> C
     {-1.0f, -0.7f, -1.0f, -0.6f, thick}  // C -> E1 
};
int wallCount = sizeof(walls) / sizeof(Wall);

//Stage 2: Layal Walls
Wall wallsEP2[] = {
    {-1.0f,  0.8f, -0.8f,  0.8f, thick}, // J -> K
    {-0.8f,  0.8f, -0.8f,  0.0f, thick}, // K -> M -> N 
    {-0.6f,  0.8f, -0.6f,  0.6f, thick}, // F -> I 
    {-0.4f,  1.0f, -0.4f,  0.8f, thick}, // H -> G 
    {-0.4f,  0.8f, -0.6f,  0.8f, thick}, // G -> F 
    {-0.4f,  0.8f, -0.2f,  0.8f, thick}, // G -> A1 
    {-0.4f,  0.8f, -0.4f,  0.6f, thick}, // G -> P
    {-0.4f,  0.6f, -0.6f,  0.6f, thick}, // P -> I

    {-0.2f,  0.8f,  0.0f,  0.8f, thick}, // A1 -> point Q
    {0.0f,   0.4f,  0.4f,  0.4f, thick}, // R -> F1 
    {0.4f,   0.4f,  0.4f,  0.6f, thick}, // F1 -> E1 
    {0.4f,   0.6f,  1.0f,  0.6f, thick}, // E1 -> I1 

   
    {-1.0f,  0.0f, -0.6f,  0.0f, thick}, // W -> Z 
    {-0.6f,  0.0f, -0.6f,  0.2f, thick}, // Z -> U1 
    {-0.6f,  0.2f, -0.8f,  0.2f, thick}, // U1 -> N  
    { 0.0f,  0.2f,  0.6f,  0.2f, thick}, // M1 -> K1 
    { 0.6f,  0.2f,  0.6f,  0.4f, thick}, // K1 -> A3 
    { 0.6f,  0.4f,  0.8f,  0.4f, thick}, // A3 -> B3 
    { 0.8f,  0.4f,  0.8f,  0.2f, thick}, // B3 -> C3 
    { 0.8f,  0.2f,  1.0f,  0.2f, thick}, // C3 -> D3 

   
    {-0.8f, -0.2f, -0.8f, -1.0f, thick}, // W1 -> C2 
    {-0.8f, -0.2f, -0.6f, -0.2f, thick}, // W1 -> bet W1 & R1 
    {-0.6f, -0.2f, -0.6f, -0.4f, thick}, // bet W1 & R1 -> R1 
    {-0.4f, -0.2f, -0.4f, -0.4f, thick}, // P1 -> Q1 

    {-0.4f, -0.2f,  0.0f, -0.2f, thick}, // P1 -> after O1 
    {-0.6f, -0.4f, -0.6f, -0.8f, thick}, // R1 -> T1 
    {-0.6f, -0.8f, -0.6f, -1.0f, thick}, // T1 -> near C2 
    {-0.4f, -0.6f, -0.2f, -0.6f, thick}, // P2 -> Q2  
    {-0.4f, -0.1f,  0.2f, -0.1f, thick}, // P1 -> U2

   
    { -0.2f, -0.4f, -0.2f, -0.6f, thick}, // R2 -> Q2 
    { -0.2f, -0.4f,  0.0f, -0.4f, thick}, // R2 -> S2 
    {  0.0f, -0.4f,  0.2f, -0.4f, thick}, // S2 -> T2 
    {  0.2f, -0.4f,  0.2f, -0.2f, thick}, // T2 -> U2 
    {  0.2f, -0.4f,  0.2f, -0.8f, thick}, // T2 -> K2 
    {  0.2f, -0.8f,  1.0f, -0.8f, thick},  // K2 -> O2 

    { 0.0f,  0.0f,  0.6f,  0.0f, thick }, //E->j3
    { 0.6f,  0.0f,  0.6,  -0.6f, thick}, //j-down
    { 0.6,  -0.6f,  0.8f,  0.2f, thick},//down ->G3
    { 0.8f,  0.2f,  1.0f,  0.2f, thick}, //G3->D3
        // fix
    { 0.8,  -0.6f,  1.0f,  -0.6f, thick }, //g1 under J ->G3
    { 0.0f,  0.4f,  0.0f,  0.8f, thick}, //R-> right from A1
    { -0.4f,  -0.2f,  -0.4f,  -0.1f, thick}, // the small gap in R
    { 0.2f,  -0.2f,  0.2f,  -0.1f, thick }, // the small gam in R (right)
    { 0.0f,  0.0f,  0.0f,  0.2f, thick}, // E -> M1
    { -0.4f,  -0.4f,  -0.4f,  -0.6f, thick } // Q1 -> P2
};
// End Layal Walls
int wallCountep2 = sizeof(wallsEP2) / sizeof(Wall);
// ==========================================

Wall wallsEP4[] = {
    // ==== OUTER BORDER ====
    {-1.0f,  1.0f,  1.0f,  1.0f, thick}, // A -> B
    { 1.0f,  1.0f,  1.0f, -1.0f, thick}, // B -> C
    { 1.0f, -1.0f, -1.0f, -1.0f, thick}, // C -> D

    {-0.8f, -0.8f, -0.4f, -0.8f, thick}, // F2 -> G2
    {-0.4f, -0.8f, -0.4f, -1.0f, thick}, // G2 -> H2

    {-0.2f, -0.8f,  0.0f, -0.8f, thick}, // I2 -> J2
    { 0.0f, -0.8f,  0.0f, -1.0f, thick}, // J2 -> K2

    { 0.2f, -0.8f,  0.8f, -0.8f, thick}, // S1 -> T1
    { 0.8f, -0.8f,  0.8f, -0.2f, thick}, // T1 -> O1
    { 0.8f, -0.2f,  0.4f, -0.2f, thick}, // O1 -> N1

    { 0.4f, -0.4f,  0.8f, -0.4f, thick}, // Q1 -> O2

    { 0.8f, -0.6f,  1.0f, -0.6f, thick}, // N2 -> U1

  
    { 0.6f, -0.6f, -0.4f, -0.6f, thick}, // V1 -> W1
    {-0.4f, -0.6f, -0.4f, -0.4f, thick}, // W1 -> Z1

 
    {-0.8f, -0.6f, -0.6f, -0.6f, thick}, // A2 -> B2
    {-0.6f, -0.6f, -0.6f, -0.4f, thick}, // B2 -> C2
    {-0.6f, -0.4f, -0.8f, -0.4f, thick}, // C2 -> D2
    {-0.8f, -0.4f, -0.8f, -0.2f, thick}, // D2 -> E2

    {-0.2f, -0.4f,  0.2f, -0.4f, thick}, // M1 -> L1
    { 0.2f, -0.4f,  0.2f,  0.0f, thick}, // L1 -> K1
    { 0.2f,  0.0f,  1.0f,  0.0f, thick}, // K1 -> J

    { 0.0f, -0.2f, -0.6f, -0.2f, thick}, // V  -> U
    {-0.6f, -0.2f, -0.6f,  0.0f, thick}, // U  -> T
    {-0.6f,  0.0f, -1.0f,  0.0f, thick}, // T  -> S

    {-0.4f,  0.0f,  0.0f,  0.0f, thick}, // H1 -> I1

    {-0.8f,  0.2f, -0.6f,  0.2f, thick}, // L2 -> M2

    { 0.2f,  0.2f, -0.4f,  0.2f, thick}, // G1 -> R
    {-0.4f,  0.2f, -0.4f,  0.4f, thick}, // R  -> Q
    {-0.4f,  0.4f, -1.0f,  0.4f, thick}, // Q  -> P

    { 0.0f,  0.2f,  0.0f,  0.4f, thick}, // O  -> N
    { 0.0f,  0.4f, -0.2f,  0.4f, thick}, // N  -> M
    {-0.2f,  0.4f, -0.2f,  0.6f, thick}, // M  -> L
    {-0.2f,  0.6f, -0.8f,  0.6f, thick}, // L  -> K

    { 0.2f,  0.4f,  0.4f,  0.4f, thick}, // C1 -> D1
    { 0.4f,  0.4f,  0.4f,  0.2f, thick}, // D1 -> E1
    { 0.4f,  0.2f,  1.0f,  0.2f, thick}, // E1 -> F1

    { 1.0f,  0.4f,  0.8f,  0.4f, thick}, // B1 -> A1
    { 0.8f,  0.4f,  0.8f,  0.6f, thick}, // A1 -> Z
    { 0.8f,  0.6f,  0.2f,  0.6f, thick}, // Z  -> W

    { 0.0f,  0.8f,  1.0f,  0.8f, thick}, // H  -> J

    {-0.8f,  0.8f, -0.2f,  0.8f, thick}, // E  -> F
    {-0.2f,  0.8f, -0.2f,  1.0f, thick}, // F  -> G
};
int wallCountep4 = sizeof(wallsEP4) / sizeof(Wall);

// --- Collision Methods --- crediting "https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection" 
bool circleWallCollide(float cx, float cy, float r, float x1, float y1, float x2, float y2, float ht) {
    float minX = fminf(x1, x2) - ht - r;
    float maxX = fmaxf(x1, x2) + ht + r;
    float minY = fminf(y1, y2) - ht - r;
    float maxY = fmaxf(y1, y2) + ht + r;
    return (cx >= minX && cx <= maxX && cy >= minY && cy <= maxY);
}

// crediting "https://stackoverflow.com/questions/17541663/sdl-collision-detection"
bool checkWallCollision(float nx, float ny) {
    
    if (gameState == 1) {
        for (int i = 0; i < wallCount; i++) {
            if (circleWallCollide(nx, ny, ghostRadius, walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2, walls[i].thickness))
                return true;
        }
    }
    else if (gameState == 3) { 
        for (int i = 0; i < wallCountep2; i++) {
            if (circleWallCollide(nx, ny, ghostRadius, wallsEP2[i].x1, wallsEP2[i].y1, wallsEP2[i].x2, wallsEP2[i].y2, wallsEP2[i].thickness))
                return true;
        }

    }
    else if (gameState == 5) {
        for (int i = 0; i < wallCountep4; i++) {
            if (circleWallCollide(nx, ny, ghostRadius, wallsEP4[i].x1, wallsEP4[i].y1, wallsEP4[i].x2, wallsEP4[i].y2, wallsEP4[i].thickness))
                return true;
        }
    }
    return false;
}

//credits to "Dr/ Omniah H. Nagoor"
GLuint LoadTexture(const char* filename, int width, int height) {
    GLuint texture;
    unsigned char* data;
    FILE* file;

    fopen_s(&file, filename, "rb");
    if (file == NULL) {
        std::cout << "Unable to open the image file\n" << "Program will exit :(\n";
        exit(0);
    }

    data = (unsigned char*)malloc(width * height * 3);
    fseek(file, 54, SEEK_SET);
    fread(data, width * height * 3, 1, file);
    fclose(file);

    for (int i = 0; i < width * height; ++i) {
        int index = i * 3;
        unsigned char B, R;
        B = data[index];
        R = data[index + 2];
        data[index] = R;
        data[index + 2] = B;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    return texture;
}

// --- Input Handling ---
void handleKeys(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 27:
        exit(0);
        break;
    }
    if (gameState == 0 && key == '1') {
        std::cout << "Hello user. Try to guid our lost ghost to the colorful light! Use the arrow to move! Our ghost has the ability to get biggr and smaller! Press -S- to get smaller and -B- to be get big! " << std::endl;
        gameState = 1;
        ghostX = -0.9f; ghostY = -0.8f;
    }
    else if (gameState == 1) {
        switch (key) {
        case 'B': if (ghostScale < 0.4f) { ghostScale += 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        case 'S': if (ghostScale > 0.04f) { ghostScale -= 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        }
    }
    else if (gameState == 2 && key == '2') {
        std::cout << "Loading next Ep..." << std::endl;
    hasPlayedEndSound = false;
        gameState = 3;
        ghostX = -0.9f; ghostY = 0.9f;
        goalX = 0.9f;
        goalY = -0.9f;
    }
    else if (gameState == 3) {
        switch (key) {
        case 'B': if (ghostScale < 0.4f) { ghostScale += 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        case 'S': if (ghostScale > 0.04f) { ghostScale -= 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        
        }
    }
    else if (gameState == 4 && key == '3') {
        std::cout << "Loading next Ep..." << std::endl;
        hasPlayedEndSound = false;
        gameState = 5;
        ghostX = -0.3f; ghostY = 0.9f;
        goalX = 0.9f;
        goalY = -0.7f;
    }
    else if (gameState == 5) {
        switch (key) {
        case 'B': if (ghostScale < 0.4f) { ghostScale += 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        case 'S': if (ghostScale > 0.04f) { ghostScale -= 0.02f; ghostRadius = ghostScale * 0.35f; } break;

        }
    }
    glutPostRedisplay();
}

void handleSpecialKeys(int key, int x, int y) {
    if (gameState == 1 || gameState == 3 || gameState == 5) {
        float nextX = ghostX;
        float nextY = ghostY;

        switch (key) {
        case GLUT_KEY_UP:    nextY += moveSpeed; break;
        case GLUT_KEY_DOWN:  nextY -= moveSpeed; break;
        case GLUT_KEY_LEFT:  nextX -= moveSpeed; break;
        case GLUT_KEY_RIGHT: nextX += moveSpeed; break;
        case 'B': if (ghostScale < 0.4f) { ghostScale += 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        case 'S': if (ghostScale > 0.04f) { ghostScale -= 0.02f; ghostRadius = ghostScale * 0.35f; } break;
        }

        if (nextX < -0.97f) nextX = -0.97f;
        if (nextX > 0.97f) nextX = 0.97f;
        if (nextY < -0.97f) nextY = -0.97f;
        if (nextY > 0.97f) nextY = 0.97f;

        if (!checkWallCollision(nextX, nextY)) {
            ghostX = nextX;
            ghostY = nextY;
        }
        else {
            if (!checkWallCollision(nextX, ghostY)) ghostX = nextX;
            else if (!checkWallCollision(ghostX, nextY)) ghostY = nextY;
        }
        //-------------------------------------------------------------------------------------------
        if (gameState == 1) {
            if (ghostX > 0.8f && ghostY > 0.6f) { //thhe coordinates for the winning point so when the ghost touches it, it playes sound
                if (!hasPlayedEndSound) {
                    PlaySound(TEXT("C:\\SAM\\soundEffect.WAV"), NULL, SND_FILENAME | SND_ASYNC);
                    hasPlayedEndSound = true;
                    gameState = 2;
                }
                gameState = 2;
            }
        }
        //-------------------------------------------------------------------------------------------
        else if (gameState == 3) {
            if (ghostX > 0.8f && ghostY < -0.8f) {
                if (!hasPlayedEndSound) {
                    PlaySound(TEXT("C:\\SAM\\soundEffect.WAV"), NULL, SND_FILENAME | SND_ASYNC);
                    hasPlayedEndSound = true;
                    gameState = 4;
                }
                gameState = 4;
            }
        }
        //-------------------------------------------------------------------------------------------
        else if (gameState == 5) {
            if (ghostX > 0.9f && ghostY < -0.6f && ghostY > -0.8f) {
                if (!hasPlayedEndSound) {
                    PlaySound(TEXT("C:\\SAM\\soundEffect.WAV"), NULL, SND_FILENAME | SND_ASYNC);
                    hasPlayedEndSound = true;
                    gameState = 6;
                }
            }
        }
        glutPostRedisplay();
    } 
}

// Stage 1 : Samaaa
// --- The Map Borders ---
void mapBorderEp1() {
    //OBJ1
    glLineWidth(5.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-1, 0);//z
    glVertex2f(-0.2, 0);//A1
    glVertex2f(-0.2, -0.2);//B1
    glVertex2f(-0.6, -0.2);//C1
    glVertex2f(-0.6, -0.4);//D1
    glVertex2f(-1, -0.4);//W
    glEnd();
    //OBJ1 Filling
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);

    glVertex2f(-1, 0);//z
    glVertex2f(-0.2, 0);//A1
    glVertex2f(-0.2, -0.2);//B1
    glVertex2f(-1.0, -0.2);//z mid
    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(-1.0, -0.2);//z mid
    glVertex2f(-0.6, -0.2);//C1
    glVertex2f(-0.6, -0.4);//D1
    glVertex2f(-1, -0.4);//W
    glEnd();


    //OBJ2
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-1.0, 0.4); // E
    glVertex2f(-0.6, 0.4); // F
    glVertex2f(-0.6, 0.6); // G
    glVertex2f(-0.4, 0.6); // H
    glVertex2f(-0.4, 0.2); // I
    glVertex2f(-1, 0.2); // J
    glEnd();
    //obj2 Filling
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);

    glVertex2f(-0.6, 0.6); // G
    glVertex2f(-0.4, 0.6); // H
    glVertex2f(-0.4, 0.2); // I
    glVertex2f(-0.6, 0.2); // I mid
    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(-0.6, 0.2); // I mid
    glVertex2f(-1, 0.2); // J
    glVertex2f(-1.0, 0.4); // E
    glVertex2f(-0.6, 0.4); // F
    glEnd();

    //OBJ3
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.9, 1.0); // K
    glVertex2f(-0.9, 0.5); // L
    glVertex2f(-0.7, 0.5); // S
    glVertex2f(-0.7, 0.6); // M
    glVertex2f(-0.8, 0.6); // T
    glVertex2f(-0.8, 0.8); // U
    glVertex2f(-0.7, 0.8); // V
    glVertex2f(-0.7, 1); // N
    glEnd();
    //OBJ3 Filling
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);

    glVertex2f(-0.9, 1.0); // K
    glVertex2f(-0.7, 1); // N
    glVertex2f(-0.7, 0.8); // V
    glVertex2f(-0.9, 0.8); // mid k
    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(-0.8, 0.8); // U
    glVertex2f(-0.8, 0.6); // T
    glVertex2f(-0.9, 0.6); // 1
    glVertex2f(-0.9, 0.8); // 2
    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(-0.9, 0.6); // mid L
    glVertex2f(-0.9, 0.5); // L
    glVertex2f(-0.7, 0.5); // S
    glVertex2f(-0.7, 0.6); // M
    glEnd();



    //OBJ4
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.6, 1.0); // O
    glVertex2f(-0.6, 0.8); // Q
    glVertex2f(-0.2, 0.8); // R
    glVertex2f(-0.2, 1); // P
    glEnd();
    //OBJ4 Filling
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.6, 1.0); // O
    glVertex2f(-0.6, 0.8); // Q
    glVertex2f(-0.2, 0.8); // R
    glVertex2f(-0.2, 1); // P
    glEnd();

    //OBJ5
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0, 1.0); // z1
    glVertex2f(0, 0.8); // O2
    glVertex2f(0.2, 0.8); // P2
    glVertex2f(0.2, 0.6); // Q2
    glVertex2f(0, 0.6); // R2
    glVertex2f(0, 0.4); // A2
    glVertex2f(-0.2, 0.4); // Q1
    glVertex2f(-0.2, 0.2); // R1
    glVertex2f(0, 0.2); // S1
    glVertex2f(0.0, 0.0); // L2
    glVertex2f(0.2, 0); // M2
    glVertex2f(0.2, 0.4); // V1
    glVertex2f(0.8, 0.4); // J1
    glVertex2f(0.8, 0.8); // I1
    glVertex2f(1, 0.8); // R1
    glEnd();

    //OBJ5 Filling
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0, 1.0); // z1
    glVertex2f(0, 0.8); // O2
    glVertex2f(1, 0.8); // H1
    glVertex2f(1, 1); //B

    glBegin(GL_QUADS);
    glVertex2f(0, 0.2); // S1
    glVertex2f(-0.2, 0.2); // R1
    glVertex2f(-0.2, 0.4); // Q1
    glVertex2f(0, 0.4); // A2

    glBegin(GL_QUADS);
    glVertex2f(0, 0.6); // R2
    glVertex2f(0.2, 0.6); // Q2
    glVertex2f(0.2, 0); // M2
    glVertex2f(0.0, 0.0); // L2

    glBegin(GL_QUADS);
    glVertex2f(0.2, 0.4); // V1
    glVertex2f(0.2, 0.8); // P2
    glVertex2f(0.8, 0.8); // I1
    glVertex2f(0.8, 0.4); // J1
    glEnd();

    //obj6
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.2, 0.4); // V1
    glVertex2f(0.2, -0.4); // W1
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.4, -0.4); // M1
    glVertex2f(-0.4, -0.6); // P1
    glVertex2f(0.2, -0.6); // S2
    glVertex2f(0.2, -0.8); // T2
    glVertex2f(0.4, -0.8); // U2
    glVertex2f(0.4, -0.4); // N1
    glVertex2f(0.2, -0.4); // W1
    glVertex2f(0.2, -0.2); // N2
    glVertex2f(0, -0.2); // T1
    glVertex2f(0.0, -0.4); // U1
    glEnd();
    //obj6 Filling
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);

    glVertex2f(-0.4, -0.4); // M1
    glVertex2f(0.4, -0.4); // N1
    glVertex2f(0.4, -0.6); // O1
    glVertex2f(-0.4, -0.6); // P1
    glEnd();

    glBegin(GL_QUADS);

    glVertex2f(0.2, -0.6); // S2
    glVertex2f(0.4, -0.6); // O1
    glVertex2f(0.4, -0.8); // U2
    glVertex2f(0.2, -0.8); // T2
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0, -0.2); // T1
    glVertex2f(0.2, -0.2); // N2
    glVertex2f(0.2, -0.4); // W1
    glVertex2f(0.0, -0.4); // U1
    glEnd();


    //OBJ7
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(1, 0.2); // B2
    glVertex2f(0.4, 0.2); // C2
    glVertex2f(0.4, 0.0); // V2
    glVertex2f(0.8, 0.0); // Z2
    glVertex2f(0.8, -0.2); // D2
    glVertex2f(0.6, -0.2); // E2
    glVertex2f(0.6, -0.4); // F2
    glVertex2f(0.8, -0.4); // G2
    glVertex2f(0.8, -0.6); // H2
    glVertex2f(0.6, -0.6); // I2
    glVertex2f(0.6, -0.8); // J2
    glVertex2f(1.0, -0.8); // K2
    glEnd();

    //obj7 Filling
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(1, 0.2); // B2
    glVertex2f(1, 0); // W2
    glVertex2f(0.4, 0.0); // V2
    glVertex2f(0.4, 0.2); // C2
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.8, 0.0); // Z2
    glVertex2f(1.0, 0.0); // Z2
    glVertex2f(1.0, -0.8); // K2
    glVertex2f(0.8, -0.8); // Mid k
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.8, -0.8); // Mid k
    glVertex2f(0.6, -0.8); // J2
    glVertex2f(0.6, -0.6); // I2
    glVertex2f(0.8, -0.6); // H2
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.8, -0.4); // G2
    glVertex2f(0.6, -0.4); // F2
    glVertex2f(0.6, -0.2); // E2
    glVertex2f(0.8, -0.2); // D2
    glEnd();

    //OBJ8
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-1, -0.6);//E1
    glVertex2f(-0.6, -0.6);//F1
    glVertex2f(-0.6, -0.7);//G1
    glVertex2f(-1, -0.7);//C
    glEnd();

    //Filling
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-1, -0.6);//E1
    glVertex2f(-0.6, -0.6);//F1
    glVertex2f(-0.6, -0.7);//G1
    glVertex2f(-1, -0.7);//C
    glEnd();
}
// 2nd stage: Layal 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// fililng 
// // --- The Map Borders ---

void mapBorderEp2() {
        //OBJ1
        // OBJ1 Filling
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-1.0, 0.0);  // W
        glVertex2f(-0.6, 0.0);  // Z
        glVertex2f(-0.6, 0.2);  // U1
        glVertex2f(-1.0, 0.2);  // Mid-fill
        glEnd();

        glLineWidth(5.0f);
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-1.0, 0.0);  // W
        glVertex2f(-0.6, 0.0);  // Z
        glVertex2f(-0.6, 0.2);  // U1
        glVertex2f(-0.8, 0.2);  // N
        glEnd();


        //OBJ2
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-0.8, 0.4);  // M
        glVertex2f(-0.8, 0.2);  // N
        glEnd();

        // OBJ2 Filling
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-0.6, 0.8);  // F
        glVertex2f(-0.4, 0.8);  // G
        glVertex2f(-0.4, 0.6);  // P
        glVertex2f(-0.6, 0.6);  // I
        glEnd();

        glColor3f(0.255f, 0.412f, 0.882f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(-0.6, 0.8);  // F
        glVertex2f(-0.6, 0.6);  // I
        glVertex2f(-0.4, 0.6);  // P
        glVertex2f(-0.4, 0.8);  // G
        glVertex2f(-0.6, 0.8);  // F
        glEnd();

        // OBJ3 Filling 
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-1.0, 0.8);  // J
        glVertex2f(-0.8, 0.8);  // K
        glVertex2f(-0.8, 0.2);  // n rather m
        glVertex2f(-1.0, 0.2);  // Fill Boundary
        glEnd();

        // OBJ3
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-1.0, 0.8);  // J
        glVertex2f(-0.8, 0.8);  // K
        glVertex2f(-0.8, 0.4);  // M
        glEnd();


        // OBJ4 Filling
        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-0.4, 1.0);  // H
        glVertex2f(-0.4, 0.8);  // G
        glVertex2f(1.0, 0.8);   // D1 foeward
        glVertex2f(1.0, 1.0);   // B
        glEnd();

        // OBJ4
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-0.4, 1.0);  // H
        glVertex2f(-0.4, 0.8);  // G
        glVertex2f(-0.2, 0.8);  // A1
        glVertex2f(0.0, 0.8);   // after A1 before D1
        glEnd();


        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0.0, 0.8); //betw A1 & D1
        glVertex2f(0.4, 0.8); //D1
        glVertex2f(0.4, 0.4); //F1
        glVertex2f(0.0, 0.4); //R
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0.4, 0.8); //D1
        glVertex2f(1.0, 0.8); //After right D1
        glVertex2f(1.0, 0.6); //I1
        glVertex2f(0.4, 0.6); //E1
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0.0, 0.2);   // M1
        glVertex2f(1.0, 0.2); //D3
        glVertex2f(1.0, 0.0);   // g
        glVertex2f(0.0, 0.0); //E
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0.6, 0.4);   // A3
        glVertex2f(0.8, 0.4);   // B3
        glVertex2f(0.8, 0.2);   // C3
        glVertex2f(0.6, 0.2);   // K1
        glEnd();


        // OBJ5
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(0.0, 0.8);   // New start to close the rectangel
        glVertex2f(0.0, 0.6);   // Q
        glVertex2f(0.0, 0.4);   // R
        glVertex2f(0.4, 0.4);   // F1
        glVertex2f(0.4, 0.6);   // E1
        glVertex2f(1.0, 0.6);   // I1
        glVertex2f(1.0, 0.2);   // D3
        glVertex2f(0.8, 0.2);   // C3
        glVertex2f(0.8, 0.4);   // B3
        glVertex2f(0.6, 0.4);   // A3
        glVertex2f(0.6, 0.2);   // K1
        glVertex2f(0.0, 0.2);   // M1
        glVertex2f(0.0, 0.0);   // E

        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0.6, 0.0);   // j3
        glVertex2f(0.6, -0.6);   // g1
        glVertex2f(1.0, -0.6);   // G3
        glVertex2f(1.0, 0.0);   // g


        glEnd();

        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(0.0, 0.0);   // E
        glVertex2f(0.6, 0.0);   // j3
        glVertex2f(0.6, -0.6);   // g1
        glVertex2f(1.0, -0.6);   // G3

        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-0.4, -0.1); // P1 up a bit in Y at -0.1
        glVertex2f(0.2, -0.1);  // U2 same as P1
        glVertex2f(0.2, -0.4);  // T2
        glVertex2f(-0.4, -0.4); // Q1
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-0.4, -0.4); // Q1
        glVertex2f(-0.4, -0.6); // P2
        glVertex2f(-0.2, -0.6); // Q2
        glVertex2f(-0.2, -0.4); // R2
        glEnd();


        // OBJ6
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-0.4, -0.1); // P1 up a bit in Y at -0.1
        glVertex2f(-0.2, -0.1); // O1 same as P1
        glVertex2f(0.2, -0.1);  // U2 same as P1
        glVertex2f(0.2, -0.4);  // T2
        glVertex2f(-0.2, -0.4); // R2
        glVertex2f(-0.2, -0.6); // Q2
        glVertex2f(-0.4, -0.6); // P2
        glVertex2f(-0.4, -0.4); // Q1
        glVertex2f(-0.4, -0.1); // P1 ap a bit to -0.1
        glEnd();


        // OBJ7
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(0.2, -0.4);  // T2
        glVertex2f(0.2, -0.8);  // K2
        glVertex2f(1.0, -0.8);  // O2
        glVertex2f(1.0, -0.6);  // G3
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-0.8, -0.2); // W1
        glVertex2f(-0.6, -0.2); // W1 next
        glVertex2f(-0.6, -1.0); // Fill
        glVertex2f(-0.8, -1.0); // C2
        glEnd();

        // OBJ8
        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-0.8, -0.2); // W1
        glVertex2f(-0.6, -0.2); // W1 next
        glVertex2f(-0.6, -1.0); // Fill
        glVertex2f(-0.6, -0.8); // T1
        glVertex2f(-0.6, -0.4); // R1
        glEnd();

        glBegin(GL_LINE_STRIP);
        glColor3f(0.255f, 0.412f, 0.882f);
        glVertex2f(-0.8, -0.2); // W1
        glVertex2f(-0.8, -1.0); // C2
        glEnd();
    }
//////////////////////////////////////////////////////////////////////////////////// end 2nd layal

void mapBorderEp4() {
    //GHALA Map
    glBegin(GL_LINE_STRIP);
    glColor3f(0.46f, 0.0f, 0.46f);

    glVertex2f(-1.0, 1.0); // A
    glVertex2f(1.0, 1.0); // b
    glVertex2f(1.0, -1.0); // c
    glVertex2f(-1.0, -1.0); // D
    glEnd();

    //MAIN BORDER 
    glBegin(GL_LINE_STRIP);

    glVertex2f(-0.8, -0.8); // F2
    glVertex2f(-0.4, -0.8); // G2
    glVertex2f(-0.4, -1.0); // H2
    glEnd();

    glBegin(GL_LINE_STRIP);

    glVertex2f(-0.2, -0.8); // I2
    glVertex2f(0.0, -0.8); // J2
    glVertex2f(0.0, -1.0); // K2   
    glEnd();

    //---------------------------------
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.2, -0.8); // S1
    glVertex2f(0.8, -0.8); // T1
    glVertex2f(0.8, -0.2); // O1
    glVertex2f(0.4, -0.2); // N1
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(0.4, -0.4); // Q1
    glVertex2f(0.8, -0.4); // O2
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8, -0.6); // N2
    glVertex2f(1.0, -0.6); // U1
    glEnd();

    //------------------------------------
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.6, -0.6); // V1
    glVertex2f(-0.4, -0.6); // W1
    glVertex2f(-0.4, -0.4); // Z1
    glEnd();

    //-----------------------------------

    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.8, -0.6); // A2
    glVertex2f(-0.6, -0.6); // B2
    glVertex2f(-0.6, -0.4); // C2
    glVertex2f(-0.8, -0.4); // D2
    glVertex2f(-0.8, -0.2); // E2
    glEnd();

    //--------------------------------

    // M1-L1-K1-J (Connecting through the center-right corridor)
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.2, -0.4); // M1
    glVertex2f(0.2, -0.4);  // L1
    glVertex2f(0.2, 0.0);   // K1
    glVertex2f(1.0, 0.0);   // J
    glEnd();

    //---------------------------------------
    // V-U-T-S (Lower-left center path)
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, -0.2);  // V
    glVertex2f(-0.6, -0.2); // U
    glVertex2f(-0.6, 0.0);  // T
    glVertex2f(-1.0, 0.0);  // S
    glEnd();

    //---------------------------------------
    // H1-I1 (Horizontal segment near center)
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.4, 0.0);  // H1
    glVertex2f(0.0, 0.0);   // I1
    glEnd();

    //---------------------------------------
    // L2-M2 (Small horizontal segment)
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.8, 0.2);  // L2
    glVertex2f(-0.6, 0.2);  // M2
    glEnd();

    //---------------------------------------
    // G1-R-Q-P (Middle-left branch)
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.2, 0.2);   // G1
    glVertex2f(-0.4, 0.2);  // R
    glVertex2f(-0.4, 0.4);  // Q
    glVertex2f(-1.0, 0.4);  // P
    glEnd();

    //---------------------------------------
    // O-N-M-L-K (Central-top horizontal and vertical)
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, 0.2);   // O
    glVertex2f(0.0, 0.4);   // N
    glVertex2f(-0.2, 0.4);  // M
    glVertex2f(-0.2, 0.6);  // L
    glVertex2f(-0.8, 0.6);  // K
    glEnd();

    //---------------------------------------
    // C1-D1-E1-F1 (Middle-right zigzag)
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.2, 0.4);   // C1
    glVertex2f(0.4, 0.4);   // D1
    glVertex2f(0.4, 0.2);   // E1
    glVertex2f(1.0, 0.2);   // F1
    glEnd();

    //---------------------------------------
    // B1-A1-Z-W (Top-right segment)
    glBegin(GL_LINE_STRIP);
    glVertex2f(1.0, 0.4);   // B1
    glVertex2f(0.8, 0.4);   // A1
    glVertex2f(0.8, 0.6);   // Z
    glVertex2f(0.2, 0.6);   // W
    glEnd();

    //---------------------------------------
    // H-J (Upper horizontal line)
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, 0.8);   // H
    glVertex2f(1.0, 0.8);   // J
    glEnd();

    //---------------------------------------
    // E-F-G (Top-left vertical L-shape)
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.8, 0.8);  // E
    glVertex2f(-0.2, 0.8);  // F
    glVertex2f(-0.2, 1.0);  // G
    glEnd();
}

void background_image() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture1);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, 1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1, 1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void background_SartMenu() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture2);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, 1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1, 1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void Two_background(){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, page2);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, 1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1, 1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void Three_background() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, page3);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, 1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1, 1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void endPagee() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, endPage);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1, 1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1, 1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void drawGhost() {
    glPushMatrix();
    glTranslatef(ghostX, ghostY, 0.0f);
    glScalef(ghostScale, ghostScale, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(0, 0, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-0.5f, 0.0f); glVertex2f(0.5f, 0.0f);
    glVertex2f(0.5f, -0.6f); glVertex2f(-0.5f, -0.6f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(-0.2f, 0.1f, 0.06f);
    drawFilledCircle(0.2f, 0.1f, 0.06f);
    glPopMatrix();
}
void winningPoint() {
    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0f, 0.0f, 0.4f);//ss
    glVertex2f(0.9, 0.8);
    glColor4f(0.255f, 0.412f, 0.882f, 0.4f);//ll
    glVertex2f(0.9, 0.6);
    glColor4f(0.54f, 0.54f, 0.54f, 0.4f);//ss2
    glVertex2f(1.0, 0.6);
    glColor4f(0.255f, 0.0f, 0.682f, 0.4f);//shsh
    glVertex2f(1.0, 0.8);
    glEnd();

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameState == 0) {
        background_SartMenu();
    }
    else if (gameState == 1) {
        // EP1
        background_image();
        mapBorderEp1();
        winningPoint();
        drawGhost();
    }
    else if (gameState == 2) {
        Two_background();
    }
 
    else if (gameState == 3) {
        background_image();
        mapBorderEp2();

        glPushMatrix();
        glTranslatef(0.0, -0.8, 0.0);
        glTranslatef(0.0, -0.8, 0.0);
        winningPoint();
        glPopMatrix();
        
        drawGhost();
    }
    else if (gameState == 4) {
        Three_background();
    }

    else if (gameState == 5) {
        background_image();
        mapBorderEp4();

        glPushMatrix();
        glTranslatef(0.0, -0.8, 0.0);
        glTranslatef(0.0, -0.6, 0.0);
        winningPoint();
        glPopMatrix();

        drawGhost();
    }
    else if (gameState == 6) {
        endPagee();
    }

    glutSwapBuffers();
}
void init() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    myTexture1 = LoadTexture(image1Path, 264, 264);
    myTexture2 = LoadTexture(image2Path, 576, 324);
    page2 = LoadTexture(page2Path, 196, 196);
    page3 = LoadTexture(page3Path, 196, 196);
    endPage = LoadTexture(EndPagePath, 196, 196);
 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_width, win_hight);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("StarryBits");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutSpecialFunc(handleSpecialKeys);
    glutMainLoop();
    return 0;
}