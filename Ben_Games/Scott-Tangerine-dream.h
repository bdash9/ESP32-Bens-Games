#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <Wire.h>
#include <math.h>

#define TFT_WIDTH 480
#define TFT_HEIGHT 320

#define BUTTON_A      6
#define BUTTON_B      7
#define BUTTON_X      9
#define BUTTON_Y      10
#define BUTTON_SELECT 14
#define JOY_X         3
#define JOY_Y         2

#define GAME_W        480
#define GAME_H        320
#define NUM_RAYS      120
#define RAY_COL_W     4
#define FOV_DEG       66
#define TWO_PI        6.2831853f
#define MOVE_STEP     0.18f
#define TURN_STEP     0.11f

// Tangerine Dream album names (chronological classics)
const char* cdNames[] = {
  "Phaedra", "Rubycon", "Stratosfear", "Cyclone",
  "Force Majeure", "Tangram", "Exit", "White Eagle",
  "Hyperborea", "Underwater Sunlight", "Tyger",
  "Poland", "Green Desert", "Encore", "Le Parc", "Risky Business"
};
#define NUM_CDS (sizeof(cdNames)/sizeof(cdNames[0]))

// Game progression maps (simple 7x7 grids)
const char* simpleMaps[NUM_CDS] = {
  // Phaedra (easy start)
  "#######"
  "#.....#"
  "#.#S#.#"
  "#..C..#"
  "#.#.#.#"
  "#G..E.#"
  "#######",
  // Rubycon
  "#######"
  "#....E#"
  "#.#S#.#"
  "#C.G.E#"
  "#.###.#"
  "#E...S#"
  "#######",
  // Stratosfear
  "#######"
  "#S....#"
  "#E#C#E#"
  "#..G..#"
  "#.###.#"
  "#E...S#"
  "#######",
  // Cyclone (more enemies)
  "#######"
  "#E.S.E#"
  "#.#C#.#"
  "#..G..#"
  "#E###E#"
  "#...S.#"
  "#######",
  // Force Majeure
  "#######"
  "#S#E#S#"
  "#.....#"
  "#EC.GE#"
  "#.....#"
  "#E#S#E#"
  "#######",
  // Tangram
  "#######"
  "#E.S.E#"
  "#.###.#"
  "#SC.GE#"
  "#.###.#"
  "#E.S.E#"
  "#######",
  // Exit
  "#######"
  "#ESE#.#"
  "#.#.#S#"
  "#...CG#"
  "#S#.#.#"
  "#.#ESE#"
  "#######",
  // White Eagle
  "#######"
  "#SE.ES#"
  "#.#C#.#"
  "#E.G.E#"
  "#.###.#"
  "#SE.ES#"
  "#######",
  // Hyperborea
  "#######"
  "#ESESE#"
  "#.....#"
  "#SCGES#"
  "#.....#"
  "#ESESE#"
  "#######",
  // Underwater Sunlight
  "#######"
  "#SE#SE#"
  "#..C..#"
  "#E#G#E#"
  "#..S..#"
  "#ES#ES#"
  "#######",
  // Tyger
  "#######"
  "#ESESE#"
  "#S#C#S#"
  "#E.G.E#"
  "#S#.#S#"
  "#ESESE#"
  "#######",
  // Poland
  "#######"
  "#SE#SE#"
  "#.SCS.#"
  "#E#G#E#"
  "#.SES.#"
  "#ES#ES#"
  "#######",
  // Green Desert
  "#######"
  "#ESESE#"
  "#SCECS#"
  "#E#G#E#"
  "#SECES#"
  "#ESESE#"
  "#######",
  // Encore
  "#######"
  "#SESES#"
  "#E#C#E#"
  "#S.G.S#"
  "#E#.#E#"
  "#SESES#"
  "#######",
  // Le Parc
  "#######"
  "#ESESE#"
  "#SCGCS#"
  "#E###E#"
  "#SESES#"
  "#EEEEE#"
  "#######",
  // Risky Business (final boss level)
  "#######"
  "#ESESE#"
  "#SCECS#"
  "#EEGEE#"
  "#SECES#"
  "#ESESE#"
  "#######"
};

// Psychedelic wall colors inspired by album art
const uint16_t wallColorSets[][4] = {
  {0x001F, 0x780F, 0xFD20, 0x4208},  // Phaedra blues/purples
  {0xF800, 0xFD20, 0xFFE0, 0xF81F},  // Rubycon reds/magentas
  {0x07E0, 0x07FF, 0x001F, 0x780F},  // Stratosfear greens/cyans
  {0xFFE0, 0xFD20, 0xF800, 0xFBE0},  // Cyclone yellows/oranges
  {0xF81F, 0x781F, 0x001F, 0x4810},  // Force Majeure purples
  {0x07FF, 0x07E0, 0xFFE0, 0xF800}   // Tangram spectrum
};
#define NUM_WALL_SETS (sizeof(wallColorSets)/sizeof(wallColorSets[0]))

// Structures
struct TangerineSprite { 
  float x, y; 
  int alive;
  int type; // 0=tangerine enemy, 1=musical note enemy
};

struct TangerinePlayer { 
  float x, y, angle;
  int ammo;
  int hasRapidFire;
  unsigned long rapidFireUntil;
};

struct Star {
  float x, y, z;
  uint16_t color;
};

#define MAX_ENEMIES 32
#define MAX_STARS 100
#define MAX_ENEMY_BULLETS 16  

struct EnemyBullet {
  float x, y;
  float dx, dy;  // Direction
  int active;
  unsigned long spawnTime;
};

struct GameState {
  int score;
  int enemiesKilled;
  int cdCount;
  unsigned long levelStartTime;
  Star stars[MAX_STARS];
  EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];  // ADD THIS
};

