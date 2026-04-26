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


// Create Text
void drawText(float x, float y, void* font, const std:: string& s) {
    glRasterPos2f(x,y);
    for(char c: s) glutBitMapCharacter(font,c); 
}

// Draw random stars
void initStars() {
    stars.clear();
    stars.reserve(300);
    for(int i = 0; i<300; i++) {
        stars.pushback({
            frand(-1.4f, 1.4f), frand(-1.0f, 1.0f), frand(0.8f, 3.0f), frand(1.0f, 3.0f)
        });
    }
}

void initPlanet() {
    planets.clear();
    for(int i = 0; i < 4; i++) {
        Planet p;
        p.x = frand(1.5f, 3.5f) + i * 1.6f;
        p.y = frand(-0.4f, 0.5f);
        p.r = frand(0.08f, 0.16f);
        p.speed = frand(0.08f, 0.18f);
        p.cr = frand(0.2f, 0.8f);
        p.cg = frand(0.2f, 0.8f);
        p.cb = frand(0.3f, 0.9f);
        planets.push_back(p);

    }
}


// STARFFIELD EFFECT -> zooming through the space in hyperspeed.
void updateStars(float deltaTime, float warpFactor) {
    for (auto& currentStar : starCollection) {
        // Move the star to the left based on its individual speed, the time elapsed, and the warp multiplier
        currentStar.xPosition -= currentStar.movementSpeed * deltaTime * warpFactor;

        // if moved pass the left boundary
        if (currentStar.xPosition < -1.5f) {
            // Reset the star to the right
            currentStar.xPosition = 1.5f;

            // Randomize the vertical position, speed(maintaining parallax depft), size of stars
            currentStar.yPosition = getRandomFloat(-1.0f, 1.0f);
            currentStar.movementSpeed = getRandomFloat(1.2f, 4.2f);
            currentStar.visualSize = getRandomFloat(1.0f, 3.0f);
        }
    }
}



void updatePlanets(float deltaTime, float warpMultiplier) {
    for (auto& planet : planets) {
        // Move the planet to the left based on its speed and time elapsed
        planet.positionX -= planet.speed * deltaTime * warpMultiplier * 0.75f;

        // Check if the planet has moved off the left side of the screen
        if (planet.positionX < -1.8f) {
            // Respawn the planet on the right with new random attributes
            planet.positionX = getRandomFloat(1.4f, 4.8f);
            planet.positionY = getRandomFloat(-0.50f, 0.58f);
            planet.radius    = getRandomFloat(0.06f, 0.18f);
            planet.speed     = getRandomFloat(0.10f, 0.24f);
            
            // Randomize the Planet Colors (Red, Green, Blue)
            planet.colorRed   = getRandomFloat(0.25f, 0.95f);
            planet.colorGreen = getRandomFloat(0.20f, 0.80f);
            planet.colorBlue  = getRandomFloat(0.30f, 0.95f);
        }
    }

}










