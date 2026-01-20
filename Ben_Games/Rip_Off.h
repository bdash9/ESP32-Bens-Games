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
const int RO_MAX_BULLETS = 10;
const int RO_MAX_PARTICLES = 50;
const int RO_NUM_FUEL_CANISTERS = 8;

// Player constants
const float PLAYER_ACCEL = 0.3f;
const float PLAYER_MAX_SPEED = 4.0f;
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
    // Player is a chevron/arrow shape pointing in direction
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    
    // Front point
    int x1 = x + cos_a * 12;
    int y1 = y + sin_a * 12;
    
    // Left wing
    int x2 = x + cos(angle - 2.5f) * 8;
    int y2 = y + sin(angle - 2.5f) * 8;
    
    // Right wing
    int x3 = x + cos(angle + 2.5f) * 8;
    int y3 = y + sin(angle + 2.5f) * 8;
    
    // Back left
    int x4 = x + cos(angle - 2.8f) * 6;
    int y4 = y + sin(angle - 2.8f) * 6;
    
    // Back right
    int x5 = x + cos(angle + 2.8f) * 6;
    int y5 = y + sin(angle + 2.8f) * 6;
    
    // Draw outline
    drawLine(buffer, x1, y1, x2, y2, color);
    drawLine(buffer, x2, y2, x4, y4, color);
    drawLine(buffer, x4, y4, x5, y5, color);
    drawLine(buffer, x5, y5, x3, y3, color);
    drawLine(buffer, x3, y3, x1, y1, color);
    
    // Draw center line
    drawLine(buffer, x, y, x1, y1, color);
}

void drawEnemyTank(TFT_eSprite* buffer, float x, float y, float angle, int type, uint16_t color) {
    // Enemy is a filled triangle
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    
    int size = 8 + type;  // Bigger for higher types
    
    // Front point
    int x1 = x + cos_a * size;
    int y1 = y + sin_a * size;
    
    // Back left
    int x2 = x + cos(angle + 2.5f) * (size * 0.6f);
    int y2 = y + sin(angle + 2.5f) * (size * 0.6f);
    
    // Back right
    int x3 = x + cos(angle - 2.5f) * (size * 0.6f);
    int y3 = y + sin(angle - 2.5f) * (size * 0.6f);
    
    // Draw filled triangle
    buffer->fillTriangle(x1, y1, x2, y2, x3, y3, color);
    buffer->drawTriangle(x1, y1, x2, y2, x3, y3, TFT_WHITE);
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
    // "RIP OFF" in blocky letters
    buffer->setTextColor(TFT_YELLOW, TFT_BLACK);
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextSize(1);
    
    // Draw outlined text for arcade feel
    for (int ox = -2; ox <= 2; ox++) {
        for (int oy = -2; oy <= 2; oy++) {
            if (ox == 0 && oy == 0) continue;
            buffer->drawString("RIP OFF", SCREEN_CENTER_X + ox, y + oy, 7);
        }
    }
    buffer->setTextColor(TFT_YELLOW);
    buffer->drawString("RIP OFF", SCREEN_CENTER_X, y, 7);
}

void showSplash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->setAttribute(PSRAM_ENABLE, true);
    buffer->createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Simple delay instead of waiting
    Serial.println("[RipOff] Splash screen starting...");
    delay(500);
    
    uint32_t startTime = millis();
    bool buttonWasPressed = false;  // ADD THIS LINE
    
    while (true) {
        buffer->fillSprite(TFT_BLACK);
        
        // Draw logo
        drawRipOffLogo(buffer, 80);
        
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
        delay(16);
        
        // Check for start button
        bool buttonPressed = !ss.digitalRead(5);  // Button A
        if (buttonPressed && !buttonWasPressed) {
            Serial.println("[RipOff] Button A pressed, starting game...");
            delay(300);
            break;
        }
        buttonWasPressed = buttonPressed;
    }  // ADD THIS CLOSING BRACE FOR THE WHILE LOOP
    
    // Wait for button release
    delay(500);
    
    buffer->deleteSprite();
    delete buffer;
    
    Serial.println("[RipOff] Splash complete, entering game...");
    gameState = STATE_PLAYING;
}  // ADD THIS CLOSING BRACE FOR THE FUNCTION


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
    // Find inactive enemy slot
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (!enemies[i].alive) {
            // Spawn from random edge
            int edge = random(4);
            switch (edge) {
                case 0: // Top
                    enemies[i].pos = Vector2(random(SCREEN_WIDTH), 0);
                    break;
                case 1: // Right
                    enemies[i].pos = Vector2(SCREEN_WIDTH, random(SCREEN_HEIGHT));
                    break;
                case 2: // Bottom
                    enemies[i].pos = Vector2(random(SCREEN_WIDTH), SCREEN_HEIGHT);
                    break;
                case 3: // Left
                    enemies[i].pos = Vector2(0, random(SCREEN_HEIGHT));
                    break;
            }
            
            enemies[i].vel = Vector2(0, 0);
            enemies[i].angle = 0;
            enemies[i].alive = true;
            enemies[i].hasFuel = false;
            enemies[i].type = wave % 6;  // Cycles through types
            
            enemiesSpawned++;
            Serial.printf("[RipOff] Spawned enemy type %d\n", enemies[i].type);
            break;
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
    
// Fire button - TRY BUTTON 6 INSTEAD OF 5
    static uint32_t lastFireTime = 0;
    static bool wasPressed = false;
    
    bool buttonPressed = !ss.digitalRead(6);  // Try button 6 instead of 5
    
    // Only fire if button was just pressed (not held)
    if (buttonPressed && !wasPressed && millis() - lastFireTime > 250) {
        fireBullet(player.pos, player.angle, true);
        lastFireTime = millis();
        Serial.println("[RipOff] FIRE!");
    }
    
    wasPressed = buttonPressed;
}

