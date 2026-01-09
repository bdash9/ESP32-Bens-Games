#ifndef FLIGHT_FIGHT_H
#define FLIGHT_FIGHT_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

#define FF_SCREEN_W 480
#define FF_SCREEN_H 320
#define FF_MAX_ENEMIES 8
#define FF_MAX_BULLETS 6
#define FF_MAX_GROUND 6
#define FF_MAX_LIVES 3
#define FF_MOUNTAIN_POINTS 30

#define FF_COLOR_CYAN TFT_CYAN
#define FF_COLOR_WHITE TFT_WHITE
#define FF_COLOR_RED TFT_RED
#define FF_COLOR_YELLOW TFT_YELLOW
#define FF_COLOR_GREEN TFT_GREEN
#define FF_COLOR_ORANGE TFT_ORANGE
#define FF_COLOR_GOLD 0xFEA0

#define FF_HUD_TOP 30
#define FF_HUD_RIGHT (FF_SCREEN_W - 35)

// ====== VECTOR FONT ======
struct FF_VecStroke { int x0, y0, x1, y1; };
struct FF_VecLetter { const FF_VecStroke* strokes; int n; };

const FF_VecStroke ff_strokes_0[] = {{0,0,6,0},{6,0,6,9},{6,9,0,9},{0,9,0,0}};
const FF_VecStroke ff_strokes_1[] = {{3,0,3,9},{2,9,4,9}};
const FF_VecStroke ff_strokes_2[] = {{0,0,6,0},{6,0,6,4},{6,4,0,4},{0,4,0,9},{0,9,6,9}};
const FF_VecStroke ff_strokes_3[] = {{0,0,6,0},{6,0,6,9},{6,9,0,9},{3,4,6,4}};
const FF_VecStroke ff_strokes_4[] = {{0,0,0,4},{0,4,6,4},{6,0,6,9}};
const FF_VecStroke ff_strokes_5[] = {{6,0,0,0},{0,0,0,4},{0,4,6,4},{6,4,6,9},{6,9,0,9}};
const FF_VecStroke ff_strokes_6[] = {{6,0,0,0},{0,0,0,9},{0,9,6,9},{6,9,6,4},{6,4,0,4}};
const FF_VecStroke ff_strokes_7[] = {{0,0,6,0},{6,0,3,9}};
const FF_VecStroke ff_strokes_8[] = {{0,0,6,0},{6,0,6,9},{6,9,0,9},{0,9,0,0},{0,4,6,4}};
const FF_VecStroke ff_strokes_9[] = {{6,9,6,0},{6,0,0,0},{0,0,0,4},{0,4,6,4}};

const FF_VecStroke ff_strokes_S[] = {{6,0,0,0},{0,0,0,4},{0,4,6,4},{6,4,6,9},{6,9,0,9}};
const FF_VecStroke ff_strokes_C[] = {{6,0,0,0},{0,0,0,9},{0,9,6,9}};
const FF_VecStroke ff_strokes_O[] = {{0,0,6,0},{6,0,6,9},{6,9,0,9},{0,9,0,0}};
const FF_VecStroke ff_strokes_R[] = {{0,9,0,0},{0,0,6,0},{6,0,6,4},{6,4,0,4},{0,4,6,9}};
const FF_VecStroke ff_strokes_E[] = {{0,0,0,9},{0,0,6,0},{0,4,5,4},{0,9,6,9}};
const FF_VecStroke ff_strokes_V[] = {{0,0,3,9},{3,9,6,0}};
const FF_VecStroke ff_strokes_A[] = {{0,9,3,0},{3,0,6,9},{1,5,5,5}};
const FF_VecStroke ff_strokes_L[] = {{0,0,0,9},{0,9,6,9}};
const FF_VecStroke ff_strokes_U[] = {{0,0,0,9},{0,9,6,9},{6,9,6,0}};
const FF_VecStroke ff_strokes_H[] = {{0,0,0,9},{6,0,6,9},{0,4,6,4}};
const FF_VecStroke ff_strokes_I[] = {{3,0,3,9}};
const FF_VecStroke ff_strokes_G[] = {{6,0,0,0},{0,0,0,9},{0,9,6,9},{6,9,6,5},{6,5,3,5}};
const FF_VecStroke ff_strokes_T[] = {{3,0,3,9},{0,0,6,0}};
const FF_VecStroke ff_strokes_F[] = {{0,9,0,0},{0,0,6,0},{0,4,5,4}};
const FF_VecStroke ff_strokes_M[] = {{0,9,0,0},{0,0,3,5},{3,5,6,0},{6,0,6,9}};
const FF_VecStroke ff_strokes_N[] = {{0,9,0,0},{0,0,6,9},{6,9,6,0}};
const FF_VecStroke ff_strokes_D[] = {{0,9,0,0},{0,0,5,1},{5,1,6,4},{6,4,5,8},{5,8,0,9}};
const FF_VecStroke ff_strokes_P[] = {{0,9,0,0},{0,0,6,0},{6,0,6,4},{6,4,0,4}};
const FF_VecStroke ff_strokes_W[] = {{0,0,2,9},{2,9,3,6},{3,6,4,9},{4,9,6,0}};
const FF_VecStroke ff_strokes_Y[] = {{0,0,3,4},{3,4,6,0},{3,4,3,9}};
const FF_VecStroke ff_strokes_B[] = {{0,9,0,0},{0,0,5,0},{5,0,6,1},{6,1,5,3},{5,3,0,4},{0,4,5,6},{5,6,6,8},{6,8,5,9},{5,9,0,9}};
const FF_VecStroke ff_strokes_K[] = {{0,0,0,9},{6,0,0,4},{0,4,6,9}};

