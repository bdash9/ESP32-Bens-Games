#ifndef PSYCHO_FLIGHT_H
#define PSYCHO_FLIGHT_H

#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <SD.h>
#include <PNGdec.h>
#include "line_superman_center.h"
//#include "top_left_aorta.h"
//#include "top_right_atrium.h"
//#include "bottom_left_ventricle.h"
//#include "bottom_right_ventricle.h"
#include "castle.h"

// ========== PSYCHO FLIGHT - HEART COLLECTION GAME ==========

namespace PsychoFlight {

// SD Card pins
const int PIN_SD_CS   = 13;
const int PIN_SD_MOSI = 15;
const int PIN_SD_MISO = 2;
const int PIN_SD_SCLK = 14;

const int SCREEN_CENTER_X = 240;
const int SCREEN_CENTER_Y = 160;
const float MOVE_SPEED = 21.0;
const float TILT_SPEED = 0.15;
const float MAX_TILT = 25.0;
const float INERTIA = 0.88;
const float BOUNCE_DAMPING = 0.7;

const int SPRITE_WIDTH = 64;
const int SPRITE_HEIGHT = 64;
const int SPRITE_FRAMES = 4;

float playerX = 240;
float playerY = 160;
float velX = 0;
float velY = 0;
float tiltX = 0;
float tiltY = 0;
float cameraOffsetY = 0.0;  // Camera vertical offset based on player position


// ========== MOUNTAIN APPROACH & ORB COLLECTION ==========
float mountainDistance = 1.0;  // 1.0 = far away, 0.0 = at citadel
const float APPROACH_SPEED = 0.0008;  // Distance closes per frame (~60 seconds to reach)

struct GoldenOrb {
    int16_t x;
    int16_t y;
    bool collected;
    uint32_t spawnTime;
    float sparklePhase;
};

GoldenOrb goldenOrbs[4] = {
    {200, 280, false, 0, 0.0},        // Closer to water (was 180, now 280)
    {300, 270, false, 15000, 0.0},    // Closer to water
    {350, 290, false, 30000, 0.0},    // Closer to water
    {250, 275, false, 45000, 0.0}     // Closer to water
};

uint8_t orbsCollected = 0;
bool brainAssembled = false;  // This should stay false until animation completes
uint32_t brainAssemblyStartTime = 0;

enum GamePhase {
    PHASE_COLLECTING_HEARTS,
    PHASE_HEART_ASSEMBLY,
    PHASE_CAVE_ESCAPE,
    PHASE_MOUNTAIN_APPROACH,
    PHASE_BRAIN_ASSEMBLY,
    PHASE_CITADEL_REACHED
};

GamePhase gamePhase = PHASE_COLLECTING_HEARTS;

// ========== DRAW ANATOMICAL HEART (vector) ==========
void drawAnatomicalHeart(TFT_eSprite* buffer, int cx, int cy, float scale, uint16_t color) {
    int s = (int)(20 * scale);
    
    // Main ventricles (lower chambers)
    buffer->drawLine(cx - s, cy, cx, cy + s*2, color);
    buffer->drawLine(cx + s, cy, cx, cy + s*2, color);
    buffer->drawLine(cx - s, cy, cx - s/2, cy - s/2, color);
    buffer->drawLine(cx + s, cy, cx + s/2, cy - s/2, color);
    
    // Atria (upper rounded parts)
    for (int i = -s/2; i <= s/2; i += 2) {
        int leftY = cy - s/2 - (int)(sqrt(abs(s*s/4 - i*i)) * 0.7);
        buffer->drawPixel(cx - s/2 + i, leftY, color);
    }
    for (int i = -s/2; i <= s/2; i += 2) {
        int rightY = cy - s/2 - (int)(sqrt(abs(s*s/4 - i*i)) * 0.7);
        buffer->drawPixel(cx + s/2 + i, rightY, color);
    }
    
    // Aorta and vessels
    buffer->drawLine(cx - s/3, cy - s/2, cx - s/3, cy - s, color);
    buffer->drawLine(cx, cy - s/2, cx, cy - s - 5, color);
    buffer->drawLine(cx + s/3, cy - s/2, cx + s/3, cy - s, color);
    
    // Interior detail
    buffer->drawLine(cx, cy, cx, cy + s*2, color);
    buffer->drawLine(cx - s/2, cy + s, cx + s/2, cy + s, color);
}

bool gameOver = false;
bool playerInvincible = false;
unsigned long invincibleStartTime = 0;
unsigned long gameStartTime = 0;

TFT_eSprite* gameBuffer = nullptr;
TFT_eSprite* playerSprites[4] = {nullptr, nullptr, nullptr, nullptr};
TFT_eSprite* spriteSheet = nullptr;
bool spritesLoaded = false;

PNG png;
TFT_eSPI* tft_ptr = nullptr;

// ========== GAME STATES ==========
enum HeartGameState {
  COLLECTING_HEARTS,
  HEART_ASSEMBLY,
  CAVE_ESCAPE,
  EMERGENCE
};

HeartGameState gameState = COLLECTING_HEARTS;

// ========== HEART PIECE TRACKING ==========
struct HeartPiece {
  const uint8_t* bitmap;
  uint16_t width;
  uint16_t height;
  int16_t x;
  int16_t y;
  bool collected;
  uint32_t spawnTime;
  uint32_t respawnDelay;
};

HeartPiece heartPieces[4] = {
    {nullptr, 20, 20, 200, 100, false, 0, 0},        // Heart 1 - spawns immediately
    {nullptr, 20, 20, 500, 150, false, 10000, 0},    // Heart 2 - spawns at 10 seconds
    {nullptr, 20, 20, 800, 120, false, 20000, 0},    // Heart 3 - spawns at 20 seconds
    {nullptr, 20, 20, 1100, 140, false, 30000, 0}    // Heart 4 - spawns at 30 seconds
};

uint32_t heartActualSpawnTime[4] = {0, 0, 0, 0};  // Tracks when each heart actually spawned
uint8_t collectedCount = 0;
uint32_t assemblyStartTime = 0;

bool assemblyComplete = false;

// Castle (appears after collection)
int16_t castleX = 400;
int16_t castleY = 30;
bool showCastle = false;

// Cave mouth (for escape)
float caveMouthGap = 200; // Distance between upper and lower jaws
bool caveMouthActive = false;
uint32_t caveMouthStartTime = 0;

// Cave contour rings
struct CaveRing {
    float z;
    bool hasTooth;
    int toothAngle;
    int toothSize;
};

const int MAX_RINGS = 25;
CaveRing rings[MAX_RINGS];

// ========== TERRAIN & MOUNTAIN SYSTEM ==========
struct TerrainRing {
    float z;
    int altitude;
    float waviness;
};

struct Mountain {
    int x;              // Screen X position
    int baseHeight;     // How tall
    bool hasCitadel;    // Castle on top
};

const int MAX_TERRAIN_RINGS = 30;
TerrainRing terrainRings[MAX_TERRAIN_RINGS];

Mountain mountains[5];  // 5 mountains total
int visibleMountains = 5;  // Starts at 5, reduces as we get closer

float mountainProgress = 0.0;  // 0.0 to 1.0 over 60 seconds
float citadelLandingProgress = 0.0;

void initTerrain() {
    for (int i = 0; i < MAX_TERRAIN_RINGS; i++) {
        terrainRings[i].z = (float)i / MAX_TERRAIN_RINGS;
        terrainRings[i].altitude = random(-20, 40);
        terrainRings[i].waviness = random(5, 15);
    }
    
    // Initialize mountains - center one has citadel
    mountains[0].x = 100;  mountains[0].baseHeight = 60;  mountains[0].hasCitadel = false;
    mountains[1].x = 180;  mountains[1].baseHeight = 80;  mountains[1].hasCitadel = false;
    mountains[2].x = 240;  mountains[2].baseHeight = 100; mountains[2].hasCitadel = true;  // CENTER
    mountains[3].x = 300;  mountains[3].baseHeight = 75;  mountains[3].hasCitadel = false;
    mountains[4].x = 380;  mountains[4].baseHeight = 65;  mountains[4].hasCitadel = false;
    
    mountainProgress = 0.0;
    visibleMountains = 5;
    citadelLandingProgress = 0.0;
    
    // Reset for mountain approach phase
    gamePhase = PHASE_MOUNTAIN_APPROACH;
    caveMouthStartTime = millis();  // Reset timer for orb spawning
    mountainDistance = 1.0;
}

void updateTerrain() {
    // Move terrain forward
    for (int i = 0; i < MAX_TERRAIN_RINGS; i++) {
        terrainRings[i].z += 0.008;
        
        if (terrainRings[i].z > 1.0) {
            terrainRings[i].z = 0.0;
            terrainRings[i].altitude = random(-20, 40);
            terrainRings[i].waviness = random(5, 15);
        }
    }
    
    // Progress toward mountain (60 seconds)
    uint32_t elapsed = millis() - caveMouthStartTime;
    mountainProgress = min(1.0f, elapsed / 60000.0f);
    
    // Reduce visible mountains as we get closer
    if (mountainProgress < 0.3) {
        visibleMountains = 5;
    } else if (mountainProgress < 0.6) {
        visibleMountains = 3;  // Only center 3
    } else {
        visibleMountains = 1;  // Only citadel mountain
    }
    
    // Landing sequence (last 5 seconds)
    if (mountainProgress > 0.92) {
        citadelLandingProgress = (mountainProgress - 0.92) / 0.08;  // 0 to 1 over last 8%
    }
}

// ========== DRAW SKY WAVES WITH CAMERA OFFSET ==========
void drawSkyWaves(TFT_eSprite* buffer) {
    uint32_t time = millis();
    
    for (int layer = 0; layer < 8; layer++) {
        float z = layer / 8.0;
        int y = 10 + layer * 8 - (int)cameraOffsetY;  // **SUBTRACT CAMERA OFFSET**
        
        if (y < -10 || y > 100) continue;  // Skip if off-screen
        
        int lastX = 0;
        int lastY = y;
        
        for (int x = 0; x <= 480; x += 15) {
            float wave = sin((x + time * 0.05) * 0.02 + z * 3) * 5;
            int nextY = y + (int)wave;
            
            uint16_t skyColor = 0x18E3 + (layer * 0x0800);
            buffer->drawLine(lastX, lastY, x, nextY, skyColor);
            
            lastX = x;
            lastY = nextY;
        }
    }
}

// ========== DRAW ELEVEN-PEAK MOUNTAIN RANGE WITH APPROACH ==========
void drawMountains(TFT_eSprite* buffer) {
    // Scale based on distance (0.3 = far, 3.0 = very close)
    float scale = 0.3 + (1.0 - mountainDistance) * 2.7;
    
    // Mountains start where terrain ends
    int horizonY = 220 - (int)cameraOffsetY;
    
    // Determine visible mountains based on distance
    int visibleCount = 11;
    int startPeak = 0;
    
    if (mountainDistance < 0.7) {
        visibleCount = 7;  // Show 7 peaks
        startPeak = 2;
    }
    if (mountainDistance < 0.4) {
        visibleCount = 5;  // Show 5 peaks
        startPeak = 3;
    }
    if (mountainDistance < 0.2) {
        visibleCount = 3;  // Show 3 peaks (focus on center)
        startPeak = 4;
    }
    if (mountainDistance < 0.1) {
        visibleCount = 1;  // Only show tallest peak with citadel
        startPeak = 4;
    }
    
    // Define 11 mountain peaks
    struct Peak {
        int baseX;
        int baseHeight;
        int baseWidth;
    };
    
Peak peaks[11] = {
    {40, 60, 40},       // Far left
    {100, 80, 50},
    {160, 95, 55},
    {220, 110, 60},
    {280, 140, 70},     // Center/tallest (was 260)
    {340, 105, 60},
    {400, 90, 55},
    {460, 75, 50},      // Far right
    {380, 85, 45},
    {120, 70, 45},
    {500, 65, 40}       // Very far right
};
    
// Draw mountains
for (int i = startPeak; i < startPeak + visibleCount && i < 11; i++) {
    bool isMainPeak = (i == 4);
    
    // Spread mountains apart from center as we get closer
    float spreadFactor = 1.0f + (1.0f - mountainDistance) * 1.5f;
    int offsetFromCenter = peaks[i].baseX - 280;
    int peakX = 280 + (int)(offsetFromCenter * spreadFactor);
    
    // Limit scale for main peak so it doesn't go off screen
    float effectiveScale = scale;
    if (isMainPeak && effectiveScale > 2.0f) {
        effectiveScale = 2.0f;  // Cap main peak scale
    }
    
    int height = (int)(peaks[i].baseHeight * effectiveScale);
    int peakY = horizonY - height;
    int width = (int)(peaks[i].baseWidth * effectiveScale);
    
    // Ensure peak stays on screen
    if (peakY < -50 + (int)cameraOffsetY) {
        peakY = -50 + (int)cameraOffsetY;
    }
        
        // Left slope
        int leftX = peakX - width;
        uint16_t leftColor = isMainPeak ? 0x6B4D : 0x5ACB;
        buffer->fillTriangle(leftX, horizonY, peakX, peakY, peakX, horizonY, leftColor);
        
        // Right slope (darker)
        int rightX = peakX + width;
        uint16_t rightColor = isMainPeak ? 0x528A : 0x4A69;
        buffer->fillTriangle(peakX, peakY, rightX, horizonY, peakX, horizonY, rightColor);
        
        // Outlines
        buffer->drawLine(leftX, horizonY, peakX, peakY, isMainPeak ? TFT_WHITE : TFT_DARKGREY);
        buffer->drawLine(peakX, peakY, rightX, horizonY, isMainPeak ? TFT_WHITE : TFT_DARKGREY);
        
        // Snow cap on tallest peak
        if (isMainPeak && mountainDistance < 0.5) {
            int snowHeight = (int)(20 * scale);
            int snowWidth = (int)(15 * scale);
            int snowY = peakY + snowHeight;
            buffer->fillTriangle(peakX - snowWidth, snowY, peakX, peakY, peakX + snowWidth, snowY, TFT_WHITE);
        }
        
// Contour lines (short at top, long at bottom)
int numContours = 3 + (height / 50);
for (int c = 1; c <= numContours; c++) {
    // Calculate position from peak (0) to base (1)
    float t = (float)c / (numContours + 1);
    
    // Calculate Y position from peak down to horizon
    int contourY = peakY + (int)((horizonY - peakY) * t);
    
    // Calculate width at this height (grows from top to bottom)
    int contourWidth = (int)(width * t);
    
    int c_leftX = peakX - contourWidth;
    int c_rightX = peakX + contourWidth;
    
    buffer->drawLine(c_leftX, contourY, c_rightX, contourY, 
                   isMainPeak ? 0x5AEB : 0x39C7);
}
    }
}

// ========== UPDATE GOLDEN ORB SPAWNING ==========
void updateGoldenOrbSpawning() {
    uint32_t elapsed = millis() - caveMouthStartTime;  // Time since emerging from cave
    
    for (int i = 0; i < 4; i++) {
        if (!goldenOrbs[i].collected && elapsed >= goldenOrbs[i].spawnTime) {
            // Orb is active, update sparkle animation
            goldenOrbs[i].sparklePhase += 0.1;
        }
    }
}

// ========== DRAW GOLDEN ORBS WITH SPARKLE ==========
void drawGoldenOrbs(TFT_eSprite* buffer) {
    uint32_t elapsed = millis() - caveMouthStartTime;
    
    for (int i = 0; i < 4; i++) {
        if (!goldenOrbs[i].collected && elapsed >= goldenOrbs[i].spawnTime) {
            // Calculate growth (0 to 1 over 2 seconds)
            uint32_t timeSinceSpawn = elapsed - goldenOrbs[i].spawnTime;
            float growth = min(1.0f, timeSinceSpawn / 2000.0f);
            
            int x = goldenOrbs[i].x;
            int y = goldenOrbs[i].y - (int)cameraOffsetY;
            
            // Floating animation
            float bounce = sin(millis() * 0.003 + i) * 5;
            y += (int)bounce;
            
            // Size based on growth
            int radius = (int)(8 * growth);
            if (radius < 2) radius = 2;
            
            // Draw golden orb
            buffer->fillCircle(x, y, radius, TFT_GOLD);
            buffer->drawCircle(x, y, radius, TFT_YELLOW);
            
            // Only sparkle when full size
            if (growth >= 1.0f) {
                goldenOrbs[i].sparklePhase += 0.1;
                float sparkle = sin(goldenOrbs[i].sparklePhase);
                if (sparkle > 0.5) {
                    buffer->drawPixel(x - 6, y - 6, TFT_WHITE);
                    buffer->drawPixel(x + 6, y - 6, TFT_WHITE);
                    buffer->drawPixel(x - 6, y + 6, TFT_WHITE);
                    buffer->drawPixel(x + 6, y + 6, TFT_WHITE);
                }
                
                // Inner glow
                buffer->fillCircle(x - 2, y - 2, 3, TFT_YELLOW);
            }
        }
    }
}

// ========== CHECK GOLDEN ORB COLLISION ==========
void checkOrbCollision() {
    uint32_t elapsed = millis() - caveMouthStartTime;
    
    for (int i = 0; i < 4; i++) {
        if (!goldenOrbs[i].collected && elapsed >= goldenOrbs[i].spawnTime) {
            // Only collectible when fully grown (2 seconds after spawn)
            uint32_t timeSinceSpawn = elapsed - goldenOrbs[i].spawnTime;
            if (timeSinceSpawn < 2000) continue;  // Not fully grown yet
            
            int ox = goldenOrbs[i].x;
            int oy = goldenOrbs[i].y;
            
            float bounce = sin(millis() * 0.003 + i) * 5;
            oy += (int)bounce;
            
            if (playerX < ox + 15 && playerX + 30 > ox - 15 &&
                playerY < oy + 15 && playerY + 30 > oy - 15) {
                
                goldenOrbs[i].collected = true;
                orbsCollected++;
                
                Serial.print("[Orb] Collected orb ");
                Serial.print(i + 1);
                Serial.print(" - Total: ");
                Serial.println(orbsCollected);
                
                if (orbsCollected >= 4) {
                    Serial.println("[Orb] ALL ORBS COLLECTED! Starting brain assembly...");
                    gamePhase = PHASE_BRAIN_ASSEMBLY;
                    brainAssemblyStartTime = millis();
                }
            }
        }
    }
}

// ========== CHECK MOUNTAIN LANDING ==========
void checkMountainLanding() {
    if (mountainDistance > 0.15f) return;  // Only when very close
    
    // Main peak position (peak #4)
    float spreadFactor = 1.0f + (1.0f - mountainDistance) * 1.5f;
    int offsetFromCenter = 280 - 280;  // Center peak
    int peakX = 280 + (int)(offsetFromCenter * spreadFactor);
    
    int horizonY = 220 - (int)cameraOffsetY;
    float scale = 0.3 + (1.0 - mountainDistance) * 2.7;
    if (scale > 2.0f) scale = 2.0f;
    
    int height = (int)(140 * scale);
    int peakY = horizonY - height;
    int width = (int)(70 * scale);
    
    // Check if player is near peak top
    int landingZoneY = peakY - 20;  // 20 pixels above peak
    int landingZoneX = peakX;
    
    if (playerX > landingZoneX - 40 && playerX < landingZoneX + 40 &&
        playerY > landingZoneY - 20 && playerY < landingZoneY + 40) {
        
        // Player is landing on mountain!
        if (gamePhase == PHASE_MOUNTAIN_APPROACH) {
            gamePhase = PHASE_CITADEL_REACHED;
            Serial.println("[Mountain] Landed on peak! Victory!");
            
            // Gentle landing
            playerY = landingZoneY;
            velY = 0;
            velX *= 0.8;
        }
    }
}

// ========== DRAW GOLDEN BRAIN ==========
void drawGoldenBrain(TFT_eSprite* buffer, int cx, int cy, float scale) {
    int size = (int)(20 * scale);
    
    // Brain outline (two hemispheres)
    buffer->fillCircle(cx - size/4, cy, size/2, TFT_GOLD);
    buffer->fillCircle(cx + size/4, cy, size/2, TFT_GOLD);
    
    // Brain wrinkles/folds
    buffer->drawLine(cx - size/3, cy - size/4, cx - size/6, cy, 0xC618);
    buffer->drawLine(cx - size/6, cy, cx - size/4, cy + size/4, 0xC618);
    buffer->drawLine(cx + size/6, cy - size/3, cx + size/4, cy, 0xC618);
    buffer->drawLine(cx + size/4, cy, cx + size/3, cy + size/3, 0xC618);
    
    // Glow
    buffer->drawCircle(cx, cy, size/2 + 2, TFT_YELLOW);
}

// ========== ANIMATE BRAIN ASSEMBLY ==========
void animateBrainAssembly(TFT_eSprite* buffer) {
    uint32_t elapsed = millis() - brainAssemblyStartTime;
    
    buffer->fillScreen(TFT_BLACK);
    
    int16_t centerX = SCREEN_CENTER_X;
    int16_t centerY = SCREEN_CENTER_Y;
    
    if (elapsed < 2000) {
        // Phase 1: 4 orbs fly together
        float progress = elapsed / 2000.0;
        progress = progress * progress;
        
        int positions[4][2] = {{-100, -100}, {100, -100}, {-100, 100}, {100, 100}};
        
        for (int i = 0; i < 4; i++) {
            int16_t x = centerX + (int)(positions[i][0] * (1 - progress));
            int16_t y = centerY + (int)(positions[i][1] * (1 - progress));
            
            buffer->fillCircle(x, y, 8, TFT_GOLD);
            buffer->drawCircle(x, y, 8, TFT_YELLOW);
        }
        
    } else if (elapsed < 4500) {
        // Phase 2: Form into golden brain
        uint32_t beatTime = elapsed % 600;
        float pulse = 1.0;
        if (beatTime < 150) {
            pulse = 1.0 + (beatTime / 150.0) * 0.2;
        }
        
        // Draw brain shape
        drawGoldenBrain(buffer, centerX, centerY, 1.5 * pulse);
        
} else {
    // Transition back to mountain approach
    brainAssembled = true;  // NOW set to true
    gamePhase = PHASE_MOUNTAIN_APPROACH;
    gameState = EMERGENCE;  // Make sure we stay in EMERGENCE
    Serial.println("[Brain] Brain complete! Continue to citadel...");
}
}


// ========== DRAW CITADEL WITH CAMERA OFFSET ==========
void drawCitadel(TFT_eSprite* buffer) {
    float scale = 0.3 + mountainProgress * 1.5;
    int horizonY = 180 - (int)(mountainProgress * 80) - (int)cameraOffsetY;  // **SUBTRACT CAMERA**
    
    int peakX = 240;
    int peakHeight = (int)(100 * scale);
    int peakY = horizonY - peakHeight;
    int citadelY = peakY - (int)(25 * scale);
    
    int size = (int)(20 * scale);
    
    uint16_t citadelColor = 0x07FF;
    
    // Central tower
    buffer->drawLine(peakX - size/2, citadelY + size, peakX - size/2, citadelY - size, citadelColor);
    buffer->drawLine(peakX + size/2, citadelY + size, peakX + size/2, citadelY - size, citadelColor);
    buffer->drawLine(peakX - size/2, citadelY - size, peakX + size/2, citadelY - size, citadelColor);
    
    if (scale > 0.8) {
        buffer->drawLine(peakX - size, citadelY, peakX - size, citadelY - size/2, citadelColor);
        buffer->drawLine(peakX + size, citadelY, peakX + size, citadelY - size/2, citadelColor);
        
        buffer->drawLine(peakX, citadelY - size, peakX, citadelY - size - 10, TFT_YELLOW);
        buffer->drawPixel(peakX, citadelY - size - 10, TFT_RED);
        
        buffer->drawLine(peakX - size/2, citadelY, peakX + size/2, citadelY, TFT_YELLOW);
    }
}

// ========== DRAW TERRAIN WITH CAMERA OFFSET AND WATER ==========
void drawTerrainContours(TFT_eSprite* buffer) {
    for (int i = 0; i < MAX_TERRAIN_RINGS; i++) {
        float z = terrainRings[i].z;
        if (z < 0.2 || z > 1.0) continue;
        
        int baseY = 220 + (int)(z * 80) - (int)(cameraOffsetY * 0.7);  // **SUBTRACT CAMERA (less parallax)**
        baseY += terrainRings[i].altitude;
        
        int width = 80 + (int)(z * 400);
        
        int segments = 32;
        int lastX = SCREEN_CENTER_X - width;
        int lastY = baseY;
        
        // **ADD WATER WHEN FLYING LOW** (if camera is down, show blue water)
        uint16_t color;
        if (cameraOffsetY < -30 && z > 0.6) {
            // Water (blue) when looking down
            color = 0x001F + (int)(z * 0x0800);
        } else {
            // Land (green)
            color = 0x0300 + (int)(z * 0x2400);
        }
        
        for (int s = 1; s <= segments; s++) {
            float angle = (float)s / segments * 2.0;
            float wave = sin(angle * 4 + z * 10) * terrainRings[i].waviness * z;
            
            int nextX = SCREEN_CENTER_X - width + (int)(angle * width);
            int nextY = baseY + (int)wave;
            
            buffer->drawLine(lastX, lastY, nextX, nextY, color);
            
            if (z > 0.7) {
                buffer->drawLine(lastX, lastY + 1, nextX, nextY + 1, color);
            }
            
            lastX = nextX;
            lastY = nextY;
        }
    }
}

// ========== SD CARD & PNG (keeping your existing code) ==========
void * pngOpen(const char *filename, int32_t *size) {
    Serial.print("PNG: Opening file: ");
    Serial.println(filename);
    File *f = new File(SD.open(filename, "r"));
    if (f && *f) {
        *size = f->size();
        Serial.print("PNG: File size: ");
        Serial.println(*size);
        return (void *)f;
    }
    Serial.println("PNG: Failed to open file");
    delete f;
    return NULL;
}

void pngClose(void *handle) {
    File *f = (File *)handle;
    if (f) {
        f->close();
        delete f;
    }
}

int32_t pngRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
    if (!handle) return 0;
    File *f = (File *)handle->fHandle;
    return f->read(buffer, length);
}

