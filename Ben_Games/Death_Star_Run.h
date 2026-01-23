#ifndef DEATH_STAR_RUN_H
#define DEATH_STAR_RUN_H

#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <math.h>

namespace DeathStarRun {

// ============================================================================
// CONSTANTS
// ============================================================================

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
const int SCREEN_CENTER_X = SCREEN_WIDTH / 2;
const int SCREEN_CENTER_Y = SCREEN_HEIGHT / 2;

// Game constants
const int MAX_SHIELDS = 6;
const int MAX_TIE_FIGHTERS = 12;
const int MAX_TURRETS = 16;
const int MAX_STORMTROOPERS = 8;
const int DSR_MAX_PROJECTILES = 32;
const int MAX_PARTICLES = 64;

// Phase timings (in milliseconds)
const uint32_t PHASE1_DURATION = 30000;  // 30 seconds
const uint32_t PHASE2_DURATION = 20000;  // 20 seconds
const uint32_t PHASE3_DURATION = 30000;  // 30 seconds
const uint32_t PHASE4_DURATION = 25000;  // 25 seconds

/* Longer levels
// Phase timings (in milliseconds)
const uint32_t PHASE1_DURATION = 60000;  // 60 seconds (was 30)
const uint32_t PHASE2_DURATION = 40000;  // 40 seconds (was 20)
const uint32_t PHASE3_DURATION = 50000;  // 50 seconds (was 30)
const uint32_t PHASE4_DURATION = 40000;  // 40 seconds (was 25)
*/

// Colors
const uint16_t COLOR_VECTOR_GREEN = 0x07E0;
const uint16_t COLOR_VECTOR_CYAN = 0x07FF;
const uint16_t COLOR_ENEMY_RED = 0xF800;
const uint16_t COLOR_LASER = 0xFFE0;
const uint16_t COLOR_EXPLOSION = 0xFD20;
const uint16_t COLOR_HUD = 0x07E0;
const uint16_t COLOR_TEXT = 0xFFFF;

// ============================================================================
// VECTOR MATH
// ============================================================================

struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    
    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    
    Vector3 operator*(float s) const {
        return Vector3(x * s, y * s, z * s);
    }
    
    float length() const {
        return sqrt(x * x + y * y + z * z);
    }
    
    Vector3 normalized() const {
        float len = length();
        if (len > 0.0001f) {
            return Vector3(x / len, y / len, z / len);
        }
        return Vector3(0, 0, 0);
    }
    
    float dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
};

struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}
};

// ============================================================================
// GAME STATES (REORDERED)
// ============================================================================

enum GameState {
    STATE_SPLASH,
    STATE_BRIEFING,
    STATE_PHASE1_FPS,           // CHANGED: FPS first
    STATE_CUTSCENE_SPACE,       // NEW: Transition to space
    STATE_PHASE2_SPACE,         // CHANGED: TIE fighters second
    STATE_CUTSCENE_SURFACE,     // Transition to surface
    STATE_PHASE3_SURFACE,       // CHANGED: Death Star surface third
    STATE_CUTSCENE_TRENCH,      // Transition to trench
    STATE_PHASE4_TRENCH,        // CHANGED: Trench run last
    STATE_EXPLOSION_CUTSCENE,   // NEW: Death Star exploding
    STATE_VICTORY,
    STATE_GAME_OVER
};

// ============================================================================
// WIREFRAME MODELS
// ============================================================================

struct Edge {
    int v1, v2;
};

struct WireframeModel {
    Vector3* vertices;
    int vertexCount;
    Edge* edges;
    int edgeCount;
};

// ============================================================================
// GAME ENTITIES
// ============================================================================

struct Projectile {
    Vector3 pos;
    Vector3 vel;
    bool active;
    bool fromPlayer;
    uint32_t spawnTime;
    bool isProtonTorpedo;  // NEW: Special torpedo flag
};

struct Particle {
    Vector3 pos;
    Vector3 vel;
    uint16_t color;
    bool active;
    uint32_t spawnTime;
    uint32_t lifetime;
};

struct TIEFighter {
    Vector3 pos;
    Vector3 vel;
    float rotation;
    float rotationSpeed;
    bool alive;
    int type;
    uint32_t lastFireTime;
    int health;
};

struct Turret {
    Vector3 pos;
    float yaw;
    float pitch;
    bool alive;
    bool rising;
    float riseProgress;
    uint32_t lastFireTime;
    int health;
};

struct Stormtrooper {
    Vector3 pos;
    float rotation;
    bool alive;
    int animFrame;
    uint32_t lastFireTime;
    Vector3 vel;
    int health;
    bool alerted;
};

struct Player {
    Vector3 pos;
    Vector3 vel;
    float yaw;
    float pitch;
    int shields;
    int score;
    bool hasProtonTorpedo;
    uint32_t lastFireTime;
    bool invincible;
    uint32_t invincibleUntil;
    bool walking;
    float walkCycle;
};

// ============================================================================
// GLOBAL GAME STATE
// ============================================================================

GameState gameState = STATE_SPLASH;
Player player;
int currentWave = 1;
uint32_t phaseStartTime = 0;

// Entities
TIEFighter tieFighters[MAX_TIE_FIGHTERS];
Turret turrets[MAX_TURRETS];
Stormtrooper stormtroopers[MAX_STORMTROOPERS];
Projectile projectiles[DSR_MAX_PROJECTILES];
Particle particles[MAX_PARTICLES];

// Phase 4 specific (now trench)
float trenchPosition = 0;
bool exhaustPortHit = false;
int trenchObstacles[20];

// Phase 1 specific (now FPS)
Vector3 moonBaseObjective;
bool objectiveComplete = false;

Vector3 leftPipePos;
Vector3 rightPipePos;
int leftPipeHealth = 3;
int rightPipeHealth = 3;
bool reactorDestroyed = false;

// Camera for 3D projection
Vector3 cameraPos;
float cameraYaw;
float cameraPitch;
float cameraRoll;

// Wireframe models
WireframeModel tieFighterModel;
WireframeModel turretBaseModel;
WireframeModel stormtrooperModel;
WireframeModel exhaustPortModel;

// ============================================================================
// 3D PROJECTION
// ============================================================================

const float FOV = 60.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;

bool project3D(Vector3 worldPos, int& screenX, int& screenY, float& depth) {
    Vector3 relative = worldPos - cameraPos;
    
    float cosYaw = cos(-cameraYaw);
    float sinYaw = sin(-cameraYaw);
    float tempX = relative.x * cosYaw - relative.z * sinYaw;
    float tempZ = relative.x * sinYaw + relative.z * cosYaw;
    relative.x = tempX;
    relative.z = tempZ;
    
    float cosPitch = cos(-cameraPitch);
    float sinPitch = sin(-cameraPitch);
    float tempY = relative.y * cosPitch - relative.z * sinPitch;
    tempZ = relative.y * sinPitch + relative.z * cosPitch;
    relative.y = tempY;
    relative.z = tempZ;
    
    depth = relative.z;
    
    if (depth < NEAR_PLANE) return false;
    
    float fovScale = 1.0f / tan(FOV * 0.5f * PI / 180.0f);
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    
    float projX = (relative.x / depth) * fovScale / aspectRatio;
    float projY = (relative.y / depth) * fovScale;
    
    screenX = SCREEN_CENTER_X + (int)(projX * SCREEN_CENTER_X);
    screenY = SCREEN_CENTER_Y - (int)(projY * SCREEN_CENTER_Y);
    
    return (screenX >= -100 && screenX < SCREEN_WIDTH + 100 && 
            screenY >= -100 && screenY < SCREEN_HEIGHT + 100);
}

// ============================================================================
// WIREFRAME MODEL DEFINITIONS
// ============================================================================

void initTIEFighterModel() {
    static Vector3 tieVertices[] = {
        {0, 3, 0}, {0, -3, 0}, {3, 0, 0}, {-3, 0, 0},
        {0, 0, 3}, {0, 0, -3},
        {-8, 8, 0}, {-8, 6, 2}, {-8, -6, 2}, {-8, -8, 0}, {-8, -6, -2}, {-8, 6, -2},
        {8, 8, 0}, {8, 6, 2}, {8, -6, 2}, {8, -8, 0}, {8, -6, -2}, {8, 6, -2},
        {-5, 2, 0}, {-5, -2, 0}, {5, 2, 0}, {5, -2, 0}
    };
    
    static Edge tieEdges[] = {
        {0, 2}, {0, 3}, {0, 4}, {0, 5},
        {1, 2}, {1, 3}, {1, 4}, {1, 5},
        {2, 4}, {4, 3}, {3, 5}, {5, 2},
        {6, 7}, {7, 8}, {8, 9}, {9, 10}, {10, 11}, {11, 6},
        {12, 13}, {13, 14}, {14, 15}, {15, 16}, {16, 17}, {17, 12},
        {3, 18}, {3, 19}, {2, 20}, {2, 21},
        {18, 6}, {18, 11}, {19, 9}, {19, 10},
        {20, 12}, {20, 17}, {21, 15}, {21, 16}
    };
    
    tieFighterModel.vertices = tieVertices;
    tieFighterModel.vertexCount = 22;
    tieFighterModel.edges = tieEdges;
    tieFighterModel.edgeCount = 36;
}

void initTurretModel() {
    static Vector3 turretBaseVertices[] = {
        {2, 0, 2}, {2, 0, -2}, {-2, 0, -2}, {-2, 0, 2},
        {2, 3, 2}, {2, 3, -2}, {-2, 3, -2}, {-2, 3, 2},
        {0, 4, -4}, {0, 4, -8}, {0, 5, -8}, {0, 5, -4}
    };
    
    static Edge turretBaseEdges[] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
        {8, 9}, {10, 11}, {8, 10}, {9, 11},
        {6, 8}, {6, 10}
    };
    
    turretBaseModel.vertices = turretBaseVertices;
    turretBaseModel.vertexCount = 12;
    turretBaseModel.edges = turretBaseEdges;
    turretBaseModel.edgeCount = 18;
}

// ============================================================================
// WIREFRAME MODEL DEFINITIONS - IMPROVED STORMTROOPER
// ============================================================================

void initStormtrooperModel() {
    // More detailed stormtrooper with armor and blaster rifle
    static Vector3 trooperVertices[] = {
        // Helmet (more rounded)
        {0, 7.5f, 0}, {0.8f, 7.2f, 0.3f}, {0.8f, 7.2f, -0.3f}, 
        {-0.8f, 7.2f, 0.3f}, {-0.8f, 7.2f, -0.3f},
        {0.6f, 6.5f, 0.5f}, {0.6f, 6.5f, -0.5f}, 
        {-0.6f, 6.5f, 0.5f}, {-0.6f, 6.5f, -0.5f},
        
        // Neck
        {0.4f, 6.2f, 0}, {-0.4f, 6.2f, 0},  // 9, 10
        
        // Torso (chest armor)
        {1.0f, 5.8f, 0.3f}, {-1.0f, 5.8f, 0.3f},  // 11, 12 - shoulders
        {0.9f, 5.0f, 0.4f}, {-0.9f, 5.0f, 0.4f},  // 13, 14 - chest
        {0.7f, 3.5f, 0.3f}, {-0.7f, 3.5f, 0.3f},  // 15, 16 - waist
        {0.8f, 3.0f, 0.2f}, {-0.8f, 3.0f, 0.2f},  // 17, 18 - belt
        
        // Right arm (holding rifle)
        {1.2f, 5.5f, 0.2f}, {1.3f, 4.5f, 0.3f},  // 19, 20 - upper arm
        {1.4f, 3.8f, 0.4f}, {1.5f, 3.5f, 0.5f},  // 21, 22 - forearm/hand
        
        // Left arm (supporting rifle)
        {-1.2f, 5.5f, 0.2f}, {-1.3f, 4.5f, 0.5f},  // 23, 24
        {-1.2f, 4.0f, 0.8f}, {-1.1f, 3.8f, 1.0f},  // 25, 26 - hand under rifle
        
        // Legs
        {0.5f, 3.0f, 0.1f}, {0.5f, 1.5f, 0.1f},  // 27, 28 - right upper
        {0.5f, 0.3f, 0.1f}, {0.5f, 0, 0.2f},     // 29, 30 - right lower/foot
        {-0.5f, 3.0f, 0.1f}, {-0.5f, 1.5f, 0.1f}, // 31, 32 - left upper
        {-0.5f, 0.3f, 0.1f}, {-0.5f, 0, 0.2f},   // 33, 34 - left lower/foot
        
        // Blaster rifle (E-11)
        {1.5f, 3.5f, 0.5f},   // 35 - grip (same as right hand)
        {0.5f, 3.8f, 1.2f},   // 36 - stock
        {-0.5f, 3.9f, 1.3f},  // 37 - rear sight
        {-1.5f, 4.0f, 1.4f},  // 38 - barrel end (MUZZLE)
        {-1.6f, 4.0f, 1.5f},  // 39 - muzzle tip
    };
    
    static Edge trooperEdges[] = {
        // Helmet
        {0, 1}, {0, 2}, {0, 3}, {0, 4},
        {1, 5}, {2, 6}, {3, 7}, {4, 8},
        {5, 6}, {6, 8}, {8, 7}, {7, 5},
        {5, 9}, {7, 10},
        
        // Neck to torso
        {9, 11}, {10, 12},
        
        // Torso armor
        {11, 12}, {11, 13}, {12, 14}, {13, 14},
        {13, 15}, {14, 16}, {15, 16},
        {15, 17}, {16, 18}, {17, 18},
        
        // Right arm (rifle holding)
        {11, 19}, {19, 20}, {20, 21}, {21, 22},
        
        // Left arm (rifle support)
        {12, 23}, {23, 24}, {24, 25}, {25, 26},
        
        // Legs
        {17, 27}, {27, 28}, {28, 29}, {29, 30},
        {18, 31}, {31, 32}, {32, 33}, {33, 34},
        
        // Belt details
        {17, 18},
        
        // Blaster rifle
        {22, 35}, {35, 36}, {36, 37}, {37, 38}, {38, 39},
        {26, 37},  // Left hand to mid-rifle
    };
    
    stormtrooperModel.vertices = trooperVertices;
    stormtrooperModel.vertexCount = 40;
    stormtrooperModel.edges = trooperEdges;
    stormtrooperModel.edgeCount = 56;
}

