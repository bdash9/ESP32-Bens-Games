#ifndef RIP_OFF_H
#define RIP_OFF_H

#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

// ========== RIP OFF - DEFEND THE FUEL! ==========
// Classic 1980 arcade clone - Single player version

namespace RipOff {

// Screen constants
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
const int SCREEN_CENTER_X = 240;
const int SCREEN_CENTER_Y = 160;

// Game constants
const int RO_MAX_ENEMIES = 8;
const int RO_MAX_BULLETS = 15;
const int RO_MAX_PARTICLES = 50;
const int RO_NUM_FUEL_CANISTERS = 8;

// Player constants
//const float PLAYER_ACCEL = 0.3f;
//const float PLAYER_MAX_SPEED = 4.0f;
// Player constants
const float PLAYER_MAX_SPEED = 6.0f;    // Double the current speed
const float PLAYER_ACCEL = 0.6f;        // Double the acceleration
const float PLAYER_FRICTION = 0.95f;
const float PLAYER_TURN_SPEED = 0.1f;
const int PLAYER_SIZE = 12;
const int RESPAWN_INVINCIBLE_TIME = 2000;

// Enemy constants
const float ENEMY_SPEEDS[6] = {0.5f, 0.8f, 1.2f, 1.6f, 2.0f, 2.5f};
const int ENEMY_POINTS[6] = {10, 20, 30, 40, 50, 60};

// Game states
enum GameState {
    STATE_SPLASH,
    STATE_PLAYING,
    STATE_GAME_OVER
};

GameState gameState = STATE_SPLASH;

// ========== STRUCTURES ==========

struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}
    
    float length() const {
        return sqrt(x * x + y * y);
    }
    
    void normalize() {
        float len = length();
        if (len > 0) {
            x /= len;
            y /= len;
        }
    }
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
};

struct Player {
    Vector2 pos;
    Vector2 vel;
    float angle;
    bool alive;
    uint32_t deathTime;
    bool invincible;
    uint32_t invincibleUntil;
    int lives;
};

struct Enemy {
    Vector2 pos;
    Vector2 vel;
    float angle;
    bool alive;
    bool hasFuel;
    int fuelIndex;
    int type;  // 0-5 for different speeds/points
    float targetAngle;
};

struct Bullet {
    Vector2 pos;
    Vector2 vel;
    bool active;
    bool fromPlayer;
    uint32_t spawnTime;
};

struct Particle {
    Vector2 pos;
    Vector2 vel;
    uint32_t spawnTime;
    uint32_t lifetime;
    bool active;
};

struct FuelCanister {
    Vector2 pos;
    bool stolen;
    bool beingTowed;
};

// ========== GAME VARIABLES ==========

Player player;
Enemy enemies[RO_MAX_ENEMIES];
Bullet bullets[RO_MAX_BULLETS];
Particle particles[RO_MAX_PARTICLES];
FuelCanister fuelCanisters[RO_NUM_FUEL_CANISTERS];

int score = 0;
int wave = 1;
int bonusLevel = 0;
int enemiesInWave = 2;
int enemiesSpawned = 0;
int enemiesDestroyed = 0;
uint32_t lastSpawnTime = 0;
uint32_t waveStartTime = 0;

// ========== VECTOR DRAWING FUNCTIONS ==========

void drawLine(TFT_eSprite* buffer, int x1, int y1, int x2, int y2, uint16_t color) {
    buffer->drawLine(x1, y1, x2, y2, color);
}