const FF_VecLetter ff_vecFont[] = {
    {ff_strokes_0, 4}, {ff_strokes_1, 2}, {ff_strokes_2, 5}, {ff_strokes_3, 4}, {ff_strokes_4, 3},
    {ff_strokes_5, 5}, {ff_strokes_6, 5}, {ff_strokes_7, 2}, {ff_strokes_8, 5}, {ff_strokes_9, 4},
    {ff_strokes_S, 5}, {ff_strokes_C, 3}, {ff_strokes_O, 4}, {ff_strokes_R, 5}, {ff_strokes_E, 4},
    {ff_strokes_V, 2}, {ff_strokes_A, 3}, {ff_strokes_L, 2}, {ff_strokes_U, 3}, {ff_strokes_H, 3},
    {ff_strokes_I, 1}, {ff_strokes_G, 5}, {ff_strokes_T, 2}, {ff_strokes_F, 3}, {ff_strokes_M, 4},
    {ff_strokes_N, 3}, {ff_strokes_D, 5}, {ff_strokes_P, 4}, {ff_strokes_W, 5}, {ff_strokes_Y, 3},
    {ff_strokes_B, 9}, {ff_strokes_K, 3}
};

int ff_getCharIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    switch(c) {
        case 'S': return 10; case 'C': return 11; case 'O': return 12; case 'R': return 13;
        case 'E': return 14; case 'V': return 15; case 'A': return 16; case 'L': return 17;
        case 'U': return 18; case 'H': return 19; case 'I': return 20; case 'G': return 21;
        case 'T': return 22; case 'F': return 23; case 'M': return 24; case 'N': return 25;
        case 'D': return 26; case 'P': return 27; case 'W': return 28; case 'Y': return 29;
        case 'B': return 30; case 'K': return 31;
        default: return -1;
    }
}

void ff_drawVecChar(TFT_eSPI &tft, char c, int x, int y, int scale, uint16_t color) {
    int idx = ff_getCharIndex(c);
    if (idx < 0) return;
    for (int i = 0; i < ff_vecFont[idx].n; i++) {
        int x0 = x + ff_vecFont[idx].strokes[i].x0 * scale;
        int y0 = y + ff_vecFont[idx].strokes[i].y0 * scale;
        int x1 = x + ff_vecFont[idx].strokes[i].x1 * scale;
        int y1 = y + ff_vecFont[idx].strokes[i].y1 * scale;
        tft.drawLine(x0, y0, x1, y1, color);
    }
}

void ff_drawVecText(TFT_eSPI &tft, const char* text, int x, int y, int scale, uint16_t color) {
    int spacing = 8 * scale;
    for (int i = 0; text[i]; i++) {
        if (text[i] != ' ') ff_drawVecChar(tft, text[i], x + i * spacing, y, scale, color);
    }
}

int ff_getTextWidth(const char* text, int scale) {
    return strlen(text) * 8 * scale;
}

// ====== ALTITUDE INDICATOR (SIMPLE VECTOR LINES) ======
void ff_drawAltitudeIndicator(TFT_eSPI &tft, float altitude) {
    int barX = FF_SCREEN_W - 25;
    int barY = 40;
    int barHeight = 200;
    int numBars = 20;
    int barSpacing = barHeight / numBars;
    
    // Draw bars from bottom to top using simple vector lines
    for (int i = 0; i < numBars; i++) {
        int y = barY + barHeight - (i * barSpacing) - barSpacing / 2;
        float altLevel = (i * 100.0f) / numBars;
        
        uint16_t barColor;
        if (altLevel < 10) {
            barColor = FF_COLOR_RED;
        } else if (altLevel < 30) {
            barColor = FF_COLOR_YELLOW;
        } else {
            barColor = FF_COLOR_GREEN;
        }
        
        // Draw single horizontal line for each segment
        if (altLevel < altitude) {
            tft.drawLine(barX, y, barX + 20, y, barColor);
        }
    }
}

