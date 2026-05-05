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

void drawWindowViewBack(float warpFactor) {
    glColor4f(0.82f, 0.90f, 1.0f, 0.06f); // very transparent
    glLineWidth(1.0f);

    for (const auto& star : stars) {
        // limit reflection to face/window center region
        if (star.positionX > -0.75f && star.positionX < 0.75f &&
            star.positionY > -0.75f && star.positionY < 0.75f) {

            // shorter reflection streak than the real star streak
            float len = star.movementSpeed * 0.006f * warpFactor;

            // scale down and shift reflection so it feels like a glass reflection,
            // not the real star position
            float rx = star.positionX * 0.35f + 0.02f;
            float ry = star.positionY * 0.35f + 0.05f;

            glBegin(GL_LINES);
                glVertex2f(rx, ry);
                glVertex2f(rx + len, ry);
            glEnd();
        }
    }
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


// STARFIELD EFFECT -> zooming through the space in hyperspeed.
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

void drawGlassOverlay() {
    glColor4f(0.70f, 0.85f, 1.0f, 0.05f);

    glBegin(GL_QUADS);
        glVertex2f(-1.0f,  1.0f);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
    glEnd();
}

// Draw the alien from the back, showing its torso, head, and fins. Add a subtle breathing motion, same method as the bulb  
void drawAlien() {
    float breathe = sin(globalTime * 1.6f) * 0.0025f;

    glPushMatrix();
    glTranslatef(-0.03f, -0.58f + breathe, 0.0f); // much lower

    // seat hint / floor contact
    glColor3f(0.55f, 0.60f, 0.70f);
    drawRect(-0.24f, -0.18f, 0.24f, -0.12f);

    // torso
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
    //bottom point
    glVertex2f(-0.08f, -0.10f);
    glVertex2f( 0.08f, -0.10f);

    //top point 
    glVertex2f( 0.10f,  0.18f);
    glVertex2f(-0.10f,  0.18f);
    glEnd();


    //legs
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.08f, -0.22f, 0.08f, -0.06f);
    
    //leg jeans
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.08f, -0.18f, 0.08f,-0.06f);

    //leg pants outline
    glColor3f(0.13f, 0.18f, 0.28f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.08f, -0.10f);
    glVertex2f( 0.08f, -0.10f);
    glEnd();


    // neck
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.04f, 0.38f, 0.04f, 0.18f);

    // head
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.35f, 0.11f, 50);

    //mouth
    glColor3f(0.0f, 0.40f, 0.05f);
    glLineWidth(2.5f);

    float mouthX = 0.05f;   // positive = move right, negative = move left
    float mouthY = 0.325f;  // positive = move up, negative = move down
    float width  = 0.025f;  // smaller than 0.06, so mouth is smaller
    float curve  = 8.0f;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++) {
        float x = -width + i * ((2.0f * width) / 30.0f);
        float y = mouthY - curve * x * x;

        glVertex2f(x + mouthX, y);
    }
    glEnd();

    // eyes
    glColor3f(1.00f, 1.00f, 1.00f);
    drawCircle( 0.04f, 0.38f, 0.027f, 20);

    // eye ball highlight
    glColor3f(0.05f, 0.05f, 0.05f);
    drawCircle( 0.045f, 0.385f, 0.013f, 20);

    // head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.35f);
    drawCircle(-0.03f, 0.38f, 0.08f, 40);

    // alien fins
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.09f, 0.39f); glVertex2f(-0.17f, 0.43f); glVertex2f(-0.11f, 0.31f);
    glEnd();

   

    // side arm
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.03f, -0.02f, 0.03f, 0.15f);

    //side arm shoulder sleeve
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.03f, 0.08f, 0.03f, 0.15f);

    /*
     // side arm outline
    glColor3f(0.01f, 0.01f, 0.01f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.03f, -0.02f);
    glVertex2f( 0.03f, -0.02f);
    glVertex2f( 0.03f,  0.15f);
    glVertex2f(-0.03f,  0.15f);
    glEnd();
    */
   

    glPopMatrix();
}