int32_t pngSeek(PNGFILE *handle, int32_t position) {
    if (!handle) return 0;
    File *f = (File *)handle->fHandle;
    return f->seek(position);
}

void pngDraw(PNGDRAW *pDraw) {
    uint16_t lineBuffer[256];
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    
    if (spriteSheet) {
        spriteSheet->pushImage(0, pDraw->y, pDraw->iWidth, 1, lineBuffer);
    }
}

bool loadSpriteSheet(TFT_eSPI &tft) {
    Serial.println("[PsychoFlight] Loading sprite sheet from SD card...");
    
    SPI.begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    Serial.println("[PsychoFlight] SPI.begin done.");
    
    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("[PsychoFlight] SD.begin FAILED!");
        return false;
    }
    Serial.println("[PsychoFlight] SD.begin succeeded.");
    
    if (!SD.exists("/superman_sprite.png")) {
        Serial.println("[PsychoFlight] superman_sprite.png not found on SD card!");
        return false;
    }
    
    Serial.println("[PsychoFlight] Found superman_sprite.png");
    
    spriteSheet = new TFT_eSprite(&tft);
    spriteSheet->setColorDepth(16);
    spriteSheet->createSprite(256, 64);
    spriteSheet->fillSprite(TFT_BLACK);
    
    tft_ptr = &tft;
    
    int16_t rc = png.open("/superman_sprite.png", pngOpen, pngClose, pngRead, pngSeek, pngDraw);
    
    if (rc == PNG_SUCCESS) {
        Serial.println("[PsychoFlight] PNG opened successfully");
        Serial.printf("[PsychoFlight] Image specs: (%d x %d), %d bpp, pixel type: %d\n",
                      png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        
        rc = png.decode(NULL, 0);
        png.close();
        
        if (rc != PNG_SUCCESS) {
            Serial.printf("[PsychoFlight] PNG decode failed with code %d\n", rc);
            return false;
        }
        
        Serial.println("[PsychoFlight] PNG decoded successfully!");
        
        for (int i = 0; i < SPRITE_FRAMES; i++) {
            playerSprites[i] = new TFT_eSprite(&tft);
            playerSprites[i]->setColorDepth(16);
            playerSprites[i]->createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
            playerSprites[i]->fillSprite(TFT_BLACK);
            
            for (int y = 0; y < SPRITE_HEIGHT; y++) {
                for (int x = 0; x < SPRITE_WIDTH; x++) {
                    uint16_t color = spriteSheet->readPixel(i * SPRITE_WIDTH + x, y);
                    playerSprites[i]->drawPixel(x, y, color);
                }
            }
            
            Serial.printf("[PsychoFlight] Extracted sprite frame %d\n", i);
        }
        
        Serial.println("[PsychoFlight] Freeing sprite sheet to save memory...");
        spriteSheet->deleteSprite();
        delete spriteSheet;
        spriteSheet = nullptr;
        
        Serial.println("[PsychoFlight] All sprite frames extracted successfully!");
        return true;
        
    } else {
        Serial.printf("[PsychoFlight] PNG open failed with code %d\n", rc);
        return false;
    }
}