// ====== DETAILED FEATHERED WINGS ======
void ff_drawFeatheredWings(TFT_eSPI &tft, int cx, int cy, uint16_t color) {
    tft.drawRect(cx - 12, cy - 8, 24, 20, color);
    tft.drawLine(cx - 12, cy + 12, cx, cy + 18, color);
    tft.drawLine(cx, cy + 18, cx + 12, cy + 12, color);
    
    for (int i = 0; i < 5; i++) {
        int x = cx - 10 + i * 5;
        tft.drawLine(x, cy - 6, x, cy + 10, color);
    }
    
    int sx = cx, sy = cy - 25;
    tft.drawLine(sx, sy - 10, sx - 3, sy - 3, color);
    tft.drawLine(sx - 3, sy - 3, sx - 10, sy - 2, color);
    tft.drawLine(sx - 10, sy - 2, sx - 5, sy + 3, color);
    tft.drawLine(sx - 5, sy + 3, sx - 7, sy + 10, color);
    tft.drawLine(sx - 7, sy + 10, sx, sy + 6, color);
    tft.drawLine(sx, sy + 6, sx + 7, sy + 10, color);
    tft.drawLine(sx + 7, sy + 10, sx + 5, sy + 3, color);
    tft.drawLine(sx + 5, sy + 3, sx + 10, sy - 2, color);
    tft.drawLine(sx + 10, sy - 2, sx + 3, sy - 3, color);
    tft.drawLine(sx + 3, sy - 3, sx, sy - 10, color);
    
    for (int i = 0; i < 4; i++) {
        int featherX = cx - 15 - i * 10;
        int featherY = cy - 4 + i;
        tft.drawLine(cx - 15, cy - 4, featherX, featherY, color);
        tft.drawLine(cx - 15, cy + 4, featherX, featherY + 8, color);
        tft.drawLine(featherX, featherY, featherX - 3, featherY + 2, color);
        tft.drawLine(featherX - 3, featherY + 2, featherX - 3, featherY + 6, color);
        tft.drawLine(featherX - 3, featherY + 6, featherX, featherY + 8, color);
    }
    
    for (int i = 0; i < 4; i++) {
        int featherX = cx - 45 - i * 12;
        int featherY = cy - 6 + i * 2;
        tft.drawLine(cx - 45, cy - 6, featherX, featherY, color);
        tft.drawLine(cx - 45, cy + 6, featherX, featherY + 12, color);
        tft.drawLine(featherX, featherY, featherX - 4, featherY + 3, color);
        tft.drawLine(featherX - 4, featherY + 3, featherX - 4, featherY + 9, color);
        tft.drawLine(featherX - 4, featherY + 9, featherX, featherY + 12, color);
    }
    
    for (int i = 0; i < 4; i++) {
        int featherX = cx - 93 - i * 14;
        int featherY = cy - 8 + i * 2;
        tft.drawLine(cx - 93, cy - 8, featherX, featherY, color);
        tft.drawLine(cx - 93, cy + 8, featherX, featherY + 16, color);
        tft.drawLine(featherX, featherY, featherX - 5, featherY + 4, color);
        tft.drawLine(featherX - 5, featherY + 4, featherX - 5, featherY + 12, color);
        tft.drawLine(featherX - 5, featherY + 12, featherX, featherY + 16, color);
    }
    
    for (int i = 0; i < 3; i++) {
        int featherX = cx - 149 - i * 16;
        int featherY = cy - 10 + i * 3;
        tft.drawLine(cx - 149, cy - 10, featherX, featherY, color);
        tft.drawLine(cx - 149, cy + 10, featherX, featherY + 20, color);
        tft.drawLine(featherX, featherY, featherX - 6, featherY + 5, color);
        tft.drawLine(featherX - 6, featherY + 5, featherX - 6, featherY + 15, color);
        tft.drawLine(featherX - 6, featherY + 15, featherX, featherY + 20, color);
    }
    
    for (int i = 0; i < 4; i++) {
        int featherX = cx + 15 + i * 10;
        int featherY = cy - 4 + i;
        tft.drawLine(cx + 15, cy - 4, featherX, featherY, color);
        tft.drawLine(cx + 15, cy + 4, featherX, featherY + 8, color);
        tft.drawLine(featherX, featherY, featherX + 3, featherY + 2, color);
        tft.drawLine(featherX + 3, featherY + 2, featherX + 3, featherY + 6, color);
        tft.drawLine(featherX + 3, featherY + 6, featherX, featherY + 8, color);
    }
    
    for (int i = 0; i < 4; i++) {
        int featherX = cx + 45 + i * 12;
        int featherY = cy - 6 + i * 2;
        tft.drawLine(cx + 45, cy - 6, featherX, featherY, color);
        tft.drawLine(cx + 45, cy + 6, featherX, featherY + 12, color);
        tft.drawLine(featherX, featherY, featherX + 4, featherY + 3, color);
        tft.drawLine(featherX + 4, featherY + 3, featherX + 4, featherY + 9, color);
        tft.drawLine(featherX + 4, featherY + 9, featherX, featherY + 12, color);
    }
    
    for (int i = 0; i < 4; i++) {
        int featherX = cx + 93 + i * 14;
        int featherY = cy - 8 + i * 2;
        tft.drawLine(cx + 93, cy - 8, featherX, featherY, color);
        tft.drawLine(cx + 93, cy + 8, featherX, featherY + 16, color);
        tft.drawLine(featherX, featherY, featherX + 5, featherY + 4, color);
        tft.drawLine(featherX + 5, featherY + 4, featherX + 5, featherY + 12, color);
        tft.drawLine(featherX + 5, featherY + 12, featherX, featherY + 16, color);
    }
    
    for (int i = 0; i < 3; i++) {
        int featherX = cx + 149 + i * 16;
        int featherY = cy - 10 + i * 3;
        tft.drawLine(cx + 149, cy - 10, featherX, featherY, color);
        tft.drawLine(cx + 149, cy + 10, featherX, featherY + 20, color);
        tft.drawLine(featherX, featherY, featherX + 6, featherY + 5, color);
        tft.drawLine(featherX + 6, featherY + 5, featherX + 6, featherY + 15, color);
        tft.drawLine(featherX + 6, featherY + 15, featherX, featherY + 20, color);
    }
}

// ====== FLIGHT STATE ======
struct FF_FlightState {
    float altitude;
    float bankAngle;
    float pitch;
    float playerX;
    float playerY;
    float playerZ;
};

FF_FlightState ff_flight;

// ====== MOUNTAIN TERRAIN ======
struct FF_Mountain {
    int heights[FF_MOUNTAIN_POINTS];
    float scroll;
};

FF_Mountain ff_mountains[3];

void ff_initMountains() {
    for (int layer = 0; layer < 3; layer++) {
        ff_mountains[layer].scroll = 0;
        for (int i = 0; i < FF_MOUNTAIN_POINTS; i++) {
            int baseHeight = 40 + layer * 20;
            ff_mountains[layer].heights[i] = baseHeight + random(0, 30);
        }
    }
}