// Function declarations
inline bool isWall(int mx, int my, char* world);
void drawTangerine(TFT_eSprite *sprite, int x, int y, int r);
void drawMusicalNote(TFT_eSprite *sprite, int x, int y, int sz);
void drawSynthesizer(TFT_eSprite *sprite, float depth, int sx);
void drawCD(TFT_eSprite *sprite, float depth, int sx, const char* cdLabel);
void drawEnemy(TFT_eSprite *sprite, float depth, int sx, int type);
void drawShotRay(TFT_eSprite *sprite, float len);
void drawPsychedelicBackground(TFT_eSprite *sprite, GameState &gs, int album);
void splashScreen(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss);
void loadLevel(int which, TangerinePlayer &player, char* world, TangerineSprite* enemies, int &num_enemies, int &cd_collected, int &player_lives, GameState &gs);
void raycastDrawDirect(TFT_eSprite *sprite, TangerinePlayer &player, int cur_album, char* world, TangerineSprite* enemies, int num_enemies, int player_lives, int shotShow, float shotLen, GameState &gs);
void fireAtEnemies(TFT_eSprite *sprite, TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, int *outShot, float *outLen, char* world, GameState &gs);
void checkCDPickup(TFT_eSPI &tft, TFT_eSprite *sprite, TangerinePlayer &player, char* world, int &cd_collected, int cur_album, GameState &gs);
void checkEnemyTouch(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss, TangerinePlayer &player, TangerineSprite* enemies, int &num_enemies, int &player_lives, int &cur_album, char* world, int &cd_collected, bool &restart, bool &exitMenu, GameState &gs);
void checkSynthesizerPickup(TangerinePlayer &player, char* world);
void showScoreScreen(TFT_eSprite *sprite, Adafruit_seesaw &ss, GameState &gs, int cur_album);
void updateEnemyShooting(TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, GameState &gs, int cur_album, char* world, int &player_lives);
void updateEnemyMovement(TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, char* world);

void run_ScottsTangerineDream(TFT_eSPI &tft, Adafruit_seesaw &ss) {
  // Create PSRAM sprite buffer
  TFT_eSprite *sprite = new TFT_eSprite(&tft);
  sprite->setColorDepth(16);
  sprite->setAttribute(PSRAM_ENABLE, true);
  
  if (!sprite->createSprite(GAME_W, GAME_H)) {
    Serial.println("Failed to create sprite!");
    delete sprite;
    return;
  }
  
  TangerinePlayer player;
  char world[7*7];
  TangerineSprite enemies[MAX_ENEMIES];
  int num_enemies, cd_collected, player_lives, cur_album;
  GameState gs;

  // Initialize stars for psychedelic effect
  for(int i = 0; i < MAX_STARS; i++) {
    gs.stars[i].x = random(0, GAME_W);
    gs.stars[i].y = random(0, GAME_H/2);
    gs.stars[i].z = random(1, 100) / 100.0f;
    uint16_t colors[] = {TFT_WHITE, TFT_CYAN, TFT_MAGENTA, TFT_YELLOW};
    gs.stars[i].color = colors[random(4)];
  }

  int joyX=512, joyY=512;
  bool btnA, btnB, btnX, btnY, btnSelect;
  bool prevA = false;

  auto updateInputs = [&]() {
    joyX      = ss.analogRead(JOY_X);
    joyY      = ss.analogRead(JOY_Y);
    btnA      = !ss.digitalRead(BUTTON_A);
    btnB      = !ss.digitalRead(BUTTON_B);
    btnX      = !ss.digitalRead(BUTTON_X);
    btnY      = !ss.digitalRead(BUTTON_Y);
    btnSelect = !ss.digitalRead(BUTTON_SELECT);
  };
  
  auto waitAllRelease = [&]() {
    updateInputs();
    while(btnA||btnB||btnX||btnY||btnSelect) { updateInputs(); delay(10);}
    prevA = false;
  };

  splashScreen(tft, sprite, ss);
  
  cur_album = 0;
  gs.score = 0;
  gs.enemiesKilled = 0;
  gs.cdCount = 0;
  loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives, gs);

  unsigned long lastDraw=0;
  int frameShot=0;
  float lastShotLen=0;

  bool exitMenu = false;
  