void initSprites(TFT_eSPI &tft) {
    spritesLoaded = loadSpriteSheet(tft);
    
    if (!spritesLoaded) {
        Serial.println("[PsychoFlight] Using fallback drawn sprites");
    }
    
    Serial.printf("[PsychoFlight] Free heap after sprite load: %d bytes\n", ESP.getFreeHeap());
}

void cleanupSprites() {
    if (gameBuffer) {
        gameBuffer->deleteSprite();
        delete gameBuffer;
        gameBuffer = nullptr;
    }
    
    for (int i = 0; i < SPRITE_FRAMES; i++) {
        if (playerSprites[i]) {
            playerSprites[i]->deleteSprite();
            delete playerSprites[i];
            playerSprites[i] = nullptr;
        }
    }
    
    if (spriteSheet) {
        spriteSheet->deleteSprite();
        delete spriteSheet;
        spriteSheet = nullptr;
    }
}

int getSpriteFrame() {
    if (abs(velX) > abs(velY)) {
        return (velX < 0) ? 0 : 1;
    } else if (abs(velY) > 2.0) {
        return (velY < 0) ? 2 : 3;
    } else {
        return 2;
    }
}

// ========== DRAW BITMAP CHARACTER ==========
void drawBitmapCharacter(TFT_eSprite* buffer, int cx, int cy, const uint8_t* bitmap, uint16_t w, uint16_t h, uint16_t color) {
    int startX = cx - (w / 2);
    int startY = cy - (h / 2);
    
    if (playerInvincible && ((millis() - invincibleStartTime) / 100) % 2 == 0) {
        color = TFT_WHITE;
    }
    
    for (uint16_t y = 0; y < h; y++) {
        for (uint16_t x = 0; x < w; x++) {
            uint16_t pixelIndex = y * w + x;
            uint16_t byteIndex = pixelIndex / 8;
            uint8_t bitIndex = 7 - (pixelIndex % 8);
            
            uint8_t bit = (bitmap[byteIndex] >> bitIndex) & 0x01;
            
            if (bit == 1) {
                int drawX = startX + x;
                int drawY = startY + y;
                
                if (drawX >= 0 && drawX < 480 && drawY >= 0 && drawY < 320) {
                    buffer->drawPixel(drawX, drawY, color);
                }
            }
        }
    }
    
// **Draw small heart on CHEST after assembly is complete**
if (collectedCount >= 4 && gameState != HEART_ASSEMBLY && assemblyComplete) {
    int heartX = cx;      // Center of chest
    int heartY = cy - 8;  // Chest area
    drawAnatomicalHeart(buffer, heartX, heartY, 0.15, TFT_RED);  // Smaller size

// Draw small golden brain on head if assembled
if (brainAssembled && gamePhase == PHASE_MOUNTAIN_APPROACH) {  // Only after assembly, during mountain approach
    int brainX = cx;
    int brainY = cy - 22;
    drawGoldenBrain(buffer, brainX, brainY, 0.35);
}
}
}