void drawPlayerTank(TFT_eSprite* buffer, float x, float y, float angle, uint16_t color) {
    // More detailed player tank matching the image
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    
    // Main body (larger, more angular)
    Vector2 body[] = {
        {-8, -6}, {8, -6}, {10, 0}, {8, 6}, {-8, 6}, {-10, 0}
    };
    
    for (int i = 0; i < 6; i++) {
        Vector2 p1 = body[i];
        Vector2 p2 = body[(i + 1) % 6];
        
        float x1 = x + (p1.x * cos_a - p1.y * sin_a);
        float y1 = y + (p1.x * sin_a + p1.y * cos_a);
        float x2 = x + (p2.x * cos_a - p2.y * sin_a);
        float y2 = y + (p2.x * sin_a + p2.y * cos_a);
        
        buffer->drawLine(x1, y1, x2, y2, color);
    }
    
    // Cockpit/center detail
    Vector2 cockpit[] = {
        {-3, -3}, {3, -3}, {4, 0}, {3, 3}, {-3, 3}, {-4, 0}
    };
    
    for (int i = 0; i < 6; i++) {
        Vector2 p1 = cockpit[i];
        Vector2 p2 = cockpit[(i + 1) % 6];
        
        float x1 = x + (p1.x * cos_a - p1.y * sin_a);
        float y1 = y + (p1.x * sin_a + p1.y * cos_a);
        float x2 = x + (p2.x * cos_a - p2.y * sin_a);
        float y2 = y + (p2.x * sin_a + p2.y * cos_a);
        
        buffer->drawLine(x1, y1, x2, y2, color);
    }
    
    // Front cannon
    float cannon_x = x + (12 * cos_a);
    float cannon_y = y + (12 * sin_a);
    buffer->drawLine(x, y, cannon_x, cannon_y, color);
    
    // Wing details
    float wing1_x = x + (-8 * cos_a - 6 * sin_a);
    float wing1_y = y + (-8 * sin_a + 6 * cos_a);
    float wing2_x = x + (-8 * cos_a + 6 * sin_a);
    float wing2_y = y + (-8 * sin_a - 6 * cos_a);
    
    buffer->drawLine(wing1_x - 2*sin_a, wing1_y + 2*cos_a, wing1_x + 2*sin_a, wing1_y - 2*cos_a, color);
    buffer->drawLine(wing2_x - 2*sin_a, wing2_y + 2*cos_a, wing2_x + 2*sin_a, wing2_y - 2*cos_a, color);
}

void drawEnemyTank(TFT_eSprite* buffer, float x, float y, float angle, int type, uint16_t color) {
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    
    // LARGER enemy body (increased from -5/-4 to -7/-6)
    Vector2 body[] = {
        {-7, -6}, {7, -6}, {8, 0}, {7, 6}, {-7, 6}, {-8, 0}
    };
    
    for (int i = 0; i < 6; i++) {
        Vector2 p1 = body[i];
        Vector2 p2 = body[(i + 1) % 6];
        
        float x1 = x + (p1.x * cos_a - p1.y * sin_a);
        float y1 = y + (p1.x * sin_a + p1.y * cos_a);
        float x2 = x + (p2.x * cos_a - p2.y * sin_a);
        float y2 = y + (p2.x * sin_a + p2.y * cos_a);
        
        buffer->drawLine(x1, y1, x2, y2, color);
    }
    
    // Center dot (slightly larger)
    buffer->fillCircle(x, y, 2, color);
    
    // Front weapon (longer)
    float weapon_x = x + (10 * cos_a);
    float weapon_y = y + (10 * sin_a);
    buffer->drawLine(x, y, weapon_x, weapon_y, color);
    
    // Type-based detail (antenna/wings based on type)
    if (type >= 3) {
        // Draw small wings for advanced types
        float wing_x1 = x + (-5 * cos_a - 4 * sin_a);
        float wing_y1 = y + (-5 * sin_a + 4 * cos_a);
        float wing_x2 = x + (-5 * cos_a + 4 * sin_a);
        float wing_y2 = y + (-5 * sin_a - 4 * cos_a);
        
        buffer->drawPixel(wing_x1, wing_y1, color);
        buffer->drawPixel(wing_x2, wing_y2, color);
    }
}

void drawFuelCanister(TFT_eSprite* buffer, float x, float y, uint16_t color) {
    // Fuel canister is an outlined triangle
    buffer->drawTriangle(x, y - 8, x - 6, y + 8, x + 6, y + 8, color);
    buffer->drawTriangle(x, y - 7, x - 5, y + 7, x + 5, y + 7, color);
}

void drawTowLine(TFT_eSprite* buffer, float x1, float y1, float x2, float y2, uint16_t color) {
    // Dashed line connecting enemy to fuel
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);
    float steps = length / 8;  // Dash every 8 pixels
    
    for (int i = 0; i < (int)steps; i += 2) {
        float t1 = i / steps;
        float t2 = (i + 1) / steps;
        int sx = x1 + dx * t1;
        int sy = y1 + dy * t1;
        int ex = x1 + dx * t2;
        int ey = y1 + dy * t2;
        drawLine(buffer, sx, sy, ex, ey, color);
    }
}

void drawExplosion(TFT_eSprite* buffer, float x, float y, uint32_t age) {
    // Radial lines expanding outward
    int numLines = 16;
    float radius = (age / 30.0f);  // Grows over time
    
    for (int i = 0; i < numLines; i++) {
        float angle = (i / (float)numLines) * TWO_PI;
        int x1 = x;
        int y1 = y;
        int x2 = x + cos(angle) * radius;
        int y2 = y + sin(angle) * radius;
        
        uint16_t brightness = TFT_WHITE;
        if (age > 200) brightness = TFT_DARKGREY;
        else if (age > 100) brightness = TFT_LIGHTGREY;
        
        drawLine(buffer, x1, y1, x2, y2, brightness);
    }
}

