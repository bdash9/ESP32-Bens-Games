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

const char* cdNames[] = {
  "Phaedra", "Rubycon", "Stratosfear", "Cyclone",
  "Force Majeure", "Tangram", "Exit", "White Eagle",
  "Hyperborea", "Underwater Sunlight", "Tyger",
  "Poland", "Green Desert", "Encore", "Le Parc", "Firestarter"
};
#define NUM_CDS (sizeof(cdNames)/sizeof(cdNames[0]))

const char* simpleMaps[NUM_CDS] = {
  "#######"
  "#.E...#"
  "#.#.#.#"
  "#..C..#"
  "#.#.#.#"
  "#....G#"
  "#######",
  "#######"
  "#C....#"
  "#.#.#.#"
  "#..E..#"
  "#.#.#.#"
  "#...G.#"
  "#######",
  "#######"
  "#E...C#"
  "#.#.#.#"
  "#.....#"
  "#.#.#.#"
  "#..G..#"
  "#######",
  "#######"
  "#.....#"
  "#.#C#.#"
  "#E...G#"
  "#.#.#.#"
  "#.....#"
  "#######",
  "#######"
  "#.....#"
  "#E#.#C#"
  "#.....#"
  "#.#.#.#"
  "#..G..#"
  "#######",
  "#######"
  "#C....#"
  "#.###.#"
  "#..E..#"
  "#.###.#"
  "#....G#"
  "#######",
  "#######"
  "#.....#"
  "#EC.#.#"
  "#.....#"
  "#E#.#C#"
  "#..G..#"
  "#######",
  "#######"
  "#....C#"
  "#.E#..#"
  "#..#..#"
  "#...E.#"
  "#G...C#"
  "#######",
  "#######"
  "#C..E.#"
  "#.#.#.#"
  "#..G..#"
  "#E#.#C#"
  "#.....#"
  "#######",
  "#######"
  "#C...E#"
  "#.###.#"
  "#.....#"
  "#.###.#"
  "#G...C#"
  "#######",
  "#######"
  "#C....#"
  "#.###.#"
  "#..E..#"
  "#.###.#"
  "#G...E#"
  "#######",
  "#######"
  "#....E#"
  "#.#C#.#"
  "#..G..#"
  "#.#E#.#"
  "#....C#"
  "#######",
  "#######"
  "#..C..#"
  "#.E#E.#"
  "#..G..#"
  "#.#.#.#"
  "#...E.#"
  "#######",
  "#######"
  "#.E...#"
  "#.#E#.#"
  "#..G..#"
  "#C#.#E#"
  "#...C.#"
  "#######",
  "#######"
  "#.E#..#"
  "#C.#E.#"
  "#..G..#"
  "#...#.#"
  "#E..C.#"
  "#######",
  "#######"
  "#C#..E#"
  "#...#C#"
  "#E.G.E#"
  "#.#.#.#"
  "#E...C#"
  "#######"
};
const uint16_t wallColorSets[][4] = {
  {TFT_BLUE, TFT_MAGENTA, TFT_ORANGE, TFT_DARKGREY},
  {TFT_ORANGE, TFT_GREEN, TFT_CYAN, TFT_PURPLE},
  {TFT_YELLOW, TFT_VIOLET, TFT_PINK, TFT_BROWN},
  {TFT_GREENYELLOW, TFT_NAVY, TFT_RED, TFT_DARKGREEN},
  {TFT_GOLD, TFT_WHITE, TFT_CYAN, TFT_OLIVE},
  {TFT_MAROON, TFT_GREENYELLOW, TFT_LIGHTGREY, TFT_BLUE}
};
#define NUM_WALL_SETS (sizeof(wallColorSets)/sizeof(wallColorSets[0]))

struct TangerineSprite { float x, y; int alive; };
struct TangerinePlayer { float x, y, angle; };

#define MAX_ENEMIES 32