void drawAlienShooter(float currentTime) {
    float recoilAmount = 0.012f * sin(currentTime * 12.0f);
    float muzzleFlashStrength = fabs(sin(currentTime * 18.0f));

    glPushMatrix();

    // Smaller size and slightly adjusted position
    glTranslatef(-0.35f, -0.20f + recoilAmount, 0.0f);
    glScalef(0.78f, 0.78f, 1.0f);


    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.08f, -0.28f, -0.02f, -0.10f); // left leg
    drawRect( 0.02f, -0.28f,  0.08f, -0.10f); // right leg

    // pants / jeans
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.09f, -0.16f, 0.09f, -0.08f);

    // pants divider
    glColor3f(0.13f, 0.18f, 0.28f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(-0.09f, -0.08f);
        glVertex2f( 0.09f, -0.08f);
    glEnd();

  
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.10f, -0.10f);
        glVertex2f( 0.10f, -0.10f);
        glVertex2f( 0.13f,  0.16f);
        glVertex2f(-0.09f,  0.16f);
    glEnd();


    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.035f, 0.14f, 0.035f, 0.22f);


    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.31f, 0.11f, 50);

    // head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.35f);
    drawCircle(-0.03f, 0.34f, 0.08f, 40);

    // alien fins
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.08f, 0.35f);
        glVertex2f(-0.16f, 0.40f);
        glVertex2f(-0.10f, 0.27f);

        glVertex2f(0.08f, 0.35f);
        glVertex2f(0.16f, 0.40f);
        glVertex2f(0.10f, 0.27f);
    glEnd();


    // eyes
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-0.035f, 0.33f, 0.022f, 20);
    drawCircle( 0.045f, 0.33f, 0.022f, 20);

    // pupils looking to the right, toward the enemy
    glColor3f(0.05f, 0.05f, 0.05f);
    drawCircle(-0.026f, 0.33f, 0.010f, 12);
    drawCircle( 0.054f, 0.33f, 0.010f, 12);

    // angry eyebrows
    glColor3f(0.05f, 0.25f, 0.08f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-0.060f, 0.365f);
        glVertex2f(-0.015f, 0.350f);

        glVertex2f(0.020f, 0.350f);
        glVertex2f(0.070f, 0.365f);
    glEnd();

    // determined mouth
    glColor3f(0.0f, 0.40f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-0.025f, 0.275f);
        glVertex2f( 0.035f, 0.275f);
    glEnd();

    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.14f, -0.03f, -0.09f, 0.12f);

    // left sleeve
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.14f, 0.06f, -0.09f, 0.12f);

 
    // upper arm / sleeve
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(0.08f, 0.04f, 0.17f, 0.10f);

    // forearm extended forward
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(0.15f, 0.02f, 0.28f, 0.08f);

    // hand
    glColor3f(0.20f, 0.65f, 0.22f);
    drawCircle(0.285f, 0.05f, 0.025f, 16);

    // gun body
    glColor3f(0.55f, 0.58f, 0.65f);
    drawRect(0.26f, 0.01f, 0.42f, 0.09f);

    // gun barrel
    glColor3f(0.35f, 0.38f, 0.45f);
    drawRect(0.40f, 0.035f, 0.52f, 0.065f);

    // top detail
    glColor3f(0.25f, 0.30f, 0.38f);
    drawRect(0.32f, 0.09f, 0.43f, 0.12f);

    // handle
    glColor3f(0.30f, 0.30f, 0.34f);
    drawRect(0.30f, -0.07f, 0.35f, 0.01f);

    // trigger guard
    glColor3f(0.12f, 0.13f, 0.15f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.32f, 0.005f);
        glVertex2f(0.38f, 0.005f);
        glVertex2f(0.38f, -0.035f);
        glVertex2f(0.32f, -0.035f);
    glEnd();

    // muzzle flash
    if (muzzleFlashStrength > 0.55f) {
        glColor4f(1.0f, 0.85f, 0.20f, 0.85f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0.52f, 0.05f);
            glVertex2f(0.64f, 0.10f);
            glVertex2f(0.64f, 0.00f);
        glEnd();

        glColor4f(1.0f, 0.35f, 0.10f, 0.55f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0.52f, 0.05f);
            glVertex2f(0.60f, 0.075f);
            glVertex2f(0.60f, 0.025f);
        glEnd();
    }

    glPopMatrix();
}