void drawBullet(TFT_eSprite* buffer, float x, float y, bool fromPlayer) {
    uint16_t color = fromPlayer ? TFT_CYAN : TFT_RED;
    buffer->fillCircle(x, y, 1, color);  // Changed from 2 to 1
    // Removed outer circle for smaller bullets
}

// ========== SPLASH SCREEN ==========

void drawRipOffLogo(TFT_eSprite* buffer, int y) {
    int letterWidth = 35;
    int letterHeight = 50;
    int spacing = 45;
    int gapBetweenWords = 30;
    
    int ripWidth = (3 * letterWidth) + (2 * spacing);
    int offWidth = (3 * letterWidth) + (2 * spacing);
    int totalWidth = ripWidth + gapBetweenWords + offWidth;
    
    // MOVED EVEN MORE TO RIGHT: Changed from +40 to +80
    int startX = (SCREEN_WIDTH - totalWidth) / 2 + 80;
    
    uint16_t yellow = TFT_YELLOW;
    
    // === R ===
    int rx = startX;
    buffer->fillRect(rx, y, 8, letterHeight, yellow);
    buffer->fillRect(rx, y, letterWidth, 8, yellow);
    buffer->fillRect(rx, y + 20, letterWidth - 5, 8, yellow);
    buffer->fillRect(rx + letterWidth - 8, y, 8, 28, yellow);
    
    // THICK diagonal leg
    for (int i = 0; i < 8; i++) {
        buffer->drawLine(rx + 15 + i, y + 28, rx + letterWidth + i, y + letterHeight, yellow);
    }
    
    // === I ===
    int ix = rx + spacing;
    buffer->fillRect(ix + 10, y, 8, letterHeight, yellow);
    buffer->fillRect(ix, y, letterWidth - 5, 8, yellow);
    buffer->fillRect(ix, y + letterHeight - 8, letterWidth - 5, 8, yellow);
    
    // === P ===
    int px = ix + spacing;
    buffer->fillRect(px, y, 8, letterHeight, yellow);
    buffer->fillRect(px, y, letterWidth, 8, yellow);
    buffer->fillRect(px, y + 22, letterWidth, 8, yellow);
    buffer->fillRect(px + letterWidth - 8, y, 8, 30, yellow);
    
    // === O ===
    int ox = px + spacing + gapBetweenWords;
    buffer->fillRect(ox, y, 8, letterHeight, yellow);
    buffer->fillRect(ox + letterWidth - 8, y, 8, letterHeight, yellow);
    buffer->fillRect(ox, y, letterWidth, 8, yellow);
    buffer->fillRect(ox, y + letterHeight - 8, letterWidth, 8, yellow);
    
    // === F ===
    int fx = ox + spacing;
    buffer->fillRect(fx, y, 8, letterHeight, yellow);
    buffer->fillRect(fx, y, letterWidth, 8, yellow);
    buffer->fillRect(fx, y + 20, letterWidth - 8, 8, yellow);
    
    // === F ===
    int fx2 = fx + spacing;
    buffer->fillRect(fx2, y, 8, letterHeight, yellow);
    buffer->fillRect(fx2, y, letterWidth, 8, yellow);
    buffer->fillRect(fx2, y + 20, letterWidth - 8, 8, yellow);
}