void ff_drawMountains(TFT_eSPI &tft) {
    int horizonY = FF_SCREEN_H / 2 + (int)(ff_flight.pitch * 5);
    float angleRad = ff_flight.bankAngle * 0.01745329;
    
    for (int layer = 0; layer < 3; layer++) {
        uint16_t color = (layer == 0 ? 0x4208 : (layer == 1 ? 0x2104 : 0x1082));
        
        int segWidth = FF_SCREEN_W / (FF_MOUNTAIN_POINTS - 1);
        int baseY = horizonY + 20 * (3 - layer);
        
        for (int i = 0; i < FF_MOUNTAIN_POINTS - 1; i++) {
            int x1 = i * segWidth;
            int x2 = (i + 1) * segWidth;
            
            int tilt1 = (int)(tan(angleRad) * (x1 - FF_SCREEN_W / 2));
            int tilt2 = (int)(tan(angleRad) * (x2 - FF_SCREEN_W / 2));
            
            int y1 = baseY + ff_mountains[layer].heights[i] + tilt1;
            int y2 = baseY + ff_mountains[layer].heights[i + 1] + tilt2;
            
            // CLIPPING: Only draw if below HUD and left of altitude indicator
            if (y1 < FF_SCREEN_H && y2 < FF_SCREEN_H && 
                y1 >= FF_HUD_TOP && y2 >= FF_HUD_TOP &&
                x1 < FF_HUD_RIGHT && x2 < FF_HUD_RIGHT) {
                tft.drawLine(x1, y1, x2, y2, color);
            }
        }
    }
}

void ff_scrollMountains(float speed) {
    for (int layer = 0; layer < 3; layer++) {
        ff_mountains[layer].scroll += speed * (1.0f + layer * 0.5f);
        
        if (ff_mountains[layer].scroll > 10) {
            ff_mountains[layer].scroll = 0;
            for (int i = 0; i < FF_MOUNTAIN_POINTS - 1; i++) {
                ff_mountains[layer].heights[i] = ff_mountains[layer].heights[i + 1];
            }
            int baseHeight = 40 + layer * 20;
            ff_mountains[layer].heights[FF_MOUNTAIN_POINTS - 1] = baseHeight + random(0, 30);
        }
    }
}

// ====== BULLET ======
struct FF_Bullet {
    float x, y, z;
    float vx, vy, vz;
    bool active;
};

void ff_drawBullet(TFT_eSPI &tft, int screenX, int screenY, float scale, bool erase = false) {
    uint16_t color = erase ? TFT_BLACK : FF_COLOR_YELLOW;
    int size = (int)(4 * scale);
    if (size < 2) size = 2;
    tft.drawLine(screenX - size, screenY, screenX + size, screenY, color);
    tft.drawLine(screenX, screenY - size, screenX, screenY + size, color);
}

// ====== ENEMY JET ======
void ff_drawJet(TFT_eSPI &tft, int x, int y, float scale, uint16_t color, bool erase = false) {
    uint16_t c = erase ? TFT_BLACK : color;
    int s = (int)(scale * 10);
    if (s < 3) s = 3;
    
    tft.drawLine(x, y - s, x, y + s, c);
    tft.drawLine(x, y, x - s * 2, y + s, c);
    tft.drawLine(x, y, x + s * 2, y + s, c);
    tft.drawLine(x - s * 2, y + s, x - s, y + s, c);
    tft.drawLine(x + s * 2, y + s, x + s, y + s, c);
    tft.drawLine(x - s/2, y - s, x + s/2, y - s, c);
}

// ====== HELICOPTER ======
void ff_drawHeli(TFT_eSPI &tft, int x, int y, float scale, bool erase = false) {
    uint16_t c = erase ? TFT_BLACK : FF_COLOR_GREEN;
    int s = (int)(scale * 10);
    if (s < 3) s = 3;
    
    tft.drawEllipse(x, y, s, s/2, c);
    tft.drawLine(x - s * 2, y - s, x + s * 2, y - s, c);
    tft.drawLine(x, y, x + s, y + s/2, c);
}

// ====== GROUND TARGET ======
void ff_drawGroundTarget(TFT_eSPI &tft, int x, int y, int type, float scale, bool erase = false) {
    uint16_t c = erase ? TFT_BLACK : FF_COLOR_RED;
    int s = (int)(scale * 6);
    if (s < 2) s = 2;
    
    if (type == 0) {
        tft.drawRect(x - s, y - s/2, s * 2, s, c);
    } else if (type == 1) {
        tft.drawRect(x - s/2, y - s, s, s * 2, c);
    } else {
        tft.drawLine(x, y, x, y - s, c);
        tft.drawCircle(x, y - s, s/2, c);
    }
}

// ====== ENEMY STRUCTURE ======
struct FF_Enemy {
    float x, y, z;
    float vx, vy, vz;
    int type;
    bool active;
    int health;
    unsigned long spawnTime;
};

struct FF_GroundTarget {
    float x, z;
    int type;
    bool active;
    int health;
};

// ====== GAME STATE ======
struct FF_GameState {
    int score;
    int lives;
    int round;
    
    FF_Bullet bullets[FF_MAX_BULLETS];
    FF_Enemy enemies[FF_MAX_ENEMIES];
    FF_GroundTarget groundTargets[FF_MAX_GROUND];
    
    bool isGroundRound;
    int targetsDestroyed;
    int roundTarget;
    
    unsigned long lastSpawn;
    
    int lastDisplayedScore;
    int lastDisplayedLives;
};

FF_GameState ff_game;

