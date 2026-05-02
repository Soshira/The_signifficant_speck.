#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FRAMES_PER_SECOND = 60.0f;
const float TOTAL_TIME = 60.0f;

// global animation time, point of reference / master clock for whole animation
float globalTime = 0.0f;


/* --------------------------   Moving Object That Exist    ----------------------------- */

// typedef struct {.....} Name; -> creates a custom type

// for star
struct Star {
    float positionX;
    float positionY;
    float movementSpeed;
    float visualSize;
};

// for planet
struct Planet {
    float positionX;
    float positionY;
    float radius;
    float movementSpeed;
    float colorRed;
    float colorGreen;
    float colorBlue;
};

//Global Arrays
std::vector<Star> stars;
std::vector<Planet> planets;


/* --------------------------   Helper Function    ----------------------------- */

// for randomizing things such as sizes, position, colors, etc.
float randomFloat(float minimum, float maximum) {
    return minimum + (maximum - minimum) * ((float) rand() / (float) RAND_MAX);
}

// to keep a number within a range, useful for smooth animation
float clampFloat(float value, float minimum, float maximum) {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

// creates smooth transition
float smoothstep(float minimum, float maximum, float input) {
    float t = clampFloat((input - minimum) / (maximum - minimum), 0.0f, 1.0f);
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
    for(char c: s) glutBitmapCharacter(font,c); 
}

// Draw random stars
void initializeStars() {
    stars.clear();
    stars.reserve(300);
    for(int i = 0; i < 300; i++) {
        stars.push_back({
            randomFloat(-1.4f, 1.4f), randomFloat(-1.0f, 1.0f), randomFloat(0.8f, 3.0f), randomFloat(1.0f, 3.0f)
        });
    }
}

void initializePlanets() {
    planets.clear();
    for(int i = 0; i < 4; i++) {
        Planet planet;
        planet.positionX = randomFloat(1.5f, 3.5f) + i * 1.6f;
        planet.positionY = randomFloat(-0.4f, 0.5f);
        planet.radius = randomFloat(0.08f, 0.16f);
        planet.movementSpeed = randomFloat(0.08f, 0.18f);
        planet.colorRed = randomFloat(0.2f, 0.8f);
        planet.colorGreen = randomFloat(0.2f, 0.8f);
        planet.colorBlue = randomFloat(0.3f, 0.9f);
        planets.push_back(planet);
    }
}


// STARFFIELD EFFECT -> zooming through the space in hyperspeed.
void updateStars(float deltaTime, float warpFactor) {
    for (auto& currentStar : stars) {
        // Move the star to the left based on its individual speed, the time elapsed, and the warp multiplier
        currentStar.positionX -= currentStar.movementSpeed * deltaTime * warpFactor;

        // if moved past the left boundary
        if (currentStar.positionX < -1.5f) {
            // Reset the star to the right
            currentStar.positionX = 1.5f;

            // Randomize the vertical position, speed, and size of stars
            currentStar.positionY = randomFloat(-1.0f, 1.0f);
            currentStar.movementSpeed = randomFloat(1.2f, 4.2f);
            currentStar.visualSize = randomFloat(1.0f, 3.0f);
        }
    }
}


// PLANETARY MOTION -> planets moving across the screen, with random sizes, speeds, and colors. When they move off the left side of the screen, they respawn on the right with new random attributes.
void updatePlanets(float deltaTime, float warpMultiplier) {
    for (auto& planet : planets) {
        // Move the planet to the left based on its speed and time elapsed
        planet.positionX -= planet.movementSpeed * deltaTime * warpMultiplier * 0.75f;

        // Check if the planet has moved off the left side of the screen
        if (planet.positionX < -1.8f) {
            // Respawn the planet on the right with new random attributes
            planet.positionX = randomFloat(1.4f, 4.8f);
            planet.positionY = randomFloat(-0.50f, 0.58f);
            planet.radius = randomFloat(0.06f, 0.18f);
            planet.movementSpeed = randomFloat(0.10f, 0.24f);
            
            // Randomize the Planet Colors (Red, Green, Blue)
            planet.colorRed = randomFloat(0.25f, 0.95f);
            planet.colorGreen = randomFloat(0.20f, 0.80f);
            planet.colorBlue = randomFloat(0.30f, 0.95f);
        }
    }
}

// Draw the space background gradient, transitioning from a deep blue at the top to a darker shade at the bottom, creating a sense of depth and vastness in space.
void drawSpaceBackgroundGradient() {
    glBegin(GL_QUADS);
    glColor3f(0.00f, 0.00f, 0.06f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.03f, 0.02f, 0.13f);
    glVertex2f( 1,-1);
    glVertex2f(-1,-1);
    glEnd();
}

void drawPlanets() {
    // Loop through each planet in the planets vector 
    for (const auto& planet : planets) {

        // Draw the planet itself with its unique color and size 
        glColor3f(planet.colorRed, planet.colorGreen, planet.colorBlue);
        drawCircle(planet.positionX, planet.positionY, planet.radius, 50);

        // Draw the subtle halo effect around the planet 
        glColor4f(1.0f, 1.0f, 1.0f, 0.18f);
        drawCircle(planet.positionX - planet.radius *0.25f, planet.positionY + planet.radius *0.20f, planet.radius * 0.48f, 30);

        // Draw the subtle glow around the planet
        glColor4f(1.0f, 1.0f, 1.0f, 0.12f);
        drawCircleOutline(planet.positionX, planet.positionY, planet.radius * 1.08f, 50);
    }
}

void drawStars(bool streaks, float warpFactor) {
    for (const auto& star : stars) {
        glColor3f(0.88f, 0.92f, 1.0f);

        // Draw the star as a small circle, with size based on its visualSize attribute
        if (streaks) {
            float len = star.movementSpeed * 0.02f * warpFactor; // length of the streak based on speed and warp factor
            glLineWidth(star.visualSize * 0.5f); // line width based on visual size
            glBegin(GL_LINES);
                glVertex2f(star.positionX + len, star.positionY);// start point of the streak (ahead of the star)
                glVertex2f(star.positionX - len, star.positionY);// end point of the streak (behind the star)
            glEnd();
        } else {
            glPointSize(star.visualSize); // point size based on visual size
            glBegin(GL_POINTS);
                glVertex2f(star.positionX, star.positionY); // draw the star as a point at its current position
            glEnd();
    }
}
}

// Draw inside of ship, Window Frame and Interior
void drawWindowFrameAndInterior() {

    // Top Frame 
    glBegin(GL_QUADS);
    glColor3f(0.98f, 0.98f, 1.0f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.82f, 0.84f, 0.90f);
    glVertex2f( 1, 0.78f);
    glVertex2f(-1, 0.78f);
    glEnd();

    //Bottom Frame
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.82f, 0.88f);
    glVertex2f(-1, -0.78f);
    glVertex2f( 1, -0.78f);
    glColor3f(0.68f, 0.72f, 0.80f);
    glVertex2f( 1, -1);
    glVertex2f(-1, -1);
    glEnd();

    //Left Side Frame
    glBegin(GL_QUADS);
    glColor3f(0.92f, 0.93f, 0.97f);
    glVertex2f(-1, 0.78f);
    glVertex2f(-0.78f, 0.78f);
    glColor3f(0.76f, 0.79f, 0.86f);
    glVertex2f(-0.78f, -0.78f);
    glVertex2f(-1, -0.78f);
    glEnd();

    //Right side Frame 
    glBegin(GL_QUADS);
    glColor3f(0.92f, 0.93f, 0.97f);
    glVertex2f(0.78f, 0.78f);
    glVertex2f(1, 0.78f);
    glColor3f(0.76f, 0.79f, 0.86f);
    glVertex2f(1, -0.78f);
    glVertex2f(0.78f, -0.78f);
    glEnd();

    //Inner shadow pieces, draw thin darker strips around the edge of the window frame
    glColor4f(0.60f, 0.64f, 0.72f, 0.55f);
    drawRect(-0.82f,  0.78f, 0.82f,  0.72f);
    drawRect(-0.82f, -0.72f, 0.82f, -0.78f);
    drawRect(-0.82f, -0.78f, -0.72f, 0.78f);
    drawRect( 0.72f, -0.78f,  0.82f, 0.78f);

    // Thick white inner outline, draws a thick white rectangular outline around the glass area.
    glColor3f(0.98f, 0.98f, 1.0f);
    glLineWidth(12.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.78f, -0.78f);
    glVertex2f( 0.78f, -0.78f);
    glVertex2f( 0.78f,  0.78f);
    glVertex2f(-0.78f,  0.78f);
    glEnd();

    //This draws a smaller thin outline inside the glass,adds detail and makes the window look more layered.
    glColor3f(0.64f, 0.69f, 0.78f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.70f, -0.70f);
    glVertex2f( 0.70f, -0.70f);
    glVertex2f( 0.70f,  0.70f);
    glVertex2f(-0.70f,  0.70f);
    glEnd();

    //extra bottom sill
    glColor3f(0.80f, 0.83f, 0.90f);
    drawRect(-1, -1, 1, -0.80f);

    //Hanging string, for the light bulb 
    glColor3f(0.60f, 0.64f, 0.72f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.55f, 0.95f);
    glVertex2f(0.55f, 0.72f);
    glEnd();
    // this is the light bulb, it bobs up and down slightly to add a bit of traveling feeling.
    glColor3f(1.0f, 0.75f, 0.25f);
    drawCircle(0.55f, 0.68f + 0.02f * sin(globalTime * 2.0f), 0.03f, 24); // bobbing effect using sine wave, based on global time 

}