WireframeModel xwingModel;

void initXWingModel() {
    static Vector3 xwingVertices[] = {
        {0, 0, 8}, {0, 0, -8},
        {1, 0, 4}, {-1, 0, 4},
        {0, 1, 3}, {0, -1, 3},
        {6, 0, 0}, {-6, 0, 0},
        {3, 0, 2}, {-3, 0, 2},
        {6, 2, 0}, {-6, 2, 0},
        {6, -2, 0}, {-6, -2, 0},
        {2, 1, -6}, {-2, 1, -6},
        {2, -1, -6}, {-2, -1, -6},
    };
    
    static Edge xwingEdges[] = {
        {0, 2}, {0, 3}, {0, 4}, {0, 5},
        {2, 4}, {3, 4}, {2, 5}, {3, 5},
        {2, 1}, {3, 1}, {4, 1}, {5, 1},
        {8, 6}, {9, 7}, {8, 10}, {9, 11}, {8, 12}, {9, 13},
        {6, 10}, {6, 12}, {7, 11}, {7, 13},
        {10, 12}, {11, 13},
        {2, 8}, {3, 9},
        {14, 1}, {15, 1}, {16, 1}, {17, 1},
        {14, 15}, {16, 17},
    };
    
    xwingModel.vertices = xwingVertices;
    xwingModel.vertexCount = 18;
    xwingModel.edges = xwingEdges;
    xwingModel.edgeCount = 30;
}

// ============================================================================
// PARTICLE SYSTEM - ADD DISINTEGRATION EFFECT
// ============================================================================

void initExhaustPortModel() {
    static Vector3 exhaustVertices[] = {
        {3, 3, 0}, {3, -3, 0}, {-3, -3, 0}, {-3, 3, 0},
        {1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {-1, 1, 0},
        {0.5f, 0.5f, -2}, {0.5f, -0.5f, -2}, {-0.5f, -0.5f, -2}, {-0.5f, 0.5f, -2}
    };
    
    static Edge exhaustEdges[] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {8, 9}, {9, 10}, {10, 11}, {11, 8},
        {4, 8}, {5, 9}, {6, 10}, {7, 11}
    };
    
    exhaustPortModel.vertices = exhaustVertices;
    exhaustPortModel.vertexCount = 12;
    exhaustPortModel.edges = exhaustEdges;
    exhaustPortModel.edgeCount = 16;
}

void initAllModels() {
    Serial.println("[DeathStar] Initializing wireframe models...");
    initTIEFighterModel();
    initTurretModel();
    initStormtrooperModel();
    initExhaustPortModel();
    initXWingModel();  // ← ADD THIS LINE
    Serial.println("[DeathStar] Models initialized");
}

// ============================================================================
// RENDERING
// ============================================================================

void drawWireframeModel(TFT_eSprite* buffer, WireframeModel* model, Vector3 worldPos, 
                        float rotX, float rotY, float rotZ, uint16_t color) {
    for (int i = 0; i < model->edgeCount; i++) {
        Vector3 v1 = model->vertices[model->edges[i].v1];
        Vector3 v2 = model->vertices[model->edges[i].v2];
        
        if (rotY != 0) {
            float cosY = cos(rotY);
            float sinY = sin(rotY);
            float tempX = v1.x * cosY - v1.z * sinY;
            v1.z = v1.x * sinY + v1.z * cosY;
            v1.x = tempX;
            tempX = v2.x * cosY - v2.z * sinY;
            v2.z = v2.x * sinY + v2.z * cosY;
            v2.x = tempX;
        }
        
        if (rotX != 0) {
            float cosX = cos(rotX);
            float sinX = sin(rotX);
            float tempY = v1.y * cosX - v1.z * sinX;
            v1.z = v1.y * sinX + v1.z * cosX;
            v1.y = tempY;
            tempY = v2.y * cosX - v2.z * sinX;
            v2.z = v2.y * sinX + v2.z * cosX;
            v2.y = tempY;
        }
        
        v1 = v1 + worldPos;
        v2 = v2 + worldPos;
        
        int x1, y1, x2, y2;
        float depth1, depth2;
        
        if (project3D(v1, x1, y1, depth1) && project3D(v2, x2, y2, depth2)) {
            if (depth1 > 50) {
                float fade = 1.0f - ((depth1 - 50) / 200.0f);
                if (fade < 0.3f) fade = 0.3f;
                if (fade > 1.0f) fade = 1.0f;
                color = (uint16_t)(color * fade);
            }
            buffer->drawLine(x1, y1, x2, y2, color);
        }
    }
}

void drawCrosshair(TFT_eSprite* buffer, bool locked) {
    bool enemyInSights = false;
    
    for (int i = 0; i < MAX_TIE_FIGHTERS; i++) {
        if (tieFighters[i].alive) {
            int sx, sy;
            float depth;
            if (project3D(tieFighters[i].pos, sx, sy, depth)) {
                int dx = sx - SCREEN_CENTER_X;
                int dy = sy - SCREEN_CENTER_Y;
                if (sqrt(dx*dx + dy*dy) < 50) {
                    enemyInSights = true;
                    break;
                }
            }
        }
    }
    
    uint16_t color = enemyInSights ? COLOR_ENEMY_RED : COLOR_VECTOR_GREEN;
    
    if (player.invincible && (millis() / 100) % 2 == 0) {
        color = COLOR_LASER;
    }
    
    int cx = SCREEN_CENTER_X;
    int cy = SCREEN_CENTER_Y;
    int size = 15;
    int gap = 5;
    
    buffer->drawLine(cx - size, cy, cx - gap, cy, color);
    buffer->drawLine(cx + gap, cy, cx + size, cy, color);
    buffer->drawLine(cx, cy - size, cx, cy - gap, color);
    buffer->drawLine(cx, cy + gap, cx, cy + size, color);
    
    buffer->fillCircle(cx, cy, 2, color);
    
    buffer->drawLine(cx - size, cy - size, cx - size + 5, cy - size, color);
    buffer->drawLine(cx - size, cy - size, cx - size, cy - size + 5, color);
    
    buffer->drawLine(cx + size, cy - size, cx + size - 5, cy - size, color);
    buffer->drawLine(cx + size, cy - size, cx + size, cy - size + 5, color);
    
    buffer->drawLine(cx - size, cy + size, cx - size + 5, cy + size, color);
    buffer->drawLine(cx - size, cy + size, cx - size, cy + size - 5, color);
    
    buffer->drawLine(cx + size, cy + size, cx + size - 5, cy + size, color);
    buffer->drawLine(cx + size, cy + size, cx + size, cy + size - 5, color);
}

void drawShields(TFT_eSprite* buffer) {
    int shieldWidth = 40;
    int shieldHeight = 8;
    int spacing = 5;
    int startX = 10;
    int startY = 10;
    
    for (int i = 0; i < MAX_SHIELDS; i++) {
        uint16_t color = (i < player.shields) ? COLOR_VECTOR_CYAN : 0x2104;
        
        if (player.invincible && (millis() / 200) % 2 == 0) {
            color = COLOR_LASER;
        }
        
        buffer->drawRect(startX + i * (shieldWidth + spacing), startY, shieldWidth, shieldHeight, color);
        if (i < player.shields) {
            buffer->fillRect(startX + i * (shieldWidth + spacing) + 2, startY + 2, shieldWidth - 4, shieldHeight - 4, color);
        }
    }
}

void drawHUD(TFT_eSprite* buffer) {
    drawShields(buffer);
    
    buffer->setTextDatum(TC_DATUM);
    buffer->setTextColor(COLOR_HUD);
    buffer->drawString(String(player.score), SCREEN_CENTER_X, 25, 4);  // CHANGED from 10 to 25
    
    buffer->setTextDatum(TR_DATUM);
    buffer->drawString("WAVE " + String(currentWave), SCREEN_WIDTH - 10, 10, 2);
    
    buffer->setTextDatum(BL_DATUM);
    String phaseText = "";
    switch(gameState) {
        case STATE_PHASE1_FPS: phaseText = "GROUND ASSAULT"; break;
        case STATE_PHASE2_SPACE: phaseText = "SPACE COMBAT"; break;
        case STATE_PHASE3_SURFACE: phaseText = "SURFACE ASSAULT"; break;
        case STATE_PHASE4_TRENCH: phaseText = "TRENCH RUN"; break;
    }
    buffer->drawString(phaseText, 10, SCREEN_HEIGHT - 10, 2);
}

// ============================================================================
// PARTICLE SYSTEM
// ============================================================================

void spawnParticle(Vector3 pos, Vector3 vel, uint16_t color, uint32_t lifetime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].pos = pos;
            particles[i].vel = vel;
            particles[i].color = color;
            particles[i].active = true;
            particles[i].spawnTime = millis();
            particles[i].lifetime = lifetime;
            return;
        }
    }
}

void spawnExplosion(Vector3 pos, uint16_t color) {
    for (int i = 0; i < 20; i++) {
        Vector3 vel(
            (random(200) - 100) / 50.0f,
            (random(200) - 100) / 50.0f,
            (random(200) - 100) / 50.0f
        );
        spawnParticle(pos, vel, color, 800);
    }
}

// ADD THIS HERE - after spawnExplosion
void spawnDisintegration(Vector3 pos) {
    for (int i = 0; i < 30; i++) {
        Vector3 vel(
            (random(100) - 50) / 100.0f,
            random(100) / 50.0f + 1.0f,
            (random(100) - 50) / 100.0f
        );
        
        uint16_t colors[] = {0xFFFF, 0xCE79, 0x9CF3, 0x632C};
        uint16_t color = colors[random(4)];
        
        spawnParticle(pos, vel, color, 800 + random(400));
    }
}

void updateParticles() {
    uint32_t now = millis();
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            if (now - particles[i].spawnTime > particles[i].lifetime) {
                particles[i].active = false;
            } else {
                particles[i].pos = particles[i].pos + particles[i].vel;
            }
        }
    }
}

void renderParticles(TFT_eSprite* buffer) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            int sx, sy;
            float depth;
            if (project3D(particles[i].pos, sx, sy, depth)) {
                buffer->drawPixel(sx, sy, particles[i].color);
            }
        }
    }
}

// ============================================================================
// PROJECTILE SYSTEM
// ============================================================================

void fireProjectile(Vector3 pos, Vector3 direction, bool fromPlayer, bool isTorpedo = false) {
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].pos = pos;
            projectiles[i].vel = direction * (isTorpedo ? 20.0f : (fromPlayer ? 15.0f : 10.0f));
            projectiles[i].active = true;
            projectiles[i].fromPlayer = fromPlayer;
            projectiles[i].spawnTime = millis();
            projectiles[i].isProtonTorpedo = isTorpedo;
            
            if (!fromPlayer) {
                Serial.printf("[DeathStar] ENEMY Projectile %d: Pos(%.1f,%.1f,%.1f)\n", 
                             i, pos.x, pos.y, pos.z);
            }
            return;
        }
    }
}

void updateProjectiles() {
    uint32_t now = millis();
    
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].pos = projectiles[i].pos + projectiles[i].vel;
            
            uint32_t maxLifetime;
            if (gameState == STATE_PHASE1_FPS) {
                maxLifetime = 1000;
            } else if (projectiles[i].isProtonTorpedo) {
                maxLifetime = 8000;
            } else {
                maxLifetime = projectiles[i].fromPlayer ? 4000 : 6000;
            }
            
            if (now - projectiles[i].spawnTime > maxLifetime) {
                projectiles[i].active = false;
            }
            
            if (abs(projectiles[i].pos.x) > 1000 || abs(projectiles[i].pos.y) > 1000 || 
                abs(projectiles[i].pos.z) > 1000) {
                projectiles[i].active = false;
            }
        }
    }
}

