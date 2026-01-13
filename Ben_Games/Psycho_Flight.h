#ifndef PSYCHO_FLIGHT_H
#define PSYCHO_FLIGHT_H

#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <SD.h>
#include <PNGdec.h>
#include "line_superman_center.h" 

// ========== PSYCHO FLIGHT - 3D CAVE TUNNEL WITH SPRITES ==========

namespace PsychoFlight {

// SD Card pins (matching your working code)
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

// Sprite dimensions
const int SPRITE_WIDTH = 64;
const int SPRITE_HEIGHT = 64;
const int SPRITE_FRAMES = 4;

float playerX = 240;
float playerY = 160;
float velX = 0;
float velY = 0;
float tiltX = 0;
float tiltY = 0;

bool gameOver = false;
bool playerInvincible = false;
unsigned long invincibleStartTime = 0;
unsigned long gameStartTime = 0;

// Sprite objects
TFT_eSprite* gameBuffer = nullptr;
TFT_eSprite* playerSprites[4] = {nullptr, nullptr, nullptr, nullptr};
TFT_eSprite* spriteSheet = nullptr;
bool spritesLoaded = false;

PNG png;
TFT_eSPI* tft_ptr = nullptr;

// Cave contour rings
struct CaveRing {
    float z;
    bool hasTooth;
    int toothAngle;
    int toothSize;
};

const int MAX_RINGS = 25;
CaveRing rings[MAX_RINGS];

// ========== SD CARD FILE CALLBACKS ==========
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

// ========== PNG DECODER CALLBACK ==========
void pngDraw(PNGDRAW *pDraw) {
    uint16_t lineBuffer[256]; // Wide enough for our 256-pixel sprite sheet
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    
    if (spriteSheet) {
        spriteSheet->pushImage(0, pDraw->y, pDraw->iWidth, 1, lineBuffer);
    }
}

// ========== LOAD SPRITE SHEET FROM SD CARD ==========
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
    
    // Create sprite sheet buffer
    spriteSheet = new TFT_eSprite(&tft);
    spriteSheet->setColorDepth(16);
    spriteSheet->createSprite(256, 64);
    spriteSheet->fillSprite(TFT_BLACK);
    
    tft_ptr = &tft;
    
    // Open and decode PNG
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
        
        // Extract individual frames from sprite sheet
        for (int i = 0; i < SPRITE_FRAMES; i++) {
            playerSprites[i] = new TFT_eSprite(&tft);
            playerSprites[i]->setColorDepth(16);
            playerSprites[i]->createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
            playerSprites[i]->fillSprite(TFT_BLACK);
            
            // Copy frame from sprite sheet (direct pixel copy)
            for (int y = 0; y < SPRITE_HEIGHT; y++) {
                for (int x = 0; x < SPRITE_WIDTH; x++) {
                    uint16_t color = spriteSheet->readPixel(i * SPRITE_WIDTH + x, y);
                    playerSprites[i]->drawPixel(x, y, color);
                }
            }
            
            Serial.printf("[PsychoFlight] Extracted sprite frame %d\n", i);
        }
        