// ====== INIT GAME ======
void ff_initGame() {
    ff_game.score = 0;
    ff_game.lives = FF_MAX_LIVES;
    ff_game.round = 1;
    ff_game.isGroundRound = false;
    ff_game.targetsDestroyed = 0;
    ff_game.roundTarget = 8;
    ff_game.lastSpawn = 0;
    ff_game.lastDisplayedScore = -1;
    ff_game.lastDisplayedLives = -1;
    
    ff_flight.altitude = 50;
    ff_flight.bankAngle = 0;
    ff_flight.pitch = 0;
    ff_flight.playerX = 0;
    ff_flight.playerY = 0;
    ff_flight.playerZ = 0;
    
    for (int i = 0; i < FF_MAX_BULLETS; i++) ff_game.bullets[i].active = false;
    for (int i = 0; i < FF_MAX_ENEMIES; i++) ff_game.enemies[i].active = false;
    for (int i = 0; i < FF_MAX_GROUND; i++) ff_game.groundTargets[i].active = false;
    
    ff_initMountains();
}

// ====== SPAWN ENEMY ======
void ff_spawnEnemy() {
    for (int i = 0; i < FF_MAX_ENEMIES; i++) {
        if (!ff_game.enemies[i].active) {
            ff_game.enemies[i].active = true;
            ff_game.enemies[i].spawnTime = millis();
            
            // Spawn ahead of player in absolute world space
            ff_game.enemies[i].z = ff_flight.playerZ + 400 + random(0, 100);
            ff_game.enemies[i].x = ff_flight.playerX + random(-150, 150);
            ff_game.enemies[i].y = 25 + random(-10, 10);
            
            // Independent movement patterns
            int pattern = random(0, 3);
            switch(pattern) {
                case 0:  // Straight flyer
                    ff_game.enemies[i].vx = random(-5, 5) * 0.1f;
                    ff_game.enemies[i].vy = 0;
                    ff_game.enemies[i].vz = -2.5f - ff_game.round * 0.2f;
                    break;
                case 1:  // Weaving
ff_game.enemies[i].vx = random(10, 25) * 0.1f * (random(0,2) ? 1 : -1);
                    ff_game.enemies[i].vy = random(-5, 5) * 0.1f;
                    ff_game.enemies[i].vz = -2.0f - ff_game.round * 0.2f;
                    break;
                case 2:  // Diving/climbing
                    ff_game.enemies[i].vx = random(-8, 8) * 0.1f;
                    ff_game.enemies[i].vy = random(-15, 15) * 0.1f;
                    ff_game.enemies[i].vz = -2.2f - ff_game.round * 0.2f;
                    break;
            }
            
            ff_game.enemies[i].type = (ff_game.isGroundRound && random(0, 2) == 0) ? 1 : 0;
            ff_game.enemies[i].health = (ff_game.enemies[i].type == 1) ? 3 : 1;
            break;
        }
    }
}

// ====== SPAWN GROUND TARGET ======
void ff_spawnGroundTarget() {
    for (int i = 0; i < FF_MAX_GROUND; i++) {
        if (!ff_game.groundTargets[i].active) {
            ff_game.groundTargets[i].active = true;
            ff_game.groundTargets[i].x = random(-100, 100);
            ff_game.groundTargets[i].z = 250 + random(0, 100);
            ff_game.groundTargets[i].type = random(0, 3);
            ff_game.groundTargets[i].health = 2;
            break;
        }
    }
}

// ====== CONVERT 3D TO 2D ======
void ff_worldToScreen(float worldX, float worldY, float worldZ, int &screenX, int &screenY, float &scale) {
    float relativeX = worldX - ff_flight.playerX;
    float relativeY = worldY - ff_flight.playerY;
    float relativeZ = worldZ - ff_flight.playerZ;
    
    if (relativeZ <= 5) relativeZ = 5;
    scale = 100.0f / relativeZ;
    
    screenX = FF_SCREEN_W / 2 + (int)(relativeX * scale);
    screenY = FF_SCREEN_H / 2 + (int)(relativeY * scale);
}

// ====== SPLASH ======
void ff_splash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    
    int textWidth = ff_getTextWidth("FLIGHT FIGHT", 4);
    int textX = (FF_SCREEN_W - textWidth) / 2;
    ff_drawVecText(tft, "FLIGHT FIGHT", textX, 30, 4, FF_COLOR_CYAN);
    
    ff_drawFeatheredWings(tft, FF_SCREEN_W / 2, FF_SCREEN_H / 2, FF_COLOR_GOLD);
    
    tft.setTextColor(FF_COLOR_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("PRESS BUTTON A TO START", FF_SCREEN_W / 2, FF_SCREEN_H - 30);
    
    while (true) {
        bool btnA = !ss.digitalRead(6);
        bool btnSel = !ss.digitalRead(14);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        if (btnA) {
            delay(200);
            break;
        }
        delay(50);
    }
    
    tft.fillScreen(TFT_BLACK);
}