void renderProjectiles(TFT_eSprite* buffer) {
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            int sx, sy;
            float depth;
            if (project3D(projectiles[i].pos, sx, sy, depth)) {
if (projectiles[i].isProtonTorpedo) {
                    // ULTRA BRIGHT TORPEDO - IMPOSSIBLE TO MISS
                    
                    // FLASH ENTIRE SCREEN while torpedo is active
                    if ((millis() / 100) % 2 == 0) {
                        buffer->fillScreen(0xFFE0);  // Yellow flash
                        delay(10);
                        buffer->fillSprite(TFT_BLACK);  // Redraw
                    }
                    
                    int size = 25;  // MASSIVE
                    
                    float pulse = 1.0f + sin(millis() / 50.0f) * 0.8f;  // Fast pulse
                    int pulseSize = (int)(size * pulse);
                    
                    // Multiple layers
                    buffer->fillCircle(sx, sy, pulseSize + 8, 0xFFE0);
                    buffer->fillCircle(sx, sy, pulseSize + 4, 0xFFFF);
                    buffer->fillCircle(sx, sy, pulseSize, 0xFFE0);
                    
                    for (int r = 0; r < 5; r++) {
                        buffer->drawCircle(sx, sy, pulseSize + r * 3, 0xFFFF);
                    }
                    
                    // SUPER THICK TRAIL
                    Vector3 trailPos = projectiles[i].pos - projectiles[i].vel.normalized() * 40.0f;
                    int tx, ty;
                    float td;
                    if (project3D(trailPos, tx, ty, td)) {
                        for (int t = -10; t <= 10; t++) {
                            buffer->drawLine(sx + t, sy, tx + t, ty, 0xFFE0);
                            buffer->drawLine(sx, sy + t, tx, ty + t, 0xFFE0);
                        }
                        for (int t = -5; t <= 5; t++) {
                            buffer->drawLine(sx + t, sy, tx + t, ty, 0xFFFF);
                            buffer->drawLine(sx, sy + t, tx, ty + t, 0xFFFF);
                        }
                    }
                    
                    // GIANT TEXT
                    buffer->setTextDatum(TC_DATUM);
                    buffer->setTextColor(0xFFFF);
                    buffer->drawString(">>> TORPEDO <<<", SCREEN_CENTER_X, 50, 4);
                    
                    Serial.printf("[TORPEDO] SCREEN:(%d,%d) WORLD:(%.1f,%.1f,%.1f) DEPTH:%.1f\n",
                                 sx, sy, projectiles[i].pos.x, projectiles[i].pos.y, projectiles[i].pos.z, depth);
                } else if (projectiles[i].fromPlayer) {
                    // Player bullets
                    if (gameState == STATE_PHASE1_FPS) {
                        // CYAN LASER BEAM for FPS
                        Vector3 trailPos = projectiles[i].pos - projectiles[i].vel.normalized() * 10.0f;
                        int tx, ty;
                        float td;
                        
                        if (project3D(trailPos, tx, ty, td)) {
                            buffer->drawLine(sx, sy, tx, ty, COLOR_VECTOR_CYAN);
                            buffer->drawLine(sx+1, sy, tx+1, ty, COLOR_VECTOR_CYAN);
                            buffer->drawLine(sx-1, sy, tx-1, ty, COLOR_VECTOR_CYAN);
                            buffer->drawLine(sx, sy+1, tx, ty+1, COLOR_VECTOR_CYAN);
                            buffer->drawLine(sx, sy-1, tx, ty-1, COLOR_VECTOR_CYAN);
                            buffer->fillCircle(sx, sy, 3, 0xFFFF);
                        }
                    } else {
                        // Space combat - yellow crosses
                        int size = 8;
                        if (depth > 20) {
                            size = (int)(8.0f * 20.0f / depth);
                            if (size < 2) size = 2;
                        }
                        buffer->fillCircle(sx, sy, size, COLOR_LASER);
                        buffer->drawLine(sx - size*2, sy, sx + size*2, sy, COLOR_LASER);
                        buffer->drawLine(sx, sy - size*2, sx, sy + size*2, COLOR_LASER);
                    }
                } else {
                    // Enemy bullets
                    if (gameState == STATE_PHASE1_FPS) {
                        // RED LASER BEAMS
                        Vector3 trailPos = projectiles[i].pos - projectiles[i].vel.normalized() * 10.0f;
                        int tx, ty;
                        float td;
                        
                        if (project3D(trailPos, tx, ty, td)) {
                            buffer->drawLine(sx, sy, tx, ty, COLOR_ENEMY_RED);
                            buffer->drawLine(sx+1, sy, tx+1, ty, COLOR_ENEMY_RED);
                            buffer->drawLine(sx-1, sy, tx-1, ty, COLOR_ENEMY_RED);
                            buffer->drawLine(sx, sy+1, tx, ty+1, COLOR_ENEMY_RED);
                            buffer->drawLine(sx, sy-1, tx, ty-1, COLOR_ENEMY_RED);
                            buffer->fillCircle(sx, sy, 3, 0xFFFF);
                        }
                    } else {
                        // Other phases - growing balls
                        int size = 3;
                        if (depth < 100) {
                            size = (int)(10.0f * (100.0f - depth) / 100.0f) + 3;
                            if (size > 15) size = 15;
                        }
                        
                        buffer->fillCircle(sx, sy, size, COLOR_ENEMY_RED);
                        
                        Vector3 trailPos = projectiles[i].pos - projectiles[i].vel.normalized() * 5.0f;
                        int tx, ty;
                        float td;
                        if (project3D(trailPos, tx, ty, td)) {
                            buffer->drawLine(sx, sy, tx, ty, COLOR_ENEMY_RED);
                        }
                    }
                }
            }
        }
    }
}

// ============================================================================
// PHASE 1: UPDATE - FIXED ONE-SHOT KILLS & REACTOR
// ============================================================================

void updatePhase1(Adafruit_seesaw &ss) {
    int joyX = ss.analogRead(2);
    int joyY = ss.analogRead(3);
    
    float inputForward = -((joyX - 512) / 512.0f);
    float inputTurn = -((joyY - 512) / 512.0f);
    
    if (abs(inputForward) < 0.15f) inputForward = 0;
    if (abs(inputTurn) < 0.15f) inputTurn = 0;
    
    cameraYaw += inputTurn * 0.05f;
    
    // CLAMP yaw to prevent flipping
    if (cameraYaw > PI) cameraYaw -= TWO_PI;
    if (cameraYaw < -PI) cameraYaw += TWO_PI;
    
    if (inputForward != 0) {
        Vector3 forward(sin(cameraYaw), 0, cos(cameraYaw));
        Vector3 newPos = cameraPos + forward * inputForward * 1.0f;
        
        // IMPROVED: Check wall collision BEFORE moving
        if (newPos.x > -23 && newPos.x < 23) {  // Allow movement if not hitting walls
            cameraPos = newPos;
            player.walking = true;
            player.walkCycle += 0.4f;
        } else {
            // Hit wall - stop but don't flip
            cameraPos.x = (cameraPos.x > 0) ? 22 : -22;  // Clamp to wall
            player.walking = false;
        }
    } else {
        player.walking = false;
    }
    
    static bool fireButtonWasPressed = false;
    bool fireButtonPressed = !ss.digitalRead(6);
    
    if (fireButtonPressed && !fireButtonWasPressed && millis() - player.lastFireTime > 200) {
        float cosYaw = cos(cameraYaw);
        float sinYaw = sin(cameraYaw);
        
        Vector3 forward(-sinYaw, 0, cosYaw);
        
        // BOTH GUNS FIRE - LOWERED to trooper height
        Vector3 leftGunOffset(-0.4f, 3.5f, 1.5f);
        float leftRotX = leftGunOffset.x * cosYaw - leftGunOffset.z * sinYaw;
        float leftRotZ = leftGunOffset.x * sinYaw + leftGunOffset.z * cosYaw;
        Vector3 leftGunTip = cameraPos + Vector3(leftRotX, 3.5f, leftRotZ);
        
        Vector3 rightGunOffset(0.4f, 3.5f, 1.5f);
        float rightRotX = rightGunOffset.x * cosYaw - rightGunOffset.z * sinYaw;
        float rightRotZ = rightGunOffset.x * sinYaw + rightGunOffset.z * cosYaw;
        Vector3 rightGunTip = cameraPos + Vector3(rightRotX, 3.5f, rightRotZ);
        
        // Fire from BOTH guns
        fireProjectile(leftGunTip, forward, true);
        fireProjectile(rightGunTip, forward, true);
        
        player.lastFireTime = millis();
        
        Serial.printf("[FPS] DUAL FIRE! Left:(%.1f,%.1f,%.1f) Right:(%.1f,%.1f,%.1f) Forward:(%.2f,%.2f,%.2f)\n",
                     leftGunTip.x, leftGunTip.y, leftGunTip.z,
                     rightGunTip.x, rightGunTip.y, rightGunTip.z,
                     forward.x, forward.y, forward.z);
    }
    fireButtonWasPressed = fireButtonPressed;
    
    for (int i = 0; i < MAX_STORMTROOPERS; i++) {
        if (!stormtroopers[i].alive) continue;
        
        Vector3 toPlayer = cameraPos - stormtroopers[i].pos;
        float distToPlayer = toPlayer.length();
        
        if (distToPlayer < 50) {
            stormtroopers[i].alerted = true;
        }
        
        if (stormtroopers[i].alerted && stormtroopers[i].alive && stormtroopers[i].health > 0) {
            stormtroopers[i].rotation = atan2(toPlayer.x, toPlayer.z);
            
            if (millis() - stormtroopers[i].lastFireTime > 2000 + random(1000)) {
                Vector3 playerTorso = cameraPos + Vector3(0, -1.5f, 0);
                Vector3 dir = (playerTorso - stormtroopers[i].pos).normalized();
                
                float muzzleLocalX = -1.6f;
                float muzzleLocalY = 3.5f;
                float muzzleLocalZ = 1.5f;
                
                float cosRot = cos(stormtroopers[i].rotation);
                float sinRot = sin(stormtroopers[i].rotation);
                
                float worldMuzzleX = muzzleLocalX * cosRot - muzzleLocalZ * sinRot;
                float worldMuzzleZ = muzzleLocalX * sinRot + muzzleLocalZ * cosRot;
                
                Vector3 muzzlePos = stormtroopers[i].pos + Vector3(worldMuzzleX, muzzleLocalY, worldMuzzleZ);
                
                fireProjectile(muzzlePos, dir, false);
                stormtroopers[i].lastFireTime = millis();
            }
        }
    }
    
    updateProjectiles();
    
    // FIXED: Better hit detection for stormtroopers
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
        if (!projectiles[i].active || !projectiles[i].fromPlayer) continue;
        
        for (int j = 0; j < MAX_STORMTROOPERS; j++) {
            if (!stormtroopers[j].alive) continue;
            
            Vector3 trooperCenter = stormtroopers[j].pos + Vector3(0, 3.5f, 0);
            
            float dx = projectiles[i].pos.x - trooperCenter.x;
            float dz = projectiles[i].pos.z - trooperCenter.z;
            float horizontalDist = sqrt(dx*dx + dz*dz);
            float dy = abs(projectiles[i].pos.y - trooperCenter.y);
            
            // BIGGER HITBOX: 8 horizontal, 6 vertical
            if (horizontalDist < 8.0f && dy < 6.0f) {
                Serial.printf("[FPS] *** HIT TROOPER %d ***\n", j);
                Serial.printf("  Bullet:(%.1f,%.1f,%.1f) Trooper:(%.1f,%.1f,%.1f) HDist:%.1f VDist:%.1f\n",
                             projectiles[i].pos.x, projectiles[i].pos.y, projectiles[i].pos.z,
                             trooperCenter.x, trooperCenter.y, trooperCenter.z, horizontalDist, dy);
                Serial.printf("  BEFORE: alive=%d health=%d\n", stormtroopers[j].alive, stormtroopers[j].health);
                
                projectiles[i].active = false;
                stormtroopers[j].alive = false;
                stormtroopers[j].health = 0;
                
                Serial.printf("  AFTER: alive=%d health=%d\n", stormtroopers[j].alive, stormtroopers[j].health);
                
                spawnDisintegration(trooperCenter);
                player.score += 150;
                break;
            }
        }
        
        // Check LEFT PIPE
        if (leftPipeHealth > 0) {
            float dx = projectiles[i].pos.x - leftPipePos.x;
            float dz = projectiles[i].pos.z - leftPipePos.z;
            float horizontalDist = sqrt(dx*dx + dz*dz);
            float dy = abs(projectiles[i].pos.y - leftPipePos.y);
            
            if (horizontalDist < 8.0f && dy < 8.0f) {
                projectiles[i].active = false;
                leftPipeHealth--;
                
                spawnExplosion(leftPipePos, COLOR_EXPLOSION);
                player.score += 100;
                
                Serial.printf("[FPS] LEFT PIPE HIT! Health:%d\n", leftPipeHealth);
                break;
            }
        }
        
        // Check RIGHT PIPE
        if (rightPipeHealth > 0) {
            float dx = projectiles[i].pos.x - rightPipePos.x;
            float dz = projectiles[i].pos.z - rightPipePos.z;
            float horizontalDist = sqrt(dx*dx + dz*dz);
            float dy = abs(projectiles[i].pos.y - rightPipePos.y);
            
            if (horizontalDist < 8.0f && dy < 8.0f) {
                projectiles[i].active = false;
                rightPipeHealth--;
                
                spawnExplosion(rightPipePos, COLOR_EXPLOSION);
                player.score += 100;
                
                Serial.printf("[FPS] RIGHT PIPE HIT! Health:%d\n", rightPipeHealth);
                break;
            }
        }
    }
    
    // Check if reactor destroyed
    if (!reactorDestroyed && leftPipeHealth <= 0 && rightPipeHealth <= 0) {
        reactorDestroyed = true;
        
        // BIG EXPLOSION
        for (int p = 0; p < 100; p++) {
            Vector3 pvel(
                (random(200) - 100) / 20.0f,
                (random(200) - 100) / 20.0f,
                (random(200) - 100) / 20.0f
            );
            spawnParticle(moonBaseObjective, pvel, COLOR_EXPLOSION, 2000);
        }
        
        player.score += 1000;
        Serial.println("[FPS] *** REACTOR DESTROYED! ***");
        
        // Move to cutscene
        phaseStartTime = millis();
        gameState = STATE_CUTSCENE_SPACE;
    }
    
    if (!player.invincible) {
        for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
            if (!projectiles[i].active || projectiles[i].fromPlayer) continue;
            
            Vector3 playerTorso = cameraPos + Vector3(0, -1.5f, 0);
            float dist = (projectiles[i].pos - playerTorso).length();
            
            if (dist < 2.5f) {
                projectiles[i].active = false;
                player.shields--;
                player.invincible = true;
                player.invincibleUntil = millis() + 1000;
                
                if (player.shields <= 0) {
                    gameState = STATE_GAME_OVER;
                }
            }
        }
    }
    
    if (player.invincible && millis() > player.invincibleUntil) {
        player.invincible = false;
    }
    
    updateParticles();
}

// ============================================================================
// PHASE 1: FPS MOON BASE (NOW FIRST)
// ============================================================================