        // FREE THE SPRITE SHEET TO SAVE MEMORY!
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

// ========== INITIALIZE SPRITES ==========
void initSprites(TFT_eSPI &tft) {
    // DON'T create game buffer here - will create in playGame() if there's enough memory
    // gameBuffer = new TFT_eSprite(&tft);
    // gameBuffer->setColorDepth(16);
    // gameBuffer->createSprite(480, 320);
    
    // Try to load sprite sheet from SD card
    spritesLoaded = loadSpriteSheet(tft);
    
    if (!spritesLoaded) {
        Serial.println("[PsychoFlight] Using fallback drawn sprites");
    }
    
    // Print free memory
    Serial.printf("[PsychoFlight] Free heap after sprite load: %d bytes\n", ESP.getFreeHeap());
}

// ========== CLEANUP SPRITES ==========
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

// ========== GET SPRITE FRAME BASED ON VELOCITY ==========
int getSpriteFrame() {
    // Frame 0: Left, Frame 1: Right, Frame 2: Up, Frame 3: Down
    
    if (abs(velX) > abs(velY)) {
        // Horizontal movement dominant
        return (velX < 0) ? 0 : 1;  // Left : Right
    } else if (abs(velY) > 2.0) {
        // Vertical movement
        return (velY < 0) ? 2 : 3;  // Up : Down
    } else {
        // Default to up/forward
        return 2;
    }
}

// ========== FALLBACK: DRAW FLYING PERSON (if sprite fails) ==========
void drawFlyingPersonFallback(TFT_eSprite* buffer, int cx, int cy, float tiltLR, bool erase) {
    uint16_t bodyColor = erase ? TFT_BLACK : TFT_CYAN;
    uint16_t darkColor = erase ? TFT_BLACK : 0x0410;
    
    if (!erase && playerInvincible && ((millis() - invincibleStartTime) / 100) % 2 == 0) {
        bodyColor = TFT_WHITE;
        darkColor = 0xC618;
    }
    
    float bankAngle = tiltLR * 0.017453;
    int tiltOffset = (int)(sin(bankAngle) * 8);
    
    buffer->fillCircle(cx, cy - 18, 6, bodyColor);
    buffer->fillRect(cx - 3, cy - 12, 6, 6, bodyColor);
    buffer->fillEllipse(cx + 2, cy - 4, 12, 8, bodyColor);
    buffer->fillRoundRect(cx - 5, cy - 8, 8, 10, 2, darkColor);
    
    int leftShoulderX = cx - 8;
    int leftShoulderY = cy - 6;
    int leftHandX = cx - 30 - tiltOffset;
    int leftHandY = cy - 6 + tiltOffset / 2;
    
    for (int i = 0; i < 5; i++) {
        buffer->drawLine(leftShoulderX, leftShoulderY + i - 2, leftHandX, leftHandY + i - 2, bodyColor);
    }
    buffer->fillCircle(leftHandX, leftHandY, 4, bodyColor);
    
    int rightShoulderX = cx + 8;
    int rightShoulderY = cy - 6;
    int rightHandX = cx + 30 - tiltOffset;
    int rightHandY = cy - 6 - tiltOffset / 2;
    
    for (int i = 0; i < 5; i++) {
        buffer->drawLine(rightShoulderX, rightShoulderY + i - 2, rightHandX, rightHandY + i - 2, bodyColor);
    }
    buffer->fillCircle(rightHandX, rightHandY, 4, bodyColor);
    
    buffer->fillEllipse(cx, cy + 4, 8, 5, bodyColor);
    
    int legsStartX = cx;
    int legsStartY = cy + 6;
    int legsEndX = cx - 16 + tiltOffset / 3;
    int legsEndY = cy + 6;
    
    for (int i = 0; i < 5; i++) {
        buffer->drawLine(legsStartX - 2, legsStartY + i - 2, legsEndX - 2, legsEndY + i - 3, bodyColor);
    }
    buffer->fillCircle(legsEndX - 2, legsEndY - 1, 4, bodyColor);
    
    for (int i = 0; i < 5; i++) {
        buffer->drawLine(legsStartX + 2, legsStartY + i - 2, legsEndX + 2, legsEndY + i + 3, bodyColor);
    }
    buffer->fillCircle(legsEndX + 2, legsEndY + 1, 4, bodyColor);
}

// ========== DRAW BITMAP CHARACTER WITH BOUNDS CHECKING ==========
void drawBitmapCharacter(TFT_eSprite* buffer, int cx, int cy, const uint8_t* bitmap, uint16_t w, uint16_t h, uint16_t color) {
    int startX = cx - (w / 2);
    int startY = cy - (h / 2);
    
    // Flash white if invincible
    if (playerInvincible && ((millis() - invincibleStartTime) / 100) % 2 == 0) {
        color = TFT_WHITE;
    }
    
    // Draw the 1-bit bitmap
    for (uint16_t y = 0; y < h; y++) {
        for (uint16_t x = 0; x < w; x++) {
            uint16_t pixelIndex = y * w + x;
            uint16_t byteIndex = pixelIndex / 8;
            uint8_t bitIndex = 7 - (pixelIndex % 8);
            
            uint8_t bit = (bitmap[byteIndex] >> bitIndex) & 0x01;
            
            // Draw pixel if bit is 1 (line)
            if (bit == 1) {
                int drawX = startX + x;
                int drawY = startY + y;
                
                // Bounds check
                if (drawX >= 0 && drawX < 480 && drawY >= 0 && drawY < 320) {
                    buffer->drawPixel(drawX, drawY, color);
                }
            }
        }
    }
}
    

// ========== GET TOOTH SPAWN RATE ==========
int getToothSpawnRate() {
    unsigned long elapsedTime = millis() - gameStartTime;
    
    if (elapsedTime < 10000) return 25;
    else if (elapsedTime < 20000) return 35;
    else if (elapsedTime < 30000) return 45;
    else if (elapsedTime < 45000) return 55;
    else return 65;
}

// ========== INITIALIZE CAVE RINGS ==========
void initCaveRings() {
    for (int i = 0; i < MAX_RINGS; i++) {
        rings[i].z = (float)i / MAX_RINGS;
        rings[i].hasTooth = (random(0, 100) < 25);
        rings[i].toothAngle = random(0, 12);
        rings[i].toothSize = random(0, 4);
    }
}

// ========== UPDATE CAVE RINGS ==========
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

// ========== DRAW CAVE RING ==========
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

// ========== DRAW TOOTH ==========
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

// ========== GET RING COLOR ==========
uint16_t getRingColor(float z) {
    if (z < 0.2) return 0x2104;
    else if (z < 0.4) return 0x4208;
    else if (z < 0.6) return 0x632C;
    else if (z < 0.8) return 0x8C51;
    else return 0xAD75;
}

// ========== CHECK AND BOUNCE ==========
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

// ========== SPLASH SCREEN - FINAL FIXED VERSION ==========
void showSplash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    const int BUTTON_START = 6;  // Button B
    
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextFont(4);
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("PSYCHO FLIGHT", 240, 30);
    