// All function declarations - now use TFT_eSprite
inline bool isWall(int mx, int my, char* world);
void drawTangerine(TFT_eSprite *sprite, int x, int y, int r);
void drawCD(TFT_eSprite *sprite, float depth, int sx, const char* cdLabel);
void drawEnemy(TFT_eSprite *sprite, float depth, int sx);
void drawShotRay(TFT_eSprite *sprite, float len);
void splashScreen(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss, TangerinePlayer &player, int &cur_album, char* world, TangerineSprite* enemies, int &num_enemies, int &cd_collected, int &player_lives);
void loadLevel(int which, TangerinePlayer &player, char* world, TangerineSprite* enemies, int &num_enemies, int &cd_collected, int &player_lives);
void raycastDrawDirect(TFT_eSprite *sprite, TangerinePlayer &player, int cur_album, char* world, TangerineSprite* enemies, int num_enemies, int player_lives, int shotShow, float shotLen);
void fireAtEnemies(TFT_eSprite *sprite, TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, int *outShot, float *outLen, char* world);
void checkCDPickup(TFT_eSPI &tft, TFT_eSprite *sprite, TangerinePlayer &player, char* world, int &cd_collected, int cur_album);
void checkEnemyTouch(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss, TangerinePlayer &player, TangerineSprite* enemies, int &num_enemies, int &player_lives, int &cur_album, char* world, int &cd_collected, bool &restart, bool &exitMenu);


void run_ScottsTangerineDream(TFT_eSPI &tft, Adafruit_seesaw &ss) {
  // Create PSRAM sprite buffer
  TFT_eSprite *sprite = new TFT_eSprite(&tft);
  sprite->setColorDepth(16);
  sprite->setAttribute(PSRAM_ENABLE, true);
  
  if (!sprite->createSprite(GAME_W, GAME_H)) {
    Serial.println("Failed to create sprite - not enough PSRAM!");
    delete sprite;
    return;
  }
  
  static TangerinePlayer player;
  char world[7*7];
  TangerineSprite enemies[MAX_ENEMIES];
  int num_enemies, cd_collected, player_lives, cur_album;

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

  splashScreen(tft, sprite, ss, player, cur_album, world, enemies, num_enemies, cd_collected, player_lives);
  cur_album = 0;
  loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives);

  static unsigned long lastDraw=0;
  static int frameShot=0;
  static float lastShotLen=0;

  bool exitMenu = false;
  while(!exitMenu) {
    if(millis()-lastDraw<28) { delay(3); continue; }
    lastDraw=millis();
    updateInputs();

    if(btnSelect) break;

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

    int shotShow = 0;
    float shotLen = 0;
    if(btnA && !prevA) {
      fireAtEnemies(sprite, player, enemies, num_enemies, &shotShow, &shotLen, world);
      frameShot = 2;
      lastShotLen = shotLen;
    }
    prevA = btnA;

    checkCDPickup(tft, sprite, player, world, cd_collected, cur_album);
    bool restart = false;
    checkEnemyTouch(tft, sprite, ss, player, enemies, num_enemies, player_lives, cur_album, world, cd_collected, restart, exitMenu);
    if(exitMenu) break;
    if(restart) {
      cur_album = 0;
      loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives);
      splashScreen(tft, sprite, ss, player, cur_album, world, enemies, num_enemies, cd_collected, player_lives);
    }

    if (frameShot > 0) {
      raycastDrawDirect(sprite, player, cur_album, world, enemies, num_enemies, player_lives, 1, lastShotLen);
      frameShot--;
    }
    else {
      raycastDrawDirect(sprite, player, cur_album, world, enemies, num_enemies, player_lives, 0, 0);
    }
    
    // Push complete frame to screen (no flicker!)
    sprite->pushSprite(0, 0);

    if(cd_collected) {
      delay(350);
      cur_album++;
      if(cur_album>=NUM_CDS) {
        sprite->fillSprite(TFT_BLACK);
        sprite->setTextDatum(MC_DATUM);
        sprite->setTextColor(TFT_GREEN, TFT_BLACK);
        sprite->drawString("YOU DREAMED THEM ALL!", GAME_W/2, GAME_H/2-38, 4);
        sprite->setTextColor(TFT_YELLOW, TFT_BLACK);
        sprite->drawString("PRESS ANY BTN FOR NEW GAME", GAME_W/2, GAME_H/2+48, 3);
        sprite->pushSprite(0, 0);
        
        waitAllRelease();
        do{ updateInputs(); if(btnA||btnB||btnX||btnY||btnSelect) break; delay(10);} while(1);
        splashScreen(tft, sprite, ss, player, cur_album, world, enemies, num_enemies, cd_collected, player_lives);
        cur_album=0;
        loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives);
      }
      else {
        loadLevel(cur_album, player, world, enemies, num_enemies, cd_collected, player_lives);
      }
    }
  }
  
  sprite->deleteSprite();
  delete sprite;
  tft.fillScreen(TFT_BLACK);
}