void initPhase1() {
    Serial.println("[DeathStar] Initializing Phase 1: Ground Assault");
    
    cameraPos = Vector3(0, 5, -50);
    cameraYaw = 0;
    cameraPitch = 0;
    player.walking = false;
    player.walkCycle = 0;
    objectiveComplete = false;
    reactorDestroyed = false;
    
    moonBaseObjective = Vector3(0, 8, 200);
    leftPipePos = Vector3(-15, 6, 190);
    rightPipePos = Vector3(15, 6, 190);
    leftPipeHealth = 3;
    rightPipeHealth = 3;
    
    int numTroopers = 5 + currentWave;  // INCREASED from 3 to 5
    if (numTroopers > MAX_STORMTROOPERS) numTroopers = MAX_STORMTROOPERS;
    
    // SPREAD troopers MORE along corridor
    for (int i = 0; i < numTroopers; i++) {
        stormtroopers[i].pos = Vector3(
            (random(36) - 18),           // WIDER: -18 to +18 (was -15 to +15)
            0,
            -30 + (i * 40)               // MORE SPREAD: every 40 units (was 50), starts earlier
        );
        stormtroopers[i].rotation = PI;
        stormtroopers[i].alive = true;
        stormtroopers[i].health = 1;
        stormtroopers[i].alerted = true;
        stormtroopers[i].lastFireTime = millis() + random(3000);
        
        Serial.printf("[FPS] Init: Trooper %d at (%.1f, %.1f, %.1f) alive=%d health=%d\n",
                     i, stormtroopers[i].pos.x, stormtroopers[i].pos.y, stormtroopers[i].pos.z,
                     stormtroopers[i].alive, stormtroopers[i].health);
    }
    
    for (int i = numTroopers; i < MAX_STORMTROOPERS; i++) {
        stormtroopers[i].alive = false;
    }
    
    phaseStartTime = millis();
    gameState = STATE_PHASE1_FPS;
}

// ============================================================================
// PHASE 1: RENDER - BETTER GUNS & REACTOR
// ============================================================================

void renderPhase1(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
// Floor grid - EXTENDED
    for (int z = -60; z < 230; z += 10) {  // CHANGED from 380 to 230 (reactor at 200)
        for (int x = -50; x <= 50; x += 10) {
            Vector3 p1(x, 0, z);
            Vector3 p2(x + 10, 0, z);
            Vector3 p3(x, 0, z + 10);
            
            int sx1, sy1, sx2, sy2, sx3, sy3;
            float d1, d2, d3;
            
            if (project3D(p1, sx1, sy1, d1) && project3D(p2, sx2, sy2, d2)) {
                buffer->drawLine(sx1, sy1, sx2, sy2, 0x31A6);
            }
            if (project3D(p1, sx1, sy1, d1) && project3D(p3, sx3, sy3, d3)) {
                buffer->drawLine(sx1, sy1, sx3, sy3, 0x31A6);
            }
        }
    }
    
    // Walls - EXTENDED
    for (int z = -60; z < 230; z += 10) {  // CHANGED from 380 to 230
        Vector3 wl1(-25, 0, z), wl2(-25, 12, z), wl3(-25, 0, z + 10), wl4(-25, 12, z + 10);
        Vector3 wr1(25, 0, z), wr2(25, 12, z), wr3(25, 0, z + 10), wr4(25, 12, z + 10);
        
        
        int sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4;
        float d1, d2, d3, d4;
        
        if (project3D(wl1, sx1, sy1, d1) && project3D(wl2, sx2, sy2, d2)) {
            buffer->drawLine(sx1, sy1, sx2, sy2, COLOR_VECTOR_CYAN);
        }
        if (project3D(wl1, sx1, sy1, d1) && project3D(wl3, sx3, sy3, d3)) {
            buffer->drawLine(sx1, sy1, sx3, sy3, COLOR_VECTOR_CYAN);
        }
        if (project3D(wl2, sx2, sy2, d2) && project3D(wl4, sx4, sy4, d4)) {
            buffer->drawLine(sx2, sy2, sx4, sy4, COLOR_VECTOR_CYAN);
        }
        
        if (project3D(wr1, sx1, sy1, d1) && project3D(wr2, sx2, sy2, d2)) {
            buffer->drawLine(sx1, sy1, sx2, sy2, COLOR_VECTOR_CYAN);
        }
        if (project3D(wr1, sx1, sy1, d1) && project3D(wr3, sx3, sy3, d3)) {
            buffer->drawLine(sx1, sy1, sx3, sy3, COLOR_VECTOR_CYAN);
        }
        if (project3D(wr2, sx2, sy2, d2) && project3D(wr4, sx4, sy4, d4)) {
            buffer->drawLine(sx2, sy2, sx4, sy4, COLOR_VECTOR_CYAN);
        }
    }
    
    // PROGRESSIVE REACTOR REVEAL - only show when player is halfway there
    float distToReactor = 200 - cameraPos.z;
    bool showReactor = (cameraPos.z > 50);   // CHANGED from 100 to 50 - shows at 1/4 distance
    
    if (!reactorDestroyed && showReactor) {
        float scaleFactor = 1.0f;
        if (distToReactor > 50) {
            scaleFactor = 0.2f + (150 - distToReactor) / 150.0f * 0.8f;  // 0.2 to 1.0 over longer range
        }
        
        // Calculate shared scale for both towers (use center distance)
        Vector3 centerBase(0, 0, 195);
        int tempX, tempY;
        float centerDist;
        project3D(centerBase, tempX, tempY, centerDist);
        
        int sharedScale = (int)(200.0f / centerDist * scaleFactor);
        if (sharedScale < 20) sharedScale = 20;
        if (sharedScale > 80) sharedScale = 80;
        
        // === LEFT COOLING TOWER (use shared scale) ===
        Vector3 leftTowerBase(-15, 0, 190);
        int ltx, lty;
        float ltd;
        if (project3D(leftTowerBase, ltx, lty, ltd)) {
            int scale = sharedScale;  // USE SHARED SCALE instead of calculating from ltd
            if (scale < 10) scale = 10;
            if (scale > 80) scale = 80;
        
        // TALLER tower - 12 layers instead of 8
        for (int layer = 0; layer < 12; layer++) {
            float t = layer / 11.0f;
            float radius = 1.0f - 0.4f * sin(t * PI);
            int w = (int)(scale * 0.8f * radius);
            int y = lty - layer * scale / 3;  // Taller spacing
            
            for (int v = -3; v <= 3; v++) {
                int x1 = ltx + (v * w) / 3;
                int nextT = (layer + 1) / 11.0f;
                float nextRadius = 1.0f - 0.4f * sin(nextT * PI);
                int nextW = (int)(scale * 0.8f * nextRadius);
                int x2 = ltx + (v * nextW) / 3;
                int y2 = lty - (layer + 1) * scale / 3;
                if (layer < 11) {
                    buffer->drawLine(x1, y, x2, y2, COLOR_VECTOR_GREEN);
                }
            }
            
            buffer->drawLine(ltx - w, y, ltx + w, y, COLOR_VECTOR_GREEN);
        }
        
        int topY = lty - 12 * scale / 3;
        buffer->drawCircle(ltx, topY, scale * 0.8f, 0xFFFF);
        if ((millis() / 300 + 0) % 3 == 0) {
            for (int i = 0; i < 5; i++) {
                buffer->drawPixel(ltx + random(-scale/2, scale/2), topY - 10 - i * 3, 0xCE79);
            }
        }
    }
    
    // === RIGHT COOLING TOWER (MUCH TALLER) ===
        Vector3 rightTowerBase(15, 0, 190);
        int rtx, rty;
        float rtd;
        if (project3D(rightTowerBase, rtx, rty, rtd)) {
            int scale = sharedScale;  // USE SHARED SCALE instead of calculating from rtd
        if (scale < 20) scale = 20;
        if (scale > 80) scale = 80;
        
        for (int layer = 0; layer < 12; layer++) {
            float t = layer / 11.0f;
            float radius = 1.0f - 0.4f * sin(t * PI);
            int w = (int)(scale * 0.8f * radius);
            int y = rty - layer * scale / 3;
            
            for (int v = -3; v <= 3; v++) {
                int x1 = rtx + (v * w) / 3;
                int nextT = (layer + 1) / 11.0f;
                float nextRadius = 1.0f - 0.4f * sin(nextT * PI);
                int nextW = (int)(scale * 0.8f * nextRadius);
                int x2 = rtx + (v * nextW) / 3;
                int y2 = rty - (layer + 1) * scale / 3;
                if (layer < 11) {
                    buffer->drawLine(x1, y, x2, y2, COLOR_VECTOR_GREEN);
                }
            }
            
            buffer->drawLine(rtx - w, y, rtx + w, y, COLOR_VECTOR_GREEN);
        }
        
        int topY = rty - 12 * scale / 3;
        buffer->drawCircle(rtx, topY, scale * 0.8f, 0xFFFF);
        if ((millis() / 300 + 1) % 3 == 0) {
            for (int i = 0; i < 5; i++) {
                buffer->drawPixel(rtx + random(-scale/2, scale/2), topY - 10 - i * 3, 0xCE79);
            }
        }
    }
    
    // === CENTER REACTOR BUILDING (TALLER) ===
  // Vector3 centerBase(0, 0, 195);  // CHANGED from 345 to 195
    int cx, cy;
    float cd;
    if (project3D(centerBase, cx, cy, cd)) {
        int scale = (int)(200.0f / cd);
        if (scale < 25) scale = 25;
        if (scale > 90) scale = 90;
        
        int buildingWidth = scale;
        int buildingHeight = scale * 2.0f;  // TALLER - was 1.5f
        
        // Main structure - 6 levels instead of 5
        for (int level = 0; level < 6; level++) {
            int w = buildingWidth - level * 3;
            int h = buildingHeight / 6;
            int yPos = cy - level * h;
            
            buffer->drawRect(cx - w, yPos, w * 2, h, COLOR_VECTOR_GREEN);
            
            for (int col = 0; col < 6; col++) {
                int xPos = cx - w + col * (w * 2) / 5;
                buffer->drawLine(xPos, yPos, xPos, yPos + h, COLOR_VECTOR_GREEN);
            }
            
            if (level > 0) {
                buffer->drawLine(cx - w, yPos, cx + w, yPos, 0xFFFF);
            }
        }
        
        // Tall central chimney
        int chimneyTop = cy - buildingHeight - scale / 2;
        buffer->drawLine(cx - 3, cy - buildingHeight, cx - 3, chimneyTop, 0xFFFF);
        buffer->drawLine(cx + 3, cy - buildingHeight, cx + 3, chimneyTop, 0xFFFF);
        buffer->drawLine(cx - 3, chimneyTop, cx + 3, chimneyTop, 0xFFFF);
        
        // Pulsing reactor core (smaller, no text)
        float pulse = 1.0f + sin(millis() / 200.0f) * 0.5f;
        int coreY = cy - buildingHeight / 2;
        buffer->fillCircle(cx, coreY, (int)(4 * pulse), COLOR_ENEMY_RED);
        buffer->drawCircle(cx, coreY, (int)(6 * pulse), COLOR_LASER);
    }
}

    // === LEFT PIPE (LONGER - connects tower to building) ===
    if (leftPipeHealth > 0 && showReactor) {
        Vector3 pipeStart(-15, 6, 190);   // At tower
        Vector3 pipeEnd(-2, 6, 194);      // CLOSER to center (was -4, 198)
    
    int px1, py1, px2, py2;
    float pd1, pd2;
    if (project3D(pipeStart, px1, py1, pd1) && project3D(pipeEnd, px2, py2, pd2)) {
        int thickness = 3;  // Thin pipe
        
        buffer->drawLine(px1, py1 - thickness, px2, py2 - thickness, COLOR_VECTOR_GREEN);
        buffer->drawLine(px1, py1 + thickness, px2, py2 + thickness, COLOR_VECTOR_GREEN);
        
        int segments = 3;
        for (int s = 0; s <= segments; s++) {
            float t = s / (float)segments;
            int jx = px1 + (px2 - px1) * t;
            int jy = py1 + (py2 - py1) * t;
            buffer->drawLine(jx, jy - thickness, jx, jy + thickness, COLOR_VECTOR_GREEN);
        }
        
        int midX = (px1 + px2) / 2;
        int midY = (py1 + py2) / 2;
        for (int i = 0; i < leftPipeHealth; i++) {
            buffer->fillRect(midX - 10 + i * 7, midY + thickness + 3, 5, 3, COLOR_ENEMY_RED);
        }
        
        if (leftPipeHealth < 3 && (millis() / 200) % 2 == 0) {
            buffer->drawPixel(midX + random(-5, 5), midY + random(-3, 3), COLOR_LASER);
        }
    }
}

  // === RIGHT PIPE (LONGER) ===
    if (rightPipeHealth > 0 && showReactor) {
        Vector3 pipeStart(15, 6, 190);    // At tower
        Vector3 pipeEnd(2, 6, 194);       // CLOSER to center (was 4, 198)
    
    int px1, py1, px2, py2;
    float pd1, pd2;
    if (project3D(pipeStart, px1, py1, pd1) && project3D(pipeEnd, px2, py2, pd2)) {
        int thickness = 3;
        
        buffer->drawLine(px1, py1 - thickness, px2, py2 - thickness, COLOR_VECTOR_GREEN);
        buffer->drawLine(px1, py1 + thickness, px2, py2 + thickness, COLOR_VECTOR_GREEN);
        
        int segments = 3;
        for (int s = 0; s <= segments; s++) {
            float t = s / (float)segments;
            int jx = px1 + (px2 - px1) * t;
            int jy = py1 + (py2 - py1) * t;
            buffer->drawLine(jx, jy - thickness, jx, jy + thickness, COLOR_VECTOR_GREEN);
        }
        
        int midX = (px1 + px2) / 2;
        int midY = (py1 + py2) / 2;
        for (int i = 0; i < rightPipeHealth; i++) {
            buffer->fillRect(midX - 10 + i * 7, midY + thickness + 3, 5, 3, COLOR_ENEMY_RED);
        }
        
        if (rightPipeHealth < 3 && (millis() / 200) % 2 == 0) {
            buffer->drawPixel(midX + random(-5, 5), midY + random(-3, 3), COLOR_LASER);
        }
    }
}

    
    // Draw stormtroopers - WITH DEBUG
    for (int i = 0; i < MAX_STORMTROOPERS; i++) {
        if (stormtroopers[i].alive) {
            drawWireframeModel(buffer, &stormtrooperModel, stormtroopers[i].pos, 0, stormtroopers[i].rotation, 0, COLOR_ENEMY_RED);
    }
    
renderProjectiles(buffer);
    renderParticles(buffer);
    
    // === DRAW 3D PERSPECTIVE GUNS (like floor/walls) ===
    uint16_t gunMetal = 0x4208;
    uint16_t barrel = 0x7BEF;
    uint16_t glow = COLOR_VECTOR_CYAN;
    
    // Calculate rotation for current camera yaw
    float cosYaw = cos(cameraYaw);
    float sinYaw = sin(cameraYaw);
    
    // LEFT GUN in 3D space
    Vector3 leftGunBase(-0.4f, 3.5f, 1.0f);  // CHANGED from 3.8f to 3.5f
    
    // Rotate gun position by camera yaw
    float leftRotX = leftGunBase.x * cosYaw - leftGunBase.z * sinYaw;
    float leftRotZ = leftGunBase.x * sinYaw + leftGunBase.z * cosYaw;
    Vector3 leftGunWorld = cameraPos + Vector3(leftRotX, leftGunBase.y, leftRotZ);
    
    // Gun tip (pointing forward)
    Vector3 leftGunTipOffset(0, 0, 0.5f);  // 0.5 units forward
    float tipRotX = leftGunTipOffset.x * cosYaw - leftGunTipOffset.z * sinYaw;
    float tipRotZ = leftGunTipOffset.x * sinYaw + leftGunTipOffset.z * cosYaw;
    Vector3 leftGunTipWorld = leftGunWorld + Vector3(tipRotX, 0, tipRotZ);
    
    // Project to screen
    int lgx, lgy, ltx, lty;
    float lgd, ltd;
    if (project3D(leftGunWorld, lgx, lgy, lgd) && project3D(leftGunTipWorld, ltx, lty, ltd)) {
        // Barrel line (3 pixels thick)
        buffer->drawLine(lgx, lgy, ltx, lty, barrel);
        buffer->drawLine(lgx, lgy + 1, ltx, lty + 1, barrel);
        buffer->drawLine(lgx, lgy - 1, ltx, lty - 1, barrel);
        
        // Gun body rectangle
        buffer->drawRect(lgx - 3, lgy - 2, 6, 4, gunMetal);
        
        // Muzzle glow at tip
        buffer->fillCircle(ltx, lty, 3, glow);
        buffer->drawCircle(ltx, lty, 4, 0xFFFF);
    }
    
    // RIGHT GUN in 3D space
    Vector3 rightGunBase(0.4f, 3.5f, 1.0f);  // CHANGED from 3.8f to 3.5f
    float rightRotX = rightGunBase.x * cosYaw - rightGunBase.z * sinYaw;
    float rightRotZ = rightGunBase.x * sinYaw + rightGunBase.z * cosYaw;
    Vector3 rightGunWorld = cameraPos + Vector3(rightRotX, rightGunBase.y, rightRotZ);
    
    Vector3 rightGunTipWorld = rightGunWorld + Vector3(tipRotX, 0, tipRotZ);
    
    int rgx, rgy, rtx, rty;
    float rgd, rtd;
    if (project3D(rightGunWorld, rgx, rgy, rgd) && project3D(rightGunTipWorld, rtx, rty, rtd)) {
        // Barrel line
        buffer->drawLine(rgx, rgy, rtx, rty, barrel);
        buffer->drawLine(rgx, rgy + 1, rtx, rty + 1, barrel);
        buffer->drawLine(rgx, rgy - 1, rtx, rty - 1, barrel);
        
        // Gun body rectangle
        buffer->drawRect(rgx - 3, rgy - 2, 6, 4, gunMetal);
        
        // Muzzle glow at tip
        buffer->fillCircle(rtx, rty, 3, glow);
        buffer->drawCircle(rtx, rty, 4, 0xFFFF);
    }
    
    drawCrosshair(buffer, false);
    drawHUD(buffer);
    
    // Objective text
   // buffer->setTextDatum(BC_DATUM);
   // buffer->setTextColor(COLOR_LASER);
   // if (leftPipeHealth > 0 || rightPipeHealth > 0) {
   //     buffer->drawString("DESTROY THE REACTOR PIPES!", SCREEN_CENTER_X, SCREEN_HEIGHT - 10, 2);
   // } else {
   //     buffer->drawString("REACTOR CRITICAL!", SCREEN_CENTER_X, SCREEN_HEIGHT - 10, 2);
   // }
}
}
// ============================================================================
// PHASE 2: SPACE COMBAT (NOW SECOND)
// ============================================================================