    TFT_eSprite tempSprite(&tft);
    tempSprite.setColorDepth(16);
    tempSprite.createSprite(80, 80);
    tempSprite.fillSprite(TFT_BLACK);
    drawFlyingPersonFallback(&tempSprite, 40, 40, 0, false);
    tempSprite.pushSprite(200, 100, TFT_BLACK);
    tempSprite.deleteSprite();
    
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Fly through the cave!", 240, 220);
    tft.drawString("Dodge the spikes!", 240, 245);
    
    if (spritesLoaded) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawString("Sprites loaded!", 240, 265);
    } else {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Using fallback graphics", 240, 265);
    }
    
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Press B to Start", 240, 290);
    
    Serial.println("[PsychoFlight] Splash displayed, waiting for button...");
    
    // Wait for button to be HIGH (not pressed)
    int timeout = 0;
    while (ss.digitalRead(BUTTON_START) == 0 && timeout < 50) {
        delay(20);
        timeout++;
    }
    
    // Now wait for button to be pressed (goes LOW)
    while (ss.digitalRead(BUTTON_START) == 1) {
        delay(10);
    }
    
    Serial.println("[PsychoFlight] Button B pressed! Starting game...");
    
    // Wait for button release
    while (ss.digitalRead(BUTTON_START) == 0) {
        delay(10);
    }
    
    delay(100);
}

// ========== DRAW CAVE RING DIRECTLY TO TFT ==========
void drawCaveRingDirect(TFT_eSPI* tft, float zPos, uint16_t color) {
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
        
        tft->drawLine(lastX, lastY, nextX, nextY, color);
        
        if (zPos > 0.5) tft->drawLine(lastX - 1, lastY, nextX - 1, nextY, color);
        if (zPos > 0.7) tft->drawLine(lastX + 1, lastY, nextX + 1, nextY, color);
        
        lastX = nextX;
        lastY = nextY;
    }
}