void showSplash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->setAttribute(PSRAM_ENABLE, true);
    buffer->createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    Serial.println("[RipOff] Splash screen starting...");
    
    // Simple delay to let things settle
    delay(300);
    
    uint32_t startTime = millis();
    int frameCount = 0;
    bool acceptInput = false;
    
    while (true) {
        buffer->fillSprite(TFT_BLACK);
        
        // "A rip-off of" text above logo
        buffer->setTextDatum(TC_DATUM);
        buffer->setTextColor(TFT_WHITE);
        buffer->drawString("A rip-off of", SCREEN_CENTER_X, 20, 2);  // Centered on screen        
        // Draw logo - HIGHER AND CENTERED
        drawRipOffLogo(buffer, 40);
        
        // Draw demo tanks
        float time = (millis() - startTime) / 1000.0f;
        
        // Player tank
        drawPlayerTank(buffer, 350 + sin(time * 2) * 20, 150, time, TFT_CYAN);
        
        // Enemy tanks around fuel
        for (int i = 0; i < 8; i++) {
            float angle = (i / 8.0f) * TWO_PI + time;
            float x = SCREEN_CENTER_X + cos(angle) * 60;
            float y = SCREEN_CENTER_Y + sin(angle) * 60;
            drawEnemyTank(buffer, x, y, angle + PI, i % 6, TFT_RED);
        }
        
        // Fuel canisters in center
        for (int i = 0; i < RO_NUM_FUEL_CANISTERS; i++) {
            float angle = (i / (float)RO_NUM_FUEL_CANISTERS) * TWO_PI;
            float x = SCREEN_CENTER_X + cos(angle) * 30;
            float y = SCREEN_CENTER_Y + sin(angle) * 30;
            drawFuelCanister(buffer, x, y, TFT_GREEN);
        }
        
        // Instructions
        buffer->setTextColor(TFT_WHITE);
        buffer->setTextDatum(MC_DATUM);
        buffer->drawString("DEFEND THE FUEL!", SCREEN_CENTER_X, 240, 4);
        buffer->drawString("Joystick: Move & Aim", SCREEN_CENTER_X, 270, 2);
        
        // Flashing "Press Button to Play"
        if ((millis() / 500) % 2 == 0) {
            buffer->setTextColor(TFT_YELLOW);
            buffer->drawString("PRESS BUTTON A TO PLAY", SCREEN_CENTER_X, SCREEN_HEIGHT - 20, 2);
        }
        
        buffer->pushSprite(0, 0);
        
        frameCount++;
        
        // Start accepting input after 10 frames
        if (frameCount > 10) {
            acceptInput = true;
        }
        
        // Simple button check
        if (acceptInput) {
            bool buttonPressed = (ss.digitalRead(5) == LOW);
            
            if (buttonPressed) {
                Serial.println("[RipOff] Button A pressed! Starting game...");
                delay(300);
                break;
            }
        }
        
        delay(16);  // ~60 FPS
    }
    
    buffer->deleteSprite();
    delete buffer;
    
    Serial.println("[RipOff] Splash complete, entering game...");
    gameState = STATE_PLAYING;
}

// ========== GAME INITIALIZATION ==========

void initGame() {
    // Init player
    player.pos = Vector2(SCREEN_CENTER_X + 100, SCREEN_CENTER_Y);
    player.vel = Vector2(0, 0);
    player.angle = PI;
    player.alive = true;
    player.invincible = false;
    player.lives = 3;
    
    // Init fuel canisters in circle around center
    for (int i = 0; i < RO_NUM_FUEL_CANISTERS; i++) {
      float angle = (i / (float)RO_NUM_FUEL_CANISTERS) * TWO_PI;        
      fuelCanisters[i].pos = Vector2(
            SCREEN_CENTER_X + cos(angle) * 40,
            SCREEN_CENTER_Y + sin(angle) * 40
        );
        fuelCanisters[i].stolen = false;
        fuelCanisters[i].beingTowed = false;
    }
    
    // Init enemies
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        enemies[i].alive = false;
    }
    
    // Init bullets
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    
    // Init particles
    for (int i = 0; i < RO_MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
    
    score = 0;
    wave = 1;
    bonusLevel = 0;
    enemiesInWave = 2;
    enemiesSpawned = 0;
    enemiesDestroyed = 0;
    waveStartTime = millis();
}

// ========== GAME LOGIC ==========

void spawnEnemy() {
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (!enemies[i].alive) {
            // Spawn at random edge
            int edge = random(4);
            switch (edge) {
                case 0: // Top
                    enemies[i].pos = Vector2(random(SCREEN_WIDTH), -20);
                    break;
                case 1: // Right
                    enemies[i].pos = Vector2(SCREEN_WIDTH + 20, random(SCREEN_HEIGHT));
                    break;
                case 2: // Bottom
                    enemies[i].pos = Vector2(random(SCREEN_WIDTH), SCREEN_HEIGHT + 20);
                    break;
                case 3: // Left
                    enemies[i].pos = Vector2(-20, random(SCREEN_HEIGHT));
                    break;
            }
            
            enemies[i].type = random(6);
            enemies[i].alive = true;
            enemies[i].hasFuel = false;
            enemies[i].vel = Vector2(0, 0);
            enemies[i].angle = 0;
            
            // REMOVED the speed line - speed scaling happens in updateEnemies()
            
            Serial.printf("[RipOff] Spawned enemy type %d at edge %d\n", enemies[i].type, edge);
            return;
        }
    }
}

void fireBullet(Vector2 pos, float angle, bool fromPlayer) {
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].pos = pos;
            bullets[i].vel = Vector2(cos(angle) * 6, sin(angle) * 6);
            bullets[i].active = true;
            bullets[i].fromPlayer = fromPlayer;
            bullets[i].spawnTime = millis();
            Serial.printf("[RipOff] Bullet fired at angle %.2f\n", angle);  // ADD THIS
            break;
        }
    }
}