// ========== HEART SPAWNING & COLLECTION ==========
void initHeartGame() {
    gameState = COLLECTING_HEARTS;
    gameStartTime = millis();
    collectedCount = 0;
    showCastle = false;
    caveMouthActive = false;
    assemblyStartTime = 0;
    brainAssembled = false;
    orbsCollected = 0;
    gamePhase = PHASE_COLLECTING_HEARTS;
    
    for (int i = 0; i < 4; i++) {
        heartPieces[i].collected = false;
        heartPieces[i].x = -100;
        heartPieces[i].y = -100;
        heartActualSpawnTime[i] = 0;  // Reset spawn times
        goldenOrbs[i].collected = false;
    }
    initTerrain(); 
}

void updateHeartSpawning() {
    if (gameState != COLLECTING_HEARTS) return;
    
    uint32_t elapsed = millis() - gameStartTime;
    
    for (int i = 0; i < 4; i++) {
        if (!heartPieces[i].collected && elapsed >= heartPieces[i].spawnTime && heartPieces[i].x < 0) {
            // Spawn in safe zone (avoid edges)
            heartPieces[i].x = random(100, 380);
            heartPieces[i].y = random(80, 240);
            heartActualSpawnTime[i] = millis();  // Record when spawned
            Serial.printf("[Heart] Spawned piece %d at (%d, %d)\n", i, heartPieces[i].x, heartPieces[i].y);
        }
    }
}

// ========== DRAW HEART PIECES (small anatomical hearts that grow) ==========
void drawHeartPieces(TFT_eSprite* buffer) {
    if (gameState != COLLECTING_HEARTS) return;
    
    for (int i = 0; i < 4; i++) {
        if (!heartPieces[i].collected && heartPieces[i].x > 0) {
            // Calculate growth (0 to 1 over 2 seconds)
            uint32_t timeSinceSpawn = millis() - heartActualSpawnTime[i];
            float growth = min(1.0f, timeSinceSpawn / 2000.0f);
            
            // Ease in cubic for smoother growth
            growth = growth * growth * growth;
            
            // Scale from 0.05 (tiny) to 0.2 (normal size)
            float scale = 0.05f + (growth * 0.15f);
            
            // Add subtle floating animation
            float bounce = sin(millis() * 0.002f + i) * 3;
            int drawY = heartPieces[i].y + (int)bounce;
            
            // Draw heart with growing scale
            drawAnatomicalHeart(buffer, heartPieces[i].x, drawY, scale, TFT_RED);
            
            // Add glow effect when fully grown
            if (growth >= 1.0f) {
                // Pulsing glow
                float pulse = (sin(millis() * 0.003f + i) + 1.0f) * 0.5f;
                if (pulse > 0.7f) {
                    drawAnatomicalHeart(buffer, heartPieces[i].x, drawY, scale * 1.1f, 0x8000);
                }
            }
        }
    }
}

void checkHeartCollision() {
    for (int i = 0; i < 4; i++) {
        if (!heartPieces[i].collected && heartPieces[i].x > 0) {
            // Only collectible when fully grown (2 seconds after spawn)
            uint32_t timeSinceSpawn = millis() - heartActualSpawnTime[i];
            if (timeSinceSpawn < 2000) continue;  // Not fully grown yet
            
            int16_t hx = heartPieces[i].x;
            int16_t hy = heartPieces[i].y;
            
            // Add floating offset to match rendering
            float bounce = sin(millis() * 0.002f + i) * 3;
            hy += (int)bounce;
            
            // Check collision with character
            if (playerX < hx + 20 && playerX + 30 > hx &&
                playerY < hy + 20 && playerY + 30 > hy) {
                
                heartPieces[i].collected = true;
                collectedCount++;
                
                Serial.print("[Heart] Collected heart ");
                Serial.print(i + 1);
                Serial.print(" - Total: ");
                Serial.println(collectedCount);
                
                // Check if all hearts collected
                if (collectedCount >= 4) {
                    Serial.println("[Heart] ALL HEARTS COLLECTED! Starting assembly...");
                    gameState = HEART_ASSEMBLY;
                    assemblyStartTime = millis();
                }
            }
        }
    }
}

// ========== HEART ASSEMBLY ANIMATION ==========
void animateHeartAssembly(TFT_eSprite* buffer) {
    uint32_t elapsed = millis() - assemblyStartTime;
    
    // Draw dark cave background so screen isn't black
    buffer->fillScreen(TFT_BLACK);
    
    int16_t centerX = SCREEN_CENTER_X;
    int16_t centerY = SCREEN_CENTER_Y;
        
    
    if (elapsed < 2000) {
        // **Phase 1**: 4 small hearts fly together (2 seconds)
        float progress = elapsed / 2000.0;
        progress = progress * progress;
        
        // Top-left
        int16_t tl_x = (int)(centerX + (1 - progress) * -150);
        int16_t tl_y = (int)(centerY + (1 - progress) * -150);
        
        // Top-right
        int16_t tr_x = (int)(centerX + (1 - progress) * 150);
        int16_t tr_y = (int)(centerY + (1 - progress) * -150);
        
        // Bottom-left
        int16_t bl_x = (int)(centerX + (1 - progress) * -150);
        int16_t bl_y = (int)(centerY + (1 - progress) * 150);
        
        // Bottom-right
        int16_t br_x = (int)(centerX + (1 - progress) * 150);
        int16_t br_y = (int)(centerY + (1 - progress) * 150);
        
        // Draw 4 small hearts converging
        drawAnatomicalHeart(buffer, tl_x, tl_y, 0.6, TFT_RED);
        drawAnatomicalHeart(buffer, tr_x, tr_y, 0.6, TFT_RED);
        drawAnatomicalHeart(buffer, bl_x, bl_y, 0.6, TFT_RED);
        drawAnatomicalHeart(buffer, br_x, br_y, 0.6, TFT_RED);
        
    } else if (elapsed < 4500) {
        // **Phase 2**: ONE BIG BEATING HEART (2.5 seconds)
        
        // Beat timing
        uint32_t beatTime = elapsed % 600;
        float pulse = 1.0;
        if (beatTime < 150) {
            pulse = 1.0 + (beatTime / 150.0) * 0.25;  // Grow
        } else if (beatTime < 300) {
            pulse = 1.25 - ((beatTime - 150) / 150.0) * 0.25;  // Shrink
        }
        
        // Pulse between red and pink
        uint16_t heartColor = (beatTime < 300) ? TFT_RED : 0xF810;  // Red/Pink
        
        // Draw one large beating heart
        drawAnatomicalHeart(buffer, centerX, centerY, 1.5 * pulse, heartColor);
        

} else {
    // **Transition to cave escape** (fly through cave)
    assemblyComplete = true;
    gameState = CAVE_ESCAPE;
    caveMouthActive = false;  // ← Start FALSE - teeth appear later
    caveMouthStartTime = millis();
    
    // Re-initialize cave rings so they're visible
    for (int i = 0; i < MAX_RINGS; i++) {
        rings[i].z = 50 + i * 20;
        rings[i].hasTooth = (i % 3 == 0);
        rings[i].toothAngle = random(0, 360);
        rings[i].toothSize = 20 + random(-5, 5);
    }
    
    Serial.println("[Heart] Heart complete! Starting cave escape...");
    Serial.println("[Cave] Flying through cave...");
}
}