void updateEnemies() {
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;
        
        Enemy& e = enemies[i];
        
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
                e.vel = diff * ENEMY_SPEEDS[e.type];
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
                    // Top-left
                    toEdge = Vector2(-1, -1);
                } else {
                    // Bottom-left
                    toEdge = Vector2(-1, 1);
                }
            } else {
                if (e.pos.y < SCREEN_CENTER_Y) {
                    // Top-right
                    toEdge = Vector2(1, -1);
                } else {
                    // Bottom-right
                    toEdge = Vector2(1, 1);
                }
            }
            
            toEdge.normalize();
            e.vel = toEdge * ENEMY_SPEEDS[e.type] * 0.7f;  // Slower when towing
            e.angle = atan2(toEdge.y, toEdge.x);
            
            // Tow fuel
            fuelCanisters[e.fuelIndex].pos = e.pos - Vector2(cos(e.angle) * 15, sin(e.angle) * 15);
            
            // Check if escaped off screen
            if (e.pos.x < -20 || e.pos.x > SCREEN_WIDTH + 20 ||
                e.pos.y < -20 || e.pos.y > SCREEN_HEIGHT + 20) {
                fuelCanisters[e.fuelIndex].stolen = true;
                e.alive = false;
                Serial.println("[RipOff] Fuel stolen!");
                
                // Check game over
                int remaining = 0;
                for (int j = 0; j < RO_NUM_FUEL_CANISTERS; j++) {
                    if (!fuelCanisters[j].stolen) remaining++;
                }
                if (remaining == 0) {
                    gameState = STATE_GAME_OVER;
                }
            }
        }
        
        // Update position
        e.pos = e.pos + e.vel;
        
        // Enemy occasionally fires
        if (random(100) < 2 && e.type > 2) {  // Higher types fire more
            fireBullet(e.pos, e.angle, false);
        }
    }
}