void createExplosion(Vector2 pos) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < RO_MAX_PARTICLES; j++) {
            if (!particles[j].active) {
                float angle = random(360) * DEG_TO_RAD;
                float speed = random(10, 30) / 10.0f;
                particles[j].pos = pos;
                particles[j].vel = Vector2(cos(angle) * speed, sin(angle) * speed);
                particles[j].spawnTime = millis();
                particles[j].lifetime = random(200, 400);
                particles[j].active = true;
                break;
            }
        }
    }
}

void updatePlayer(Adafruit_seesaw &ss) {
    if (!player.alive) {
        if (millis() - player.deathTime > 2000) {
            // Respawn
            player.pos = Vector2(SCREEN_CENTER_X + 100, SCREEN_CENTER_Y);
            player.vel = Vector2(0, 0);
            player.angle = PI;
            player.alive = true;
            player.invincible = true;
            player.invincibleUntil = millis() + RESPAWN_INVINCIBLE_TIME;
        }
        return;
    }
    
    // Check invincibility
    if (player.invincible && millis() > player.invincibleUntil) {
        player.invincible = false;
    }
    
// Read joystick - FIXED MAPPING
    int joyX = ss.analogRead(2);  // JOY_X
    int joyY = ss.analogRead(3);  // JOY_Y
    
    // Swap and invert for correct orientation
    float inputX = (joyY - 512) / 512.0f;   // Y controls X (swapped)
    float inputY = (joyX - 512) / 512.0f;   // X controls Y (swapped)
    
    if (abs(inputX) < 0.15f) inputX = 0;
    if (abs(inputY) < 0.15f) inputY = 0;
    
    // Update angle based on joystick
    if (inputX != 0 || inputY != 0) {
        float targetAngle = atan2(inputY, inputX);
        player.angle = targetAngle;
    }
    
    // Acceleration
    if (inputX != 0 || inputY != 0) {
        player.vel.x += cos(player.angle) * PLAYER_ACCEL;
        player.vel.y += sin(player.angle) * PLAYER_ACCEL;
    }
    
    // Limit speed
    float speed = player.vel.length();
    if (speed > PLAYER_MAX_SPEED) {
        player.vel.normalize();
        player.vel = player.vel * PLAYER_MAX_SPEED;
    }
    
    // Apply friction
    player.vel = player.vel * PLAYER_FRICTION;
    
    // Update position
    player.pos = player.pos + player.vel;
    
    // Screen wrapping
    if (player.pos.x < 0) player.pos.x = SCREEN_WIDTH;
    if (player.pos.x > SCREEN_WIDTH) player.pos.x = 0;
    if (player.pos.y < 0) player.pos.y = SCREEN_HEIGHT;
    if (player.pos.y > SCREEN_HEIGHT) player.pos.y = 0;
    
// Fire button - FIXED
static uint32_t lastFireTime = 0;
static bool fireButtonWasPressed = false;

bool fireButtonPressed = !ss.digitalRead(6);  // Button B for fire

if (fireButtonPressed && !fireButtonWasPressed && millis() - lastFireTime > 250) {
    fireBullet(player.pos, player.angle, true);
    lastFireTime = millis();
    Serial.println("[RipOff] FIRE!");
}

fireButtonWasPressed = fireButtonPressed;
}