// ========== DRAW CASTLE IN DISTANCE ==========
void drawCastle(TFT_eSprite* buffer) {
    if (!showCastle) return;
    
    // Draw small and faded for distance
    drawBitmapCharacter(buffer, castleX, castleY,
                       castle_bitmap,
                       castle_width / 2, // Half size for distance
                       castle_height / 2,
                       TFT_DARKGREY);
}

// ========== CAVE RING CODE (keeping your existing) ==========
int getToothSpawnRate() {
    unsigned long elapsedTime = millis() - gameStartTime;
    
    if (gameState != COLLECTING_HEARTS) return 15; // Fewer during collection
    
    if (elapsedTime < 10000) return 25;
    else if (elapsedTime < 20000) return 35;
    else if (elapsedTime < 30000) return 45;
    else if (elapsedTime < 45000) return 55;
    else return 65;
}

void initCaveRings() {
    for (int i = 0; i < MAX_RINGS; i++) {
        rings[i].z = (float)i / MAX_RINGS;
        rings[i].hasTooth = (random(0, 100) < 25);
        rings[i].toothAngle = random(0, 12);
        rings[i].toothSize = random(0, 4);
    }
}

void updateCaveRings() {
    for (int i = 0; i < MAX_RINGS; i++) {
        rings[i].z += 0.005;
        
        if (rings[i].z > 1.0) {
            rings[i].z = 0.0;
            int spawnRate = getToothSpawnRate();
            rings[i].hasTooth = (random(0, 100) < spawnRate);
            rings[i].toothAngle = random(0, 12);
            rings[i].toothSize = random(0, 4);
        }
    }
}

// ========== DRAW CAVE RING (existing function) ==========
void drawCaveRing(TFT_eSprite* buffer, float zPos, uint16_t color) {
    if (zPos < 0 || zPos > 1.0) return;
    
    int radiusX = 40 + (int)(zPos * 450);
    int radiusY = 25 + (int)(zPos * 280);
    
    int segments = 32;
    int lastX = SCREEN_CENTER_X + radiusX;
    int lastY = SCREEN_CENTER_Y;
    
    for (int i = 1; i <= segments; i++) {
        float angle = (float)i * 2.0 * PI / segments;
        
        float rockiness = 2 + zPos * 4;
        float wave = sin(angle * 6 + zPos * 8) * rockiness;
        float jag = sin(angle * 15 + zPos * 20) * (rockiness * 0.5);
        
        int nextX = SCREEN_CENTER_X + (int)((radiusX + wave + jag) * cos(angle));
        int nextY = SCREEN_CENTER_Y + (int)((radiusY + wave + jag) * sin(angle));
        
        buffer->drawLine(lastX, lastY, nextX, nextY, color);
        
        if (zPos > 0.5) buffer->drawLine(lastX - 1, lastY, nextX - 1, nextY, color);
        if (zPos > 0.7) buffer->drawLine(lastX + 1, lastY, nextX + 1, nextY, color);
        
        lastX = nextX;
        lastY = nextY;
    }
}

// ========== DRAW CAVE MOUTH WITH OUTSIDE VIEW THROUGH OPENING ==========
void drawCaveMouthRing(TFT_eSprite* buffer) {
    if (!caveMouthActive) return;
    
    uint32_t elapsed = millis() - caveMouthStartTime;
    float zPos = 0.95;
    
    int radiusX = 40 + (int)(zPos * 450);
    int radiusY = 25 + (int)(zPos * 280);
    
    // ========== DRAW OUTSIDE WORLD (OPTIMIZED - draw shapes instead of pixels) ==========
    
    // Draw sky (blue ellipse background)
    buffer->fillEllipse(SCREEN_CENTER_X, SCREEN_CENTER_Y, (int)(radiusX * 0.7), (int)(radiusY * 0.7), 0x18E3);
    
    // **DISTANT MOUNTAINS** (simple triangles)
    for (int i = 0; i < 5; i++) {
        int mx = SCREEN_CENTER_X - 150 + i * 80;
        int my = SCREEN_CENTER_Y - 30 + (i % 2) * 20;
        int baseY = SCREEN_CENTER_Y + 20;
        buffer->fillTriangle(mx - 40, baseY, mx, my, mx + 40, baseY, TFT_DARKGREY);
    }
    
    // **GREEN TERRAIN CONTOURS** (animated horizontal lines)
    uint32_t time = millis();
    for (int layer = 0; layer < 8; layer++) {
        float z = (time * 0.003 + layer * 0.15);
        z = z - (int)z;  // Wrap 0-1
        
        int contourY = SCREEN_CENTER_Y + (int)(z * 100);
        int leftX = SCREEN_CENTER_X - (int)(radiusX * 0.6 * z);
        int rightX = SCREEN_CENTER_X + (int)(radiusX * 0.6 * z);
        
        uint16_t greenShade = 0x0300 + (int)(z * 0x2000);
        
        // Draw wavy horizontal line
        for (int x = leftX; x < rightX; x += 2) {
            float wave = sin((x + z * 100) * 0.05) * (5 * z);
            int finalY = contourY + (int)wave;
            buffer->drawPixel(x, finalY, greenShade);
            buffer->drawPixel(x, finalY + 1, greenShade);
        }
    }
    
    // ========== DRAW TEETH AROUND THE OPENING ==========
    int numTeeth = 24;
    for (int i = 0; i < numTeeth; i++) {
        float angle = (i / (float)numTeeth) * TWO_PI;
        
        // Tooth base position (on the ellipse edge)
        int baseX = SCREEN_CENTER_X + (int)(radiusX * 0.7 * cos(angle));
        int baseY = SCREEN_CENTER_Y + (int)(radiusY * 0.7 * sin(angle));
        
        // Tooth tip position (pointing inward)
        int tipX = SCREEN_CENTER_X + (int)(radiusX * 0.4 * cos(angle));
        int tipY = SCREEN_CENTER_Y + (int)(radiusY * 0.4 * sin(angle));
        
        // Draw tooth triangle (white/grey)
        int perpX = -(int)(10 * sin(angle));
        int perpY = (int)(10 * cos(angle));
        
        buffer->fillTriangle(
            baseX + perpX, baseY + perpY,
            baseX - perpX, baseY - perpY,
            tipX, tipY,
            0xE73C  // Light grey/white
        );
        
        // Outline
        buffer->drawTriangle(
            baseX + perpX, baseY + perpY,
            baseX - perpX, baseY - perpY,
            tipX, tipY,
            TFT_WHITE
        );
    }
    
    // Draw cave ring around the outside
    drawCaveRing(buffer, zPos, 0xAD75);
}

void drawTooth(TFT_eSprite* buffer, float zPos, int toothAngle, int toothSize, uint16_t fillColor, uint16_t edgeColor) {
    if (zPos < 0.25 || zPos > 0.85) return;
    
    int radiusX = 40 + (int)(zPos * 450);
    int radiusY = 25 + (int)(zPos * 280);
    
    float toothAngleRad = toothAngle * PI / 6.0;
    
    float baseLength, widthMultiplier, inwardRatio;
    if (toothSize == 0) {
        baseLength = 20.0; widthMultiplier = 0.5; inwardRatio = 0.25;
    } else if (toothSize == 1) {
        baseLength = 45.0; widthMultiplier = 0.9; inwardRatio = 0.45;
    } else if (toothSize == 2) {
        baseLength = 75.0; widthMultiplier = 1.3; inwardRatio = 0.65;
    } else if (toothSize == 3) {
        baseLength = 110.0; widthMultiplier = 1.8; inwardRatio = 0.80;
    } else {
        baseLength = 150.0; widthMultiplier = 2.3; inwardRatio = 0.90;
    }
    
    int toothLen = (int)(baseLength * (1.3 - zPos));
    
    if (toothLen > 12) {
        int baseX = SCREEN_CENTER_X + (int)(radiusX * cos(toothAngleRad));
        int baseY = SCREEN_CENTER_Y + (int)(radiusY * sin(toothAngleRad));
        
        int tipX = baseX + (int)((SCREEN_CENTER_X - baseX) * inwardRatio);
        int tipY = baseY + (int)((SCREEN_CENTER_Y - baseY) * inwardRatio);
        
        int perpX = (int)(-sin(toothAngleRad) * 4 * widthMultiplier);
        int perpY = (int)(cos(toothAngleRad) * 4 * widthMultiplier);
        
        int steps = (int)(3 * widthMultiplier);
        for (int s = -steps; s <= steps; s++) {
            int offsetX = perpX * s / steps;
            int offsetY = perpY * s / steps;
            buffer->drawLine(baseX + offsetX, baseY + offsetY, tipX, tipY, fillColor);
        }
        
        buffer->drawLine(baseX + perpX, baseY + perpY, tipX, tipY, edgeColor);
        buffer->drawLine(baseX - perpX, baseY - perpY, tipX, tipY, edgeColor);
        buffer->drawPixel(tipX, tipY, edgeColor);
    }
}