// ====== MAIN GAME ======
void run_FlightFight(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    while (true) {
        ff_splash(tft, ss);
        ff_initGame();
        
        unsigned long lastFrame = millis();
        bool gameOver = false;
        
        // Initial full screen clear and HUD draw
        tft.fillScreen(TFT_BLACK);
        
        while (!gameOver && ff_game.lives > 0) {
            unsigned long now = millis();
            unsigned long dt = now - lastFrame;
            if (dt < 25) {
                delay(25 - dt);
                now = millis();
                dt = now - lastFrame;
            }
            lastFrame = now;
            
            if (!ss.digitalRead(14)) {
                tft.fillScreen(TFT_BLACK);
                return;
            }
            
            int joyX = ss.analogRead(3);
            int joyY = ss.analogRead(2);
            
// Clear game area only (not HUD areas)
            tft.fillRect(0, FF_HUD_TOP, FF_HUD_RIGHT, FF_SCREEN_H - FF_HUD_TOP, TFT_BLACK);
            
            // Clear protective border strips to catch any stray pixels
            tft.fillRect(0, 18, FF_SCREEN_W, 12, TFT_BLACK);  // Horizontal strip below HUD text
            
            // Update flight controls
            float targetBank = 0;
            float targetPitch = 0;
            
            if (joyX < 400) targetBank = 35;
            else if (joyX > 624) targetBank = -35;
            
            if (joyY < 400) targetPitch = 30;
            else if (joyY > 624) targetPitch = -30;
            
            ff_flight.bankAngle += (targetBank - ff_flight.bankAngle) * 0.15f;
            ff_flight.pitch += (targetPitch - ff_flight.pitch) * 0.1f;
            
            // Update altitude with limits: 0 (ground) to 80 (ceiling)
            ff_flight.altitude += ff_flight.pitch * 0.08f;
            if (ff_flight.altitude < 0) ff_flight.altitude = 0;
            if (ff_flight.altitude > 80) ff_flight.altitude = 80;
            
            // Update player position
            ff_flight.playerX += ff_flight.bankAngle * 0.5f;
            ff_flight.playerY += ff_flight.pitch * 0.4f;
            ff_flight.playerZ += 3.0f;
            
            // Draw mountains
            ff_drawMountains(tft);
            ff_scrollMountains(0.5f);
            
            // Fire button
            bool btnA = !ss.digitalRead(6);
            static bool prevBtnA = false;
            if (btnA && !prevBtnA) {
                for (int i = 0; i < FF_MAX_BULLETS; i++) {
                    if (!ff_game.bullets[i].active) {
                        ff_game.bullets[i].active = true;
                        ff_game.bullets[i].x = ff_flight.playerX;
                        ff_game.bullets[i].y = ff_flight.playerY;
                        ff_game.bullets[i].z = ff_flight.playerZ + 10;
                        ff_game.bullets[i].vx = 0;
                        ff_game.bullets[i].vy = 0;
                        ff_game.bullets[i].vz = 15.0f;
                        break;
                    }
                }
            }
            prevBtnA = btnA;
            
// Update bullets
            for (int i = 0; i < FF_MAX_BULLETS; i++) {
                if (ff_game.bullets[i].active) {
                    ff_game.bullets[i].x += ff_game.bullets[i].vx;
                    ff_game.bullets[i].y += ff_game.bullets[i].vy;
                    ff_game.bullets[i].z += ff_game.bullets[i].vz;
                    
                    if (ff_game.bullets[i].z > ff_flight.playerZ + 500) {
                        ff_game.bullets[i].active = false;
                        continue;
                    }
                    
                    int sx, sy;
                    float scale;
                    ff_worldToScreen(ff_game.bullets[i].x, ff_game.bullets[i].y, ff_game.bullets[i].z, sx, sy, scale);
                    // CLIPPING: Only draw if in game area (not in HUD zones)
                    if (sx >= 0 && sx < FF_HUD_RIGHT && sy >= FF_HUD_TOP && sy < FF_SCREEN_H) {
                        ff_drawBullet(tft, sx, sy, scale, false);
                    }
                }
            }
            
            // Spawn enemies
            if (now - ff_game.lastSpawn > 1500) {
                if (!ff_game.isGroundRound) {
                    ff_spawnEnemy();
                } else {
                    if (random(0, 2) == 0) {
                        ff_spawnEnemy();
                    } else {
                        ff_spawnGroundTarget();
                    }
                }
                ff_game.lastSpawn = now;
            }
            
// Update enemies
            for (int i = 0; i < FF_MAX_ENEMIES; i++) {
                if (ff_game.enemies[i].active) {
                    ff_game.enemies[i].x += ff_game.enemies[i].vx;
                    ff_game.enemies[i].y += ff_game.enemies[i].vy;
                    ff_game.enemies[i].z += ff_game.enemies[i].vz;
                    
                    unsigned long enemyAge = now - ff_game.enemies[i].spawnTime;
                    if (enemyAge % 2000 < 50) {
                        ff_game.enemies[i].vx += random(-5, 5) * 0.05f;
                        ff_game.enemies[i].vy += random(-3, 3) * 0.05f;
                    }
                    
                    if (ff_game.enemies[i].y < 10) ff_game.enemies[i].vy = abs(ff_game.enemies[i].vy);
                    if (ff_game.enemies[i].y > 50) ff_game.enemies[i].vy = -abs(ff_game.enemies[i].vy);
                    
                    if (ff_game.enemies[i].z < ff_flight.playerZ - 100) {
                        ff_game.enemies[i].active = false;
                        continue;
                    }
                    
                    float dx = ff_flight.playerX - ff_game.enemies[i].x;
                    float dy = ff_flight.playerY - ff_game.enemies[i].y;
                    float dz = ff_flight.playerZ - ff_game.enemies[i].z;
                    float dist = sqrt(dx*dx + dy*dy + dz*dz);
                    
                    if (dist < 30) {
                        ff_game.enemies[i].active = false;
                        ff_game.lives--;
                        tft.fillScreen(FF_COLOR_RED);
                        delay(150);
                        tft.fillScreen(TFT_BLACK);
                        ff_game.lastDisplayedScore = -1;
                        ff_game.lastDisplayedLives = -1;
                        continue;
                    }
                    
                    int sx, sy;
                    float scale;
                    ff_worldToScreen(ff_game.enemies[i].x, ff_game.enemies[i].y, ff_game.enemies[i].z, sx, sy, scale);
                    // CLIPPING: Only draw if in game area (not in HUD zones)
                    if (sx >= -50 && sx < FF_HUD_RIGHT && sy >= FF_HUD_TOP && sy < FF_SCREEN_H + 50) {
                        if (ff_game.enemies[i].type == 0) {
                            ff_drawJet(tft, sx, sy, scale, FF_COLOR_RED, false);
                        } else {
                            ff_drawHeli(tft, sx, sy, scale, false);
                        }
                    }
                }
            }
            
// Update ground targets
            for (int i = 0; i < FF_MAX_GROUND; i++) {
                if (ff_game.groundTargets[i].active) {
                    ff_game.groundTargets[i].z -= 1.5f;
                    
                    if (ff_game.groundTargets[i].z < 40) {
                        ff_game.groundTargets[i].active = false;
                        continue;
                    }
                    
                    int sx, sy;
                    float scale;
                    ff_worldToScreen(ff_game.groundTargets[i].x, 0, ff_game.groundTargets[i].z, sx, sy, scale);
                    
                    int horizonY = FF_SCREEN_H / 2 + (int)(ff_flight.pitch * 5);
                    float angleRad = ff_flight.bankAngle * 0.01745329;
                    int tilt = (int)(tan(angleRad) * (sx - FF_SCREEN_W / 2));
                    sy = horizonY + 60 + tilt;
                    
                    // CLIPPING: Only draw if in game area (not in HUD zones)
                    if (sx >= -50 && sx < FF_HUD_RIGHT && sy >= FF_HUD_TOP && sy < FF_SCREEN_H) {
                        ff_drawGroundTarget(tft, sx, sy, ff_game.groundTargets[i].type, scale, false);
                    }
                }
            }
            
            // Check bullet collisions
            for (int i = 0; i < FF_MAX_BULLETS; i++) {
                if (!ff_game.bullets[i].active) continue;
                
                for (int j = 0; j < FF_MAX_ENEMIES; j++) {
                    if (!ff_game.enemies[j].active) continue;
                    
                    float dx = ff_game.bullets[i].x - ff_game.enemies[j].x;
                    float dy = ff_game.bullets[i].y - ff_game.enemies[j].y;
                    float dz = ff_game.bullets[i].z - ff_game.enemies[j].z;
                    float dist = sqrt(dx*dx + dy*dy + dz*dz);
                    
                    if (dist < 25) {
                        ff_game.enemies[j].health--;
                        ff_game.bullets[i].active = false;
                        
                        if (ff_game.enemies[j].health <= 0) {
                            int sx, sy;
                            float scale;
                            ff_worldToScreen(ff_game.enemies[j].x, ff_game.enemies[j].y, ff_game.enemies[j].z, sx, sy, scale);
                            for (int e = 0; e < 3; e++) {
                                tft.drawCircle(sx, sy, 4 + e * 6, FF_COLOR_ORANGE);
                            }
                            
                            ff_game.enemies[j].active = false;
                            ff_game.targetsDestroyed++;
                            ff_game.score += (ff_game.enemies[j].type == 0) ? 100 : 300;
                            ff_game.lastDisplayedScore = -1;
                        }
                        break;
                    }
                }
                
                for (int j = 0; j < FF_MAX_GROUND; j++) {
                    if (!ff_game.groundTargets[j].active) continue;
                    
                    float dx = ff_game.bullets[i].x - ff_game.groundTargets[j].x;
                    float dz = ff_game.bullets[i].z - ff_game.groundTargets[j].z;
                    float dist = sqrt(dx*dx + dz*dz);
                    
                    if (dist < 25) {
                        ff_game.groundTargets[j].health--;
                        ff_game.bullets[i].active = false;
                        
                        if (ff_game.groundTargets[j].health <= 0) {
                            int sx, sy;
                            float scale;
                            ff_worldToScreen(ff_game.groundTargets[j].x, 0, ff_game.groundTargets[j].z, sx, sy, scale);
                            int horizonY = FF_SCREEN_H / 2 + (int)(ff_flight.pitch * 5);
                            float angleRad = ff_flight.bankAngle * 0.01745329;
                            int tilt = (int)(tan(angleRad) * (sx - FF_SCREEN_W / 2));
                            sy = horizonY + 60 + tilt;
                            for (int e = 0; e < 3; e++) {
                                tft.drawCircle(sx, sy, 3 + e * 5, FF_COLOR_ORANGE);
                            }
                            
                            ff_game.groundTargets[j].active = false;
                            ff_game.targetsDestroyed++;
                            ff_game.score += 200;
                            ff_game.lastDisplayedScore = -1;
                        }
                        break;
                    }
                }
            }
            
// Draw HUD only when values change
            if (ff_game.score != ff_game.lastDisplayedScore) {
                tft.fillRect(0, 0, 150, 30, TFT_BLACK);
                char buf[32];
                sprintf(buf, "SCORE %d", ff_game.score);
                ff_drawVecText(tft, buf, 10, 5, 1, FF_COLOR_CYAN);
                ff_game.lastDisplayedScore = ff_game.score;
            }
            
            // VALUE 100 - draw once at start
            static bool valueDrawn = false;
            if (!valueDrawn) {
                ff_drawVecText(tft, "VALUE 100", 200, 5, 1, FF_COLOR_CYAN);
                valueDrawn = true;
            }
            
if (ff_game.lives != ff_game.lastDisplayedLives) {
                tft.fillRect(FF_SCREEN_W - 120, 0, 120, 30, TFT_BLACK);
                char buf[32];
                sprintf(buf, "LIVES %d", ff_game.lives);
                ff_drawVecText(tft, buf, 370, 5, 1, FF_COLOR_CYAN);
                ff_game.lastDisplayedLives = ff_game.lives;
            }
            
            // Draw altitude indicator only when altitude changes
            static int lastAltDrawn = -1;
            if (abs((int)ff_flight.altitude - lastAltDrawn) >= 1) {
                tft.fillRect(FF_SCREEN_W - 35, 35, 35, 215, TFT_BLACK);
                ff_drawAltitudeIndicator(tft, ff_flight.altitude);
                lastAltDrawn = (int)ff_flight.altitude;
            }
            
            // Altitude warnings
            if (ff_flight.altitude < 10 && ff_flight.altitude >= 3) {
                if ((millis() / 300) % 2 == 0) {
                    tft.fillRect(FF_SCREEN_W/2 - 60, FF_SCREEN_H - 25, 120, 20, FF_COLOR_YELLOW);
                    tft.setTextColor(TFT_BLACK, FF_COLOR_YELLOW);
                    tft.setTextSize(2);
                    tft.setTextDatum(MC_DATUM);
                    tft.drawString("PULL UP!", FF_SCREEN_W/2, FF_SCREEN_H - 15);
                } else {
                    tft.fillRect(FF_SCREEN_W/2 - 60, FF_SCREEN_H - 25, 120, 20, TFT_BLACK);
                }
            } else if (ff_flight.altitude < 3) {
                if ((millis() / 200) % 2 == 0) {
                    tft.fillRect(FF_SCREEN_W/2 - 60, FF_SCREEN_H - 25, 120, 20, FF_COLOR_RED);
                    tft.setTextColor(TFT_WHITE, FF_COLOR_RED);
                    tft.setTextSize(2);
                    tft.setTextDatum(MC_DATUM);
                    tft.drawString("PULL UP!", FF_SCREEN_W/2, FF_SCREEN_H - 15);
                } else {
                    tft.fillRect(FF_SCREEN_W/2 - 60, FF_SCREEN_H - 25, 120, 20, TFT_BLACK);
                }
            }
/*            
            // Ceiling warning
            if (ff_flight.altitude > 75) {
                if ((millis() / 300) % 2 == 0) {
                    tft.fillRect(FF_SCREEN_W/2 - 70, 25, 140, 20, FF_COLOR_YELLOW);
                    tft.setTextColor(TFT_BLACK, FF_COLOR_YELLOW);
                    tft.setTextSize(2);
                    tft.setTextDatum(MC_DATUM);
                    tft.drawString("MAX ALTITUDE!", FF_SCREEN_W/2, 35);
                } else {
                    tft.fillRect(FF_SCREEN_W/2 - 70, 25, 140, 20, TFT_BLACK);
                }
            }
*/            
            // Round completion
            if (ff_game.targetsDestroyed >= ff_game.roundTarget) {
                tft.fillScreen(TFT_BLACK);
                ff_drawVecText(tft, "ROUND COMPLETE", 120, 140, 2, FF_COLOR_GREEN);
                delay(2000);
                
                ff_game.round++;
                ff_game.isGroundRound = !ff_game.isGroundRound;
                ff_game.targetsDestroyed = 0;
                ff_game.roundTarget = 6 + ff_game.round * 2;
                
                for (int i = 0; i < FF_MAX_ENEMIES; i++) ff_game.enemies[i].active = false;
                for (int i = 0; i < FF_MAX_GROUND; i++) ff_game.groundTargets[i].active = false;
                for (int i = 0; i < FF_MAX_BULLETS; i++) ff_game.bullets[i].active = false;
                
                ff_game.lastDisplayedScore = -1;
                ff_game.lastDisplayedLives = -1;
                lastAltDrawn = -1;
                valueDrawn = false;
                
                tft.fillScreen(TFT_BLACK);
                
                if (ff_game.isGroundRound) {
                    ff_drawVecText(tft, "GROUND ATTACK", 100, 140, 2, FF_COLOR_RED);
                } else {
                    ff_drawVecText(tft, "AIR COMBAT", 130, 140, 2, FF_COLOR_CYAN);
                }
                delay(2000);
                tft.fillScreen(TFT_BLACK);
            }
            
            // Altitude crash
            if (ff_flight.altitude < 1) {
                ff_game.lives--;
                tft.fillScreen(FF_COLOR_RED);
                delay(200);
                ff_flight.altitude = 50;
                ff_flight.pitch = 0;
                ff_flight.bankAngle = 0;
                ff_game.lastDisplayedScore = -1;
                ff_game.lastDisplayedLives = -1;
                lastAltDrawn = -1;
                tft.fillScreen(TFT_BLACK);
            }
            
            if (ff_game.lives <= 0) {
                gameOver = true;
            }
        }
        
        // Game over
        if (gameOver) {
            tft.fillScreen(TFT_BLACK);
            ff_drawVecText(tft, "GAME OVER", 140, 100, 3, FF_COLOR_RED);
            char buf[32];
            sprintf(buf, "SCORE %d", ff_game.score);
            ff_drawVecText(tft, buf, 150, 160, 2, FF_COLOR_YELLOW);
            
            tft.setTextColor(FF_COLOR_WHITE, TFT_BLACK);
            tft.setTextSize(1);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("HIT BUTTON TO CONTINUE", FF_SCREEN_W / 2, FF_SCREEN_H - 30);
            
            bool waiting = true;
            while (waiting) {
                bool btnA = !ss.digitalRead(6);
                bool btnB = !ss.digitalRead(7);
                bool btnX = !ss.digitalRead(9);
                bool btnY = !ss.digitalRead(10);
                bool btnSel = !ss.digitalRead(14);
                
                if (btnSel) {
                    tft.fillScreen(TFT_BLACK);
                    return;
                }
                
                if (btnA || btnB || btnX || btnY) {
                    delay(200);
                    waiting = false;
                }
                
                delay(50);
            }
            
            tft.fillScreen(TFT_BLACK);
        }
    }
}

#endif // FLIGHT_FIGHT_H