while(!exitMenu) {
    Serial.println("[TD] Frame start"); // ADD THIS DEBUG LINE
    
    if(millis()-lastDraw<28) { delay(3); continue; }
    lastDraw=millis();
    updateInputs();

    if(btnSelect) break;

    // Check rapid fire expiration
    if(player.hasRapidFire && millis() > player.rapidFireUntil) {
      player.hasRapidFire = 0;
    }

    float ahead = 0, turn = 0;
    if(joyY < 400) ahead = MOVE_STEP;
    else if(joyY > 624) ahead=-MOVE_STEP;
    if(joyX < 400) turn = -TURN_STEP;
    else if(joyX > 624) turn = +TURN_STEP;

    player.angle += turn;
    if(player.angle<0) player.angle += TWO_PI;
    if(player.angle>TWO_PI) player.angle -= TWO_PI;

    if(ahead!=0) {
      float nx = player.x + cosf(player.angle)*ahead;
      float ny = player.y + sinf(player.angle)*ahead;
      if(!isWall(int(nx),int(player.y),world)) player.x=nx;
      if(!isWall(int(player.x),int(ny),world)) player.y=ny;
    }

    // Check for synthesizer power-up pickup
    checkSynthesizerPickup(player, world);

        // Update enemy movement
    updateEnemyMovement(player, enemies, num_enemies, world);

    updateEnemyShooting(player, enemies, num_enemies, gs, cur_album, world, player_lives);

    int shotShow = 0;
    float shotLen = 0;
    
    // Rapid fire or normal fire
    if(btnA && (!prevA || player.hasRapidFire)) {
      if(player.ammo > 0 || player.hasRapidFire) {
        fireAtEnemies(sprite, player, enemies, num_enemies, &shotShow, &shotLen, world, gs);
        frameShot = 2;
        lastShotLen = shotLen;
        if(!player.hasRapidFire) player.ammo--;
      }
    }
    prevA = btnA;

    checkCDPickup(tft, sprite, player, world, cd_collected, cur_album, gs);
    
    bool restart = false;
    checkEnemyTouch(tft, sprite, ss, player, enemies, num_enemies, player_lives, cur_album, world, cd_collected, restart, exitMenu, gs);
    
    if(exitMenu) break;
    
    if(restart) {
      cur_album = 0;
      gs.score = 0;
      gs.enemiesKilled = 0;
      gs.cdCount = 0;
      loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives, gs);
      splashScreen(tft, sprite, ss);
    }

    if (frameShot > 0) {
      raycastDrawDirect(sprite, player, cur_album, world, enemies, num_enemies, player_lives, 1, lastShotLen, gs);
      frameShot--;
    }
    else {
      raycastDrawDirect(sprite, player, cur_album, world, enemies, num_enemies, player_lives, 0, 0, gs);
    }
    
    sprite->pushSprite(0, 0);

    if(cd_collected) {
      delay(350);
      gs.cdCount++;
      cur_album++;
      
      if(cur_album>=NUM_CDS) {
        showScoreScreen(sprite, ss, gs, cur_album);
        splashScreen(tft, sprite, ss);
        cur_album=0;
        gs.score = 0;
        gs.enemiesKilled = 0;
        gs.cdCount = 0;
        loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives, gs);
      }
      else {
        loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives, gs);
      }
    }
  }
  
  sprite->deleteSprite();
  delete sprite;
  tft.fillScreen(TFT_BLACK);
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

inline bool isWall(int mx, int my, char* world) {
  if(mx<0 || mx>=7 || my<0 || my>=7) return true;
  return world[my*7+mx]=='#';
}

void drawTangerine(TFT_eSprite *sprite, int x, int y, int r) {
  sprite->fillCircle(x, y, r, TFT_ORANGE);
  sprite->drawCircle(x, y, r, TFT_DARKCYAN);
  // Leaf on top
  sprite->fillTriangle(x-r/3, y-r+2, x, y-r-r/2, x+r/3, y-r+2, TFT_GREEN);
  // Segments
  for(int i = 0; i < 6; i++) {
    float angle = i * PI / 3;
    int x1 = x + cos(angle) * r * 0.7;
    int y1 = y + sin(angle) * r * 0.7;
    sprite->drawLine(x, y, x1, y1, TFT_DARKCYAN);
  }
}

void drawMusicalNote(TFT_eSprite *sprite, int x, int y, int sz) {
  // Note head
  sprite->fillEllipse(x, y + sz/2, sz/3, sz/4, TFT_CYAN);
  // Note stem
  sprite->fillRect(x + sz/3 - 2, y, 3, sz/2, TFT_CYAN);
  // Note flag
  sprite->fillTriangle(x + sz/3, y, x + sz/3 + sz/3, y + sz/4, x + sz/3, y + sz/3, TFT_MAGENTA);
}

void drawSynthesizer(TFT_eSprite *sprite, float depth, int sx) {
  int sz = GAME_H/(depth*2.5f);
  if(sz<12 || sx<0 || sx>=GAME_W) return;
  int y0 = GAME_H/2-sz/2;
  
  // Synth body
  sprite->fillRect(sx-sz/2, y0, sz, sz/2, TFT_DARKGREY);
  sprite->drawRect(sx-sz/2, y0, sz, sz/2, TFT_WHITE);
  
  // Keys
  for(int i = 0; i < 5; i++) {
    int kx = sx - sz/2 + 5 + i * (sz/6);
    sprite->fillRect(kx, y0 + sz/3, sz/8, sz/6, TFT_WHITE);
  }
  
  // Knobs
  for(int i = 0; i < 4; i++) {
    int kx = sx - sz/2 + 8 + i * (sz/5);
    sprite->fillCircle(kx, y0 + sz/8, 3, TFT_CYAN);
  }
  
  // Label
  sprite->setTextColor(TFT_GREEN, TFT_DARKGREY);
  sprite->setTextDatum(MC_DATUM);
  if(sz > 20) sprite->drawString("SYN", sx, y0 + sz/2 - 5, 1);
}

void drawCD(TFT_eSprite *sprite, float depth, int sx, const char* cdLabel) {
  int cdh = GAME_H / (depth*2.5f);
  if(cdh<9 || sx<0 || sx>=GAME_W) return;
  int y0  = GAME_H/2 - cdh/2;

  uint16_t rainbow[6] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_MAGENTA};
  for(int a=0; a<6; a++) {
    float th1 = (TWO_PI * a) / 6.0f;
    float th2 = (TWO_PI * (a+1)) / 6.0f;
    for(float th=th1; th<th2; th+=0.059) {
      int x1 = sx + cos(th) * (cdh/2-1);
      int y1 = y0+cdh/2 + sin(th) * (cdh/2-1);
      sprite->drawLine(sx, y0+cdh/2, x1, y1, rainbow[a]);
    }
  }
  sprite->fillEllipse(sx-cdh/7, y0+cdh/2-cdh/6, cdh/10, cdh/13, TFT_WHITE);
  sprite->fillEllipse(sx, y0+cdh/2, cdh/4, cdh/4, TFT_BLACK);

  sprite->setTextColor(TFT_WHITE, TFT_BLACK);
  sprite->setTextDatum(MC_DATUM);
  int fontSize = cdh > 60 ? 4 : (cdh > 36 ? 2 : 1);
  sprite->drawString(cdLabel, sx, y0+cdh/2, fontSize);
}

