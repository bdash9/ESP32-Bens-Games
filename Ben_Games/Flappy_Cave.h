#ifndef FLAPPY_CAVE_H
#define FLAPPY_CAVE_H

#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

// ========== FLAPPY_CAVE - CAVE FLYER GAME ==========
// Fly through a dangerous cave filled with stalactites and stalagmites!

namespace FLAPPY_CAVE {

// Game constants (adjusted for 480x320 screen)
const int PERSON_X = 120;          // Person stays at fixed X position
const int PERSON_SIZE = 20;        // Size of person sprite
const float GRAVITY = 0.25;        // Downward pull
const float THRUST = 0.8;          // Upward thrust when holding UP
const float MAX_VELOCITY = 5.0;    // Speed limit
const int CAVE_SPEED = 4;          // How fast obstacles scroll
const int GAP_SIZE = 120;          // Space between stalactites/stalagmites
const int MIN_HEIGHT = 40;         // Minimum obstacle height
const int MAX_HEIGHT = 180;        // Maximum obstacle height

// Game state
float personY = 160;
float velocityY = 0;
int score = 0;
int highScore = 0;
bool gameOver = false;

// Obstacle structure
struct Obstacle {
    int x;
    int topHeight;      // Stalactite height from top
    int bottomHeight;   // Stalagmite height from bottom
    bool passed;
};

const int MAX_OBSTACLES = 6;
Obstacle obstacles[MAX_OBSTACLES];

// ========== VECTOR PERSON DRAWING (CYAN, FROM BEHIND) ==========
void drawPerson(TFT_eSPI &tft, int x, int y, uint16_t color) {
    // Person viewed from behind and slightly above
    // Flying pose with arms spread
    
    // Head (circle)
    tft.drawCircle(x, y - 8, 4, color);
    tft.drawCircle(x, y - 8, 3, color);
    
    // Neck/spine
    tft.drawLine(x, y - 4, x, y + 4, color);
    
    // Shoulders (wide, foreshortened)
    tft.drawLine(x - 6, y - 2, x + 6, y - 2, color);
    
    // Left arm (extended, behind perspective)
    tft.drawLine(x - 6, y - 2, x - 14, y - 4, color);
    tft.drawLine(x - 14, y - 4, x - 16, y + 2, color);
    
    // Right arm (extended, behind perspective)
    tft.drawLine(x + 6, y - 2, x + 14, y - 4, color);
    tft.drawLine(x + 14, y - 4, x + 16, y + 2, color);
    
    // Torso/body (tapered)
    tft.drawLine(x - 4, y, x - 3, y + 8, color);
    tft.drawLine(x + 4, y, x + 3, y + 8, color);
    tft.drawLine(x - 4, y, x + 4, y, color);
    
    // Left leg (bent, flying pose)
    tft.drawLine(x - 3, y + 8, x - 6, y + 12, color);
    tft.drawLine(x - 6, y + 12, x - 8, y + 16, color);
    
    // Right leg (bent, flying pose)
    tft.drawLine(x + 3, y + 8, x + 6, y + 12, color);
    tft.drawLine(x + 6, y + 12, x + 8, y + 16, color);
    
    // Add detail lines for depth
    tft.drawLine(x - 2, y + 2, x + 2, y + 2, color);  // Belt/waist
    
    // Backpack straps (visible from behind)
    tft.drawLine(x - 2, y - 2, x - 2, y + 6, color);
    tft.drawLine(x + 2, y - 2, x + 2, y + 6, color);
}

// ========== CAVE OBSTACLE GENERATION ==========
void initObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].x = 480 + i * 120;
        obstacles[i].topHeight = random(MIN_HEIGHT, MAX_HEIGHT);
        obstacles[i].bottomHeight = 320 - obstacles[i].topHeight - GAP_SIZE;
        
        // Make sure there's always a gap
        if (obstacles[i].bottomHeight < MIN_HEIGHT) {
            obstacles[i].bottomHeight = MIN_HEIGHT;
            obstacles[i].topHeight = 320 - GAP_SIZE - MIN_HEIGHT;
        }
        
        obstacles[i].passed = false;
    }
}