void drawAlienFaceScene(float t) {
    //so that the starfield effect still works.
    float warp = 1.2f;
    updateStars(1.0f / FRAMES_PER_SECOND, warp);
    updatePlanets(1.0f / FRAMES_PER_SECOND, warp);




    //rectangle that fills the whole screen, act as background.
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.06f, 0.10f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.02f, 0.03f, 0.05f);
    glVertex2f( 1,-1);
    glVertex2f(-1,-1);
    glEnd();

    //Window View from face perspective
    drawPlanets();
    drawStars(true, warp);
    drawGlassOverlay();
    drawWindowViewBack(1.5f);



    
    float blink = 0.0006f * sin(t * 2.2f);// eyes blink with a sine wave, very subtle

    // neck - moved lower so it connects under the head
    glColor3f(0.20f, 0.60f, 0.22f);
    drawRect(-0.10f, -0.38f, 0.10f, -0.24f);

    // left sleeve (upper arm / bicep area)
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(-0.30f, -0.55f, -0.20f, -0.38f);

    // left arm
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(-0.30f, -0.98f, -0.20f, -0.55f);

    // right sleeve (upper arm / bicep area)
    glColor3f(0.18f, 0.24f, 0.35f);
    drawRect(0.20f, -0.55f, 0.30f, -0.38f);

    // right arm
    glColor3f(0.22f, 0.75f, 0.25f);
    drawRect(0.20f, -0.98f, 0.30f, -0.55f);

    // body - moved lower
    glColor3f(0.18f, 0.24f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.20f, -0.38f);
        glVertex2f( 0.20f, -0.38f);
        glVertex2f( 0.20f, -1.00f);
        glVertex2f(-0.20f, -1.00f);
    glEnd();


    //Alien head shape 
    glColor3f(0.22f, 0.75f, 0.25f);
    drawCircle(0.0f, 0.05f, 0.40f, 80);

    //Head shading
    glColor4f(0.12f, 0.45f, 0.15f, 0.30f);
    drawCircle(-0.08f, 0.12f, 0.30f, 60);

    //Alien Ears
    glColor3f(0.15f, 0.62f, 0.18f);
    glBegin(GL_TRIANGLES); // Every group of three vertices forms a triangle, so we can draw the ears with just two triangles. 
        //Left Ear
        glVertex2f(-0.28f, 0.22f); glVertex2f(-0.43f, 0.34f); glVertex2f(-0.30f, 0.05f);
        //Right Ear
        glVertex2f( 0.28f, 0.22f); glVertex2f( 0.43f, 0.34f); glVertex2f( 0.30f, 0.05f);
    glEnd();


    //White part of eyeball + blinking animation 
    glColor3f(0.92f, 0.94f, 0.97f);
    drawCircle(-0.13f, 0.12f + blink, 0.07f, 30);
    drawCircle( 0.13f, 0.12f - blink, 0.07f, 30);

    

    //pupils
    glColor3f(0.08f, 0.10f, 0.12f);
    drawCircle(-0.11f, 0.12f + blink, 0.03f, 20);
    drawCircle( 0.15f, 0.12f - blink, 0.03f, 20);

    //eye highlights
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-0.12f, 0.14f + blink, 0.01f, 10);
    drawCircle( 0.14f, 0.14f - blink, 0.01f, 10);

    //nose 
    glColor3f(0.10f, 0.55f, 0.15f);
    drawRect(-0.025f, -0.02f, 0.025f, 0.10f);

    
    // nose
    glColor3f(0.10f, 0.55f, 0.15f);
    drawRect(-0.025f, -0.02f, 0.025f, 0.10f);

    


    float smile = smoothstep(0.0f, 3.5f, t);// smile grows from 0 to 3.5 as t goes from 0 to 1 
    //SMILE (SAD ALSO )ANIMATIONNNN
    glLineWidth(3.0f);
    glColor3f(0.10f, 0.35f, 0.10f);

    glBegin(GL_LINE_STRIP); // this draws a continuous connected line through all vertices in order, basically if many points, it makes one connected curve
    for (int i = 0; i <= 40; i++) {
        float u = -0.14f + i * (0.28f / 40.0f);
        float normalized = u / 0.14f;               // -1 to 1
        float curve = normalized * normalized;      // 0 center, 1 edges

        //change here to change the smile curve
        float v = -0.17f + smile * (0.010f-0.045f * curve);
        glVertex2f(u, v);
    }
    glEnd();
 
}