void drawEnemy(TFT_eSprite *sprite, float depth, int sx, int type) {
  int sz = GAME_H/(depth*2.1f);
  if(sz<10 || sx<0 || sx>=GAME_W) return;
  int y0 = GAME_H/2-sz/2;
  
  if(type == 0) {
    drawTangerine(sprite, sx, y0+sz/2, sz/2-2);
  } else {
    drawMusicalNote(sprite, sx, y0, sz-4);
  }
}

void drawShotRay(TFT_eSprite *sprite, float len) {
  int x0 = GAME_W / 2;
  int y0 = GAME_H - 1;
  int x1 = GAME_W / 2;
  int y1 = GAME_H / 2;
  
  // Laser beam effect
  for (int i = -4; i <= 4; i++) {
    uint16_t color = (abs(i) < 2) ? TFT_WHITE : TFT_RED;
    sprite->drawLine(x0 + i, y0, x1 + i, y1, color);
  }
}

void drawPsychedelicBackground(TFT_eSprite *sprite, GameState &gs, int album) {
  // Animate stars with bounds checking
  for(int i = 0; i < MAX_STARS; i++) {
    if(i < 0 || i >= MAX_STARS) break;
    
    gs.stars[i].z += 0.002f;
    if(gs.stars[i].z > 1.0f || gs.stars[i].z < 0.0f) {
      gs.stars[i].z = 0.01f;
      gs.stars[i].x = random(0, GAME_W);
      gs.stars[i].y = random(0, GAME_H/2);
    }
    
    int sx = (int)gs.stars[i].x;
    int sy = (int)gs.stars[i].y;
    
    // Bounds check before drawing
    if(sx >= 0 && sx < GAME_W && sy >= 0 && sy < GAME_H) {
      int brightness = (int)(gs.stars[i].z * 255);
      if(brightness > 200 && brightness < 300) {
        sprite->drawPixel(sx, sy, gs.stars[i].color);
      }
    }
  }
}

void splashScreen(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss) {
  sprite->fillSprite(TFT_BLACK);
  sprite->setTextDatum(MC_DATUM);

  // Draw tangerines - MOVED UP
  drawTangerine(sprite, 70, GAME_H - 100, 45);
  drawTangerine(sprite, GAME_W - 70, GAME_H - 100, 45);

  // Title
  sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
  sprite->drawString("SCOTT'S", GAME_W/2, 50, 4);

  // Tangerine Dream in rainbow
  const char *txt = "TANGERINE DREAM";
  int L = strlen(txt);
  int charWidth = 20;
  int px = GAME_W/2 - (L*charWidth)/2;
  uint16_t tCols[6] = {TFT_ORANGE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA, TFT_RED, TFT_GREEN};
  for (int i = 0; i < L; i++) {
    char one[2]{txt[i], 0};
    sprite->setTextColor(tCols[i%6], TFT_BLACK);
    sprite->drawString(one, px + i*charWidth, 100, 4);
  }

  sprite->setTextColor(TFT_CYAN, TFT_BLACK);
  sprite->drawString("ELECTRONIC DREAMS FPS", GAME_W/2, 150, 4);
  
  sprite->setTextColor(TFT_WHITE, TFT_BLACK);
  sprite->drawString("Based on the legendary band", GAME_W/2, 190, 2);
  sprite->drawString("Pioneers of electronic music", GAME_W/2, 210, 2);
  
  sprite->pushSprite(0, 0);

  // Wait for button with blinking text at BOTTOM
  bool pressed = false;
  unsigned long lastBlink = 0;
  bool showText = true;
  
  do {
    if(millis() - lastBlink > 500) {
      lastBlink = millis();
      showText = !showText;
      
      sprite->fillRect(0, GAME_H - 30, GAME_W, 30, TFT_BLACK);
      
      if(showText) {
        sprite->setTextColor(TFT_MAGENTA, TFT_BLACK);
        sprite->drawString("Press any button", GAME_W/2, GAME_H - 15, 2);
      }
      
      sprite->pushSprite(0, 0);
    }
    
    pressed = !ss.digitalRead(BUTTON_A) || !ss.digitalRead(BUTTON_B) || 
              !ss.digitalRead(BUTTON_X) || !ss.digitalRead(BUTTON_Y);
    delay(10);
  } while(!pressed);
  
  do {
    pressed = !ss.digitalRead(BUTTON_A) || !ss.digitalRead(BUTTON_B) || 
              !ss.digitalRead(BUTTON_X) || !ss.digitalRead(BUTTON_Y);
    delay(10);
  } while(pressed);
  
  sprite->fillSprite(TFT_BLACK);
  sprite->pushSprite(0, 0);
  delay(200);
}

void loadLevel(int which, TangerinePlayer &player, char* world, TangerineSprite* enemies, int &num_enemies, int &cd_collected, int &player_lives, GameState &gs) {
  memcpy(world, simpleMaps[which], 7*7);
  
  // Find player start
  for(int y=0;y<7;y++) for(int x=0;x<7;x++) {
    char c = world[y*7+x];
    if(c=='G') { 
      player.x = x+0.5f; 
      player.y = y+0.5f; 
      player.angle = 0.0f;
      player.ammo = 20 + (which * 5);
      player.hasRapidFire = 0;
      world[y*7+x]='.'; 
    }
  }
  
  // Load enemies
  num_enemies=0;
  for(int y=0;y<7;y++) for(int x=0;x<7;x++) {
    char c = world[y*7+x];
    if(c=='E' && num_enemies<MAX_ENEMIES) {
      enemies[num_enemies].x = x+0.5f;
      enemies[num_enemies].y = y+0.5f;
      enemies[num_enemies].alive = 1;
      enemies[num_enemies].type = (which > 5) ? random(2) : 0;
      world[y*7+x]='.';
      num_enemies++;
    }
  }
  
  cd_collected=0;
  if(which == 0) player_lives=3;
  gs.levelStartTime = millis();

  // ZERO INITIALIZE ALL BULLETS - CRITICAL!
  for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    gs.enemyBullets[i].x = 0.0f;
    gs.enemyBullets[i].y = 0.0f;
    gs.enemyBullets[i].dx = 0.0f;
    gs.enemyBullets[i].dy = 0.0f;
    gs.enemyBullets[i].active = 0;
    gs.enemyBullets[i].spawnTime = 0;
  }
}

