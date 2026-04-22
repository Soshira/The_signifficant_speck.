#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int W = 1280;
const int H = 720;
const float FPS = 60.0f;
const float TOTAL_TIME = 60.0f

//global animation time, point of reference/ master clock for whole animation
float gTime = 0.0f;


/* --------------------------   Moving Object That Exist    ----------------------------- */

// typedef struct {.....} Name; -> creates a custom type

//for star
struct Star {
    float x, y, speed, size;
};

//for planet
struct Planet {
    float x, y, r, speed;
    float cr, cg, cb; // color channels red, green, blue
};

//Global Arrays
std::vector<Star> stars;
std::vector<Planet> planets;


/* --------------------------   Helper Function    ----------------------------- */

//for randomizing things such as sizes, position, colors, ect.
float frand(float a, float b) {
    return a + (b-a) * ((float) rand() / (float) RAND_MAX);
}

//to keep a number within a range, useful for smooth animation 
float clampf(float v, float a, float b) {
    if (v < a ) return a;
    if (v > b ) return b;
    return v;
 }

 // creates smooth transition
 float smoothstep(float a, float b, float x) {
    float t = clampf((x-a) / (b-a), 0.0f,1.0);
    return t * t * (3.0f - 2.0f * t);
 }

 /* --------------------------   Drawing Helper Function    ----------------------------- */
 
 //to draw RECTANGLE, using 4 corner vertices (for walls, buildings, subtitle box, ect.)
void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS); //draws quadrilaterals
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
    glEnd();
}

//Draws a FILLED circle, using Triangle Fan
void drawCircle(float centerX, float centerY, float radius, int segment = 60) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, centerY);
        for(int i = 0; i<= segment; i++) {
            float a = 2.0f * 3.1415926f * i / segment;

            glVertex2f(centerX + cosf(a) * radius, centerY + sinf(a) * radius);
        }
    glEnd();
}

//just for circle outline, basically a normal circle
void drawCircleOutline(float centerX, float centerY, float radius, float segment) {
    glBegin(GL_LINE_LOOP);
            for(int i=0; i<= segment; i++) {
                float a = 2.0f * 3.14152926f * i / segment;
                glVertex2f(centerX + cosf(a) * radius, centerY + sinf(a) * radius);
            }
        glEnd();
}