void initPhase2() {
    Serial.println("[DeathStar] Initializing Phase 2: Space Combat");
    
    cameraPos = Vector3(0, 0, 0);
    cameraYaw = 0;
    cameraPitch = 0;
    
    int numTIEs = 4 + (currentWave * 2);
    if (numTIEs > MAX_TIE_FIGHTERS) numTIEs = MAX_TIE_FIGHTERS;
    
    float speedMultiplier = 0.5f + (currentWave - 1) * 0.1f;
    if (speedMultiplier > 2.0f) speedMultiplier = 2.0f;
    
    for (int i = 0; i < numTIEs; i++) {
        tieFighters[i].pos = Vector3(
            random(200) - 100,
            random(100) - 50,
            random(100) + 150
        );
        tieFighters[i].vel = Vector3(
            (random(100) - 50) / 100.0f * speedMultiplier,
            (random(100) - 50) / 100.0f * speedMultiplier,
            (-2.0f - (currentWave * 0.3f)) * speedMultiplier
        );
        tieFighters[i].rotation = random(628) / 100.0f;
        tieFighters[i].rotationSpeed = (random(100) - 50) / 100.0f;
        tieFighters[i].alive = true;
        tieFighters[i].type = (i == 0 && currentWave >= 3) ? 1 : 0;
        tieFighters[i].health = tieFighters[i].type == 1 ? 3 : 1;
        tieFighters[i].lastFireTime = millis();
    }
    
    for (int i = numTIEs; i < MAX_TIE_FIGHTERS; i++) {
        tieFighters[i].alive = false;
    }
    
    phaseStartTime = millis();
    gameState = STATE_PHASE2_SPACE;
}

void updatePhase2(Adafruit_seesaw &ss) {
    int joyX = ss.analogRead(2);
    int joyY = ss.analogRead(3);
    
    float inputX = -((joyY - 512) / 512.0f);
    float inputY = ((joyX - 512) / 512.0f);
    
    if (abs(inputX) < 0.15f) inputX = 0;
    if (abs(inputY) < 0.15f) inputY = 0;
    
    cameraYaw += inputX * 0.03f;
    cameraPitch += inputY * 0.03f;
    
    if (cameraPitch > HALF_PI * 0.8f) cameraPitch = HALF_PI * 0.8f;
    if (cameraPitch < -HALF_PI * 0.8f) cameraPitch = -HALF_PI * 0.8f;
    
    static bool fireButtonWasPressed = false;
    bool fireButtonPressed = !ss.digitalRead(6);
    
    if (fireButtonPressed && !fireButtonWasPressed && millis() - player.lastFireTime > 150) {
        float cosYaw = cos(cameraYaw);
        float sinYaw = sin(cameraYaw);
        float cosPitch = cos(cameraPitch);
        float sinPitch = sin(cameraPitch);
        
        Vector3 forward(-sinYaw * cosPitch, -sinPitch, cosYaw * cosPitch);
        
        Vector3 viewCorners[4] = {
            {-30, 20, 1},
            {30, 20, 1},
            {-30, -20, 1},
            {30, -20, 1}
        };
        
        for (int i = 0; i < 4; i++) {
            Vector3 corner = viewCorners[i];
            
            float cosY = cos(-cameraYaw);
            float sinY = sin(-cameraYaw);
            float rotX = corner.x * cosY - corner.z * sinY;
            float rotZ = corner.x * sinY + corner.z * cosY;
            corner.x = rotX;
            corner.z = rotZ;
            
            float cosP = cos(-cameraPitch);
            float sinP = sin(-cameraPitch);
            float rotY = corner.y * cosP - corner.z * sinP;
            rotZ = corner.y * sinP + corner.z * cosP;
            corner.y = rotY;
            corner.z = rotZ;
            
            Vector3 spawnPos = cameraPos + corner;
            fireProjectile(spawnPos, forward, true);
        }
        
        player.lastFireTime = millis();
    }
    fireButtonWasPressed = fireButtonPressed;
    
    for (int i = 0; i < MAX_TIE_FIGHTERS; i++) {
        if (!tieFighters[i].alive) continue;
        
        tieFighters[i].pos = tieFighters[i].pos + tieFighters[i].vel;
        tieFighters[i].rotation += tieFighters[i].rotationSpeed;
        
        if (tieFighters[i].pos.z < -20 || tieFighters[i].pos.z > 300) {
            tieFighters[i].alive = false;
            continue;
        }
        
        if (tieFighters[i].pos.z < -15) {
            tieFighters[i].pos = Vector3(
                random(200) - 100,
                random(100) - 50,
                200 + random(100)
            );
            tieFighters[i].vel.z = -2.0f - (currentWave * 0.3f);
        }
        
        if (millis() - tieFighters[i].lastFireTime > 2000 + random(2000)) {
            Vector3 dirToPlayer = (cameraPos - tieFighters[i].pos).normalized();
            fireProjectile(tieFighters[i].pos, dirToPlayer, false);
            tieFighters[i].lastFireTime = millis();
        }
    }
    
updateProjectiles();
    
    // Check collisions - player shots vs TIE fighters
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
        if (!projectiles[i].active || !projectiles[i].fromPlayer) continue;
        
        // Check vs TIE fighters
        for (int j = 0; j < MAX_TIE_FIGHTERS; j++) {
            if (!tieFighters[j].alive) continue;
            
            float dist = (projectiles[i].pos - tieFighters[j].pos).length();
            if (dist < 8.0f) {
                tieFighters[j].health--;
                projectiles[i].active = false;
                
                if (tieFighters[j].health <= 0) {
                    tieFighters[j].alive = false;
                    spawnExplosion(tieFighters[j].pos, COLOR_EXPLOSION);
                    player.score += (tieFighters[j].type == 1) ? 500 : 100;
                }
                break;
            }
        }
        
        // NEW: Check vs ENEMY BULLETS (shoot them down!)
        for (int j = 0; j < DSR_MAX_PROJECTILES; j++) {
            if (i == j) continue;  // Don't check self
            if (!projectiles[j].active || projectiles[j].fromPlayer) continue;
            
            float dist = (projectiles[i].pos - projectiles[j].pos).length();
            if (dist < 5.0f) {  // Close enough to intercept
                projectiles[i].active = false;
                projectiles[j].active = false;
                
                // Small explosion
                spawnExplosion(projectiles[j].pos, COLOR_LASER);
                player.score += 10;  // Bonus for shooting down bullets
                
                Serial.println("[Space] Shot down enemy bullet!");
                break;
            }
        }
    }
    
    if (!player.invincible) {
        for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
            if (!projectiles[i].active || projectiles[i].fromPlayer) continue;
            
            int sx, sy;
            float depth;
            bool onScreen = project3D(projectiles[i].pos, sx, sy, depth);
            
            if (!onScreen) continue;
            
            float zDiff = abs(projectiles[i].pos.z - cameraPos.z);
            int dx = sx - SCREEN_CENTER_X;
            int dy = sy - SCREEN_CENTER_Y;
            float screenDist = sqrt(dx*dx + dy*dy);
            
            if (zDiff < 2.0f && screenDist < 60.0f) {
                projectiles[i].active = false;
                player.shields--;
                player.invincible = true;
                player.invincibleUntil = millis() + 1500;
                
                spawnExplosion(projectiles[i].pos, COLOR_ENEMY_RED);
                
                if (player.shields <= 0) {
                    gameState = STATE_GAME_OVER;
                }
            }
        }
    }
    
    if (player.invincible && millis() > player.invincibleUntil) {
        player.invincible = false;
    }
    
    updateParticles();
    
    if (millis() - phaseStartTime > PHASE2_DURATION) {
        if (player.shields < MAX_SHIELDS) {
            player.shields++;
        }
        gameState = STATE_CUTSCENE_SURFACE;
    }
}

void renderPhase2(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    for (int i = 0; i < 200; i++) {
        float angle1 = (i * 2.4f);
        float angle2 = (i * 1.7f);
        
        float starX = cos(angle1) * 150;
        float starY = sin(angle2) * 100;
        float starZ = sin(angle1) * 150;
        
        Vector3 starPos(starX, starY, starZ);
        int sx, sy;
        float depth;
        if (project3D(starPos, sx, sy, depth)) {
            buffer->drawPixel(sx, sy, 0xFFFF);
        }
    }
    
    Vector3 deathStarPos(0, 0, 250);
    int dsx, dsy;
    float dsd;
    if (project3D(deathStarPos, dsx, dsy, dsd)) {
        int radius = 30;
        buffer->drawCircle(dsx, dsy, radius, COLOR_VECTOR_GREEN);
        buffer->drawCircle(dsx, dsy, radius - 8, COLOR_VECTOR_GREEN);
        buffer->drawLine(dsx - radius, dsy, dsx + radius, dsy, COLOR_VECTOR_GREEN);
    }
    
    for (int i = 0; i < MAX_TIE_FIGHTERS; i++) {
        if (tieFighters[i].alive) {
            uint16_t color = (tieFighters[i].type == 1) ? TFT_YELLOW : TFT_RED;
            drawWireframeModel(buffer, &tieFighterModel, tieFighters[i].pos, 
                             0, tieFighters[i].rotation, 0, color);
        }
    }
    
    renderProjectiles(buffer);
    renderParticles(buffer);
    drawCrosshair(buffer, false);
    drawHUD(buffer);
}

// ============================================================================
// PHASE 3: DEATH STAR SURFACE (NOW THIRD) - IMPROVED
// ============================================================================