void raycastDrawDirect(TFT_eSprite *sprite, TangerinePlayer &player, int cur_album, char* world, TangerineSprite* enemies, int num_enemies, int player_lives, int shotShow, float shotLen, GameState &gs) {
  Serial.printf("[TD] Raycast: album=%d enemies=%d\n", cur_album, num_enemies); // ADD THIS
  
  sprite->fillSprite(TFT_BLACK);
  
  // Draw psychedelic stars
  drawPsychedelicBackground(sprite, gs, cur_album);
  
  const uint16_t* wallCols = wallColorSets[cur_album%NUM_WALL_SETS];

  // Raycasting
  float px = player.x, py = player.y, pa = player.angle;
  for(int rx=0;rx<NUM_RAYS;rx++) {
    float angle = pa-(FOV_DEG*0.5f*3.14159f/180.0f) + (FOV_DEG*3.14159f/180.0f) * rx/float(NUM_RAYS);
    float dx = cosf(angle), dy = sinf(angle);
    float dist=0.1f;
    int wallType=0;
    for(;dist<12.0f;dist+=0.045f) {
      int sx = int(px+dx*dist), sy = int(py+dy*dist);
      if(isWall(sx,sy,world)) { wallType=(sx+sy)&3; break; }
    }
    int col = wallCols[wallType%4];
    int wall_x = (rx * RAY_COL_W);
    int wall_col_w = RAY_COL_W;
    int h = int(GAME_H/(dist*1.2f));
    h = constrain(h,8,GAME_H);
    int top = GAME_H/2 - h/2;
    sprite->fillRect(wall_x, top, wall_col_w, h, col);
    sprite->fillRect(wall_x, top+h, wall_col_w, GAME_H/2-h/2, TFT_DARKGREY);
    sprite->fillRect(wall_x, 0, wall_col_w, top, TFT_BLACK);
  }
  
  if (shotShow) drawShotRay(sprite, shotLen);
  
  // Draw enemies
  for(int e=0;e<num_enemies;e++) if(enemies[e].alive) {
    float dx=enemies[e].x-px, dy=enemies[e].y-py;
    float dist = sqrtf(dx*dx+dy*dy);
    float angle2 = atan2f(dy,dx)-pa;
    if(angle2<-3.14159f) angle2+=TWO_PI;
    if(angle2>3.14159f) angle2-=TWO_PI;
    float fov2 = FOV_DEG*3.14159f/180.0f;
    if(fabsf(angle2)<fov2/2 && dist>0.45f) {
      int sx = GAME_W/2 + (angle2/(fov2/2))*GAME_W/2;
      drawEnemy(sprite, dist, sx, enemies[e].type);
    }
  }
  
  // Draw CDs
  for(int y=0;y<7;y++) {
    for(int x=0;x<7;x++) {
      if(world[y*7+x]=='C') {
        float dx = x+0.5f-px, dy=y+0.5f-py;
        float dist = sqrtf(dx*dx+dy*dy);
        float angle2 = atan2f(dy,dx)-pa;
        if(angle2 < -3.14159f) angle2 += TWO_PI;
        if(angle2 >  3.14159f) angle2 -= TWO_PI;
        float fov2 = FOV_DEG*3.14159f/180.0f;
        if(fabsf(angle2) < fov2/2 && dist>0.34f) {
          int sx = GAME_W/2 + (angle2/(fov2/2))*GAME_W/2;
          drawCD(sprite, dist, sx, cdNames[cur_album]);
        }
      }
    }
  }

  // Draw enemy bullets - SIMPLIFIED SAFE VERSION
  for(int b = 0; b < MAX_ENEMY_BULLETS; b++) {
    if(b < 0 || b >= MAX_ENEMY_BULLETS) break;
    if(gs.enemyBullets[b].active != 1) continue;
    
    float bx = gs.enemyBullets[b].x;
    float by = gs.enemyBullets[b].y;
    
    if(bx < -5.0f || bx > 15.0f || by < -5.0f || by > 15.0f) {
      gs.enemyBullets[b].active = 0;
      continue;
    }
    
    if(!isfinite(bx) || !isfinite(by)) {
      gs.enemyBullets[b].active = 0;
      continue;
    }
    
    float dx = bx - px;
    float dy = by - py;
    
    if(!isfinite(dx) || !isfinite(dy)) continue;
    if(fabsf(dx) > 10.0f || fabsf(dy) > 10.0f) continue;
    
    float distSq = dx*dx + dy*dy;
    if(distSq < 0.01f || distSq > 100.0f) continue;
    
    float dist = sqrtf(distSq);
    if(!isfinite(dist)) continue;
    
    float angle2 = atan2f(dy, dx) - pa;
    if(!isfinite(angle2)) continue;
    
    while(angle2 < -3.14159f) angle2 += TWO_PI;
    while(angle2 > 3.14159f) angle2 -= TWO_PI;
    
    float fov2 = FOV_DEG * 3.14159f / 180.0f;
    
    if(fabsf(angle2) < fov2 / 2.0f) {
      float screenFactor = angle2 / (fov2 / 2.0f);
      if(!isfinite(screenFactor)) continue;
      
      int sx = GAME_W/2 + (int)(screenFactor * GAME_W/2);
      if(sx < 10 || sx >= GAME_W - 10) continue;
      
      int sy = GAME_H/2;
      int size = constrain((int)(15.0f / dist), 3, 10);
      
      sprite->fillCircle(sx, sy, size, TFT_ORANGE);
    }
  }
    
  // Draw synthesizers
  for(int y=0;y<7;y++) {
    for(int x=0;x<7;x++) {
      if(world[y*7+x]=='S') {
        float dx = x+0.5f-px, dy=y+0.5f-py;
        float dist = sqrtf(dx*dx+dy*dy);
        float angle2 = atan2f(dy,dx)-pa;
        if(angle2 < -3.14159f) angle2 += TWO_PI;
        if(angle2 >  3.14159f) angle2 -= TWO_PI;
        float fov2 = FOV_DEG*3.14159f/180.0f;
        if(fabsf(angle2) < fov2/2 && dist>0.34f) {
          int sx = GAME_W/2 + (angle2/(fov2/2))*GAME_W/2;
          drawSynthesizer(sprite, dist, sx);
        }
      }
    }
  }
  
  // HUD
  sprite->setTextColor(TFT_CYAN, TFT_BLACK);
  sprite->setTextDatum(TL_DATUM);
  sprite->drawString(cdNames[cur_album], 10, 5, 2);
  
  sprite->setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite->drawString("Lives: " + String(player_lives), 10, 25, 1);
  
  sprite->setTextColor(TFT_WHITE, TFT_BLACK);
  sprite->drawString("Ammo: " + String(player.ammo), 10, 40, 1);
  
  sprite->setTextColor(TFT_GREEN, TFT_BLACK);
  sprite->drawString("Score: " + String(gs.score), 10, 55, 1);
  
  // Rapid fire indicator
  if(player.hasRapidFire) {
    unsigned long remaining = (player.rapidFireUntil - millis()) / 1000;
    sprite->setTextColor(TFT_MAGENTA, TFT_BLACK);
    sprite->drawString("RAPID FIRE: " + String(remaining) + "s", GAME_W - 150, 5, 2);
  }
  
  // Level progress
  sprite->setTextDatum(TR_DATUM);
  sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
  sprite->drawString("Album " + String(cur_album + 1) + "/" + String(NUM_CDS), GAME_W - 10, 25, 1);
  sprite->drawString("Enemies: " + String(num_enemies), GAME_W - 10, 40, 1);
}