uint16_t getRingColor(float z) {
    if (z < 0.2) return 0x2104;
    else if (z < 0.4) return 0x4208;
    else if (z < 0.6) return 0x632C;
    else if (z < 0.8) return 0x8C51;
    else return 0xAD75;
}

void checkAndBounce() {
    if (playerInvincible && millis() - invincibleStartTime < 500) return;
    
    for (int i = 0; i < MAX_RINGS; i++) {
        if (rings[i].z > 0.80 && rings[i].z < 1.0) {
            int radiusX = 40 + (int)(rings[i].z * 450);
            int radiusY = 25 + (int)(rings[i].z * 280);
            
            float dx = (playerX - SCREEN_CENTER_X) / (float)radiusX;
            float dy = (playerY - SCREEN_CENTER_Y) / (float)radiusY;
            float distFromCenter = sqrt(dx * dx + dy * dy);
            
            if (distFromCenter > 0.65) {
                float angle = atan2(playerY - SCREEN_CENTER_Y, playerX - SCREEN_CENTER_X);
                velX = -cos(angle) * abs(velX) * BOUNCE_DAMPING;
                velY = -sin(angle) * abs(velY) * BOUNCE_DAMPING;
                
                float safeDist = 0.60;
                playerX = SCREEN_CENTER_X + cos(angle) * safeDist * radiusX;
                playerY = SCREEN_CENTER_Y + sin(angle) * safeDist * radiusY;
                
                playerInvincible = true;
                invincibleStartTime = millis();
                return;
            }
            
            if (rings[i].hasTooth) {
                float toothAngleRad = rings[i].toothAngle * PI / 6.0;
                int baseX = SCREEN_CENTER_X + (int)(radiusX * cos(toothAngleRad));
                int baseY = SCREEN_CENTER_Y + (int)(radiusY * sin(toothAngleRad));
                
                float inwardRatio, hitRadius;
                if (rings[i].toothSize == 0) {
                    inwardRatio = 0.25; hitRadius = 25;
                } else if (rings[i].toothSize == 1) {
                    inwardRatio = 0.45; hitRadius = 30;
                } else if (rings[i].toothSize == 2) {
                    inwardRatio = 0.65; hitRadius = 40;
                } else if (rings[i].toothSize == 3) {
                    inwardRatio = 0.80; hitRadius = 50;
                } else {
                    inwardRatio = 0.90; hitRadius = 60;
                }
                
                int tipX = baseX + (int)((SCREEN_CENTER_X - baseX) * inwardRatio);
                int tipY = baseY + (int)((SCREEN_CENTER_Y - baseY) * inwardRatio);
                
                float distToTooth = sqrt((playerX - tipX) * (playerX - tipX) + 
                                        (playerY - tipY) * (playerY - tipY));
                
                if (distToTooth < hitRadius) {
                    float toothDirX = tipX - baseX;
                    float toothDirY = tipY - baseY;
                    
                    float toothLen = sqrt(toothDirX * toothDirX + toothDirY * toothDirY);
                    if (toothLen > 0) {
                        toothDirX /= toothLen;
                        toothDirY /= toothLen;
                    }
                    
                    float perpDirX = -toothDirY;
                    float perpDirY = toothDirX;
                    
                    float playerRelX = playerX - tipX;
                    float playerRelY = playerY - tipY;
                    float dotProduct = playerRelX * perpDirX + playerRelY * perpDirY;
                    
                    if (dotProduct < 0) {
                        perpDirX = -perpDirX;
                        perpDirY = -perpDirY;
                    }
                    
                    velX = perpDirX * MOVE_SPEED * 1.2;
                    velY = perpDirY * MOVE_SPEED * 1.2;
                    
                    playerX = tipX + perpDirX * (hitRadius + 15);
                    playerY = tipY + perpDirY * (hitRadius + 15);
                    
                    playerInvincible = true;
                    invincibleStartTime = millis();
                    return;
                }
            }
        }
    }
    
    if (playerInvincible && millis() - invincibleStartTime > 500) {
        playerInvincible = false;
    }
}

// ========== DRAW DETAILED CLAWED HAND WITH MUSCULAR ARM ==========
void drawClawedHand(TFT_eSprite* buffer, int cx, int cy, float closedness, float entryProgress) {
    // closedness: 0.0 (open) to 1.0 (closed)
    // entryProgress: 0.0 (off screen left) to 1.0 (at heart)
    
    int palmX = cx - 60 + (int)(entryProgress * 60);  // Moves from left
    int palmY = cy;
    
    // ========== DRAW ARM (extends back to left side) ==========
    int armLength = 150;
    int armStartX = palmX - armLength;
    int armStartY = palmY;
    
    // FOREARM (lower arm - from wrist to elbow)
    int forearmWidth = 18;
    int elbowX = palmX - (int)(armLength * 0.6);
    int elbowY = palmY + 5;
    
    // Forearm muscles (extensor group)
    uint16_t muscleGreen1 = 0x0480;  // Medium green
    uint16_t muscleGreen2 = 0x0580;  // Lighter green
    uint16_t darkGreen = 0x0300;     // Dark green
    
    // Draw forearm top muscle group
    buffer->fillTriangle(palmX - 10, palmY - forearmWidth/2,
                        elbowX, elbowY - forearmWidth/2 - 3,
                        elbowX, elbowY + 2,
                        muscleGreen1);
    
    // Draw forearm bottom muscle group
    buffer->fillTriangle(palmX - 10, palmY + forearmWidth/2,
                        elbowX, elbowY + forearmWidth/2 + 3,
                        elbowX, elbowY - 2,
                        muscleGreen2);
    
    // Muscle striations (detail lines)
    for (int i = 0; i < 4; i++) {
        int lineX = palmX - 20 - i * 20;
        buffer->drawLine(lineX, palmY - 7, lineX + 5, palmY - 4, darkGreen);
        buffer->drawLine(lineX, palmY + 7, lineX + 5, palmY + 4, darkGreen);
    }
    
    // **EXPOSED BONE SECTION 1 (forearm) - Ulna bone showing**
    int bone1X = palmX - 50;
    int bone1Y = palmY + 8;
    int bone1Width = 30;
    
    // Tear in flesh revealing bone
    buffer->fillEllipse(bone1X, bone1Y, 15, 8, TFT_BLACK);  // Wound opening
    
    // White bone visible through tear
    buffer->drawLine(bone1X - 12, bone1Y, bone1X + 12, bone1Y, TFT_WHITE);
    buffer->drawLine(bone1X - 11, bone1Y + 1, bone1X + 11, bone1Y + 1, 0xE71C);  // Off-white
    buffer->drawLine(bone1X - 10, bone1Y - 1, bone1X + 10, bone1Y - 1, 0xE71C);
    
    // Bone texture
    buffer->drawPixel(bone1X - 5, bone1Y, 0xD69A);
    buffer->drawPixel(bone1X + 5, bone1Y, 0xD69A);
    
    // Torn flesh edges (dark red/brown)
    buffer->drawEllipse(bone1X, bone1Y, 15, 8, 0x4000);
    
    // UPPER ARM (bicep/tricep)
    int upperArmWidth = 22;
    
    // Bicep muscle (top)
    buffer->fillTriangle(elbowX, elbowY - upperArmWidth/2 - 2,
                        armStartX, armStartY - upperArmWidth/2,
                        elbowX + 10, elbowY,
                        0x0600);  // Bright green
    
    // Tricep muscle (bottom)
    buffer->fillTriangle(elbowX, elbowY + upperArmWidth/2 + 2,
                        armStartX, armStartY + upperArmWidth/2,
                        elbowX + 10, elbowY,
                        muscleGreen1);
    
    // Muscle definition (darker lines)
    buffer->drawLine(elbowX, elbowY - 8, armStartX + 20, armStartY - 10, darkGreen);
    buffer->drawLine(elbowX, elbowY + 8, armStartX + 20, armStartY + 10, 0x0200);
    
    // **EXPOSED BONE SECTION 2 (upper arm) - Humerus bone**
    int bone2X = elbowX - 30;
    int bone2Y = elbowY - 10;
    
    // Larger wound
    buffer->fillEllipse(bone2X, bone2Y, 20, 10, TFT_BLACK);
    
    // Bone showing through (thicker bone here)
    for (int b = -1; b <= 1; b++) {
        buffer->drawLine(bone2X - 15, bone2Y + b, bone2X + 15, bone2Y + b, TFT_WHITE);
    }
    buffer->drawLine(bone2X - 14, bone2Y + 2, bone2X + 14, bone2Y + 2, 0xE71C);
    buffer->drawLine(bone2X - 14, bone2Y - 2, bone2X + 14, bone2Y - 2, 0xE71C);
    
    // Joint knob detail
    buffer->fillCircle(bone2X - 12, bone2Y, 3, TFT_WHITE);
    buffer->drawCircle(bone2X - 12, bone2Y, 3, 0xD69A);
    
    // Torn flesh edges
    buffer->drawEllipse(bone2X, bone2Y, 20, 10, 0x4000);
    buffer->drawEllipse(bone2X, bone2Y, 21, 11, 0x2000);
    
    // Elbow joint bulge
    buffer->fillCircle(elbowX, elbowY, 12, darkGreen);
    buffer->drawCircle(elbowX, elbowY, 12, 0x0200);
    buffer->fillCircle(elbowX, elbowY, 8, 0x0400);
    
    // Veins/tendons visible
    buffer->drawLine(palmX - 30, palmY - 5, elbowX + 10, elbowY - 8, 0x0260);  // Dark vein
    buffer->drawLine(palmX - 40, palmY + 6, elbowX + 5, elbowY + 9, 0x0260);
    
    // ========== PALM BASE (dark green) ==========
    uint16_t palmColor = 0x0300;
    buffer->fillRect(palmX - 5, palmY - 20, 10, 40, palmColor);
    buffer->drawRect(palmX - 5, palmY - 20, 10, 40, 0x0200);
    
    // Knuckle details
    for (int k = 0; k < 3; k++) {
        buffer->drawFastHLine(palmX - 4, palmY - 15 + k * 10, 8, 0x0500);
    }
    
    // ========== 4 THICK, GNARLED FINGERS ==========
    for (int f = 0; f < 4; f++) {
        int fingerBaseY = palmY - 18 + f * 12;
        
        // Finger color variation
        uint16_t fingerColor = 0x0400 + (f * 0x0100);
        uint16_t darkFingerGreen = fingerColor - 0x0100;
        
        // Finger extends and curls based on closedness
        int tipX = palmX + 60 - (int)(closedness * 60);
        int tipY = fingerBaseY + (int)(closedness * (cy - fingerBaseY) * 0.9);
        
        // First segment
        int joint1X = palmX + 20 - (int)(closedness * 10);
        int joint1Y = fingerBaseY + (int)(closedness * (cy - fingerBaseY) * 0.2);
        
        // Second segment
        int joint2X = palmX + 40 - (int)(closedness * 30);
        int joint2Y = fingerBaseY + (int)(closedness * (cy - fingerBaseY) * 0.5);
        
        // Draw thick finger segments (3 pixels wide)
        for (int thick = -1; thick <= 1; thick++) {
            buffer->drawLine(palmX, fingerBaseY + thick, joint1X, joint1Y + thick, fingerColor);
            buffer->drawLine(joint1X, joint1Y + thick, joint2X, joint2Y + thick, darkFingerGreen);
            buffer->drawLine(joint2X, joint2Y + thick, tipX, tipY + thick, fingerColor);
        }
        
        // Knuckle joints
        buffer->fillCircle(joint1X, joint1Y, 3, darkFingerGreen);
        buffer->drawCircle(joint1X, joint1Y, 3, 0x0200);
        buffer->fillCircle(joint2X, joint2Y, 3, darkFingerGreen);
        buffer->drawCircle(joint2X, joint2Y, 3, 0x0200);
        
        // Gnarled texture
        buffer->drawLine(palmX + 10, fingerBaseY, joint1X - 5, joint1Y - 2, 0x0600);
        buffer->drawLine(joint1X + 5, joint1Y, joint2X - 5, joint2Y - 2, 0x0600);
        
        // ========== RED TALON (claw nail) ==========
        int talonLength = 8;
        uint16_t talonColor = TFT_RED;
        uint16_t darkRed = 0x8000;
        
        int talonTipX = tipX + talonLength - (int)(closedness * 3);
        int talonTipY = tipY - 4;
        
        // Draw thick talon
        for (int t = -1; t <= 1; t++) {
            buffer->drawLine(tipX, tipY + t, talonTipX, talonTipY + t, talonColor);
        }
        
        // Talon highlight
        buffer->drawLine(tipX, tipY - 1, talonTipX - 2, talonTipY - 1, 0xF800);
        
        // Talon shadow
        buffer->drawLine(tipX, tipY + 1, talonTipX, talonTipY + 2, darkRed);
        
        // Sharp point
        buffer->drawPixel(talonTipX, talonTipY, 0xF800);
        buffer->drawPixel(talonTipX + 1, talonTipY, darkRed);
    }
    
    // Wrist connection
    buffer->fillRect(palmX - 8, palmY - 22, 6, 14, 0x0300);
    buffer->drawRect(palmX - 8, palmY - 22, 6, 14, 0x0200);
}