// ========== DRAW TOOTH DIRECTLY TO TFT ==========
void drawToothDirect(TFT_eSPI* tft, float zPos, int toothAngle, int toothSize, uint16_t fillColor, uint16_t edgeColor) {
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
            tft->drawLine(baseX + offsetX, baseY + offsetY, tipX, tipY, fillColor);
        }
        
        tft->drawLine(baseX + perpX, baseY + perpY, tipX, tipY, edgeColor);
        tft->drawLine(baseX - perpX, baseY - perpY, tipX, tipY, edgeColor);
        tft->drawPixel(tipX, tipY, edgeColor);
    }
}

// ========== DRAW FLYING PERSON DIRECTLY TO TFT ==========
void drawFlyingPersonDirect(TFT_eSPI* tft, int cx, int cy, float tiltLR) {
    uint16_t bodyColor = TFT_CYAN;
    uint16_t darkColor = 0x0410;
    
    if (playerInvincible && ((millis() - invincibleStartTime) / 100) % 2 == 0) {
        bodyColor = TFT_WHITE;
        darkColor = 0xC618;
    }
    
    float bankAngle = tiltLR * 0.017453;
    int tiltOffset = (int)(sin(bankAngle) * 8);
    
    tft->fillCircle(cx, cy - 18, 6, bodyColor);
    tft->fillRect(cx - 3, cy - 12, 6, 6, bodyColor);
    tft->fillEllipse(cx + 2, cy - 4, 12, 8, bodyColor);
    tft->fillRoundRect(cx - 5, cy - 8, 8, 10, 2, darkColor);
    
    int leftShoulderX = cx - 8;
    int leftShoulderY = cy - 6;
    int leftHandX = cx - 30 - tiltOffset;
    int leftHandY = cy - 6 + tiltOffset / 2;
    
    for (int i = 0; i < 5; i++) {
        tft->drawLine(leftShoulderX, leftShoulderY + i - 2, leftHandX, leftHandY + i - 2, bodyColor);
    }
    tft->fillCircle(leftHandX, leftHandY, 4, bodyColor);
    
    int rightShoulderX = cx + 8;
    int rightShoulderY = cy - 6;
    int rightHandX = cx + 30 - tiltOffset;
    int rightHandY = cy - 6 - tiltOffset / 2;
    
    for (int i = 0; i < 5; i++) {
        tft->drawLine(rightShoulderX, rightShoulderY + i - 2, rightHandX, rightHandY + i - 2, bodyColor);
    }
    tft->fillCircle(rightHandX, rightHandY, 4, bodyColor);
    
    tft->fillEllipse(cx, cy + 4, 8, 5, bodyColor);
    
    int legsStartX = cx;
    int legsStartY = cy + 6;
    int legsEndX = cx - 16 + tiltOffset / 3;
    int legsEndY = cy + 6;
    
    for (int i = 0; i < 5; i++) {
        tft->drawLine(legsStartX - 2, legsStartY + i - 2, legsEndX - 2, legsEndY + i - 3, bodyColor);
    }
    tft->fillCircle(legsEndX - 2, legsEndY - 1, 4, bodyColor);
    
    for (int i = 0; i < 5; i++) {
        tft->drawLine(legsStartX + 2, legsStartY + i - 2, legsEndX + 2, legsEndY + i + 3, bodyColor);
    }
    tft->fillCircle(legsEndX + 2, legsEndY + 1, 4, bodyColor);
}