void fireAtEnemies(TFT_eSprite *sprite, TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, int *outShot, float *outLen, char* world, GameState &gs) {
  float fx=player.x, fy=player.y, dx=cosf(player.angle), dy=sinf(player.angle);
  float shotEnd = 1.6f;
  
  for(float dd=0.12f; dd<2.8f; dd+=0.09f) {
    fx = player.x+dx*dd;
    fy = player.y+dy*dd;
    
    // Check enemy hits
    for(int e=0;e<num_enemies;e++) {
      if(enemies[e].alive) {
        float dist=sqrtf((enemies[e].x-fx)*(enemies[e].x-fx)+(enemies[e].y-fy)*(enemies[e].y-fy));
        if(dist<0.28f) {
          enemies[e].alive=0;
          shotEnd = dd*GAME_H/2.6f;
          *outShot = 1;
          *outLen = shotEnd;
          
          // Score points
          gs.score += (enemies[e].type == 0) ? 100 : 150; // Musical notes worth more
          gs.enemiesKilled++;
          
          // Flash effect
          sprite->fillCircle(GAME_W/2, GAME_H/2, 45, TFT_YELLOW);
          sprite->setTextColor(TFT_RED, TFT_YELLOW);
          sprite->setTextDatum(MC_DATUM);
          if(enemies[e].type == 0) {
            sprite->drawString("TANGERINATED!", GAME_W/2, GAME_H/2, 4);
          } else {
            sprite->drawString("SILENCED!", GAME_W/2, GAME_H/2, 4);
          }
          sprite->pushSprite(0, 0);
          delay(400);
          return;
        }
      }
    }
    
    // Check wall hits
    int mx = int(fx), my = int(fy);
    if (isWall(mx, my, world)) {
      shotEnd = dd*GAME_H/2.6f;
      *outShot = 1;
      *outLen = shotEnd;
      return;
    }
  }
  *outShot = 1;
  *outLen = shotEnd;
}

void checkCDPickup(TFT_eSPI &tft, TFT_eSprite *sprite, TangerinePlayer &player, char* world, int &cd_collected, int cur_album, GameState &gs) {
  int cx = int(player.x), cy = int(player.y);
  if(world[cy*7+cx]=='C') {
    world[cy*7+cx]='.';
    cd_collected = 1;
    
    // Score bonus
    gs.score += 500;
    
    sprite->fillSprite(TFT_BLACK);
    sprite->setTextDatum(MC_DATUM);
    
    // Draw the CD spinning
    for(int frame = 0; frame < 3; frame++) {
      sprite->fillSprite(TFT_BLACK);
      drawCD(sprite, 2.0f - frame * 0.3f, GAME_W/2, cdNames[cur_album]);
      sprite->pushSprite(0, 0);
      delay(200);
    }
    
    sprite->fillSprite(TFT_BLACK);
    sprite->setTextColor(TFT_YELLOW, TFT_BLACK);
    sprite->drawString(cdNames[cur_album], GAME_W/2, GAME_H/2-40, 4);
    sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
    sprite->drawString("ALBUM COLLECTED!", GAME_W/2, GAME_H/2+20, 4);
    sprite->setTextColor(TFT_GREEN, TFT_BLACK);
    sprite->drawString("+500 POINTS", GAME_W/2, GAME_H/2+60, 2);
    sprite->pushSprite(0, 0);
    delay(1500);
  }
}