void updateEnemies() {
    // Calculate speed multiplier for this wave
    float speedMultiplier = 1.0f + (wave - 1) * 0.05f;
    if (speedMultiplier > 2.0f) speedMultiplier = 2.0f;
    
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;
        
        Enemy& e = enemies[i];
        
        
        // Get current speed for this enemy type with wave multiplier
        float currentSpeed = ENEMY_SPEEDS[e.type] * speedMultiplier;
        
        // REMOVE ENEMIES THAT GO TOO FAR OFF SCREEN (without fuel)
        if (!e.hasFuel) {
            if (e.pos.x < -100 || e.pos.x > SCREEN_WIDTH + 100 ||
                e.pos.y < -100 || e.pos.y > SCREEN_HEIGHT + 100) {
                Serial.printf("[RipOff] Enemy %d wandered off screen, removing\n", i);
                e.alive = false;
                continue;
            }
        }
        
 if (!e.hasFuel) {
            // Find nearest non-stolen fuel
            float minDist = 999999;
            int targetFuel = -1;
            
            for (int j = 0; j < RO_NUM_FUEL_CANISTERS; j++) {
                if (!fuelCanisters[j].stolen && !fuelCanisters[j].beingTowed) {
                    Vector2 diff = fuelCanisters[j].pos - e.pos;
                    float dist = diff.length();
                    if (dist < minDist) {
                        minDist = dist;
                        targetFuel = j;
                    }
                }
            }
            
            if (targetFuel >= 0) {
                // Move toward fuel
                Vector2 diff = fuelCanisters[targetFuel].pos - e.pos;
                diff.normalize();
                e.vel = diff * currentSpeed;  // USE currentSpeed
                e.angle = atan2(diff.y, diff.x);
                
                // Check if reached fuel
                if (minDist < 15) {
                    e.hasFuel = true;
                    e.fuelIndex = targetFuel;
                    fuelCanisters[targetFuel].beingTowed = true;
                    Serial.println("[RipOff] Enemy grabbed fuel!");
                }
            }
        } else {
            // Has fuel - head to nearest edge
            Vector2 toEdge;
            if (e.pos.x < SCREEN_CENTER_X) {
                if (e.pos.y < SCREEN_CENTER_Y) {
                    toEdge = Vector2(-1, -1);
                } else {
                    toEdge = Vector2(-1, 1);
                }
            } else {
                if (e.pos.y < SCREEN_CENTER_Y) {
                    toEdge = Vector2(1, -1);
                } else {
                    toEdge = Vector2(1, 1);
                }
            }
            
            toEdge.normalize();
            e.vel = toEdge * currentSpeed * 0.7f;  // USE currentSpeed
            e.angle = atan2(toEdge.y, toEdge.x);
            
            // Tow fuel
            fuelCanisters[e.fuelIndex].pos = e.pos - Vector2(cos(e.angle) * 15, sin(e.angle) * 15);
            
            // Check if escaped off screen
            if (e.pos.x < -20 || e.pos.x > SCREEN_WIDTH + 20 ||
                e.pos.y < -20 || e.pos.y > SCREEN_HEIGHT + 20) {
                fuelCanisters[e.fuelIndex].stolen = true;
                fuelCanisters[e.fuelIndex].beingTowed = false;  // Release the tow flag
                e.alive = false;
                Serial.println("[RipOff] Fuel stolen!");
                
                // Check game over
                int remaining = 0;
                for (int j = 0; j < RO_NUM_FUEL_CANISTERS; j++) {
                    if (!fuelCanisters[j].stolen) remaining++;
                }
                if (remaining == 0) {
                    Serial.println("[RipOff] All fuel stolen! Game Over!");
                    gameState = STATE_GAME_OVER;
                }
                continue;  // Skip rest of update for this enemy
            }
        }
        
        // Update position
        e.pos = e.pos + e.vel;
        
        // Enemy shooting - ONLY AFTER LEVEL 1
        if (wave > 1) {  // ADD THIS CHECK
            if (random(100) < 2 && e.type > 2) {  // Higher types fire more
                fireBullet(e.pos, e.angle, false);
            }
        }
    }
}

void updateBullets() {
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        
        // Update position using velocity vector
        bullets[i].pos = bullets[i].pos + bullets[i].vel;
        
        // Different screen bounds for player vs enemy bullets
        float margin = bullets[i].fromPlayer ? 100 : 20;  // Player bullets go 100px off-screen
        
        // Remove if too far off screen
        if (bullets[i].pos.x < -margin || bullets[i].pos.x > SCREEN_WIDTH + margin ||
            bullets[i].pos.y < -margin || bullets[i].pos.y > SCREEN_HEIGHT + margin) {
            bullets[i].active = false;
        }
    }
}

void updateParticles() {
    for (int i = 0; i < RO_MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        
        particles[i].pos = particles[i].pos + particles[i].vel;
        particles[i].vel = particles[i].vel * 0.95f;  // Slow down
        
        if (millis() - particles[i].spawnTime > particles[i].lifetime) {
            particles[i].active = false;
        }
    }
}

