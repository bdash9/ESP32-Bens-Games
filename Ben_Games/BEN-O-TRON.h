#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <math.h>

#define TFT_LIGHTPURPLE 0xB4FC
#define BUG_BODY_YELLOW TFT_YELLOW
#define CHIP_GREEN      0x0400

// ---- Controls/Hardware setup/macros for local scope
#define JOY_ADDR      0x49
#define BUTTON_A      6 // right shoot
#define BUTTON_B      7 // down shoot
#define BUTTON_X      9 // left shoot
#define BUTTON_Y      10 // up shoot
#define BUTTON_SELECT 14
#define JOY_X         3
#define JOY_Y         2

#define PLAYER_R      7
#define PLAYER_SPEED  3
#define SHOT_SPEED    9
#define ENEMY_SHOT_SPEED 4
#define PLAYER_SHOT_R 3
#define ENEMY_SHOT_R  3
#define BUG_R         8
#define CHIP_R        14
#define CHIP_COLL_R   10
#define VIRUS_R       10
#define WORM_R        8
#define PROGRAMMER_R  7

#define MAX_BUGS      16
#define MAX_WORMS     2
#define MAX_VIRUSES   5
#define MAX_PLAYER_SHOTS 8
#define MAX_ENEMY_SHOTS 16
#define MAX_PROGRAMMERS 3
#define MAX_CHIPS      4

#define WAVE_START_LIVES 3
#define REQUIRED_CHIPS  3

struct Entity {
  int x, y;
  int dx, dy;
  bool active;
};
struct Chip {
  int x, y;
  bool collected;
  bool active;
};
struct Shot {
  int x, y;
  int dx, dy;
  bool active;
};
struct Programmer {
  int x, y;
  bool alive, rescued;
};
struct Player {
  int x, y;
  int lives;
  bool invuln;
  unsigned long invuln_end;
  unsigned long lastFireDir[4];
};

// --- Helper macros for button logic
#define READ_INPUTS \
  joyX = ss.analogRead(JOY_X); \
  joyY = ss.analogRead(JOY_Y); \
  btnA      = !ss.digitalRead(BUTTON_A); \
  btnB      = !ss.digitalRead(BUTTON_B); \
  btnX      = !ss.digitalRead(BUTTON_X); \
  btnY      = !ss.digitalRead(BUTTON_Y); \
  btnSelect = !ss.digitalRead(BUTTON_SELECT);

inline int constrainVal(int v, int mn, int mx) { return (v<mn)?mn:(v>mx)?mx:v; }