void initPhase3() {
    Serial.println("[DeathStar] Initializing Phase 3: Surface Assault");
    
    cameraPos = Vector3(0, 20, -200);  // LOWERED from 50 to 20
    cameraYaw = 0;
    cameraPitch = -0.3f;  // CHANGED from -0.2f - look down more
    
    int numTurrets = 8 + (currentWave * 2);
    if (numTurrets > MAX_TURRETS) numTurrets = MAX_TURRETS;
    
    float deathStarRadius = 500.0f;  // Smaller curve for more visible curvature
    
// SPREAD OUT turrets across entire approach
    for (int i = 0; i < numTurrets; i++) {
        float turretX = (random(180) - 90);      // WIDER: -90 to +90 (was -70 to +70)
        float turretZ = 30 + (i * 50) + random(20);
        
        float distFromCenter = sqrt(turretX*turretX + turretZ*turretZ);
        float distSq = distFromCenter * distFromCenter;
        float radiusSq = deathStarRadius * deathStarRadius;
        
        float turretY = 0;
        if (distSq < radiusSq) {
            turretY = sqrt(radiusSq - distSq) - deathStarRadius;
        }
        
        turrets[i].pos = Vector3(turretX, turretY, turretZ);
        turrets[i].yaw = 0;
        turrets[i].pitch = 0;
        turrets[i].alive = true;
        turrets[i].rising = true;
        turrets[i].riseProgress = 0;
        turrets[i].health = 2;
        turrets[i].lastFireTime = millis();
    }
    
    for (int i = numTurrets; i < MAX_TURRETS; i++) {
        turrets[i].alive = false;
    }
    
    phaseStartTime = millis();
    gameState = STATE_PHASE3_SURFACE;
}

void updatePhase3(Adafruit_seesaw &ss) {
    // Auto-scroll forward
    cameraPos.z += 1.5f;
    
    int joyX = ss.analogRead(2);
    int joyY = ss.analogRead(3);
    
    float inputX = -((joyY - 512) / 512.0f);
    float inputY = ((joyX - 512) / 512.0f);
    
    if (abs(inputX) < 0.15f) inputX = 0;
    if (abs(inputY) < 0.15f) inputY = 0;
    
    cameraYaw += inputX * 0.05f;
    cameraPitch += inputY * 0.03f;
    
    cameraPos.x += inputX * 0.8f;
    
    // CLAMP
    if (cameraPos.x < -40) cameraPos.x = -40;
    if (cameraPos.x > 40) cameraPos.x = 40;
    
    // CALCULATE surface height at current position
    float deathStarRadius = 500.0f;
    float distFromCenter = sqrt(cameraPos.x * cameraPos.x + cameraPos.z * cameraPos.z);
    float distSq = distFromCenter * distFromCenter;
    float radiusSq = deathStarRadius * deathStarRadius;
    
    float surfaceHeight = 0;
    if (distSq < radiusSq) {
        surfaceHeight = sqrt(radiusSq - distSq) - deathStarRadius;
    }
    
    // Keep ship CLOSER - 12 units above surface (was 20)
    float targetHeight = surfaceHeight + 12.0f;  // CHANGED from 20 to 12
    
    // Allow small up/down adjustments
    cameraPos.y += inputY * 0.3f;  // REDUCED from 0.5f
    
    // Constrain to ±5 units from target (tighter than before)
    if (cameraPos.y < targetHeight - 5) cameraPos.y = targetHeight - 5;
    if (cameraPos.y > targetHeight + 5) cameraPos.y = targetHeight + 5;
    
    // STRONGLY pull back to target height (maintains consistent distance)
    cameraPos.y += (targetHeight - cameraPos.y) * 0.15f;  // INCREASED from 0.05f
    
    if (cameraYaw > 0.5f) cameraYaw = 0.5f;
    if (cameraYaw < -0.5f) cameraYaw = -0.5f;
    if (cameraPitch > 0.3f) cameraPitch = 0.3f;
    if (cameraPitch < -0.5f) cameraPitch = -0.5f;
    
    static bool fireButtonWasPressed = false;
    bool fireButtonPressed = !ss.digitalRead(6);
    
    if (fireButtonPressed && !fireButtonWasPressed && millis() - player.lastFireTime > 150) {
        float cosYaw = cos(cameraYaw);
        float sinYaw = sin(cameraYaw);
        float cosPitch = cos(cameraPitch);
        float sinPitch = sin(cameraPitch);
        
        Vector3 forward(-sinYaw * cosPitch, -sinPitch, cosYaw * cosPitch);
        
        Vector3 viewCorners[4] = {
            {-30, 20, 1},
            {30, 20, 1},
            {-30, -20, 1},
            {30, -20, 1}
        };
        
        for (int i = 0; i < 4; i++) {
            Vector3 corner = viewCorners[i];
            
            float cosY = cos(-cameraYaw);
            float sinY = sin(-cameraYaw);
            float rotX = corner.x * cosY - corner.z * sinY;
            float rotZ = corner.x * sinY + corner.z * cosY;
            corner.x = rotX;
            corner.z = rotZ;
            
            float cosP = cos(-cameraPitch);
            float sinP = sin(-cameraPitch);
            float rotY = corner.y * cosP - corner.z * sinP;
            rotZ = corner.y * sinP + corner.z * cosP;
            corner.y = rotY;
            corner.z = rotZ;
            
            Vector3 spawnPos = cameraPos + corner;
            fireProjectile(spawnPos, forward, true);
        }
        
        player.lastFireTime = millis();
    }
    fireButtonWasPressed = fireButtonPressed;
    
    for (int i = 0; i < MAX_TURRETS; i++) {
        if (!turrets[i].alive) continue;
        
        if (turrets[i].rising && turrets[i].riseProgress < 1.0f) {
            turrets[i].riseProgress += 0.02f;
            if (turrets[i].riseProgress >= 1.0f) {
                turrets[i].rising = false;
            }
        }
        
        Vector3 toPlayer = cameraPos - turrets[i].pos;
        turrets[i].yaw = atan2(toPlayer.x, toPlayer.z);
        turrets[i].pitch = atan2(toPlayer.y, sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z));
        
        if (!turrets[i].rising && millis() - turrets[i].lastFireTime > 2000 + random(1000)) {
            Vector3 dir = toPlayer.normalized();
            Vector3 spawnPos = turrets[i].pos + Vector3(0, 5, 0);
            fireProjectile(spawnPos, dir, false);
            turrets[i].lastFireTime = millis();
        }
    }
    
    updateProjectiles();
    
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
        if (!projectiles[i].active || !projectiles[i].fromPlayer) continue;
        
        // Check vs turrets
        for (int j = 0; j < MAX_TURRETS; j++) {
            if (!turrets[j].alive) continue;
            
            Vector3 turretPos = turrets[j].pos;
            turretPos.y += turrets[j].riseProgress * 8.0f;
            
            float dist = (projectiles[i].pos - turretPos).length();
            
            if (dist < 15.0f) {
                turrets[j].health--;
                projectiles[i].active = false;
                
                for (int p = 0; p < 10; p++) {
                    Vector3 pvel(
                        (random(200) - 100) / 30.0f,
                        (random(200) - 100) / 30.0f,
                        (random(200) - 100) / 30.0f
                    );
                    spawnParticle(turretPos, pvel, COLOR_LASER, 500);
                }
                
                if (turrets[j].health <= 0) {
                    turrets[j].alive = false;
                    
                    for (int p = 0; p < 30; p++) {
                        Vector3 pvel(
                            (random(200) - 100) / 20.0f,
                            (random(200) - 100) / 20.0f,
                            (random(200) - 100) / 20.0f
                        );
                        spawnParticle(turretPos, pvel, COLOR_EXPLOSION, 1000);
                    }
                    
                    player.score += 200;
                }
                break;
            }
        }
        
        // NEW: Check vs ENEMY BULLETS
        for (int j = 0; j < DSR_MAX_PROJECTILES; j++) {
            if (i == j) continue;
            if (!projectiles[j].active || projectiles[j].fromPlayer) continue;
            
            float dist = (projectiles[i].pos - projectiles[j].pos).length();
            if (dist < 5.0f) {
                projectiles[i].active = false;
                projectiles[j].active = false;
                
                spawnExplosion(projectiles[j].pos, COLOR_LASER);
                player.score += 10;
                
                Serial.println("[Surface] Shot down enemy bullet!");
                break;
            }
        }
    }
    
    if (!player.invincible) {
        for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
            if (!projectiles[i].active || projectiles[i].fromPlayer) continue;
            
            if ((projectiles[i].pos - cameraPos).length() < 10.0f) {
                projectiles[i].active = false;
                player.shields--;
                player.invincible = true;
                player.invincibleUntil = millis() + 1000;
                
                if (player.shields <= 0) {
                    gameState = STATE_GAME_OVER;
                }
            }
        }
    }
    
    if (player.invincible && millis() > player.invincibleUntil) {
        player.invincible = false;
    }
    
    updateParticles();
    
    if (millis() - phaseStartTime > PHASE3_DURATION) {
        if (player.shields < MAX_SHIELDS) {
            player.shields++;
        }
        gameState = STATE_CUTSCENE_TRENCH;
    }
}

void renderPhase3(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    // Draw stars in background
    for (int i = 0; i < 100; i++) {
        float starX = ((i * 137) % 200) - 100;
        float starY = ((i * 219) % 200) + 50;
        float starZ = ((i * 311) % 200) + 200;
        
        Vector3 starPos(starX, starY, starZ);
        int sx, sy;
        float depth;
        if (project3D(starPos, sx, sy, depth)) {
            buffer->drawPixel(sx, sy, 0xFFFF);
        }
    }
    
    // WIDER CURVED Death Star surface grid
    float deathStarRadius = 500.0f;
    
    for (int z = -100; z < 400; z += 15) {
        for (int x = -150; x <= 150; x += 15) {  // EXTENDED from -100/100 to -150/150
            float worldZ = z;
            float worldX = x;
            
            float distFromCenter = sqrt(worldX*worldX + worldZ*worldZ);
            float distSq = distFromCenter * distFromCenter;
            float radiusSq = deathStarRadius * deathStarRadius;
            
            float curveHeight = 0;
            if (distSq < radiusSq) {
                curveHeight = sqrt(radiusSq - distSq) - deathStarRadius;
            }
            
            Vector3 p1(worldX, curveHeight, worldZ);
            Vector3 p2(worldX + 15, curveHeight, worldZ);
            
            float distFromCenter2 = sqrt(worldX*worldX + (worldZ + 15)*(worldZ + 15));
            float distSq2 = distFromCenter2 * distFromCenter2;
            float curveHeight2 = 0;
            if (distSq2 < radiusSq) {
                curveHeight2 = sqrt(radiusSq - distSq2) - deathStarRadius;
            }
            Vector3 p3(worldX, curveHeight2, worldZ + 15);
            
            int sx1, sy1, sx2, sy2, sx3, sy3;
            float d1, d2, d3;
            
            if (project3D(p1, sx1, sy1, d1) && project3D(p2, sx2, sy2, d2)) {
                buffer->drawLine(sx1, sy1, sx2, sy2, COLOR_VECTOR_GREEN);
            }
            if (project3D(p1, sx1, sy1, d1) && project3D(p3, sx3, sy3, d3)) {
                buffer->drawLine(sx1, sy1, sx3, sy3, COLOR_VECTOR_GREEN);
            }
        }
    }
    
// Calculate player's surface position
    float playerDistFromCenter = sqrt(cameraPos.x * cameraPos.x + cameraPos.z * cameraPos.z);
    float playerDistSq = playerDistFromCenter * playerDistFromCenter;
    float playerRadiusSq = deathStarRadius * deathStarRadius;
    
    float playerSurfaceHeight = 0;
    if (playerDistSq < playerRadiusSq) {
        playerSurfaceHeight = sqrt(playerRadiusSq - playerDistSq) - deathStarRadius;
    }
    
    // Draw turrets - IMPROVED VISIBILITY CHECK
    for (int i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i].alive) {
            // Calculate turret's actual surface height
            float turretDistFromCenter = sqrt(turrets[i].pos.x * turrets[i].pos.x + 
                                             turrets[i].pos.z * turrets[i].pos.z);
            float turretDistSq = turretDistFromCenter * turretDistFromCenter;
            
            float turretSurfaceHeight = 0;
            if (turretDistSq < playerRadiusSq) {
                turretSurfaceHeight = sqrt(playerRadiusSq - turretDistSq) - deathStarRadius;
            }
            
            // Turret position WITH rise animation
            Vector3 turretPos = turrets[i].pos;
            turretPos.y += turrets[i].riseProgress * 8.0f;
            
            // BETTER visibility check:
            // 1. Turret must be ahead of player OR very close
            // 2. Turret must be visible over horizon (not hidden by curve)
            bool isVisible = false;
            
            float zDist = turrets[i].pos.z - cameraPos.z;
            
            if (zDist > -100) {  // Within reasonable range (behind or ahead)
                // Check if turret is on visible side of curve
                // If turret's Z is close to player's Z, it's on same "side"
                if (zDist > -50) {  // Close to player or ahead
                    isVisible = true;
                } else {
                    // Behind player - only visible if high enough above surface
                    if (turretSurfaceHeight >= playerSurfaceHeight - 10) {
                        isVisible = true;
                    }
                }
            }
            
            if (isVisible) {
                int tx, ty;
                float td;
                if (project3D(turretPos, tx, ty, td)) {
                    // Draw turret model
                    drawWireframeModel(buffer, &turretBaseModel, turretPos, 0, turrets[i].yaw, 0, COLOR_ENEMY_RED);
                    
                    // Draw BASE connection line to surface (shows it's attached)
                    Vector3 basePos = turrets[i].pos;
                    basePos.y = turretSurfaceHeight;  // Put base ON the surface
                    int bx, by;
                    float bd;
                    if (project3D(basePos, bx, by, bd)) {
                        buffer->drawLine(tx, ty, bx, by, COLOR_ENEMY_RED);
                        
                        // Draw small base platform on grid
                        buffer->fillCircle(bx, by, 3, COLOR_ENEMY_RED);
                    }
                    
                    // Targeting box
                    int boxSize = (int)(50.0f / (td / 20.0f));
                    if (boxSize < 10) boxSize = 10;
                    if (boxSize > 40) boxSize = 40;
                    
                    buffer->drawRect(tx - boxSize/2, ty - boxSize/2, boxSize, boxSize, COLOR_ENEMY_RED);
                    
                    // Health bar
                    int healthBarWidth = boxSize;
                    int healthFilled = (turrets[i].health * healthBarWidth) / 2;
                    buffer->drawRect(tx - healthBarWidth/2, ty - boxSize/2 - 8, healthBarWidth, 4, 0xFFFF);
                    buffer->fillRect(tx - healthBarWidth/2 + 1, ty - boxSize/2 - 7, healthFilled, 2, COLOR_VECTOR_GREEN);
                }
            }
        }
    }
    
    renderProjectiles(buffer);
    renderParticles(buffer);
    drawCrosshair(buffer, false);
    drawHUD(buffer);
}