// ========== DRAW STALACTITE (from top) ==========
void drawStalactite(TFT_eSPI &tft, int x, int height) {
    if (height <= 0) return;
    
    uint16_t darkGray = 0x4208;
    uint16_t lightGray = 0x8410;
    
    // Jagged, rocky stalactite
    int baseWidth = 30;
    int tipY = height;
    
    // Main body - triangular with jagged edges
    for (int i = 0; i < 5; i++) {
        int offset = i * 2;
        int width = baseWidth - i * 5;
        
        tft.drawLine(x - width/2, offset, x - width/2 + 3, offset + 8, darkGray);
        tft.drawLine(x + width/2, offset, x + width/2 - 3, offset + 8, darkGray);
    }
    
    // Main outline
    tft.drawLine(x - baseWidth/2, 0, x - 10, tipY - 15, lightGray);
    tft.drawLine(x + baseWidth/2, 0, x + 10, tipY - 15, lightGray);
    tft.drawLine(x - 10, tipY - 15, x, tipY, TFT_WHITE);
    tft.drawLine(x + 10, tipY - 15, x, tipY, TFT_WHITE);
    
    // Fill with darker color
    tft.fillTriangle(x - baseWidth/2, 0, x, tipY, x + baseWidth/2, 0, darkGray);
    
    // Add rocky texture lines
    for (int i = 10; i < height - 10; i += 15) {
        int w = map(i, 0, height, baseWidth, 6);
        tft.drawLine(x - w/2 + random(-3, 3), i, x + w/2 + random(-3, 3), i + 5, lightGray);
    }
}

// ========== DRAW STALAGMITE (from bottom) ==========
void drawStalagmite(TFT_eSPI &tft, int x, int height) {
    if (height <= 0) return;
    
    uint16_t darkGray = 0x4208;
    uint16_t lightGray = 0x8410;
    
    int baseWidth = 30;
    int baseY = 320;
    int tipY = baseY - height;
    
    // Jagged base
    for (int i = 0; i < 5; i++) {
        int offset = baseY - i * 2;
        int width = baseWidth - i * 5;
        
        tft.drawLine(x - width/2, offset, x - width/2 + 3, offset - 8, darkGray);
        tft.drawLine(x + width/2, offset, x + width/2 - 3, offset - 8, darkGray);
    }
    
    // Main outline
    tft.drawLine(x - baseWidth/2, baseY, x - 10, tipY + 15, lightGray);
    tft.drawLine(x + baseWidth/2, baseY, x + 10, tipY + 15, lightGray);
    tft.drawLine(x - 10, tipY + 15, x, tipY, TFT_WHITE);
    tft.drawLine(x + 10, tipY + 15, x, tipY, TFT_WHITE);
    
    // Fill
    tft.fillTriangle(x - baseWidth/2, baseY, x, tipY, x + baseWidth/2, baseY, darkGray);
    
    // Rocky texture
    for (int i = 10; i < height - 10; i += 15) {
        int w = map(i, 0, height, baseWidth, 6);
        int yPos = baseY - i;
        tft.drawLine(x - w/2 + random(-3, 3), yPos, x + w/2 + random(-3, 3), yPos - 5, lightGray);
    }
}

// ========== COLLISION DETECTION ==========
bool checkCollision() {
    int personLeft = PERSON_X - 16;
    int personRight = PERSON_X + 16;
    int personTop = personY - 12;
    int personBottom = personY + 16;
    
    // Check ceiling and floor
    if (personTop < 0 || personBottom > 320) {
        return true;
    }
    
    // Check obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].x > personLeft - 25 && obstacles[i].x < personRight + 25) {
            // Check stalactite collision
            if (personTop < obstacles[i].topHeight) {
                return true;
            }
            // Check stalagmite collision
            if (personBottom > 320 - obstacles[i].bottomHeight) {
                return true;
            }
        }
    }
    
    return false;
}

// ========== UPDATE OBSTACLES ==========
void updateObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].x -= CAVE_SPEED;
        
        // Check if passed
        if (!obstacles[i].passed && obstacles[i].x < PERSON_X) {
            obstacles[i].passed = true;
            score++;
        }
        
        // Respawn obstacle when off screen
        if (obstacles[i].x < -40) {
            obstacles[i].x = 480;
            obstacles[i].topHeight = random(MIN_HEIGHT, MAX_HEIGHT);
            obstacles[i].bottomHeight = 320 - obstacles[i].topHeight - GAP_SIZE;
            
            if (obstacles[i].bottomHeight < MIN_HEIGHT) {
                obstacles[i].bottomHeight = MIN_HEIGHT;
                obstacles[i].topHeight = 320 - GAP_SIZE - MIN_HEIGHT;
            }
            
            obstacles[i].passed = false;
        }
    }
}