// ========== MAIN GAME LOOP - DOUBLE BUFFERED WITH 8-BIT COLOR ==========
void playGame(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[PsychoFlight] playGame() started");
    Serial.printf("[PsychoFlight] Bitmap width: %d, height: %d\n", flyingMan_centerWidth, flyingMan_centerHeight);
    Serial.printf("[PsychoFlight] First bitmap bytes: %02X %02X %02X %02X\n", 
                  flyingMan_centerBitmap[0], flyingMan_centerBitmap[1], 
                  flyingMan_centerBitmap[2], flyingMan_centerBitmap[3]);
    
    // FORCE CLEAR EVERYTHING
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
    gameStartTime = millis();
    
// Create buffer with 16-bit color using PSRAM
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->setAttribute(PSRAM_ENABLE, true);  // Use PSRAM if available
    
    bool bufferCreated = buffer->createSprite(480, 320);
    
    if (!bufferCreated) {
        Serial.println("[PsychoFlight] ERROR: 8-bit buffer creation failed!");
        Serial.printf("[PsychoFlight] Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.println("[PsychoFlight] Trying 8-bit with smaller resolution...");
        
        // Try half resolution
        delete buffer;
        buffer = new TFT_eSprite(&tft);
        buffer->setColorDepth(8);
        bufferCreated = buffer->createSprite(240, 160);
        
        if (!bufferCreated) {
            Serial.println("[PsychoFlight] Even small buffer failed, using direct rendering");
            delete buffer;
            buffer = nullptr;
        } else {
            Serial.println("[PsychoFlight] Half-res buffer created!");
        }
    } else {
        Serial.println("[PsychoFlight] Full 8-bit buffer created successfully!");
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
        
        checkAndBounce();
        updateCaveRings();
        
        // ========== RENDER ==========
        if (buffer) {
            // Render to buffer (no flicker)
            buffer->fillSprite(TFT_BLACK);
            
            // Draw cave rings to buffer
            for (int i = 0; i < MAX_RINGS; i++) {
                drawCaveRing(buffer, rings[i].z, getRingColor(rings[i].z));
            }
            
            // Draw teeth to buffer
            for (int i = 0; i < MAX_RINGS; i++) {
                if (rings[i].hasTooth) {
                    drawTooth(buffer, rings[i].z, rings[i].toothAngle, 
                             rings[i].toothSize, 0x8410, getRingColor(rings[i].z));
                }
            }
            
// Draw player to buffer using bitmap
            drawBitmapCharacter(buffer, (int)playerX, (int)playerY, 
                              flyingMan_centerBitmap, 
                              flyingMan_centerWidth, 
                              flyingMan_centerHeight, 
                              TFT_CYAN);
            
            // Push entire buffer to screen at once (no flicker!)
            buffer->pushSprite(0, 0);
            
        } else {
            // Fallback: direct rendering (will flicker)
            tft.fillScreen(TFT_BLACK);
            
            for (int i = 0; i < MAX_RINGS; i++) {
                drawCaveRingDirect(&tft, rings[i].z, getRingColor(rings[i].z));
            }
            
            for (int i = 0; i < MAX_RINGS; i++) {
                if (rings[i].hasTooth) {
                    drawToothDirect(&tft, rings[i].z, rings[i].toothAngle, 
                                   rings[i].toothSize, 0x8410, getRingColor(rings[i].z));
                }
            }
            
            int frame = getSpriteFrame();
            if (spritesLoaded && playerSprites[frame]) {
                playerSprites[frame]->pushSprite((int)playerX - SPRITE_WIDTH/2, 
                                                (int)playerY - SPRITE_HEIGHT/2, 
                                                TFT_BLACK);
            } else {
                drawFlyingPersonDirect(&tft, (int)playerX, (int)playerY, tiltX);
            }
        }
        
        frameCount++;
        
        if (frameCount == 1) {
            Serial.printf("[PsychoFlight] First frame rendered, buffer=%p\n", buffer);
        }
        
        // Check for exit (SELECT button = 14)
        if (!ss.digitalRead(14)) {
            Serial.println("[PsychoFlight] SELECT pressed, exiting.");
            if (buffer) {
                buffer->deleteSprite();
                delete buffer;
            }
            tft.fillScreen(TFT_BLACK);
            return;
        }
        
        // Frame rate control (30 FPS)
        unsigned long frameTime = millis() - frameStart;
        if (frameTime < 33) delay(33 - frameTime);
    }
    
    if (buffer) {
        buffer->deleteSprite();
        delete buffer;
    }
}



}
void run_Psycho_Flight(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[PsychoFlight] Starting game...");
    
    // Use bitmap character, no sprites needed
    PsychoFlight::spritesLoaded = false;
    
    Serial.printf("[PsychoFlight] Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Go to game with bitmap character
    PsychoFlight::playGame(tft, ss);
}

#endif // PSYCHO_FLIGHT_H