// ========== NEW SPLASH SCREEN ==========
void showSplash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    
    // Create animation buffer
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->createSprite(480, 320);
    
    uint32_t startTime = millis();
    
    // **Phase 1**: Black screen (250ms)
    while (millis() - startTime < 250) {
        buffer->fillSprite(TFT_BLACK);
        buffer->pushSprite(0, 0);
        delay(16);
    }
    
    // **Phase 2**: Beating heart alone (2000ms - DOUBLED)
    uint32_t heartStart = millis();
    while (millis() - heartStart < 2000) {
        buffer->fillSprite(TFT_BLACK);
        
        // Pulse effect (2 beats)
        uint32_t beatTime = (millis() - heartStart) % 600;
        float pulse = 1.0;
        if (beatTime < 150) {
            pulse = 1.0 + (beatTime / 150.0) * 0.25;  // Grow
        } else if (beatTime < 300) {
            pulse = 1.25 - ((beatTime - 150) / 150.0) * 0.25;  // Shrink
        }
        
        uint16_t heartColor = (beatTime < 300) ? TFT_RED : 0x8800;  // Bright/dim
        
        drawAnatomicalHeart(buffer, 240, 160, pulse, heartColor);
        
        buffer->pushSprite(0, 0);
        delay(16);
    }
    
    // **Phase 3**: Hand enters slowly while heart still beats (2500ms - SLOWER)
    uint32_t handStart = millis();
    float handReachTime = 2500.0;  // Time to reach heart
    
    while (millis() - handStart < handReachTime) {
        buffer->fillSprite(TFT_BLACK);
        
        float elapsed = (millis() - handStart) / handReachTime;
        float entryProgress = elapsed;  // 0 to 1
        
        // **Heart keeps beating**
        uint32_t totalTime = millis() - startTime;
        uint32_t beatTime = totalTime % 600;
        float pulse = 1.0;
        if (beatTime < 150) {
            pulse = 1.0 + (beatTime / 150.0) * 0.25;
        } else if (beatTime < 300) {
            pulse = 1.25 - ((beatTime - 150) / 150.0) * 0.25;
        }
        
        uint16_t heartColor = (beatTime < 300) ? TFT_RED : 0x8800;
        
        drawAnatomicalHeart(buffer, 240, 160, pulse, heartColor);
        
        // Hand enters (not closing yet)
drawClawedHand(buffer, 240, 160, 0.0f, entryProgress);
        
        buffer->pushSprite(0, 0);
        delay(16);
    }
    
    // **Phase 4**: Hand closes around heart (1000ms)
    uint32_t closeStart = millis();
    while (millis() - closeStart < 1000) {
        buffer->fillSprite(TFT_BLACK);
        
        float elapsed = (millis() - closeStart) / 1000.0;
        float closedness = elapsed;  // 0 to 1
        
        // Heart gets crushed (stops beating, shrinks)
        float heartScale = 1.0 - closedness * 0.4;
        drawAnatomicalHeart(buffer, 240, 160, heartScale, TFT_RED);
        
        // Hand closes
        drawClawedHand(buffer, 240, 160, closedness, 1.0);
        
        buffer->pushSprite(0, 0);
        delay(16);
    }
    
    // **Phase 5**: Heart splits and pieces fly to corners (1500ms)
    uint32_t splitStart = millis();
    while (millis() - splitStart < 1500) {
        buffer->fillSprite(TFT_BLACK);
        
        float elapsed = (millis() - splitStart) / 1500.0;
        
        // Closed hand remains
drawClawedHand(buffer, 240, 160, 1.0f, 1.0f);
        
        // 4 pieces fly to corners
        int spread = (int)(elapsed * 200);  // 0 to 200 pixels
        
        // Top-left corner
        int tl_x = 240 - spread;
        int tl_y = 160 - spread;
        
        // Top-right corner
        int tr_x = 240 + spread;
        int tr_y = 160 - spread;
        
        // Bottom-left corner
        int bl_x = 240 - spread;
        int bl_y = 160 + spread;
        
        // Bottom-right corner
        int br_x = 240 + spread;
        int br_y = 160 + spread;
        
        // Draw 4 heart pieces (smaller)
        float pieceScale = 0.4 - elapsed * 0.1;  // Shrink as they fly away
        
        drawAnatomicalHeart(buffer, tl_x, tl_y, pieceScale, 0xC800);  // Dark red
        drawAnatomicalHeart(buffer, tr_x, tr_y, pieceScale, 0xC800);
        drawAnatomicalHeart(buffer, bl_x, bl_y, pieceScale, 0xC800);
        drawAnatomicalHeart(buffer, br_x, br_y, pieceScale, 0xC800);
        
        buffer->pushSprite(0, 0);
        delay(16);
    }
    
    // **Phase 6**: Fade to black (500ms)
    for (int fade = 255; fade >= 0; fade -= 15) {
        buffer->fillSprite(TFT_BLACK);
        buffer->pushSprite(0, 0);
        delay(20);
    }
    
    buffer->deleteSprite();
    delete buffer;
    
    tft.fillScreen(TFT_BLACK);
    Serial.println("[PsychoFlight] Splash complete, starting game...");
}