// Animation for the first 14 seconds, where the alien is travelling through space. All Space effect are implemented.
void drawSceneWindowFast() {
    float warp = 2.0f; // control how fast the starfield and planets move, higher => faster
    updateStars(1.0f / FRAMES_PER_SECOND, warp);
    updatePlanets(3.7f / FRAMES_PER_SECOND, warp); // 'f' here adjust the speed of planet showing.

    drawSpaceBackgroundGradient();

    drawPlanets();
    drawStars(true, warp);

    //subtle window glow
    glColor4f(0.7f, 0.85f, 1.0f, 0.04f);
    drawRect(-0.78f, -0.78f, 0.78f, 0.78f);

    drawWindowFrameAndInterior();
    drawAlien();
}


void drawSciFiFlashbackBattle(float t) {
    // sky
    glBegin(GL_QUADS);
    glColor3f(0.03f, 0.02f, 0.08f);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glColor3f(0.10f, 0.03f, 0.10f);
    glVertex2f( 1,-1);
    glVertex2f(-1,-1);
    glEnd();

     // horizon glow
    glColor4f(0.55f, 0.15f, 0.55f, 0.22f);
    drawRect(-1.0f, -0.24f, 1.0f, -0.18f);

    // STRAIGHT GROUND
    glColor3f(0.18f, 0.07f, 0.16f);
    drawRect(-1.0f, -1.0f, 1.0f, -0.24f);

    // distant city skyline - now aligned to straight ground
    for (int i = 0; i < 12; i++) {
        float x = -1.0f + i * 0.18f;
        float h = 0.12f + 0.18f * ((i % 4) / 3.0f);

        glColor3f(0.14f, 0.18f, 0.26f);
        drawRect(x, -0.24f, x + 0.10f, -0.24f + h);

        glColor3f(0.0f, 0.75f, 0.95f);
        for (int w = 0; w < 3; w++) {
            float wx = x + 0.02f + w * 0.025f;
            drawRect(wx, -0.18f, wx + 0.01f, -0.15f + 0.02f * (w % 2));
        }
    }


    // foreground towers - also stable
    for (int i = 0; i < 6; i++) {
        float x = -0.9f + i * 0.34f;
        float h = 0.30f + 0.08f * (i % 3);

        glColor3f(0.22f, 0.32f, 0.45f);
        drawRect(x, -0.25f, x + 0.08f, h);

        glColor3f(0.0f, 0.9f, 1.0f);
        drawRect(x + 0.02f, 0.0f, x + 0.04f, h - 0.05f);

        // antenna or side detail
        glColor3f(0.6f, 0.7f, 0.85f);
        drawRect(x + 0.055f, h - 0.02f, x + 0.065f, h + 0.07f);
    }

    // little flying traffic to sell "city"
    for (int i = 0; i < 5; i++) {
        float px = -1.2f + fmod(t * (0.18f + 0.03f * i) + i * 0.3f, 2.6f);
        float py = 0.15f + 0.08f * sin(t * 1.2f + i);

        glColor3f(1.0f, 0.55f, 0.2f);
        drawRect(px, py, px + 0.04f, py + 0.01f);

        glColor4f(1.0f, 0.55f, 0.2f, 0.35f);
        drawRect(px - 0.03f, py + 0.002f, px, py + 0.008f);
    }

    // alien shooter
    drawAlienShooter(t);
    

    // lasers
    for (int i = 0; i < 4; i++) {
        float phase = fmod(t * 0.9f + i * 0.2f, 1.2f);
        float x1 = -0.08f + phase * 0.75f;
        float y1 = -0.14f + 0.015f * sin(t * 6.0f + i);

        glLineWidth(3.0f);
        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x1 + 0.16f, y1 + 0.035f);
        glEnd();
    }

    // enemy drones
    for (int i = 0; i < 3; i++) {
        float x = 0.45f + i * 0.18f;
        float y = 0.25f + 0.06f * sin(t * 1.5f + i * 0.9f);

        glColor3f(0.85f, 0.15f, 0.25f);
        drawCircle(x, y, 0.035f, 24);

        glColor3f(1.0f, 0.85f, 0.2f);
        drawCircle(x + 0.01f, y, 0.008f, 12);
    }
}