void checkSynthesizerPickup(TangerinePlayer &player, char* world) {
  int cx = int(player.x), cy = int(player.y);
  if(world[cy*7+cx]=='S') {
    world[cy*7+cx]='.';
    
    // Grant rapid fire for 10 seconds
    player.hasRapidFire = 1;
    player.rapidFireUntil = millis() + 10000;
  }

}

void updateEnemyShooting(TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, GameState &gs, int cur_album, char* world, int &player_lives) {
  // Safety checks
  if(cur_album < 1) return;
  if(player_lives <= 0) return;
  if(num_enemies <= 0) return;
  
  unsigned long now = millis();
  
  // FIRST: Clean up any corrupted bullets
  for(int b = 0; b < MAX_ENEMY_BULLETS; b++) {
    if(gs.enemyBullets[b].active) {
      // Check for NaN or invalid values
      if(!isfinite(gs.enemyBullets[b].x) || !isfinite(gs.enemyBullets[b].y) ||
         !isfinite(gs.enemyBullets[b].dx) || !isfinite(gs.enemyBullets[b].dy)) {
        gs.enemyBullets[b].active = 0;
      }
    }
  }
  
  // Enemy shooting
  for(int e = 0; e < num_enemies && e < MAX_ENEMIES; e++) {
    if(!enemies[e].alive) continue;
    
    // Validate enemy position
    if(!isfinite(enemies[e].x) || !isfinite(enemies[e].y)) continue;
    if(enemies[e].x < 0 || enemies[e].x > 7 || 
       enemies[e].y < 0 || enemies[e].y > 7) continue;
    
    float dx = player.x - enemies[e].x;
    float dy = player.y - enemies[e].y;
    
    if(!isfinite(dx) || !isfinite(dy)) continue;
    
    float dist = sqrtf(dx*dx + dy*dy);
    
    if(!isfinite(dist) || dist < 0.01f) continue;
    
    // Only shoot if player is close
    if(dist < 5.0f && dist > 0.5f) {
      // Random shooting - less frequent
      if((now + e * 1000) % (4000 + random(2000)) < 25) {
        
        // Find free bullet slot
        for(int b = 0; b < MAX_ENEMY_BULLETS; b++) {
          if(!gs.enemyBullets[b].active) {
            // Initialize bullet with validated values
            gs.enemyBullets[b].x = enemies[e].x;
            gs.enemyBullets[b].y = enemies[e].y;
            
// Normalize direction carefully
            float len = sqrtf(dx*dx + dy*dy);
            if(len > 0.1f && isfinite(len)) {
              gs.enemyBullets[b].dx = (dx / len) * 0.10f;  // DOUBLED from 0.05f
              gs.enemyBullets[b].dy = (dy / len) * 0.10f;  // DOUBLED from 0.05f
              
              // Validate computed direction
              if(isfinite(gs.enemyBullets[b].dx) && isfinite(gs.enemyBullets[b].dy)) {
                gs.enemyBullets[b].active = 1;
                gs.enemyBullets[b].spawnTime = now;
              }
            }
            break;
          }
        }
      }
    }
  }
  
  // Update bullet positions with validation
  for(int b = 0; b < MAX_ENEMY_BULLETS; b++) {
    if(!gs.enemyBullets[b].active) continue;
    
    // Validate before updating
    if(!isfinite(gs.enemyBullets[b].dx) || !isfinite(gs.enemyBullets[b].dy)) {
      gs.enemyBullets[b].active = 0;
      continue;
    }
    
    // Update position
    gs.enemyBullets[b].x += gs.enemyBullets[b].dx;
    gs.enemyBullets[b].y += gs.enemyBullets[b].dy;
    
    // Validate after updating
    if(!isfinite(gs.enemyBullets[b].x) || !isfinite(gs.enemyBullets[b].y)) {
      gs.enemyBullets[b].active = 0;
      continue;
    }
    
    // Bounds check
    if(gs.enemyBullets[b].x < -1.5f || gs.enemyBullets[b].x > 8.5f ||
       gs.enemyBullets[b].y < -1.5f || gs.enemyBullets[b].y > 8.5f) {
      gs.enemyBullets[b].active = 0;
      continue;
    }
    
    // Wall collision check
    int mx = (int)gs.enemyBullets[b].x;
    int my = (int)gs.enemyBullets[b].y;
    if(mx >= 0 && mx < 7 && my >= 0 && my < 7) {
      if(isWall(mx, my, world)) {
        gs.enemyBullets[b].active = 0;
        continue;
      }
    }
    
    // Timeout
    if(now - gs.enemyBullets[b].spawnTime > 5000) {
      gs.enemyBullets[b].active = 0;
      continue;
    }
    
    // Player hit check
    if(player_lives > 0) {
      float pdx = player.x - gs.enemyBullets[b].x;
      float pdy = player.y - gs.enemyBullets[b].y;
      
      if(isfinite(pdx) && isfinite(pdy)) {
        float pdist = sqrtf(pdx*pdx + pdy*pdy);
        
        if(isfinite(pdist) && pdist < 0.35f) {
          gs.enemyBullets[b].active = 0;
          player_lives--;
        }
      }
    }
  }
}