void checkCollisions() {
    // Player bullets vs enemies
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        if (!bullets[i].active || !bullets[i].fromPlayer) continue;
        
        for (int j = 0; j < RO_MAX_ENEMIES; j++) {
            if (!enemies[j].alive) continue;
            
            Vector2 diff = bullets[i].pos - enemies[j].pos;
            if (diff.length() < 12) {
                // Hit!
                bullets[i].active = false;
                enemies[j].alive = false;
                
                // Release fuel if towing
                if (enemies[j].hasFuel) {
                    fuelCanisters[enemies[j].fuelIndex].beingTowed = false;
                }
                
                createExplosion(enemies[j].pos);
                score += ENEMY_POINTS[enemies[j].type] + bonusLevel;
                enemiesDestroyed++;
                
                Serial.printf("[RipOff] Enemy destroyed! Score: %d\n", score);
                break;
            }
        }
    }
    
    // Enemy bullets vs player
    if (player.alive && !player.invincible) {
        for (int i = 0; i < RO_MAX_BULLETS; i++) {
            if (!bullets[i].active || bullets[i].fromPlayer) continue;
            
            Vector2 diff = bullets[i].pos - player.pos;
            if (diff.length() < 12) {
                // Player hit!
                bullets[i].active = false;
                player.alive = false;
                player.deathTime = millis();
                player.lives--;
                
                createExplosion(player.pos);
                Serial.println("[RipOff] Player destroyed!");
                
                if (player.lives <= 0) {
                    gameState = STATE_GAME_OVER;
                }
                break;
            }
        }
    }
    
    // Player collision with enemies
    if (player.alive) {
        for (int j = 0; j < RO_MAX_ENEMIES; j++) {
            if (!enemies[j].alive) continue;
            
            Vector2 diff = player.pos - enemies[j].pos;
            if (diff.length() < 15) {
                // Collision destroys both!
                enemies[j].alive = false;
                
                if (enemies[j].hasFuel) {
                    fuelCanisters[enemies[j].fuelIndex].beingTowed = false;
                }
                
                createExplosion(enemies[j].pos);
                score += ENEMY_POINTS[enemies[j].type] + bonusLevel;
                enemiesDestroyed++;
                
                if (!player.invincible) {
                    player.alive = false;
                    player.deathTime = millis();
                    player.lives--;
                    createExplosion(player.pos);
                    
                    if (player.lives <= 0) {
                        gameState = STATE_GAME_OVER;
                    }
                }
                break;
            }
        }
    }
}

void updateWaveLogic() {
    // Count remaining fuel
    int activeFuelCount = 0;
    for (int i = 0; i < RO_NUM_FUEL_CANISTERS; i++) {
        if (!fuelCanisters[i].stolen) activeFuelCount++;
    }
    
    // Game over if all fuel is gone
    if (activeFuelCount == 0) {
        Serial.println("[RipOff] All fuel destroyed! Game Over!");
        gameState = STATE_GAME_OVER;
        return;
    }
    
    // Count alive enemies
    int aliveEnemies = 0;
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (enemies[i].alive) aliveEnemies++;
    }
    
    // NON-BLOCKING wave transition
    static bool waveComplete = false;
    static uint32_t waveCompleteTime = 0;
    
    if (aliveEnemies <= 0 && !waveComplete) {
        // Wave just completed
        waveComplete = true;
        waveCompleteTime = millis();
        Serial.printf("[RipOff] Wave %d complete! Next wave in 2 seconds...\n", wave);
    }
    
    // Check if 2 seconds have passed since wave completion
    if (waveComplete && millis() - waveCompleteTime >= 2000) {
        wave++;
        
        // Start with 2 enemies on level 1, increase each wave
        int enemiesToSpawn = 2 + (wave - 1);
        if (enemiesToSpawn > RO_MAX_ENEMIES) enemiesToSpawn = RO_MAX_ENEMIES;
        
        for (int i = 0; i < enemiesToSpawn; i++) {
            spawnEnemy();
        }
        
        waveComplete = false;  // Reset flag
        Serial.printf("[RipOff] Wave %d started with %d enemies\n", wave, enemiesToSpawn);
    }
}

// ========== RENDER ==========

void render(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    // Draw fuel canisters
    for (int i = 0; i < RO_NUM_FUEL_CANISTERS; i++) {
        if (!fuelCanisters[i].stolen) {
            drawFuelCanister(buffer, fuelCanisters[i].pos.x, fuelCanisters[i].pos.y, TFT_GREEN);
        }
    }
    
    // Draw bullets
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            uint16_t color = bullets[i].fromPlayer ? TFT_CYAN : TFT_RED;
            buffer->fillCircle(bullets[i].pos.x, bullets[i].pos.y, 2, color);
        }
    }
    
    // Draw enemies
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (enemies[i].alive) {
            uint16_t color;
            switch (enemies[i].type) {
                case 0: color = TFT_RED; break;
                case 1: color = TFT_MAGENTA; break;
                case 2: color = TFT_ORANGE; break;
                case 3: color = TFT_YELLOW; break;
                case 4: color = 0xF800 | 0x001F; break;
                case 5: color = TFT_WHITE; break;
                default: color = TFT_RED; break;
            }
            drawEnemyTank(buffer, enemies[i].pos.x, enemies[i].pos.y, enemies[i].angle, enemies[i].type, color);
        }
    }
    
    // Draw player
    if (player.alive) {
        drawPlayerTank(buffer, player.pos.x, player.pos.y, player.angle, TFT_CYAN);
    }
    
    // Draw particles
    for (int i = 0; i < RO_MAX_PARTICLES; i++) {
        if (particles[i].active) {
            buffer->fillCircle(particles[i].pos.x, particles[i].pos.y, 1, TFT_ORANGE);
        }
    }
    
    // === CLEAN HUD - TOP ROW ONLY ===
    buffer->setTextColor(TFT_CYAN);
    buffer->setTextDatum(TL_DATUM);
    
    // Left: Level number only
    buffer->drawString(String(wave), 10, 10, 4);
    
    // Center: Score number only
    buffer->setTextDatum(TC_DATUM);
    buffer->drawString(String(score), SCREEN_CENTER_X, 10, 4);
    
    // Right: Ship icons for lives
    for (int i = 0; i < player.lives; i++) {
        drawPlayerTank(buffer, SCREEN_WIDTH - 30 - (i * 25), 20, 0, TFT_CYAN);
    }
}