std::string currentLine() {
    
     if (globalTime < 4.0f)
        return "I kept chasing the light ahead of me.";

    if (globalTime < 8.0f)
        return "For duty. For glory. For the ones counting on me.";

    if (globalTime < 12.5f)
        return "But every star outside the window pulled me backward.";

    if (globalTime < 16.5f)
        return "Back to the night I thought courage meant fighting.";

    if (globalTime < 21.0f)
        return "I fired until the sky broke into sparks... and still, I wondered what I had won.";


    return "";

    
}

//helper functin for better script pacing
float currentLineStartTime() {
    if (globalTime < 4.0f) return 0.0f;
    if (globalTime < 8.0f) return 4.0f;
    if (globalTime < 12.5f) return 8.0f;
    if (globalTime < 16.5f) return 12.5f;
    if (globalTime < 21.0f) return 16.5f;
    
    
    return 21.0f;
}



void drawChatBox() {
     std::string line = currentLine();
    if (line.empty()) return;

    float segmentStart = currentLineStartTime();

    int visible = std::min(
        (int)line.size(),
        std::max(0, (int)((globalTime - segmentStart) * 35.0f))
    );

    std::string shown = line.substr(0, visible);

    // subtitle background box
    glColor4f(0.0f, 0.0f, 0.0f, 0.68f);
    drawRect(-0.90f, -0.95f, 0.90f, -0.76f);

    // small glowing top border
    glColor4f(0.8f, 0.9f, 1.0f, 0.22f);
    drawRect(-0.90f, -0.76f, 0.90f, -0.745f);

    // speaker name / label
    glColor3f(0.55f, 0.85f, 1.0f);
    drawText(-0.84f, -0.81f, GLUT_BITMAP_HELVETICA_12, "ALIEN MEMORY LOG");

    // actual subtitle text
    glColor3f(0.95f, 0.97f, 1.0f);
    drawText(-0.84f, -0.89f, GLUT_BITMAP_HELVETICA_18, shown);
}



void display() {

    glClear(GL_COLOR_BUFFER_BIT); // clear the screen to the background color defined in initGL
    glLoadIdentity(); // reset transformations, start fresh for this frame

    if (globalTime < 7.0f) {
        drawSceneWindowFast();
    } else if (globalTime < 14.0f) {
        drawAlienFaceScene(globalTime - 7.0f);
    } else if (globalTime < 21.0f) {
        drawSciFiFlashbackBattle(globalTime - 14.0f);
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.05f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "FIN");
    }
     
    drawChatBox();

    glutSwapBuffers();

}




void initGL() {
    glClearColor(0.01f, 0.01f, 0.03f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    srand((unsigned int)time(nullptr));
    initializeStars();
    initializePlanets();
}

void timer(int) {
    globalTime += 1.0f / FRAMES_PER_SECOND;
    glutPostRedisplay();
    glutTimerFunc((unsigned int)(1000.0f / FRAMES_PER_SECOND), timer, 0);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("The Final Coordinate - Revised");

    glutFullScreen();

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}