// ============================================================================
// PHASE 4: TRENCH RUN (NOW LAST) - IMPROVED
// ============================================================================

void initPhase4() {
    Serial.println("[DeathStar] Initializing Phase 4: Trench Run");
    
    cameraPos = Vector3(0, 0, 0);
    cameraYaw = 0;
    cameraPitch = 0;
    trenchPosition = 0;
    exhaustPortHit = false;
    player.hasProtonTorpedo = true;
    
    for (int i = 0; i < 20; i++) {
        if (currentWave == 1) {
            trenchObstacles[i] = (random(100) < 30) ? random(1, 4) : 0;
        } else if (currentWave == 2) {
            trenchObstacles[i] = (random(100) < 50) ? random(1, 4) : 0;
        } else {
            trenchObstacles[i] = (random(100) < 70) ? random(1, 4) : 0;
        }
    }
    
    phaseStartTime = millis();
    gameState = STATE_PHASE4_TRENCH;
}

void updatePhase4(Adafruit_seesaw &ss) {
    trenchPosition += 2.0f + (currentWave * 0.3f);
    
    int joyX = ss.analogRead(2);
    int joyY = ss.analogRead(3);
    
    float inputX = ((joyY - 512) / 512.0f);    // REMOVED THE NEGATIVE - was: -((joyY - 512) / 512.0f)
    float inputY = ((joyX - 512) / 512.0f);
    
    if (abs(inputX) < 0.15f) inputX = 0;
    if (abs(inputY) < 0.15f) inputY = 0;
    
    cameraPos.x += inputX * 0.5f;   // Now left joystick = left movement
    cameraPos.y -= inputY * 0.5f;
    
    if (cameraPos.x < -15) cameraPos.x = -15;
    if (cameraPos.x > 15) cameraPos.x = 15;
    if (cameraPos.y < -8) cameraPos.y = -8;
    if (cameraPos.y > 8) cameraPos.y = 8;
    
// Fire proton torpedo - NOW WITH A BUTTON
    static bool torpedoButtonWasPressed = false;
    bool torpedoButtonPressed = !ss.digitalRead(6);  // CHANGED from 7 to 6 (A button)
    
    // Only allow torpedo when close to target
    if (torpedoButtonPressed && !torpedoButtonWasPressed && player.hasProtonTorpedo && trenchPosition > 150) {
        Vector3 forward(0, 0, 1);
        fireProjectile(cameraPos + Vector3(0, 0, 5), forward, true, true);
        player.hasProtonTorpedo = false;
        Serial.println("[Trench] *** PROTON TORPEDO FIRED! ***");
    }
    torpedoButtonWasPressed = torpedoButtonPressed;
    
    updateProjectiles();
    
// Check if torpedo hits exhaust port
    if (trenchPosition > 180) {
        Vector3 exhaustPos(0, 0, 220);
        
        for (int i = 0; i < DSR_MAX_PROJECTILES; i++) {
            if (projectiles[i].active && projectiles[i].fromPlayer && projectiles[i].isProtonTorpedo) {
                
                // FIXED: Much larger hitbox for exhaust port
                float dx = projectiles[i].pos.x - exhaustPos.x;
                float dy = projectiles[i].pos.y - exhaustPos.y;
                float dz = projectiles[i].pos.z - exhaustPos.z;
                float dist = sqrt(dx*dx + dy*dy + dz*dz);
                
                Serial.printf("[Trench] Torpedo at (%.1f,%.1f,%.1f) vs Port at (%.1f,%.1f,%.1f) Dist:%.1f\n",
                             projectiles[i].pos.x, projectiles[i].pos.y, projectiles[i].pos.z,
                             exhaustPos.x, exhaustPos.y, exhaustPos.z, dist);
                
                if (dist < 25.0f) {  // HUGE hitbox - 25 units!
                    exhaustPortHit = true;
                    
                    for (int p = 0; p < 50; p++) {
                        Vector3 pvel(
                            (random(200) - 100) / 15.0f,
                            (random(200) - 100) / 15.0f,
                            (random(200) - 100) / 15.0f
                        );
                        spawnParticle(exhaustPos, pvel, COLOR_LASER, 1500);
                    }
                    
                    player.score += 1000;
                    projectiles[i].active = false;
                    
                    Serial.println("[Trench] *** EXHAUST PORT HIT! ***");
                    
                    if (player.shields < MAX_SHIELDS) {
                        player.shields += 2;
                        if (player.shields > MAX_SHIELDS) player.shields = MAX_SHIELDS;
                    }
                    
                    gameState = STATE_EXPLOSION_CUTSCENE;
                    phaseStartTime = millis();
                    return;
                }
            }
        }
    }
    
    if (trenchPosition > 280 && !exhaustPortHit) {
        player.shields--;
        if (player.shields <= 0) {
            gameState = STATE_GAME_OVER;
        } else {
            Serial.println("[DeathStar] Missed exhaust port! Retry...");
            initPhase4();
        }
    }
}

void renderPhase4(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    // Draw trench walls
    for (int z = -50; z < 300; z += 10) {
        float worldZ = z - trenchPosition;
        
        // Left wall
        for (int y = -10; y <= 10; y += 5) {
            Vector3 p1(-20, y, worldZ);
            Vector3 p2(-20, y + 5, worldZ);
            Vector3 p3(-20, y, worldZ + 10);
            
            int sx1, sy1, sx2, sy2, sx3, sy3;
            float d1, d2, d3;
            
            if (project3D(p1, sx1, sy1, d1) && project3D(p2, sx2, sy2, d2)) {
                buffer->drawLine(sx1, sy1, sx2, sy2, COLOR_VECTOR_GREEN);
            }
            if (project3D(p1, sx1, sy1, d1) && project3D(p3, sx3, sy3, d3)) {
                buffer->drawLine(sx1, sy1, sx3, sy3, COLOR_VECTOR_GREEN);
            }
        }
        
        // Right wall
        for (int y = -10; y <= 10; y += 5) {
            Vector3 p1(20, y, worldZ);
            Vector3 p2(20, y + 5, worldZ);
            Vector3 p3(20, y, worldZ + 10);
            
            int sx1, sy1, sx2, sy2, sx3, sy3;
            float d1, d2, d3;
            
            if (project3D(p1, sx1, sy1, d1) && project3D(p2, sx2, sy2, d2)) {
                buffer->drawLine(sx1, sy1, sx2, sy2, COLOR_VECTOR_GREEN);
            }
            if (project3D(p1, sx1, sy1, d1) && project3D(p3, sx3, sy3, d3)) {
                buffer->drawLine(sx1, sy1, sx3, sy3, COLOR_VECTOR_GREEN);
            }
        }
        
        // Bottom
        for (int x = -20; x <= 20; x += 5) {
            Vector3 p1(x, -10, worldZ);
            Vector3 p2(x + 5, -10, worldZ);
            Vector3 p3(x, -10, worldZ + 10);
            
            int sx1, sy1, sx2, sy2, sx3, sy3;
            float d1, d2, d3;
            
            if (project3D(p1, sx1, sy1, d1) && project3D(p2, sx2, sy2, d2)) {
                buffer->drawLine(sx1, sy1, sx2, sy2, COLOR_VECTOR_GREEN);
            }
            if (project3D(p1, sx1, sy1, d1) && project3D(p3, sx3, sy3, d3)) {
                buffer->drawLine(sx1, sy1, sx3, sy3, COLOR_VECTOR_GREEN);
            }
        }
    }
    
    // Draw END WALL with exhaust port
    if (trenchPosition > 100) {
        // Draw solid end wall
        Vector3 wallCenter(0, 0, 220);
        
        // Draw wall grid
        for (int y = -10; y <= 10; y += 3) {
            for (int x = -20; x <= 20; x += 3) {
                Vector3 p1(x, y, 220);
                Vector3 p2(x + 3, y, 220);
                Vector3 p3(x, y + 3, 220);
                
                int sx1, sy1, sx2, sy2, sx3, sy3;
                float d1, d2, d3;
                
                if (project3D(p1, sx1, sy1, d1) && project3D(p2, sx2, sy2, d2)) {
                    buffer->drawLine(sx1, sy1, sx2, sy2, 0x39E7);
                }
                if (project3D(p1, sx1, sy1, d1) && project3D(p3, sx3, sy3, d3)) {
                    buffer->drawLine(sx1, sy1, sx3, sy3, 0x39E7);
                }
            }
        }
        
// Draw exhaust port on the wall
        Vector3 exhaustPos(0, 0, 220);
        int ex, ey;
        float ed;
        if (project3D(exhaustPos, ex, ey, ed)) {
            float pulse = 1.0f + sin(millis() / 200.0f) * 0.3f;
            int baseRadius = (int)(50.0f * (250.0f / ed));  // BIGGER - was 30
            
            // Multiple glowing rings
            buffer->drawCircle(ex, ey, (int)(baseRadius * pulse), COLOR_ENEMY_RED);
            buffer->drawCircle(ex, ey, (int)((baseRadius - 8) * pulse), COLOR_ENEMY_RED);
            buffer->drawCircle(ex, ey, (int)((baseRadius - 16) * pulse), COLOR_ENEMY_RED);
            buffer->drawCircle(ex, ey, (int)((baseRadius - 24) * pulse), COLOR_LASER);
            buffer->fillCircle(ex, ey, 5, COLOR_LASER);  // Bigger center dot
            
            // Bigger crosshairs
            buffer->drawLine(ex - 60, ey, ex + 60, ey, COLOR_ENEMY_RED);
            buffer->drawLine(ex, ey - 60, ex, ey + 60, COLOR_ENEMY_RED);
            
            buffer->setTextDatum(BC_DATUM);
            buffer->setTextColor(ed < 150 ? COLOR_ENEMY_RED : COLOR_LASER);
            buffer->drawString("TARGET: " + String((int)ed) + "m", SCREEN_CENTER_X, SCREEN_HEIGHT - 30, 2);
            
if (ed < 200 && player.hasProtonTorpedo) {
                buffer->setTextColor(COLOR_LASER);
                buffer->drawString("FIRE TORPEDO NOW! (BUTTON A)", SCREEN_CENTER_X, SCREEN_HEIGHT - 10, 2);  // Changed from B
            }
        }
    }
    
    // Draw obstacles
    for (int i = 0; i < 20; i++) {
        float obsZ = (i * 15) - trenchPosition;
        if (obsZ < -20 || obsZ > 250) continue;
        
        Vector3 obsPos;
        switch(trenchObstacles[i]) {
            case 1: obsPos = Vector3(-15, 0, obsZ); break;
            case 2: obsPos = Vector3(15, 0, obsZ); break;
            case 3: obsPos = Vector3(0, 8, obsZ); break;
            default: continue;
        }
        
        int sx, sy;
        float d;
        if (project3D(obsPos, sx, sy, d)) {
            buffer->drawRect(sx - 10, sy - 10, 20, 20, COLOR_ENEMY_RED);
        }
    }
    
    renderProjectiles(buffer);
    renderParticles(buffer);
    drawCrosshair(buffer, trenchPosition > 130);
    drawHUD(buffer);
    
if (player.hasProtonTorpedo) {
        buffer->setTextDatum(BR_DATUM);
        buffer->setTextColor(COLOR_LASER);
        buffer->drawString("TORPEDO READY (A)", SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 2);  // Say A not B
    }
}

// ============================================================================
// EXPLOSION CUTSCENE - UPDATED FOR PHASE 1
// ============================================================================