void showGameOver(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    buffer->setTextDatum(MC_DATUM);
    
    // GAME OVER - at top
    buffer->setTextColor(TFT_RED);
    buffer->drawString("GAME OVER", SCREEN_CENTER_X, 60, 4);
    
    // FINAL SCORE label
    buffer->setTextColor(TFT_YELLOW);
    buffer->drawString("FINAL SCORE", SCREEN_CENTER_X, 110, 4);
    
    // Score value
    buffer->setTextColor(TFT_WHITE);
    buffer->drawString(String(score), SCREEN_CENTER_X, 150, 4);
    
    // Level reached
    buffer->setTextColor(TFT_CYAN);
    buffer->drawString("LEVEL REACHED: " + String(wave), SCREEN_CENTER_X, 190, 2);
    
    // Flashing prompt at bottom
    if ((millis() / 500) % 2 == 0) {
        buffer->setTextColor(TFT_GREEN);
        buffer->drawString("PRESS BUTTON TO CONTINUE", SCREEN_CENTER_X, SCREEN_HEIGHT - 30, 2);
    }
}

// ========== MAIN GAME LOOP ==========

void playGame(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[RipOff] Starting Rip Off...");
    
    // Simple approach: just add a delay instead of checking buttons
    Serial.println("[RipOff] Initial delay...");
    delay(500);
    
    // INITIALIZE JOYSTICK PINS
    ss.pinMode(14, INPUT);
    ss.pinMode(15, INPUT);
    Serial.println("[RipOff] Joystick pins initialized");
    
    // Show splash
    showSplash(tft, ss);
    
    // Initialize game
    initGame();
    
    // Create sprite buffer
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->setAttribute(PSRAM_ENABLE, true);
    
    bool bufferCreated = buffer->createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    if (!bufferCreated) {
        Serial.println("[RipOff] ERROR: Buffer creation failed!");
        delete buffer;
        return;
    }
    
    Serial.println("[RipOff] Buffer created, starting game loop...");
    
    // Main game loop
while (true) {
    unsigned long frameStart = millis();
    
    if (gameState == STATE_PLAYING) {
        // Update
        updatePlayer(ss);
        updateEnemies();
        updateBullets();
        updateParticles();
        checkCollisions();
        updateWaveLogic();
        
        // Render
        render(buffer);
        
    } else if (gameState == STATE_GAME_OVER) {
        showGameOver(buffer);
        
        // Check for button press to return to splash
        static bool gameOverButtonPressed = false;
        bool buttonPressed = !ss.digitalRead(5);  // Button A
        
        if (buttonPressed && !gameOverButtonPressed) {
            Serial.println("[RipOff] Returning to splash screen...");
            delay(300);
            // Reset game and go back to splash
            initGame();
            gameState = STATE_PLAYING;
            showSplash(tft, ss);  // Show splash again
            gameOverButtonPressed = false;
        }
        
        gameOverButtonPressed = buttonPressed;
    }
    
    buffer->pushSprite(0, 0);
    
    // Check for exit (SELECT button)
    if (!ss.digitalRead(14)) {
        Serial.println("[RipOff] SELECT pressed, exiting.");
        break;
    }
    
    // Frame rate control (30 FPS)
    unsigned long frameTime = millis() - frameStart;
    if (frameTime < 33) delay(33 - frameTime);
}
    
    // Cleanup
    buffer->deleteSprite();
    delete buffer;
    tft.fillScreen(TFT_BLACK);
}

} // namespace RipOff

// Global function to run the game
void run_Rip_Off(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[RipOff] Launching Rip Off game...");
    RipOff::playGame(tft, ss);
}

#endif // RIP_OFF_H