void updateBullets() {
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        
        bullets[i].pos = bullets[i].pos + bullets[i].vel;
        
        // Remove if off screen or too old
        if (bullets[i].pos.x < 0 || bullets[i].pos.x > SCREEN_WIDTH ||
            bullets[i].pos.y < 0 || bullets[i].pos.y > SCREEN_HEIGHT ||
            millis() - bullets[i].spawnTime > 2000) {
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
    // Check if wave complete
    if (enemiesSpawned >= enemiesInWave && enemiesDestroyed >= enemiesInWave) {
        // Next wave
        wave++;
        enemiesInWave = min(2 + wave / 2, 3);  // 2-3 enemies per wave
        enemiesSpawned = 0;
        enemiesDestroyed = 0;
        waveStartTime = millis();
        
        // Bonus level increases every 5 waves
        if (wave % 5 == 0) {
            bonusLevel += 10;
            Serial.printf("[RipOff] Bonus level increased to %d\n", bonusLevel);
        }
        
        Serial.printf("[RipOff] Wave %d start!\n", wave);
    }
    
    // Spawn enemies periodically
    if (enemiesSpawned < enemiesInWave && millis() - lastSpawnTime > 3000) {
        spawnEnemy();
        lastSpawnTime = millis();
    }
}

// ========== RENDER ==========

void render(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
// Draw fuel canisters
    for (int i = 0; i < RO_NUM_FUEL_CANISTERS; i++) {
        if (!fuelCanisters[i].stolen) {
            uint16_t color = fuelCanisters[i].beingTowed ? TFT_YELLOW : TFT_GREEN;
            drawFuelCanister(buffer, fuelCanisters[i].pos.x, fuelCanisters[i].pos.y, color);
        }
    }
    
    // Draw enemies
    for (int i = 0; i < RO_MAX_ENEMIES; i++) {
        if (enemies[i].alive) {
            uint16_t color = TFT_RED;
            if (enemies[i].type >= 4) color = TFT_MAGENTA;  // Higher types are different color
            
            drawEnemyTank(buffer, enemies[i].pos.x, enemies[i].pos.y, enemies[i].angle, enemies[i].type, color);
            
            // Draw tow line if carrying fuel
            if (enemies[i].hasFuel) {
                drawTowLine(buffer, enemies[i].pos.x, enemies[i].pos.y, 
                           fuelCanisters[enemies[i].fuelIndex].pos.x,
                           fuelCanisters[enemies[i].fuelIndex].pos.y, TFT_YELLOW);
            }
        }
    }
    
    // Draw bullets
    for (int i = 0; i < RO_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            drawBullet(buffer, bullets[i].pos.x, bullets[i].pos.y, bullets[i].fromPlayer);
        }
    }
    
    // Draw particles
    for (int i = 0; i < RO_MAX_PARTICLES; i++) {
        if (particles[i].active) {
            uint32_t age = millis() - particles[i].spawnTime;
            uint16_t color = TFT_WHITE;
            if (age > 200) color = TFT_DARKGREY;
            else if (age > 100) color = TFT_LIGHTGREY;
            buffer->fillCircle(particles[i].pos.x, particles[i].pos.y, 1, color);
        }
    }
    
    // Draw player
    if (player.alive) {
        // Flashing when invincible
        if (!player.invincible || (millis() / 100) % 2 == 0) {
            drawPlayerTank(buffer, player.pos.x, player.pos.y, player.angle, TFT_CYAN);
        }
    } else {
        // Explosion animation
        uint32_t age = millis() - player.deathTime;
        if (age < 500) {
            drawExplosion(buffer, player.pos.x, player.pos.y, age);
        }
    }
    
// HUD - Redesigned
    buffer->setTextColor(TFT_WHITE, TFT_BLACK);
    buffer->setTextDatum(TL_DATUM);
    buffer->setTextSize(1);
    
// Wave number - upper left (number only, smaller font)
buffer->drawString(String(wave), 10, 10, 2);
    
    // Score - top center
    buffer->setTextDatum(TC_DATUM);
    buffer->drawString(String(score), SCREEN_CENTER_X, 10, 4);
    
    // Lives icons - upper right (no text)
    buffer->setTextDatum(TR_DATUM);
    for (int i = 0; i < player.lives; i++) {
        drawPlayerTank(buffer, SCREEN_WIDTH - 20 - (i * 25), 20, 0, TFT_CYAN);
    }
}

void showGameOver(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    // Game Over text
    buffer->setTextColor(TFT_RED);
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextSize(1);
    buffer->drawString("GAME OVER", SCREEN_CENTER_X, SCREEN_CENTER_Y - 40, 7);
    
    // Final score
    buffer->setTextColor(TFT_WHITE);
    buffer->drawString("FINAL SCORE", SCREEN_CENTER_X, SCREEN_CENTER_Y + 30, 4);
    buffer->drawString(String(score), SCREEN_CENTER_X, SCREEN_CENTER_Y + 60, 4);
    
    buffer->drawString("WAVE: " + String(wave), SCREEN_CENTER_X, SCREEN_CENTER_Y + 90, 2);
    
    buffer->setTextColor(TFT_YELLOW);
    buffer->drawString("Press A to Restart", SCREEN_CENTER_X, SCREEN_CENTER_Y + 120, 2);
}

// ========== MAIN GAME LOOP ==========

void playGame(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[RipOff] Starting Rip Off...");
    
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
    
    // WAIT FOR ALL BUTTONS TO BE RELEASED BEFORE GAME STARTS
    delay(500);
    Serial.println("[RipOff] Ready to play!");
    
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
            
            // Check for restart
            if (!ss.digitalRead(5)) {  // Button A
                delay(200);
                initGame();
                gameState = STATE_PLAYING;
            }
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