// Draw the alien from the back, showing its torso, head, and fins. Add a subtle breathing motion, same method as the bulb  
void drawAlienBackView() {
    float breathe = sin(globalTime * 1.6f) * 0.0025f;

    glPushMatrix();
    glTranslatef(-0.03f, -0.58f + breathe, 0.0f); // much lower

    // seat hint / floor contact
    glColor3f(0.55f, 0.60f, 0.70f);
    drawRect(-0.24f, -0.18f, 0.24f, -0.12f);

    // torso
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.20f, -0.10f);
    glVertex2f( 0.20f, -0.10f);
    glVertex2f( 0.14f,  0.18f);
    glVertex2f(-0.14f,  0.18f);
    glEnd();

    // shoulder armor
    glColor3f(0.30f, 0.40f, 0.55f);
    drawRect(-0.23f, 0.05f, -0.12f, 0.14f);
    drawRect( 0.12f, 0.05f,  0.23f, 0.14f);

    // neck
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.04f, 0.17f, 0.04f, 0.25f);

    // head
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.35f, 0.11f, 50);

    // head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.35f);
    drawCircle(-0.03f, 0.38f, 0.08f, 40);

    // alien fins
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.09f, 0.39f); glVertex2f(-0.17f, 0.43f); glVertex2f(-0.11f, 0.31f);
    glVertex2f( 0.09f, 0.39f); glVertex2f( 0.17f, 0.43f); glVertex2f( 0.11f, 0.31f);
    glEnd();

    // suit stripe
    glColor3f(0.0f, 0.85f, 0.85f);
    drawRect(-0.01f, -0.02f, 0.01f, 0.15f);

    glPopMatrix();
}