void renderExplosionCutscene(TFT_eSprite* buffer) {
    static uint32_t explosionStart = 0;
    if (explosionStart == 0) explosionStart = millis();
    
    uint32_t elapsed = millis() - explosionStart;
    
    buffer->fillSprite(TFT_BLACK);
    
    // PHASE 1: Show Death Star (0-1000ms)
    if (elapsed < 1000) {
        // Draw Death Star
        int radius = 80;
        buffer->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, radius, COLOR_VECTOR_GREEN);
        buffer->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, radius - 10, COLOR_VECTOR_GREEN);
        buffer->drawLine(SCREEN_CENTER_X - radius, SCREEN_CENTER_Y, SCREEN_CENTER_X + radius, SCREEN_CENTER_Y, COLOR_VECTOR_GREEN);
        
        // Pulsing red core
        float pulse = 1.0f + sin(millis() / 100.0f) * 0.5f;
        buffer->fillCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, (int)(20 * pulse), COLOR_ENEMY_RED);
        
        buffer->setTextDatum(BC_DATUM);
        buffer->setTextColor(COLOR_ENEMY_RED);
        buffer->drawString("CRITICAL", SCREEN_CENTER_X, SCREEN_CENTER_Y - 40, 4);
    }
    // PHASE 2: MASSIVE EXPLOSION (1000-4000ms)
    else if (elapsed < 4000) {
        uint32_t explodeTime = elapsed - 1000;
        
        // WHITE FLASH at start
        if (explodeTime < 200) {
            buffer->fillScreen(0xFFFF);
        } else {
            // Expanding rings
            for (int ring = 0; ring < 10; ring++) {
                int radius = (int)(explodeTime / 10) - ring * 30;
                if (radius > 0 && radius < 300) {
                    uint16_t colors[] = {0xFFFF, COLOR_LASER, COLOR_EXPLOSION, COLOR_ENEMY_RED, 0x4208};
                    uint16_t color = colors[ring % 5];
                    
                    buffer->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, radius, color);
                    buffer->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, radius + 1, color);
                    buffer->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, radius + 2, color);
                }
            }
            
            // Debris particles
            for (int i = 0; i < 100; i++) {
                float angle = (i * TWO_PI / 100);
                float speed = 0.5f + (i % 10) / 10.0f;
                float dist = explodeTime * speed;
                int px = SCREEN_CENTER_X + (int)(cos(angle) * dist);
                int py = SCREEN_CENTER_Y + (int)(sin(angle) * dist);
                
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    uint16_t pcolor = (i % 3 == 0) ? COLOR_LASER : COLOR_EXPLOSION;
                    buffer->fillCircle(px, py, 2 + (i % 3), pcolor);
                }
            }
            
            // Screen shake effect
            if ((millis() / 50) % 2 == 0 && explodeTime < 2000) {
                int shakeX = random(-10, 10);
                int shakeY = random(-10, 10);
                // Text shakes
            }
        }
        
        // Explosion text
        if (explodeTime > 500) {
            buffer->setTextDatum(MC_DATUM);
            buffer->setTextColor(COLOR_LASER);
            buffer->drawString("DEATH STAR", SCREEN_CENTER_X, SCREEN_CENTER_Y - 60, 4);
            buffer->drawString("DESTROYED!", SCREEN_CENTER_X, SCREEN_CENTER_Y - 20, 4);
        }
    }
    // PHASE 3: Congratulations (4000-6000ms)
    else {
        buffer->setTextDatum(MC_DATUM);
        buffer->setTextColor(COLOR_VECTOR_GREEN);
        buffer->drawString("CONGRATULATIONS", SCREEN_CENTER_X, SCREEN_CENTER_Y - 40, 4);
        
        buffer->setTextColor(COLOR_TEXT);
        buffer->drawString("MISSION ACCOMPLISHED", SCREEN_CENTER_X, SCREEN_CENTER_Y + 10, 2);
        buffer->drawString("SCORE: " + String(player.score), SCREEN_CENTER_X, SCREEN_CENTER_Y + 50, 3);
        
        // Stars twinkling
        for (int i = 0; i < 50; i++) {
            if ((millis() + i * 100) % 1000 < 500) {
                int sx = (i * 73) % SCREEN_WIDTH;
                int sy = (i * 127) % SCREEN_HEIGHT;
                buffer->drawPixel(sx, sy, 0xFFFF);
            }
        }
    }
    
    // Auto-advance - LOOP BACK TO FPS
    if (elapsed > 6000) {
        explosionStart = 0;
        
        // Keep score and wave, loop back to Phase 1
        currentWave++;
        
        if (player.shields < MAX_SHIELDS) {
            player.shields = MAX_SHIELDS;  // Full restore
        }
        
        Serial.printf("[DeathStar] ENDLESS MODE - Starting Wave %d\n", currentWave);
        
        initPhase1();  // GO BACK TO FPS LEVEL
    }
}
// ============================================================================
// CUTSCENES
// ============================================================================

void showCutscene(TFT_eSprite* buffer, int sceneNum) {
    buffer->fillSprite(TFT_BLACK);
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextColor(COLOR_VECTOR_CYAN);
    
    switch(sceneNum) {
        case 1: // To space
            buffer->drawString("LAUNCHING TO", SCREEN_CENTER_X, SCREEN_CENTER_Y - 20, 4);
            buffer->drawString("OUTER SPACE", SCREEN_CENTER_X, SCREEN_CENTER_Y + 20, 4);
            break;
            
        case 2: // Surface approach
            buffer->drawString("APPROACHING", SCREEN_CENTER_X, SCREEN_CENTER_Y - 20, 4);
            buffer->drawString("DEATH STAR SURFACE", SCREEN_CENTER_X, SCREEN_CENTER_Y + 20, 4);
            break;
            
        case 3: // Trench run
            buffer->drawString("ENTERING", SCREEN_CENTER_X, SCREEN_CENTER_Y - 20, 4);
            buffer->drawString("EQUATORIAL TRENCH", SCREEN_CENTER_X, SCREEN_CENTER_Y + 20, 4);
            break;
    }
    
    buffer->setTextColor(COLOR_VECTOR_GREEN);
    buffer->drawString("+1 SHIELD", SCREEN_CENTER_X, SCREEN_CENTER_Y + 60, 2);
    drawShields(buffer);
}

void updateCutscene(int sceneNum) {
    static uint32_t cutsceneStart = 0;
    
    if (cutsceneStart == 0) {
        cutsceneStart = millis();
    }
    
    if (millis() - cutsceneStart > 2000) {
        cutsceneStart = 0;
        
        switch(sceneNum) {
            case 1: initPhase2(); break;
            case 2: initPhase3(); break;
            case 3: initPhase4(); break;
        }
    }
}

// ============================================================================
// SPLASH SCREEN - TIE FIGHTER CENTERED
// ============================================================================

void showSplash(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    // Title - Green and at top
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextColor(COLOR_VECTOR_GREEN);
    buffer->drawString("DEATH STAR", SCREEN_CENTER_X, 40, 4);
    buffer->drawString("RUN", SCREEN_CENTER_X, 75, 4);
    
    // Rotating TIE fighter - CENTERED VERTICALLY
    static float rotation = 0;
    rotation += 0.03f;
    
    Vector3 tiePos(0, 0, 30);  // Changed Y from -5 to 0 for vertical center
    
    cameraPos = Vector3(0, 0, 0);
    cameraYaw = 0;
    cameraPitch = 0;
    
    drawWireframeModel(buffer, &tieFighterModel, tiePos, 0, rotation, 0, COLOR_ENEMY_RED);
    
    // Instructions at bottom
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextColor(COLOR_TEXT);
    buffer->drawString("Joystick: Aim/Move", SCREEN_CENTER_X, 220, 2);
    buffer->drawString("Button A: Fire Lasers", SCREEN_CENTER_X, 250, 2);
    buffer->drawString("Button B: Proton Torpedo", SCREEN_CENTER_X, 280, 2);
    
    // Flashing prompt
    if ((millis() / 500) % 2 == 0) {
        buffer->setTextColor(COLOR_LASER);
        buffer->drawString("PRESS BUTTON A", SCREEN_CENTER_X, SCREEN_HEIGHT - 20, 2);
    }
}

void showBriefing(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextColor(COLOR_LASER);
    buffer->drawString("MISSION BRIEFING", SCREEN_CENTER_X, 30, 4);
    
    buffer->setTextColor(COLOR_TEXT);
    buffer->setTextDatum(TL_DATUM);
    
    int y = 80;
    int spacing = 25;
    
    buffer->drawString("PHASE 1: Infiltrate moon base", 20, y, 2);
    y += spacing;
    buffer->drawString("PHASE 2: Defend against TIE fighters", 20, y, 2);
    y += spacing;
    buffer->drawString("PHASE 3: Destroy surface turrets", 20, y, 2);
    y += spacing;
    buffer->drawString("PHASE 4: Navigate trench and hit", 20, y, 2);
    y += spacing;
    buffer->drawString("         exhaust port with torpedo", 20, y, 2);
    y += spacing + 10;
    
    buffer->setTextColor(COLOR_ENEMY_RED);
    buffer->drawString("You have 6 shields. Don't lose them all!", 20, y, 2);
    
    buffer->setTextDatum(MC_DATUM);
    if ((millis() / 500) % 2 == 0) {
        buffer->setTextColor(COLOR_VECTOR_CYAN);
        buffer->drawString("PRESS BUTTON A TO START", SCREEN_CENTER_X, SCREEN_HEIGHT - 20, 2);
    }
}

void showGameOver(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextColor(COLOR_ENEMY_RED);
    buffer->drawString("MISSION FAILED", SCREEN_CENTER_X, 80, 4);
    
    buffer->setTextColor(COLOR_TEXT);
    buffer->drawString("FINAL SCORE", SCREEN_CENTER_X, 140, 2);
    buffer->drawString(String(player.score), SCREEN_CENTER_X, 170, 4);
    buffer->drawString("WAVE: " + String(currentWave), SCREEN_CENTER_X, 210, 2);
    
    if ((millis() / 500) % 2 == 0) {
        buffer->setTextColor(COLOR_VECTOR_CYAN);
        buffer->drawString("PRESS BUTTON A", SCREEN_CENTER_X, SCREEN_HEIGHT - 20, 2);
    }
}

void showVictory(TFT_eSprite* buffer) {
    buffer->fillSprite(TFT_BLACK);
    
    buffer->setTextDatum(MC_DATUM);
    buffer->setTextColor(COLOR_LASER);
    buffer->drawString("DEATH STAR", SCREEN_CENTER_X, 60, 4);
    buffer->drawString("DESTROYED!", SCREEN_CENTER_X, 100, 4);
    
    buffer->setTextColor(COLOR_TEXT);
    buffer->drawString("SCORE: " + String(player.score), SCREEN_CENTER_X, 150, 2);
    
    buffer->setTextColor(COLOR_VECTOR_CYAN);
    buffer->drawString("Starting Wave " + String(currentWave + 1), SCREEN_CENTER_X, 190, 2);
    
    buffer->setTextColor(COLOR_VECTOR_GREEN);
    buffer->drawString("SHIELDS RECHARGED!", SCREEN_CENTER_X, 220, 2);
    drawShields(buffer);
    
    static uint32_t victoryTime = 0;
    if (victoryTime == 0) victoryTime = millis();
    
    if (millis() - victoryTime > 3000) {
        victoryTime = 0;
        currentWave++;
        
        player.shields = MAX_SHIELDS;
        Serial.printf("[DeathStar] Wave %d - Full shields restored!\n", currentWave);
        
        initPhase1();
    }
}

// ============================================================================
// GAME INITIALIZATION
// ============================================================================

void initGame() {
    Serial.println("[DeathStar] Initializing game...");
    
    cameraPos = Vector3(0, 0, 0);
    cameraYaw = 0;
    cameraPitch = 0;
    cameraRoll = 0;
    
    player.pos = Vector3(0, 0, 0);
    player.vel = Vector3(0, 0, 0);
    player.yaw = 0;
    player.pitch = 0;
    player.shields = MAX_SHIELDS;
    player.score = 0;
    player.hasProtonTorpedo = false;
    player.lastFireTime = 0;
    player.invincible = false;
    player.walking = false;
    
    currentWave = 1;
    
    for (int i = 0; i < MAX_TIE_FIGHTERS; i++) tieFighters[i].alive = false;
    for (int i = 0; i < MAX_TURRETS; i++) turrets[i].alive = false;
    for (int i = 0; i < MAX_STORMTROOPERS; i++) stormtroopers[i].alive = false;
    for (int i = 0; i < DSR_MAX_PROJECTILES; i++) projectiles[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
    
    initAllModels();
    
    gameState = STATE_SPLASH;
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

void playGame(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.println("[DeathStar] Starting Death Star Run...");
    
    initGame();
    
    TFT_eSprite* buffer = new TFT_eSprite(&tft);
    buffer->setColorDepth(16);
    buffer->setAttribute(PSRAM_ENABLE, true);
    
    bool bufferCreated = buffer->createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    if (!bufferCreated) {
        Serial.println("[DeathStar] ERROR: Buffer creation failed!");
        delete buffer;
        return;
    }
    
    Serial.println("[DeathStar] Buffer created successfully");
    
    while (true) {
        unsigned long frameStart = millis();
        
        switch (gameState) {
            case STATE_SPLASH:
                showSplash(buffer);
                {
                    static bool buttonWasPressed = false;
                    bool buttonPressed = !ss.digitalRead(6);
                    if (buttonPressed && !buttonWasPressed) {
                        gameState = STATE_BRIEFING;
                        delay(200);
                    }
                    buttonWasPressed = buttonPressed;
                }
                break;
                
            case STATE_BRIEFING:
                showBriefing(buffer);
                {
                    static bool buttonWasPressed = false;
                    bool buttonPressed = !ss.digitalRead(6);
                    if (buttonPressed && !buttonWasPressed) {
                        initPhase1();
                        delay(200);
                    }
                    buttonWasPressed = buttonPressed;
                }
                break;
                
            case STATE_PHASE1_FPS:
                updatePhase1(ss);
                renderPhase1(buffer);
                break;
                
            case STATE_CUTSCENE_SPACE:
                showCutscene(buffer, 1);
                updateCutscene(1);
                break;
                
            case STATE_PHASE2_SPACE:
                updatePhase2(ss);
                renderPhase2(buffer);
                break;
                
            case STATE_CUTSCENE_SURFACE:
                showCutscene(buffer, 2);
                updateCutscene(2);
                break;
                
            case STATE_PHASE3_SURFACE:
                updatePhase3(ss);
                renderPhase3(buffer);
                break;
                
            case STATE_CUTSCENE_TRENCH:
                showCutscene(buffer, 3);
                updateCutscene(3);
                break;
                
            case STATE_PHASE4_TRENCH:
                updatePhase4(ss);
                renderPhase4(buffer);
                break;
                
            case STATE_EXPLOSION_CUTSCENE:
                renderExplosionCutscene(buffer);
                break;
                
            case STATE_VICTORY:
                showVictory(buffer);
                break;
                
            case STATE_GAME_OVER:
                showGameOver(buffer);
                {
                    static bool buttonWasPressed = false;
                    bool buttonPressed = !ss.digitalRead(6);
                    if (buttonPressed && !buttonWasPressed) {
                        buttonWasPressed = false;
                        initGame();
                        delay(200);
                    }
                    buttonWasPressed = buttonPressed;
                }
                break;
        }
        
        buffer->pushSprite(0, 0);
        
        if (!ss.digitalRead(14)) {
            Serial.println("[DeathStar] SELECT pressed, exiting");
            break;
        }
        
        unsigned long frameTime = millis() - frameStart;
        if (frameTime < 33) delay(33 - frameTime);
    }
    
    buffer->deleteSprite();
    delete buffer;
    tft.fillScreen(TFT_BLACK);
}

} // namespace DeathStarRun

#endif // DEATH_STAR_RUN_H