// ========== MAIN GAME LOOP ==========
void playGame(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[PsychoFlight] playGame() started");
    
    tft.fillScreen(TFT_BLACK);
    delay(100);
    tft.fillScreen(TFT_BLACK);
    
    Serial.printf("[PsychoFlight] Free heap at game start: %d bytes\n", ESP.getFreeHeap());
    
    playerX = SCREEN_CENTER_X;
    playerY = SCREEN_CENTER_Y;
    velX = 0;
    velY = 0;
    tiltX = 0;
    tiltY = 0;
    gameOver = false;
    playerInvincible = false;
    
    initCaveRings();
    initHeartGame();
    

// Wait for any buttons to be released
delay(200);
Serial.println("[PsychoFlight] Ready to play!");

    
    // Create buffer with 16-bit color
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->setAttribute(PSRAM_ENABLE, true);
    
    bool bufferCreated = buffer->createSprite(480, 320);
    
    if (!bufferCreated) {
        Serial.println("[PsychoFlight] ERROR: Buffer creation failed!");
        Serial.printf("[PsychoFlight] Free heap: %d bytes\n", ESP.getFreeHeap());
        delete buffer;
        buffer = nullptr;
    } else {
        Serial.println("[PsychoFlight] Buffer created successfully!");
        Serial.printf("[PsychoFlight] Free heap after buffer: %d bytes\n", ESP.getFreeHeap());
    }
    
    Serial.println("[PsychoFlight] Starting game loop...");
    
    int frameCount = 0;
    
    while (!gameOver) {
        unsigned long frameStart = millis();
        
        // Read joystick
        int joyX = ss.analogRead(JOY_X);
        int joyY = ss.analogRead(JOY_Y);
        
        float inputX = (joyX - 512) / 512.0;
        float inputY = (joyY - 512) / 512.0;
        
        if (abs(inputX) < 0.15) inputX = 0;
        if (abs(inputY) < 0.15) inputY = 0;
        
        velX += inputX * 1.8;
        velY += inputY * 1.8;
        
        velX *= INERTIA;
        velY *= INERTIA;
        
        velX = constrain(velX, -MOVE_SPEED, MOVE_SPEED);
        velY = constrain(velY, -MOVE_SPEED, MOVE_SPEED);
        
        playerX += velX;
        playerY += velY;
        
        playerX = constrain(playerX, 60, 420);
        playerY = constrain(playerY, 60, 260);
        
        float targetTiltX = velX * 5;
        tiltX += (targetTiltX - tiltX) * TILT_SPEED;
        
// Update game logic
        if (gameState == COLLECTING_HEARTS) {
            updateHeartSpawning();
            checkHeartCollision();
            checkAndBounce();
            updateCaveRings();
          

            } else if (gameState == HEART_ASSEMBLY) {
    // **NO GAME UPDATES** - just let animation play
    // (animation advances based on time in animateHeartAssembly)

} else if (gameState == CAVE_ESCAPE) {
    // **UPDATE CAVE ESCAPE** - keep cave moving, check collisions
    updateCaveRings();
    checkAndBounce();
    
    uint32_t escapeElapsed = millis() - caveMouthStartTime;
    
    // Show cave mouth/teeth after 5 seconds of flying through cave
    if (escapeElapsed > 5000 && !caveMouthActive) {
        caveMouthActive = true;
        Serial.println("[Cave] Cave exit with teeth now visible!");
    }
    
    // Debug output every second
    if (escapeElapsed % 1000 < 33) {
        Serial.print("[Cave] Flying time: ");
        Serial.print(escapeElapsed / 1000);
        Serial.println("s");
    }
    
    // Exit cave after 12 seconds total (5s cave + 7s teeth approach)
    if (escapeElapsed > 12000) {
        gameState = EMERGENCE;
        initTerrain();
        Serial.println("[Cave] Emerged into outside world!");
    }

            
} else if (gameState == EMERGENCE) {
    updateTerrain();
    
    // Update mountain approach
    if (gamePhase == PHASE_MOUNTAIN_APPROACH) {
        mountainDistance -= APPROACH_SPEED;
        if (mountainDistance < 0) mountainDistance = 0;
        
        updateGoldenOrbSpawning();
        checkOrbCollision();
        checkMountainLanding();  // Add this line
        
        // Check if reached citadel
        if (mountainDistance < 0.05) {
            gamePhase = PHASE_CITADEL_REACHED;
            Serial.println("[Mountain] Reached the citadel!");
        }
    }
    
    // Camera follows altitude (allow seeing mountain top)
    float targetOffsetY = (playerY - SCREEN_CENTER_Y) * 1.2;  // Increased from 0.8
    targetOffsetY = constrain(targetOffsetY, -100, 100);  // Limit range
    cameraOffsetY += (targetOffsetY - cameraOffsetY) * 0.1;
}
        
// ========== RENDER ==========
        if (buffer) {
            buffer->fillSprite(TFT_BLACK);
            
            // Draw based on game state
            if (gameState == COLLECTING_HEARTS) {
                // Draw cave
                for (int i = 0; i < MAX_RINGS; i++) {
                    drawCaveRing(buffer, rings[i].z, getRingColor(rings[i].z));
                }
                
                for (int i = 0; i < MAX_RINGS; i++) {
                    if (rings[i].hasTooth) {
                        drawTooth(buffer, rings[i].z, rings[i].toothAngle, 
                                 rings[i].toothSize, 0x8410, getRingColor(rings[i].z));
                    }
                }
                
                // Draw heart pieces
                drawHeartPieces(buffer);
                
                // Draw castle in distance
                drawCastle(buffer);
                
                // Draw player
                drawBitmapCharacter(buffer, (int)playerX, (int)playerY, 
                                  flyingMan_centerBitmap, 
                                  flyingMan_centerWidth, 
                                  flyingMan_centerHeight, 
                                  TFT_CYAN);
                
            } else if (gameState == HEART_ASSEMBLY) {
                // **DRAW ASSEMBLY ANIMATION** (black background, just the heart pieces)
                animateHeartAssembly(buffer);
                
} else if (gameState == CAVE_ESCAPE) {
    // **DRAW CAVE WITH MOUTH AND EXIT**
    Serial.println("[DEBUG] Rendering CAVE_ESCAPE");  // ← ADD THIS
    
    // Draw cave rings
    for (int i = 0; i < MAX_RINGS; i++) {
        drawCaveRing(buffer, rings[i].z, getRingColor(rings[i].z));
    }
    
    // Draw teeth
    for (int i = 0; i < MAX_RINGS; i++) {
        if (rings[i].hasTooth) {
            drawTooth(buffer, rings[i].z, rings[i].toothAngle, 
                     rings[i].toothSize, 0x8410, getRingColor(rings[i].z));
        }
    }
    
    // **DRAW CAVE MOUTH/EXIT**
    if (caveMouthActive) {
        drawCaveMouthRing(buffer);
    }
    
    // Draw castle (visible outside)
    drawCastle(buffer);
    
// Draw player with small heart on chest
    drawBitmapCharacter(buffer, (int)playerX, (int)playerY, 
                      flyingMan_centerBitmap, 
                      flyingMan_centerWidth, 
                      flyingMan_centerHeight, 
                      TFT_CYAN);
                      
} else if (gameState == EMERGENCE) {
    // **DRAW EMERGENCE - Flying outside toward mountains**
    
    // Check if brain assembly animation should play
    if (gamePhase == PHASE_BRAIN_ASSEMBLY) {
        animateBrainAssembly(buffer);
    } else {
        // Normal emergence rendering
        
        // Draw sky waves
        drawSkyWaves(buffer);
        
// Draw mountains
drawMountains(buffer);

// Draw landing indicator when close to main peak
if (mountainDistance < 0.15f && gamePhase == PHASE_MOUNTAIN_APPROACH) {
    float spreadFactor = 1.0f + (1.0f - mountainDistance) * 1.5f;
    int peakX = 280;
    int horizonY = 220 - (int)cameraOffsetY;
    float scale = 0.3 + (1.0 - mountainDistance) * 2.7;
    if (scale > 2.0f) scale = 2.0f;
    int height = (int)(140 * scale);
    int peakY = horizonY - height;
    
    // Draw landing zone circle
    buffer->drawCircle(peakX, peakY - 20, 30, TFT_YELLOW);
    buffer->drawCircle(peakX, peakY - 20, 31, TFT_YELLOW);
}

// Draw terrain contours
drawTerrainContours(buffer);
        
        drawGoldenOrbs(buffer); 

        // Draw player with heart
        drawBitmapCharacter(buffer, (int)playerX, (int)playerY, 
                          flyingMan_centerBitmap, 
                          flyingMan_centerWidth, 
                          flyingMan_centerHeight, 
                          TFT_CYAN);
    }
}
            
// Push to screen
buffer->pushSprite(0, 0);
            
} else {  // <-- Closes if (buffer) and starts the else
    // Fallback if buffer creation failed
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Buffer failed - low memory", 240, 160, 4);
}  // <-- Closes the else block
                
        frameCount++;
        
// Check for exit (SELECT button = 14)
static uint32_t lastButtonCheck = 0;
static bool wasPressed = false;

if (millis() - lastButtonCheck > 100) {  // Check every 100ms
    bool isPressed = !ss.digitalRead(14);  // Active low
    
    if (isPressed && !wasPressed) {
        Serial.println("[PsychoFlight] SELECT pressed, exiting.");
        if (buffer) {
            buffer->deleteSprite();
            delete buffer;
        }
        tft.fillScreen(TFT_BLACK);
        return;
    }
    wasPressed = isPressed;
    lastButtonCheck = millis();
}

        // Frame rate control (30 FPS)
        unsigned long frameTime = millis() - frameStart;
        if (frameTime < 33) delay(33 - frameTime);
    }  // End of while loop
    
    if (buffer) {
        buffer->deleteSprite();
        delete buffer;
    }
}  // End of playGame() function

} // CLOSING BRACE FOR NAMESPACE

void run_Psycho_Flight(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[PsychoFlight] Starting Heart Quest game...");
    
    PsychoFlight::spritesLoaded = false;
    
    Serial.printf("[PsychoFlight] Free heap: %d bytes\n", ESP.getFreeHeap());
    
    PsychoFlight::showSplash(tft, ss);
    PsychoFlight::playGame(tft, ss);
}

#endif // PSYCHO_FLIGHT_H