void updateEnemyMovement(TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, char* world) {
  static unsigned long lastMoveTime = 0;
  unsigned long now = millis();
  
  // Only update enemy movement every 100ms to reduce CPU load
  if(now - lastMoveTime < 100) return;
  lastMoveTime = now;
  
  for(int e = 0; e < num_enemies && e < MAX_ENEMIES; e++) {
    if(!enemies[e].alive) continue;
    
    // Validate enemy position
    if(!isfinite(enemies[e].x) || !isfinite(enemies[e].y)) continue;
    if(enemies[e].x < 0.5f || enemies[e].x > 6.5f || 
       enemies[e].y < 0.5f || enemies[e].y > 6.5f) continue;
    
    // Calculate direction to player
    float dx = player.x - enemies[e].x;
    float dy = player.y - enemies[e].y;
    
    if(!isfinite(dx) || !isfinite(dy)) continue;
    
    float dist = sqrtf(dx*dx + dy*dy);
    
    if(!isfinite(dist) || dist < 0.2f) continue; // Too close or invalid
    
    // Only move if player is in range
    if(dist < 6.0f && dist > 0.3f) {
      // Normalize direction
      float moveSpeed = 0.03f; // Slower than player
      float moveDx = (dx / dist) * moveSpeed;
      float moveDy = (dy / dist) * moveSpeed;
      
      if(!isfinite(moveDx) || !isfinite(moveDy)) continue;
      
      // Try to move toward player
      float testX = enemies[e].x + moveDx;
      float testY = enemies[e].y + moveDy;
      
      // Validate new position
      if(!isfinite(testX) || !isfinite(testY)) continue;
      
      int checkX = (int)testX;
      int checkY = (int)testY;
      
      // Simple wall check
      if(checkX >= 0 && checkX < 7 && checkY >= 0 && checkY < 7) {
        if(!isWall(checkX, checkY, world)) {
          // Check collision with other enemies (simplified)
          bool canMove = true;
          for(int other = 0; other < num_enemies && other < MAX_ENEMIES; other++) {
            if(other == e || !enemies[other].alive) continue;
            
            float odx = enemies[other].x - testX;
            float ody = enemies[other].y - testY;
            float oDist = sqrtf(odx*odx + ody*ody);
            
            if(oDist < 0.35f) {
              canMove = false;
              break;
            }
          }
          
          if(canMove) {
            enemies[e].x = testX;
            enemies[e].y = testY;
          }
        }
      }
    }
  }
}

void checkEnemyTouch(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss, TangerinePlayer &player, TangerineSprite* enemies, int &num_enemies, int &player_lives, int &cur_album, char* world, int &cd_collected, bool &restart, bool &exitMenu, GameState &gs) {
  // Only check if player is alive
  if(player_lives <= 0) return;
  
  for(int e=0; e<num_enemies; e++) {
    if(!enemies[e].alive) continue;
    
    float dist=sqrtf((player.x-enemies[e].x)*(player.x-enemies[e].x)+(player.y-enemies[e].y)*(player.y-enemies[e].y));
    if(dist<0.27f) {
      enemies[e].alive=0;
      player_lives--;
      
      sprite->fillSprite(TFT_BLACK);
      sprite->setTextDatum(MC_DATUM);
      sprite->setTextColor(TFT_RED, TFT_BLACK);
      if(enemies[e].type == 0) {
        sprite->drawString("TANGERINED!", GAME_W/2, GAME_H/2, 6);
      } else {
        sprite->drawString("DISCORDANT NOTE!", GAME_W/2, GAME_H/2, 4);
      }
      sprite->pushSprite(0, 0);
      delay(800);
      
      if(player_lives<=0) {
        showScoreScreen(sprite, ss, gs, cur_album);
        
        sprite->fillSprite(TFT_BLACK);
        sprite->setTextColor(TFT_WHITE, TFT_BLACK);
        sprite->drawString("A: Restart   SELECT: Menu", GAME_W/2, GAME_H/2+36, 2);
        sprite->pushSprite(0, 0);
        
        bool restartBtn=false, menuBtn=false;
        do {
          restartBtn = !ss.digitalRead(BUTTON_A);
          menuBtn = !ss.digitalRead(BUTTON_SELECT);
          delay(20);
        } while(!restartBtn&&!menuBtn);
        
        if(restartBtn) { restart=true; }
        if(menuBtn) { exitMenu=true; }
      }
      return;
    }
  }
}

void showScoreScreen(TFT_eSprite *sprite, Adafruit_seesaw &ss, GameState &gs, int cur_album) {
  unsigned long timePlayed = (millis() - gs.levelStartTime) / 1000;
  
  sprite->fillSprite(TFT_BLACK);
  sprite->setTextDatum(MC_DATUM);
  
  bool victory = (cur_album >= NUM_CDS);
  
  if(victory) {
    sprite->setTextColor(TFT_YELLOW, TFT_BLACK);
    sprite->drawString("ELECTRONIC DREAMS", GAME_W/2, 40, 4);
    sprite->drawString("COMPLETED!", GAME_W/2, 80, 4);
  } else {
    sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
    sprite->drawString("THE DREAM ENDS", GAME_W/2, 50, 4);
  }
  
  sprite->setTextColor(TFT_CYAN, TFT_BLACK);
  sprite->drawString("FINAL SCORE", GAME_W/2, 120, 2);
  
  sprite->setTextColor(TFT_WHITE, TFT_BLACK);
  sprite->drawString(String(gs.score), GAME_W/2, 150, 4);
  
  sprite->setTextColor(TFT_GREEN, TFT_BLACK);
  sprite->drawString("Albums Collected: " + String(gs.cdCount), GAME_W/2, 190, 2);
  sprite->drawString("Enemies Defeated: " + String(gs.enemiesKilled), GAME_W/2, 215, 2);
  sprite->drawString("Time: " + String(timePlayed) + "s", GAME_W/2, 240, 2);
  
  if(victory) {
    sprite->setTextColor(TFT_MAGENTA, TFT_BLACK);
    sprite->drawString("You are a true synth pioneer!", GAME_W/2, 270, 1);
  }
  
  sprite->pushSprite(0, 0);
  delay(3000);
}