// --------- HELPERS (functions) - updated to use sprite -------------

inline bool isWall(int mx, int my, char* world) {
  if(mx<0 || mx>=7 || my<0 || my>=7) return true;
  return world[my*7+mx]=='#';
}

void drawTangerine(TFT_eSprite *sprite, int x, int y, int r) {
  sprite->fillCircle(x, y, r, TFT_ORANGE);
  sprite->drawCircle(x, y, r, TFT_BROWN);
  sprite->fillRect(x-8, y-r+2, 16, (r/2), TFT_GREEN);
  sprite->drawLine(x-7, y-r+2, x+7, y-r+(r/2), TFT_DARKGREEN);
  sprite->drawLine(x+7, y-r+2, x-7, y-r+(r/2), TFT_DARKGREEN);
  sprite->fillTriangle(x-3, y-r+11, x-19, y-r+18, x-7, y-r+21, TFT_GREEN);
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

void drawEnemy(TFT_eSprite *sprite, float depth, int sx) {
  int sz = GAME_H/(depth*2.1f);
  if(sz<10 || sx<0 || sx>=GAME_W) return;
  int y0 = GAME_H/2-sz/2;
  drawTangerine(sprite, sx, y0+sz/2, sz/2-2);
}

void drawShotRay(TFT_eSprite *sprite, float len) {
  int x0 = GAME_W / 2;
  int y0 = GAME_H - 1;
  int x1 = GAME_W / 2;
  int y1 = GAME_H / 2;
  for (int i = -3; i <= 3; i++) {
    sprite->drawLine(x0 + i, y0, x1 + i, y1, TFT_RED);
  }
}

void splashScreen(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss, TangerinePlayer &player, int &cur_album, char* world, TangerineSprite* enemies, int &num_enemies, int &cd_collected, int &player_lives) {
  sprite->fillSprite(TFT_BLACK);
  sprite->setTextDatum(MC_DATUM);

  drawTangerine(sprite, GAME_W/2-142, GAME_H/2+24, 36);
  drawTangerine(sprite, GAME_W/2+142, GAME_H/2+24, 36);

  sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
  sprite->drawString("SCOTT'S", GAME_W/2, 24, 2);

  const char *txt = "TANGERINE DREAM";
  int L = strlen(txt);
  int px = GAME_W/2 - (L*13)/2 + 8;
  uint16_t tCols[6] = {TFT_ORANGE, TFT_YELLOW, 0xFEA0, 0xF6C0, 0xFFE0, TFT_ORANGE};
  for (int i = 0; i < L; i++) {
    char one[2]{txt[i], 0};
    int colidx = (i+2)%6;
    sprite->setTextColor(tCols[colidx], TFT_BLACK);
    sprite->drawString(one, px + i*13, 58, 2);
  }

  sprite->setTextColor(TFT_CYAN, TFT_BLACK);
  sprite->drawString("FPS EDITION", GAME_W/2, 90, 0);
  sprite->drawString("Control: Move/Turn with joystick", GAME_W/2, 113, 1);

  sprite->setTextColor(TFT_RED, TFT_BLACK);
  sprite->drawString("A: FIRE", GAME_W/2, 164, 1);

  sprite->setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite->drawString("Collect: CD's", GAME_W/2, 200, 1);

  sprite->setTextColor(TFT_WHITE, TFT_BLACK);
  sprite->drawString("Beware of Floating Tangerines!", GAME_W/2, 250, 1);

  sprite->setTextColor(TFT_GREEN, TFT_BLACK);
  sprite->drawString("Press ANY button...", GAME_W/2, 285, 1);
  
  sprite->pushSprite(0, 0);

  bool btnA=false, btnB=false, btnX=false, btnY=false, btnSelect=false;
  do {
    btnA = !ss.digitalRead(BUTTON_A);
    btnB = !ss.digitalRead(BUTTON_B);
    btnX = !ss.digitalRead(BUTTON_X);
    btnY = !ss.digitalRead(BUTTON_Y);
    btnSelect = !ss.digitalRead(BUTTON_SELECT);
    delay(13);
  } while(!btnA && !btnB && !btnX && !btnY && !btnSelect);
  
  sprite->fillSprite(TFT_BLACK);
  sprite->pushSprite(0, 0);
  delay(120);
}

void loadLevel(int which, TangerinePlayer &player, char* world, TangerineSprite* enemies, int &num_enemies, int &cd_collected, int &player_lives) {
  memcpy(world, simpleMaps[which], 7*7);
  for(int y=0;y<7;y++) for(int x=0;x<7;x++) {
    char c = world[y*7+x];
    if(c=='G') { player.x = x+0.5f; player.y = y+0.5f; player.angle = 0.0f; world[y*7+x]='.'; }
  }
  num_enemies=0;
  int target_enemies = (which+1);
  for(int y=0;y<7;y++) for(int x=0;x<7;x++) {
    char c = world[y*7+x];
    if(c=='E' && num_enemies<MAX_ENEMIES && num_enemies<target_enemies) {
      enemies[num_enemies].x = x+0.5f;
      enemies[num_enemies].y = y+0.5f;
      enemies[num_enemies].alive = 1;
      world[y*7+x]='.';
      num_enemies++;
    }
  }
  while(num_enemies < target_enemies && num_enemies < MAX_ENEMIES) {
    int addTry = 0;
    while(addTry++ < 60) {
      int rx = random(1,5), ry = random(1,5);
      bool occupied = false;
      for(int k=0;k<num_enemies;k++)
        if(int(enemies[k].x)==rx && int(enemies[k].y)==ry) occupied=true;
      if(!isWall(rx,ry,world) && world[ry*7+rx]=='.' && !occupied &&
         (fabsf(player.x-rx)>1.3f || fabsf(player.y-ry)>1.3f)) {
        enemies[num_enemies].x = rx+0.5f;
        enemies[num_enemies].y = ry+0.5f;
        enemies[num_enemies].alive = 1;
        num_enemies++;
        break;
      }
    }
  }
  cd_collected=0;
  player_lives=3;
}

void raycastDrawDirect(TFT_eSprite *sprite, TangerinePlayer &player, int cur_album, char* world, TangerineSprite* enemies, int num_enemies, int player_lives, int shotShow, float shotLen) {
  sprite->fillSprite(TFT_BLACK);
  const uint16_t* wallCols = wallColorSets[cur_album%NUM_WALL_SETS];

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
  for(int e=0;e<num_enemies;e++) if(enemies[e].alive) {
    float dx=enemies[e].x-px, dy=enemies[e].y-py;
    float dist = sqrtf(dx*dx+dy*dy);
    float angle2 = atan2f(dy,dx)-pa;
    if(angle2<-3.14159f) angle2+=TWO_PI;
    if(angle2>3.14159f) angle2-=TWO_PI;
    float fov2 = FOV_DEG*3.14159f/180.0f;
    if(fabsf(angle2)<fov2/2 && dist>0.45f) {
      int sx = GAME_W/2 + (angle2/(fov2/2))*GAME_W/2;
      drawEnemy(sprite, dist, sx);
    }
  }
  for(int y=0;y<7;y++)
    for(int x=0;x<7;x++)
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
  sprite->setTextColor(TFT_WHITE, TFT_BLACK);
  sprite->setTextDatum(TL_DATUM);
  sprite->drawString(cdNames[cur_album], 10, 7, 2);
  sprite->setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite->drawString("Lives:", 160, 7, 2);
  sprite->drawNumber(player_lives, 225, 7, 2);
}

void fireAtEnemies(TFT_eSprite *sprite, TangerinePlayer &player, TangerineSprite* enemies, int num_enemies, int *outShot, float *outLen, char* world) {
  float fx=player.x, fy=player.y, dx=cosf(player.angle), dy=sinf(player.angle);
  float shotEnd = 1.6f;
  for(float dd=0.12f; dd<2.8f; dd+=0.09f) {
    fx = player.x+dx*dd;
    fy = player.y+dy*dd;
    for(int e=0;e<num_enemies;e++) {
      if(enemies[e].alive) {
        float dist=sqrtf((enemies[e].x-fx)*(enemies[e].x-fx)+(enemies[e].y-fy)*(enemies[e].y-fy));
        if(dist<0.28f) {
          enemies[e].alive=0;
          shotEnd = dd*GAME_H/2.6f;
          *outShot = 1;
          *outLen = shotEnd;
          sprite->fillCircle(GAME_W/2, GAME_H/2, 35, TFT_YELLOW);
          sprite->setTextColor(TFT_RED, TFT_YELLOW);
          sprite->setTextDatum(MC_DATUM);
          sprite->drawString("TANGERINATED!", GAME_W/2, GAME_H/2, 4);
          sprite->pushSprite(0, 0);
          delay(500);
          return;
        }
      }
    }
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

void checkCDPickup(TFT_eSPI &tft, TFT_eSprite *sprite, TangerinePlayer &player, char* world, int &cd_collected, int cur_album) {
  int cx = int(player.x), cy = int(player.y);
  if(world[cy*7+cx]=='C') {
    world[cy*7+cx]='.';
    cd_collected = 1;
    sprite->fillSprite(TFT_BLACK);
    sprite->setTextDatum(MC_DATUM);
    sprite->setTextColor(TFT_YELLOW, TFT_BROWN);
    sprite->drawString(cdNames[cur_album], GAME_W/2, GAME_H/2-36, 4);
    sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
    sprite->drawString("CD COLLECTED!", GAME_W/2, GAME_H/2+32, 4);
    sprite->pushSprite(0, 0);
    delay(900);
  }
}

void checkEnemyTouch(TFT_eSPI &tft, TFT_eSprite *sprite, Adafruit_seesaw &ss, TangerinePlayer &player, TangerineSprite* enemies, int &num_enemies, int &player_lives, int &cur_album, char* world, int &cd_collected, bool &restart, bool &exitMenu) {
  for(int e=0;e<num_enemies;e++) if(enemies[e].alive) {
    float dist=sqrtf((player.x-enemies[e].x)*(player.x-enemies[e].x)+(player.y-enemies[e].y)*(player.y-enemies[e].y));
    if(dist<0.27f) {
      enemies[e].alive=0;
      player_lives--;
      sprite->fillSprite(TFT_BLACK);
      sprite->setTextDatum(MC_DATUM);
      sprite->setTextColor(TFT_RED, TFT_BLACK);
      sprite->drawString("TANGERINED!", GAME_W/2, GAME_H/2, 6);
      sprite->pushSprite(0, 0);
      delay(700);
      if(player_lives<=0) {
        sprite->fillSprite(TFT_BLACK);
        sprite->setTextDatum(MC_DATUM);
        sprite->setTextColor(TFT_ORANGE, TFT_BLACK);
        sprite->drawString("The Dream is over", GAME_W/2, GAME_H/2, 4);
        sprite->pushSprite(0, 0);
        delay(1000);

        sprite->setTextColor(TFT_WHITE, TFT_BLACK);
        sprite->drawString("A: Restart   SELECT: Menu", GAME_W/2, GAME_H/2+36, 2);
        sprite->pushSprite(0, 0);
        
        // Wait for A or SELECT
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