// ====== BEN-O-TRON GAME BEGIN ===================================================
void run_BenOTron(TFT_eSPI &tft, Adafruit_seesaw &ss)
{
  // ENSURE ALL BEN-O-TRON TEXT IS CONSISTENT
    tft.setTextFont(2);   // Or whatever Ben-O-Tron is designed for
    tft.setTextSize(1);   // 1 is normal, bigger numbers = bigger letters
  // ------------ GAME STATE & VARIABLES -------------
  int GAME_W = tft.width(), GAME_H = tft.height();
  Player player;
  Entity bugs[MAX_BUGS];
  Entity worms[MAX_WORMS];
  Entity viruses[MAX_VIRUSES];
  Programmer programmers[MAX_PROGRAMMERS];
  Chip chips[MAX_CHIPS];
  Shot playerShots[MAX_PLAYER_SHOTS];
  Shot enemyShots[MAX_ENEMY_SHOTS];

  int joyX = 512, joyY = 512;
  bool btnA = false, btnB = false, btnX = false, btnY = false, btnSelect = false;
  bool prevBtnA = false, prevBtnB = false, prevBtnX = false, prevBtnY = false, prevBtnSelect = false;
  int score = 0, wave = 1, enemiesLeft = 0, chipsCollected = 0, chipsThisWave = 0;
  bool gameOver = false;

  // ------------ Helper Lambdas ------------

  auto printRetroLine = [&](const char* str, int x, int y, int font=2, int perCharDelay=38) {
    tft.setTextFont(font);
    tft.setTextColor(CHIP_GREEN, TFT_BLACK);
    int len = strlen(str);
    for (int i=0; i<len; i++) {
      char s[2]={str[i],0};
      tft.drawString(s, x + tft.textWidth("A")*i, y, font);
      tft.drawString("_", x + tft.textWidth("A")*(i+1), y, font);
      delay(perCharDelay);
      tft.drawString(" ", x + tft.textWidth("A")*(i+1), y, font);
    }
    tft.drawString("_", x + tft.textWidth("A")*len, y, font);
    delay(340);
    tft.drawString(" ", x + tft.textWidth("A")*len, y, font);
  };

  auto introRetroSystemMsg = [&]() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(CHIP_GREEN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    int y0 = 40; int font = 2; int x0 = 22;
    printRetroLine("# system compromises detected", x0, y0+0, font);
    delay(320);
    printRetroLine("# initiating purge protocol",   x0, y0+28, font);
    delay(320);
    printRetroLine("# ./Ben-o-Tron --purge",        x0, y0+56, font);
    delay(660);
  };

  auto waitForAllButtonsReleased = [&]() {
    do { READ_INPUTS; delay(10);}
    while (btnSelect || btnA || btnB || btnX || btnY);
    prevBtnA = prevBtnB = prevBtnX = prevBtnY = prevBtnSelect = false;
  };

  auto loseLife = [&]() {
    --player.lives;
    if(player.lives<=0) {
      tft.fillScreen(TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.drawString("GAME OVER", GAME_W/2, GAME_H/2-20, 4);
      char buf[40];
      sprintf(buf,"Final Score: %d", score);
      tft.drawString(buf, GAME_W/2, GAME_H/2+32,2);
      tft.drawString("Press any key to restart", GAME_W/2, GAME_H/2+64,2);
      do {
        READ_INPUTS;
        if (btnA||btnB||btnX||btnY||btnSelect) break;
        delay(10);
      } while(1);
      delay(350);
      player.lives=WAVE_START_LIVES;
      score=0;
      wave=1;
      gameOver = true;
    }
  };
  auto collides = [](int x0,int y0,int r0, int x1,int y1,int r1) {
    int dx = x0-x1, dy=y0-y1;
    return (dx*dx+dy*dy)<=((r0+r1)*(r0+r1));
  };

  // ---- Drawing helpers ----
  auto drawHUD = [&]() {
    char buf[24];
    int wSection = GAME_W/4;
    tft.setTextDatum(TC_DATUM);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    sprintf(buf, "WAVE:%d", wave);
    tft.drawString(buf, wSection/2, 6, 2);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    sprintf(buf, "LIVES:%d", player.lives);
    tft.drawString(buf, GAME_W/2-wSection/2, 6, 2);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    sprintf(buf, "SCORE:%d", score);
    tft.drawString(buf, GAME_W/2 + wSection/2, 6, 2);

    tft.setTextColor(CHIP_GREEN, TFT_BLACK);
    sprintf(buf,"Chips:%d/%d",chipsCollected,REQUIRED_CHIPS);
    tft.drawString(buf, GAME_W-wSection/2,6,2);
  };
  auto drawPlayerMan = [&](int x,int y,int r) {
    tft.fillCircle(x,y-r/2,r/2+1,TFT_CYAN);
    tft.drawLine(x,y-r/4,x,y+r/2,TFT_CYAN);
    tft.drawLine(x-r/2+1,y,x+r/2-1,y,TFT_CYAN);
    tft.drawLine(x,y+r/2,x-r/3,y+r,TFT_CYAN);
    tft.drawLine(x,y+r/2,x+r/3,y+r,TFT_CYAN);
  };
  auto drawChipIcon = [&](int x, int y, int r, bool highlight) {
    uint16_t color = highlight ? TFT_WHITE : CHIP_GREEN;
    tft.fillRect(x-r/2, y-r/2, r, r, color);
    tft.drawRect(x-r/2, y-r/2, r, r, TFT_WHITE);
    for(int i=-1;i<=1;i++) {
      tft.drawPixel(x+i*r/3,y-r/2-1,TFT_LIGHTGREY); tft.drawPixel(x+i*r/3,y+r/2+1,TFT_LIGHTGREY);
      tft.drawPixel(x-r/2-1,y+i*r/3,TFT_LIGHTGREY); tft.drawPixel(x+r/2+1,y+i*r/3,TFT_LIGHTGREY);
    }
  };
  auto drawVirus = [&](int x, int y, int r) {
    tft.fillCircle(x, y, r-1, TFT_BLUE);
    for(int i=0;i<8;i++)
      tft.drawLine(x, y, x + (int)((r+3)*cos(i*0.785)), y + (int)((r+3)*sin(i*0.785)), TFT_LIGHTGREY);
  };
  auto drawBugWithLegs = [&](int x, int y, int r) {
    tft.fillCircle(x, y, r-2, BUG_BODY_YELLOW);
    float angleStart = M_PI * 5 / 6, angleStep = M_PI / 6;
    for(int k=0;k<3;k++) {
      float a = angleStart + k * angleStep;
      int lx0 = x - r*0.3, ly0 = y + (k-1)*2;
      int lx1 = lx0 + (int)(cos(a)*(r+5)), ly1 = ly0 + (int)(sin(a)*(r+5));
      tft.drawLine(lx0,ly0,lx1,ly1,TFT_BROWN); tft.drawPixel(lx1,ly1,TFT_BLACK);
    }
    for(int k=0;k<3;k++) {
      float a = -angleStart - k * angleStep;
      int lx0 = x + r*0.3, ly0 = y + (k-1)*2;
      int lx1 = lx0 + (int)(cos(a)*(r+5)), ly1 = ly0 + (int)(sin(a)*(r+5));
      tft.drawLine(lx0,ly0,lx1,ly1,TFT_BROWN); tft.drawPixel(lx1,ly1,TFT_BLACK);
    }
    tft.fillEllipse(x,y-r/4,r/3, r/5,TFT_BLACK);
    tft.drawPixel(x-2, y-r/4-1, TFT_WHITE); tft.drawPixel(x+2, y-r/4-1, TFT_WHITE);
  };
  auto drawProgrammer = [&](int x, int y, int r, bool dead, bool rescued) {
    if (rescued) return;
    uint16_t color = dead ? TFT_LIGHTGREY : TFT_PINK; uint16_t lcol  = dead ? TFT_DARKGREY : TFT_MAGENTA;
    tft.fillCircle(x, y - r/2, r/2, color);
    if (!dead) { tft.fillCircle(x-2, y-r/2, 1, TFT_BLACK); tft.fillCircle(x+2, y-r/2, 1, TFT_BLACK);}
    tft.drawLine(x, y - r/4, x, y + r/2, lcol);
    tft.drawLine(x - r/2 +1, y + r/6, x + r/2 -1, y + r/6, lcol);
    tft.drawLine(x, y + r/2, x - r/3, y + r, lcol); tft.drawLine(x, y + r/2, x + r/3, y + r, lcol);
  };
  auto drawWorm = [&](int x, int y, int r) {
    tft.drawCircle(x,y,r,TFT_LIGHTPURPLE);
    for(int i=0;i<7;i++) {
      int xp = x + (int)(cos(i*0.45)*r*0.75);
      int yp = y + (int)(sin(i*0.45)*r*0.75);
      tft.fillCircle(xp,yp,2,TFT_LIGHTPURPLE);
    }
  };
  auto drawAll = [&]() {
    tft.fillScreen(TFT_BLACK);
    drawHUD();
    for(int i=0;i<chipsThisWave;i++)
      if(chips[i].active && !chips[i].collected)
        drawChipIcon(chips[i].x,chips[i].y,CHIP_R,false);
    for(int h=0; h<MAX_PROGRAMMERS; h++)
      drawProgrammer(programmers[h].x, programmers[h].y, PROGRAMMER_R, !programmers[h].alive, programmers[h].rescued);
    for(int i=0;i<MAX_BUGS;i++) if(bugs[i].active) drawBugWithLegs(bugs[i].x, bugs[i].y, BUG_R);
    for(int i=0;i<MAX_WORMS;i++) if(worms[i].active) drawWorm(worms[i].x, worms[i].y, WORM_R);
    for(int i=0;i<MAX_VIRUSES;i++) if(viruses[i].active) drawVirus(viruses[i].x,viruses[i].y,VIRUS_R);
    for(int i=0;i<MAX_PLAYER_SHOTS;i++)
      if(playerShots[i].active) tft.fillCircle(playerShots[i].x, playerShots[i].y, PLAYER_SHOT_R, TFT_WHITE);
    for(int i=0;i<MAX_ENEMY_SHOTS;i++)
      if(enemyShots[i].active) tft.fillCircle(enemyShots[i].x, enemyShots[i].y, ENEMY_SHOT_R, TFT_ORANGE);
    if(player.invuln && (millis()%200>100)) ; else drawPlayerMan(player.x, player.y, PLAYER_R);
  };

  // ---- Other helpers ----
  auto firePlayerShot = [&](int dir) {
    unsigned long now = millis();
    if(now-player.lastFireDir[dir]<110) return;
    player.lastFireDir[dir]=now;
    int dx=0,dy=0;
    if(dir==0) dy=-SHOT_SPEED;
    if(dir==1) dy= SHOT_SPEED;
    if(dir==2) dx=-SHOT_SPEED;
    if(dir==3) dx= SHOT_SPEED;
    for(int i=0;i<MAX_PLAYER_SHOTS;i++)
      if(!playerShots[i].active) {
        playerShots[i].x = player.x; playerShots[i].y = player.y;
        playerShots[i].dx = dx; playerShots[i].dy = dy; playerShots[i].active = true; break;
      }
  };
  auto fireVirusAt = [&](int ex, int ey, int tx, int ty) {
    float dx = tx-ex, dy=ty-ey;
    float mag=sqrt(dx*dx+dy*dy); if(mag<2) return;
    dx *= ENEMY_SHOT_SPEED/mag; dy *= ENEMY_SHOT_SPEED/mag;
    for(int i=0;i<MAX_ENEMY_SHOTS;i++)
      if(!enemyShots[i].active) {
        enemyShots[i].x = ex; enemyShots[i].y = ey;
        enemyShots[i].dx = (int)dx; enemyShots[i].dy = (int)dy; enemyShots[i].active=true; break;
      }
  };
  auto respawnPlayerAndEnemies = [&]() {
    player.x = GAME_W/2; player.y = GAME_H/2 + 40;
    player.invuln = true; player.invuln_end = millis() + 2000;
    for (int i=0;i<MAX_PLAYER_SHOTS;i++) playerShots[i].active = false;
    for (int i=0;i<MAX_ENEMY_SHOTS;i++) enemyShots[i].active = false;
    for(int i=0;i<4;i++) player.lastFireDir[i]=0;
    int n = wave;
    int nBug   = std::min(MAX_BUGS, 6 + n*2);
    int nWorm  = std::min(MAX_WORMS, n/4+1);
    int nVirus = std::min(MAX_VIRUSES, n/2);
    for(int i=0;i<MAX_BUGS;i++) bugs[i].active=false;
    for(int i=0;i<MAX_WORMS;i++) worms[i].active=false;
    for(int i=0;i<MAX_VIRUSES;i++) viruses[i].active=false;
    for(int i=0;i<nBug;i++) {
      bugs[i].x = (i%2)?BUG_R:GAME_W-BUG_R-3;
      bugs[i].y = random(BUG_R+40, GAME_H-BUG_R);
      bugs[i].active = true;
    }
    for(int i=0;i<nWorm;i++) {
      worms[i].x = (i?GAME_W-WORM_R:WORM_R);
      worms[i].y = random(WORM_R+40, GAME_H-WORM_R-1);
      worms[i].dx = (random(0,2)*2-1);
      worms[i].dy = (random(0,2)*2-1);
      worms[i].active = true;
    }
    for(int i=0;i<nVirus;i++) {
      viruses[i].x = random(VIRUS_R,GAME_W-VIRUS_R);
      viruses[i].y = random(VIRUS_R+40,GAME_H-VIRUS_R);
      viruses[i].active = true;
    }
  };
  auto startWave = [&](int n) {
    introRetroSystemMsg();
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(CHIP_GREEN, TFT_BLACK);
    char buf[40];
    sprintf(buf,"Memory Sector %d (0x%04X)", n, n);
    tft.drawString(buf, GAME_W/2, GAME_H/2, 4);
    delay(1200);
    chipsCollected = 0;
    chipsThisWave = REQUIRED_CHIPS;
    for (int i=0;i<MAX_PLAYER_SHOTS;i++) playerShots[i].active = false;
    for (int i=0;i<MAX_ENEMY_SHOTS;i++) enemyShots[i].active = false;
    for(int i=0;i<4;i++) player.lastFireDir[i]=0;
    player.invuln = true; player.invuln_end = millis() + 2000;
    player.x = GAME_W/2; player.y = GAME_H/2 + 40;
    for (int h=0; h<MAX_PROGRAMMERS; h++) {
      programmers[h].x = random(PROGRAMMER_R,GAME_W-PROGRAMMER_R);
      programmers[h].y = random(PROGRAMMER_R+40,GAME_H-PROGRAMMER_R);
      programmers[h].alive = true; programmers[h].rescued = false;
    }
    int nBug   = std::min(MAX_BUGS, 6 + n*2);
    int nWorm  = std::min(MAX_WORMS, n/4+1);
    int nVirus = std::min(MAX_VIRUSES, n/2);
    for(int i=0;i<MAX_BUGS;i++) bugs[i].active=false;
    for(int i=0;i<MAX_WORMS;i++) worms[i].active=false;
    for(int i=0;i<MAX_VIRUSES;i++) viruses[i].active=false;
    for(int i=0;i<MAX_CHIPS;i++) chips[i].active=false, chips[i].collected=false;
    for(int i=0;i<nBug;i++) {
      bugs[i].x = (i%2)?BUG_R:GAME_W-BUG_R-3;
      bugs[i].y = random(BUG_R+40, GAME_H-BUG_R);
      bugs[i].active = true;
    }
    for(int i=0;i<nWorm;i++) {
      worms[i].x = (i?GAME_W-WORM_R:WORM_R);
      worms[i].y = random(WORM_R+40, GAME_H-WORM_R-1);
      worms[i].dx = (random(0,2)*2-1);
      worms[i].dy = (random(0,2)*2-1);
      worms[i].active = true;
    }
    for(int i=0;i<nVirus;i++) {
      viruses[i].x = random(VIRUS_R,GAME_W-VIRUS_R);
      viruses[i].y = random(VIRUS_R+40,GAME_H-VIRUS_R);
      viruses[i].active = true;
    }
    for(int i=0; i<chipsThisWave; i++) {
      int tries=0;
      do {
        chips[i].x = random(CHIP_R,GAME_W-CHIP_R);
        chips[i].y = random(CHIP_R+38,GAME_H-CHIP_R-5);
        tries++;
        if (abs(chips[i].x-GAME_W/2)<40 && abs(chips[i].y-(GAME_H/2+40))<40) continue;
        bool tooClose=false;
        for(int j=0;j<i;j++) if(abs(chips[i].x-chips[j].x)+abs(chips[i].y-chips[j].y)<40) {tooClose=true;break;}
        if(!tooClose) break;
      } while(tries<12);
      chips[i].active=true; chips[i].collected=false;
    }
  };
  auto printWaveClearedScreen = [&](int n) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    char buf[40];
    tft.setTextColor(CHIP_GREEN, TFT_BLACK);
    sprintf(buf, "Memory Sector %d:", n);
    tft.drawString(buf, GAME_W/2, GAME_H/2-18, 4);
    tft.setTextColor(TFT_GREEN,TFT_BLACK);
    tft.drawString("CLEARED", GAME_W/2, GAME_H/2+28, 6);
    delay(1300);
    tft.fillScreen(TFT_BLACK);
    delay(110);
  };

  // ------------- Title Splash ---------------
  auto fancyTitleScreen = [&]() {
    tft.fillScreen(TFT_BLACK);
    int midx = GAME_W/2, title_y = GAME_H/2-64;
    const uint16_t cycledColors[6] = {TFT_YELLOW, TFT_ORANGE, TFT_CYAN, TFT_LIGHTPURPLE, TFT_MAGENTA, TFT_GREEN};
    for(int step=0; step<23; step++) {
      for(int y=42; y<GAME_H-60; y+=13) {
        int xs = midx-(GAME_W/2-step*10), xe = midx+(GAME_W/2-step*10);
        tft.drawLine(xs, y, midx, title_y+18, TFT_LIGHTPURPLE);
        tft.drawLine(xe, y, midx, title_y+18, TFT_CYAN);
      }
      delay(11+step);
    }
    int ncols = sizeof(cycledColors)/sizeof(cycledColors[0]);
    const char* txt = "BEN-O-TRON";
    int L = strlen(txt);
    int x = midx - (L*20)/2 + 12;
    for (int cycle=0; cycle<14; cycle++) {
      for(int i=0;i<L;i++) {
        char c[2] = {txt[i],0};
        int colidx = (i + cycle) % ncols;
        tft.setTextColor(cycledColors[colidx], TFT_BLACK);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(c, x + i*20, title_y, 4);
      }
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      int row_y = title_y+44;
      drawBugWithLegs(midx-94, row_y, 15);
      drawChipIcon(midx-50, row_y, 16, false);
      drawVirus(midx, row_y,12);
      drawWorm(midx+50, row_y,10);
      drawPlayerMan(midx+94, row_y,11);
      tft.setTextColor(TFT_YELLOW,TFT_BLACK);
      tft.drawString("BUG", midx-94, row_y+19, 2);
      tft.setTextColor(CHIP_GREEN,TFT_BLACK);
      tft.drawString("CHIP", midx-50, row_y+19, 2);
      tft.setTextColor(TFT_BLUE,TFT_BLACK);
      tft.drawString("VIRUS", midx, row_y+19, 2);
      tft.setTextColor(TFT_LIGHTPURPLE,TFT_BLACK);
      tft.drawString("WORM", midx+50, row_y+19, 2);
      tft.setTextColor(TFT_CYAN,TFT_BLACK);
      tft.drawString("HERO", midx+94, row_y+19, 2);
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
      tft.drawString("Rescue Programmers. Destroy Bugs & Viruses.", midx, row_y+43, 2);
      tft.drawString("Collect 3 Microchips per sector.", midx, row_y+64, 2);
      tft.setTextColor((cycle%2)?TFT_YELLOW:TFT_ORANGE,TFT_BLACK);
      tft.drawString("PRESS ANY BUTTON TO START", midx, GAME_H-44, 4);
      delay(110+cycle*7);
    }
    tft.setTextColor(TFT_YELLOW,TFT_BLACK);
    tft.drawString("PRESS ANY BUTTON TO START", midx, GAME_H-44, 4);
    do {
      READ_INPUTS;
      if (btnA || btnB || btnX || btnY || btnSelect) break;
      delay(12);
    } while(1);
    tft.fillScreen(TFT_BLACK);
    delay(180);
    waitForAllButtonsReleased();
  };

  // ------------- MAIN GAME LOOP -------------
  randomSeed(analogRead(0)+millis());
  // hardware is already initialized/setup by menu
  fancyTitleScreen();
  player.lives = WAVE_START_LIVES; score = 0; wave = 1;
  
gameStart:
  startWave(wave);

  unsigned long frameLast = 0;
  while (!gameOver) {
    if(millis()-frameLast < 22) { delay(2); continue; }
    frameLast = millis();

    READ_INPUTS;
    if (btnSelect && !prevBtnSelect) break;
    //-- Player move stick --
    int move_dx = 0, move_dy = 0, dz=120;
    if (joyX < (512-dz)) move_dx = -PLAYER_SPEED;
    if (joyX > (512+dz)) move_dx =  PLAYER_SPEED;
    if (joyY < (512-dz)) move_dy = -PLAYER_SPEED;
    if (joyY > (512+dz)) move_dy =  PLAYER_SPEED;
    player.x = constrainVal(player.x + move_dx, PLAYER_R, GAME_W-PLAYER_R-1);
    player.y = constrainVal(player.y + move_dy, PLAYER_R+30, GAME_H-PLAYER_R-1);

    //-- Shooting (Y=Up, B=Down, X=Left, A=Right) --
    if (btnY && !prevBtnY) firePlayerShot(0);
    if (btnB && !prevBtnB) firePlayerShot(1);
    if (btnX && !prevBtnX) firePlayerShot(2);
    if (btnA && !prevBtnA) firePlayerShot(3);
    prevBtnA = btnA; prevBtnB = btnB; prevBtnX = btnX; prevBtnY = btnY;
    prevBtnSelect = btnSelect;
    if(player.invuln && millis() > player.invuln_end) player.invuln = false;
    for (int i=0;i<MAX_PLAYER_SHOTS;i++) {
      if(!playerShots[i].active) continue;
      playerShots[i].x += playerShots[i].dx;
      playerShots[i].y += playerShots[i].dy;
      if(playerShots[i].x<0 || playerShots[i].x>=GAME_W ||
         playerShots[i].y<30 || playerShots[i].y>=GAME_H)
        playerShots[i].active = false;
    }
    for (int i=0;i<MAX_ENEMY_SHOTS;i++) {
      if(!enemyShots[i].active) continue;
      enemyShots[i].x += enemyShots[i].dx;
      enemyShots[i].y += enemyShots[i].dy;
      if(enemyShots[i].x<0 || enemyShots[i].x>=GAME_W ||
         enemyShots[i].y<30 || enemyShots[i].y>=GAME_H)
        enemyShots[i].active = false;
    }
    for (int i=0;i<MAX_BUGS;i++) {
      if(!bugs[i].active) continue;
      int dx = player.x - bugs[i].x, dy = player.y - bugs[i].y;
      float mag = sqrt(dx*dx+dy*dy) + 0.1;
      bugs[i].x += (int)(dx*2.1/mag);
      bugs[i].y += (int)(dy*2.1/mag);
    }
    for (int i=0; i<chipsThisWave; i++) {
      if(!chips[i].active || chips[i].collected) continue;
      if(collides(chips[i].x, chips[i].y, CHIP_COLL_R, player.x, player.y, PLAYER_R)) {
        chips[i].collected = true;
        chipsCollected++;
        score += 500;
      }
    }
    static unsigned long worm_last = 0;
    if(millis()-worm_last>1500) {
      worm_last = millis();
      for (int w=0; w<MAX_WORMS; w++) {
        if(!worms[w].active) continue;
        int wx = worms[w].x, wy = worms[w].y;
        for (int v=0; v<MAX_VIRUSES; v++)
          if(!viruses[v].active) {
            viruses[v].x = wx+random(-5,5); viruses[v].y=wy+random(-5,5);
            viruses[v].active = true;
            break;
          }
      }
    }
    static unsigned long virus_lastshot = 0;
    for (int v=0; v<MAX_VIRUSES; v++) {
      if(!viruses[v].active) continue;
      int dx = player.x - viruses[v].x, dy = player.y - viruses[v].y;
      float mag = sqrt(dx*dx+dy*dy)+1;
      viruses[v].x += (int)(dx*1.3/mag);
      viruses[v].y += (int)(dy*1.3/mag);
      if(millis() - virus_lastshot > 530) {
        fireVirusAt(viruses[v].x, viruses[v].y, player.x, player.y);
        virus_lastshot = millis();
      }
    }
    for(int w=0; w<MAX_WORMS; w++) {
      if(!worms[w].active) continue;
      if(random(10)==0) { worms[w].dx = random(-1,2); worms[w].dy = random(-1,2);}
      worms[w].x = constrainVal(worms[w].x + worms[w].dx, WORM_R, GAME_W-WORM_R-1);
      worms[w].y = constrainVal(worms[w].y + worms[w].dy, WORM_R+40, GAME_H-WORM_R-1);
    }

    bool lostThisFrame = false;
    for(int i=0;i<MAX_ENEMY_SHOTS; i++) {
      if(!enemyShots[i].active) continue;
      if(collides(enemyShots[i].x, enemyShots[i].y, ENEMY_SHOT_R, player.x, player.y, PLAYER_R)) {
        if(!player.invuln) { loseLife(); lostThisFrame=true; break;}
      }
    }
    if(!lostThisFrame)
    for(int i=0; i<MAX_BUGS; i++) {
      if(!bugs[i].active) continue;
      if(collides(bugs[i].x, bugs[i].y, BUG_R+1, player.x, player.y, PLAYER_R)) {
        if(!player.invuln) { loseLife(); lostThisFrame=true; break;}
      }
    }
    if(!lostThisFrame)
    for(int i=0;i<MAX_VIRUSES;i++) {
      if(!viruses[i].active) continue;
      if(collides(viruses[i].x, viruses[i].y, VIRUS_R, player.x, player.y, PLAYER_R)) {
        if(!player.invuln) { loseLife(); lostThisFrame=true; break;}
      }
    }
    if(!lostThisFrame)
    for(int i=0;i<MAX_WORMS;i++) {
      if(!worms[i].active) continue;
      if(collides(worms[i].x, worms[i].y, WORM_R, player.x, player.y, PLAYER_R)) {
        if(!player.invuln) { loseLife(); lostThisFrame=true; break;}
      }
    }

    for (int i=0;i<MAX_PLAYER_SHOTS;i++) {
      if (!playerShots[i].active) continue;
      for (int j=0;j<MAX_BUGS;j++) {
        if(bugs[j].active && collides(bugs[j].x,bugs[j].y,BUG_R,playerShots[i].x,playerShots[i].y,PLAYER_SHOT_R)) {
          bugs[j].active = false; playerShots[i].active = false; score += 100;
        }
      }
      for (int j=0;j<MAX_VIRUSES;j++) {
        if(viruses[j].active && collides(viruses[j].x,viruses[j].y,VIRUS_R,playerShots[i].x,playerShots[i].y,PLAYER_SHOT_R)) {
          viruses[j].active = false; playerShots[i].active = false; score += 200;
        }
      }
      for(int j=0;j<MAX_WORMS;j++) {
        if(worms[j].active && collides(worms[j].x,worms[j].y,WORM_R,playerShots[i].x,playerShots[i].y,PLAYER_SHOT_R)) {
          worms[j].active = false; playerShots[i].active = false; score += 500;
        }
      }
    }
    for(int h=0; h<MAX_PROGRAMMERS; h++) {
      if(programmers[h].alive && !programmers[h].rescued && collides(player.x, player.y, PLAYER_R, programmers[h].x, programmers[h].y, PROGRAMMER_R)) {
        score += 1000; programmers[h].rescued = true; programmers[h].alive=false;
      }
    }
    int left = 0;
    for(int i=0; i<MAX_BUGS; i++) if(bugs[i].active) left++;
    for(int i=0; i<MAX_VIRUSES; i++) if(viruses[i].active) left++;
    for(int i=0; i<MAX_WORMS; i++) if(worms[i].active) left++;
    enemiesLeft = left;
    bool allChips = (chipsCollected>=REQUIRED_CHIPS);
    if (enemiesLeft==0 && allChips) {
      printWaveClearedScreen(wave);
      player.lives++; wave++; startWave(wave);
      continue;
    }
    drawAll();
  } // main game while
  tft.fillScreen(TFT_BLACK);
}