// ========== DRAW STAR FIELD BACKGROUND ==========
void drawStarfield(TFT_eSPI &tft) {
    static int stars[60][2];
    static bool initialized = false;
    
    if (!initialized) {
        for (int i = 0; i < 60; i++) {
            stars[i][0] = random(0, 480);
            stars[i][1] = random(0, 320);
        }
        initialized = true;
    }
    
    for (int i = 0; i < 60; i++) {
        tft.drawPixel(stars[i][0], stars[i][1], TFT_DARKGREY);
    }
}

// ========== SPLASH SCREEN ==========
void showSplash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    
    // Title
    tft.setTextFont(4);
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("PSYCHO FLIGHT", 240, 30);
    
    // Draw sample person in center
    tft.setTextSize(1);
    drawPerson(tft, 240, 120, TFT_CYAN);
    
    // Draw sample obstacles
    drawStalactite(tft, 100, 70);
    drawStalagmite(tft, 380, 70);
    
    // Instructions
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Navigate through the cave!", 240, 180);
    tft.drawString("Joystick UP to fly higher", 240, 205);
    tft.drawString("Joystick DOWN to descend", 240, 230);
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Press A to Start", 240, 270);
    tft.drawString("SELECT to Quit", 240, 295);
    
    // Wait for A button
    while (ss.digitalRead(BUTTON_A)) {  // Button is active LOW
        delay(50);
    }
    while (!ss.digitalRead(BUTTON_A)) {  // Wait for release
        delay(10);
    }
    delay(200);
}

// ========== GAME OVER SCREEN ==========
void showGameOver(TFT_eSPI &tft, Adafruit_seesaw &ss, int finalScore, int high) {
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextFont(4);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("CRASH!", 240, 60);
    
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Score: " + String(finalScore), 240, 140);
    tft.drawString("High Score: " + String(high), 240, 170);
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Press A to Continue", 240, 240);
    
    // Wait for A button
    while (ss.digitalRead(BUTTON_A)) {
        delay(50);
    }
    while (!ss.digitalRead(BUTTON_A)) {
        delay(10);
    }
    delay(200);
}

// ========== MAIN GAME LOOP ==========
void playGame(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    // Initialize game state
    personY = 160;
    velocityY = 0;
    score = 0;
    gameOver = false;
    
    initObstacles();
    
    unsigned long lastFrame = millis();
    
    while (!gameOver) {
        unsigned long frameStart = millis();
        
        // Clear screen
        tft.fillScreen(TFT_BLACK);
        drawStarfield(tft);
        
        // Read joystick Y axis (0-1023, center ~512)
        int joyY = ss.analogRead(JOY_Y);
        
        // Handle input
        if (joyY < 300) {  // Joystick UP - fly up
            velocityY -= THRUST;
        } else if (joyY > 700) {  // Joystick DOWN - fall faster
            velocityY += THRUST;
        }
        
        // Apply gravity
        velocityY += GRAVITY;
        
        // Clamp velocity
        if (velocityY > MAX_VELOCITY) velocityY = MAX_VELOCITY;
        if (velocityY < -MAX_VELOCITY) velocityY = -MAX_VELOCITY;
        
        // Update position
        personY += velocityY;
        
        // Update obstacles
        updateObstacles();
        
        // Draw obstacles
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles[i].x > -40 && obstacles[i].x < 480) {
                drawStalactite(tft, obstacles[i].x, obstacles[i].topHeight);
                drawStalagmite(tft, obstacles[i].x, obstacles[i].bottomHeight);
            }
        }
        
        // Draw person
        drawPerson(tft, PERSON_X, (int)personY, TFT_CYAN);
        
        // Draw score
        tft.setTextFont(2);
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(TL_DATUM);
        tft.drawString("Score: " + String(score), 10, 10);
        
        // Check collision
        if (checkCollision()) {
            gameOver = true;
            if (score > highScore) {
                highScore = score;
            }
        }
        
        // Check for SELECT button to exit
        if (!ss.digitalRead(BUTTON_SELECT)) {
            return; // Exit game
        }
        
        // Frame rate control (30 FPS)
        unsigned long frameTime = millis() - frameStart;
        if (frameTime < 33) {
            delay(33 - frameTime);
        }
    }
    
    // Game over
    showGameOver(tft, ss, score, highScore);
}

} // namespace FLAPPY_CAVE

// ========== PUBLIC INTERFACE ==========
void run_Flappy_Cave(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.setRotation(1);  // 480x320 landscape
    
    FLAPPY_CAVE::showSplash(tft, ss);
    FLAPPY_CAVE::playGame(tft, ss);
}

#endif // FLAPPY_CAVE_H