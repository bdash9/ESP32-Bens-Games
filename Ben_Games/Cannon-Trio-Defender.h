#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <math.h>

#define SCREEN_W      480
#define SCREEN_H      320
#define NUM_CANNONS    3
#define CANNON_BASE_W  24
#define CANNON_BASE_H  18
#define CANNON_BARREL_L   22
#define CANNON_BARREL_THK 4
#define CANNON_Y      (SCREEN_H-18)
#define CANNON_SPREAD 120
#define CANNON_CX     (SCREEN_W/2)
#define ANGLE_LEFT_LIMIT   (-M_PI/3)
#define ANGLE_RIGHT_LIMIT  (M_PI/3)
#define PROJECTILE_W   10
#define PROJECTILE_H   12
#define PROJECTILE_SPEED  14
#define BIG_PROJECTILE_W   (PROJECTILE_W*5)
#define BIG_PROJECTILE_H   (PROJECTILE_H*5)
#define FIREBALL_W    18
#define FIREBALL_SPEED 10
#define FIREBALL_SPREAD_ANGLE 0.21
#define FIREBALL_SHOTS_PER_POWERUP 3
#define ZIGZAG_SHOTS_PER_POWERUP 3
#define ZIGZAG_SPEED 12.0f
#define ZIGZAG_PERIOD 24.0f
#define ZIGZAG_AMPLITUDE 36.0f
#define ENEMY_W        20
#define ENEMY_H        20
#define ENEMY_SPAWN_INTERVAL  1200
#define MAX_ENEMIES     6
#define MAX_PROJECTILES 8
#define MAX_POWERUPS    5
#define POWERUP_W       16
#define POWERUP_H       16
#define POWERUP_SPEED   2
#define EXPLOSION_PARTICLES 8
#define BOSS_W  60
#define BOSS_H  60
#define BOSS_SPEED  1
#define BOSS_HITS   10
#define BOSS_DROPS  3
#define BIG_EXPLOSION_PARTICLES 34
#define TFT_BROWN 0x79E0
#define ENEMY_SPEED_NORM_BASE 1.0
#define ENEMY_SPEED_FAST_BASE 2.0
#define ENEMY_SPEED_INC_PER_WAVE 0.08
#define COMBO_INTERVAL 2000

enum EnemyType { ENEMY_NORMAL=0, ENEMY_FAST=1, ENEMY_BOSS=2 };
enum PowerupType { POWERUP_BOMB=0, POWERUP_BIGBULLETS=1, POWERUP_FIREBALL=2, POWERUP_ZIGZAG=3 };
enum ProjectileType { PROJECTILE_NORMAL=0, PROJECTILE_BIG, PROJECTILE_FIREBALL, PROJECTILE_ZIGZAG };

struct Projectile {
  bool active;
  float x, y;
  float dx, dy;
  uint8_t cannon;
  bool big;
  ProjectileType ptype;
  float travel_dist;
  float zigzag_angle;
  float zigzag_phase;
};
struct Enemy {
  bool active;
  int x, y;
  int hp;
  EnemyType type;
  int boss_zig_dir;
  int boss_zig_counter;
};
struct Powerup {
  bool active;
  int x, y;
  int vy;
  unsigned long expire_ts;
  PowerupType type;
};
struct ExplosionParticle {
  bool active;
  int x, y;
  float dx, dy;
  uint16_t color;
  uint8_t ttl;
};

// --------- ALL STATIC VARIABLES ---------
static Projectile ctd_projectiles[MAX_PROJECTILES];
static Enemy ctd_enemies[MAX_ENEMIES];
static Powerup ctd_powerups[MAX_POWERUPS];
static ExplosionParticle ctd_explosions[BIG_EXPLOSION_PARTICLES];
static float ctd_barrel_angle;
static int ctd_score;
static int ctd_lives;
static bool ctd_bomb_available;
static bool ctd_bomb_effect;
static uint32_t ctd_last_enemy_spawn;
static int ctd_cannon_x[NUM_CANNONS];
static int ctd_joyX, ctd_joyY;
static bool ctd_btnA, ctd_btnB, ctd_btnX, ctd_btnY, ctd_btnSelect;
static bool ctd_prevBtnA, ctd_prevBtnB, ctd_prevBtnX, ctd_prevBtnY, ctd_prevBtnSelect;
static uint32_t ctd_last_enemy_destroyed;
static int ctd_combo_count;
static float ctd_combo_multiplier;
static uint32_t ctd_combo_display_until;
static int ctd_combo_shown;
static int ctd_wave;
static int ctd_bosses_killed;
static int ctd_kills_this_wave;
static bool ctd_boss_incoming;
static bool ctd_boss_active;
static uint32_t ctd_boss_warning_until;
static int ctd_big_bullets_left;
static int ctd_fireball_shots_left;
static int ctd_zigzag_shots_left;
static float ctd_enemy_speed_normal;
static float ctd_enemy_speed_fast;
static int ctd_next_life_boss_score;
static unsigned long ctd_extra_life_message_timer;

static void ctd_updateJoywingInputs(Adafruit_seesaw &ss) {
  ctd_joyX = ss.analogRead(3);
  ctd_joyY = ss.analogRead(2);
  ctd_btnA      = !ss.digitalRead(6);
  ctd_btnB      = !ss.digitalRead(7);
  ctd_btnX      = !ss.digitalRead(9);
  ctd_btnY      = !ss.digitalRead(10);
  ctd_btnSelect = !ss.digitalRead(14);
}

static void ctd_setupCannons() {
  int start = CANNON_CX - CANNON_SPREAD;
  for (int i = 0; i < NUM_CANNONS; ++i)
    ctd_cannon_x[i] = start + i*CANNON_SPREAD;
}

static void ctd_drawCannon(TFT_eSPI &tft, int idx, float angle, bool erase) {
  int bx = ctd_cannon_x[idx];
  int bcol = erase ? TFT_BLACK : TFT_BLUE;
  int gcol = erase ? TFT_BLACK : TFT_DARKGREY;
  if (erase) 
    tft.fillRect(bx - 28, CANNON_Y-CANNON_BARREL_L-8, 56, CANNON_BARREL_L+CANNON_BASE_H+15, TFT_BLACK);
  tft.fillRect(bx - CANNON_BASE_W/2, CANNON_Y - CANNON_BASE_H/2, CANNON_BASE_W, CANNON_BASE_H, bcol);
  int bar_sx = bx, bar_sy = CANNON_Y - CANNON_BASE_H/2;
  int bar_ex = bar_sx + cos(angle)*CANNON_BARREL_L;
  int bar_ey = bar_sy + sin(angle)*CANNON_BARREL_L;
  for (int w = -CANNON_BARREL_THK/2; w <= CANNON_BARREL_THK/2; ++w)
    tft.drawLine(bar_sx+w, bar_sy, bar_ex+w, bar_ey, gcol);
  tft.fillCircle(bar_ex, bar_ey, CANNON_BARREL_THK/2, gcol);
}

static void ctd_drawAllCannons(TFT_eSPI &tft, float angle, bool erase) {
  for (int i = 0; i < NUM_CANNONS; i++)
    ctd_drawCannon(tft, i, angle, erase);
  int lineY = CANNON_Y + CANNON_BASE_H / 2;
  if (!erase)
    tft.fillRect(0, lineY, SCREEN_W, 16, TFT_BROWN);
  else
    tft.fillRect(0, lineY, SCREEN_W, 17, TFT_BLACK);
}

static void ctd_drawProjectile(TFT_eSPI &tft, Projectile &p, bool erase=false) {
  if (p.ptype == PROJECTILE_FIREBALL) {
    float size = FIREBALL_W + p.travel_dist * 0.085;
    int rad = (int)size/2;
    int col = erase ? TFT_BLACK : TFT_RED;
    int x = (int)p.x, y = (int)p.y;
    tft.fillCircle(x, y, rad, col);
    if (!erase) {
      int center_rad = max(3, rad/2);
      tft.fillCircle(x, y, center_rad, TFT_YELLOW);
      tft.drawCircle(x, y, rad, TFT_ORANGE);
    }
    return;
  }
  if (p.ptype == PROJECTILE_ZIGZAG) {
    int col = erase ? TFT_BLACK : TFT_MAGENTA;
    int x = (int)p.x, y = (int)p.y;
    tft.fillRect(x-PROJECTILE_W/2, y-PROJECTILE_H/2, PROJECTILE_W, PROJECTILE_H, col);
    if (!erase) tft.drawRect(x-PROJECTILE_W/2, y-PROJECTILE_H/2, PROJECTILE_W, PROJECTILE_H, TFT_WHITE);
    return;
  }
  int col = erase ? TFT_BLACK : (p.big ? TFT_CYAN : TFT_WHITE);
  if (p.big) {
    tft.fillRect((int)p.x-BIG_PROJECTILE_W/2, (int)p.y-BIG_PROJECTILE_H/2, BIG_PROJECTILE_W, BIG_PROJECTILE_H, col);
    tft.drawRect((int)p.x-BIG_PROJECTILE_W/2, (int)p.y-BIG_PROJECTILE_H/2, BIG_PROJECTILE_W, BIG_PROJECTILE_H, TFT_DARKCYAN);
  } else {
    tft.fillRect((int)p.x-PROJECTILE_W/2, (int)p.y-PROJECTILE_H/2, PROJECTILE_W, PROJECTILE_H, col);
  }
}

static void ctd_drawEnemy(TFT_eSPI &tft, Enemy &e, bool erase=false) {
  if (e.type == ENEMY_BOSS) {
    int col = erase ? TFT_BLACK : TFT_PURPLE;
    tft.fillRect(e.x-BOSS_W/2, e.y-BOSS_H/2, BOSS_W,BOSS_H, col);
    if (!erase) {
      tft.drawRect(e.x-BOSS_W/2, e.y-BOSS_H/2, BOSS_W,BOSS_H, TFT_WHITE);
      char hpstr[16];
      sprintf(hpstr, "HP:%d", e.hp);
      tft.setTextColor(TFT_WHITE, col);
      tft.drawString(hpstr, e.x-22, e.y-8, 2);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    return;
  }
  int col;
  if (erase) col = TFT_BLACK;
  else if (e.type == ENEMY_NORMAL) col = e.hp>1 ? TFT_ORANGE : TFT_RED;
  else if (e.type == ENEMY_FAST) col = TFT_CYAN;
  else col = TFT_RED;
  tft.fillRect(e.x-ENEMY_W/2, e.y-ENEMY_H/2, ENEMY_W, ENEMY_H, col);
  if (!erase)
    tft.drawRect(e.x-ENEMY_W/2, e.y-ENEMY_H/2, ENEMY_W, ENEMY_H, TFT_WHITE);
}

static void ctd_drawPowerup(TFT_eSPI &tft, Powerup &p, bool erase=false) {
  uint16_t col = TFT_BLACK, outcol = TFT_BLACK; char c = '?';
  if (!erase) {
    if (p.type == POWERUP_BOMB)         { col = TFT_GREEN;    outcol = TFT_WHITE;   c='B'; }
    else if (p.type == POWERUP_BIGBULLETS) { col = TFT_BLUE;  outcol = TFT_WHITE;   c='L'; }
    else if (p.type == POWERUP_FIREBALL)   { col = TFT_RED;   outcol = TFT_YELLOW;  c='F'; }
    else if (p.type == POWERUP_ZIGZAG)     { col = TFT_MAGENTA;outcol = TFT_WHITE;  c='Z'; }
  }
  tft.fillCircle(p.x, p.y, POWERUP_W/2, col);
  tft.drawCircle(p.x, p.y, POWERUP_W/2, outcol);
  if (!erase) tft.drawChar(p.x-5, p.y-7, c, TFT_WHITE, col, 2);
}

static void ctd_drawExplosionParticle(TFT_eSPI &tft, ExplosionParticle &e, bool erase=false) {
  int col = erase ? TFT_BLACK : e.color;
  tft.fillCircle(e.x, e.y, 3, col);
}

static void ctd_drawUI(TFT_eSPI &tft) {
  tft.fillRect(0,0,SCREEN_W,36,TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("SCORE:" + String(ctd_score), 4, 8, 2);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawString("LIVES:" + String(ctd_lives), 124,8,2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("WAVE:" + String(ctd_wave), 224,8,2);
  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  tft.drawString("BOSSES:" + String(ctd_bosses_killed), 314,8,2);
  if (ctd_bomb_available) {
    tft.setTextColor(TFT_ORANGE,TFT_BLACK);
    tft.drawString("BOMB READY!", 401,8,2);
  }
  if (ctd_big_bullets_left>0) {
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("BIG!",385,25,2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(String(ctd_big_bullets_left), 435,25,2);
  }
  if (ctd_fireball_shots_left>0) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("FIRE:", 230, 25, 2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(String(ctd_fireball_shots_left), 285, 25, 2);
  }
  if (ctd_zigzag_shots_left>0) {
    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.drawString("ZIG:", 310, 25, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(String(ctd_zigzag_shots_left), 355, 25, 2);
  }
  if (millis() < ctd_combo_display_until && ctd_combo_shown >= 3) {
    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.drawString("Combo x"+String(ctd_combo_shown)+"!", 230, 24, 2);
  }
  if (ctd_extra_life_message_timer && millis() < ctd_extra_life_message_timer) {
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_RED);
    tft.drawString("Extra Life!", SCREEN_W/2, 20, 4);
    tft.setTextDatum(TL_DATUM);
  }
}

static void ctd_spawnPowerup(TFT_eSPI &tft, int x, int y) {
  for (int i=0;i<MAX_POWERUPS;i++) if (!ctd_powerups[i].active) {
    ctd_powerups[i].active = true;
    ctd_powerups[i].x = x;
    ctd_powerups[i].y = y;
    ctd_powerups[i].vy = POWERUP_SPEED;
    ctd_powerups[i].expire_ts = millis() + 5000;
    int r = random(0,5);
    if (r==0)        ctd_powerups[i].type = POWERUP_BOMB;
    else if (r==1)   ctd_powerups[i].type = POWERUP_BIGBULLETS;
    else if (r==2)   ctd_powerups[i].type = POWERUP_FIREBALL;
    else if (r==3)   ctd_powerups[i].type = POWERUP_ZIGZAG;
    else             ctd_powerups[i].type = POWERUP_BIGBULLETS;
    ctd_drawPowerup(tft, ctd_powerups[i]);
    return;
  }
}

static void ctd_fireCannon(TFT_eSPI &tft, int idx, float angle) {
  if (ctd_fireball_shots_left>0) {
    for (int p0=0;p0<MAX_PROJECTILES;p0++) if (!ctd_projectiles[p0].active) {
      ctd_projectiles[p0].active = true;
      int bx = ctd_cannon_x[idx] + cos(angle)*CANNON_BARREL_L;
      int by = CANNON_Y - CANNON_BASE_H/2 + sin(angle)*CANNON_BARREL_L;
      float dx = cos(angle)*FIREBALL_SPEED;
      float dy = sin(angle)*FIREBALL_SPEED;
      ctd_projectiles[p0].x = bx;
      ctd_projectiles[p0].y = by;
      ctd_projectiles[p0].dx = dx;
      ctd_projectiles[p0].dy = dy;
      ctd_projectiles[p0].cannon = idx;
      ctd_projectiles[p0].ptype = PROJECTILE_FIREBALL;
      ctd_projectiles[p0].big = false;
      ctd_projectiles[p0].travel_dist = 0.0;
      ctd_drawProjectile(tft, ctd_projectiles[p0]);
      break;
    }
    for (int p1=0;p1<MAX_PROJECTILES;p1++) if (!ctd_projectiles[p1].active) {
      float ang = angle - FIREBALL_SPREAD_ANGLE;
      ctd_projectiles[p1].active = true;
      int bx = ctd_cannon_x[idx] + cos(ang)*CANNON_BARREL_L;
      int by = CANNON_Y - CANNON_BASE_H/2 + sin(ang)*CANNON_BARREL_L;
      float dx = cos(ang)*FIREBALL_SPEED;
      float dy = sin(ang)*FIREBALL_SPEED;
      ctd_projectiles[p1].x = bx;
      ctd_projectiles[p1].y = by;
      ctd_projectiles[p1].dx = dx;
      ctd_projectiles[p1].dy = dy;
      ctd_projectiles[p1].cannon = idx;
      ctd_projectiles[p1].ptype = PROJECTILE_FIREBALL;
      ctd_projectiles[p1].big = false;
      ctd_projectiles[p1].travel_dist = 0.0;
      ctd_drawProjectile(tft, ctd_projectiles[p1]);
      break;
    }
    for (int p2=0;p2<MAX_PROJECTILES;p2++) if (!ctd_projectiles[p2].active) {
      float ang = angle + FIREBALL_SPREAD_ANGLE;
      ctd_projectiles[p2].active = true;
      int bx = ctd_cannon_x[idx] + cos(ang)*CANNON_BARREL_L;
      int by = CANNON_Y - CANNON_BASE_H/2 + sin(ang)*CANNON_BARREL_L;
      float dx = cos(ang)*FIREBALL_SPEED;
      float dy = sin(ang)*FIREBALL_SPEED;
      ctd_projectiles[p2].x = bx;
      ctd_projectiles[p2].y = by;
      ctd_projectiles[p2].dx = dx;
      ctd_projectiles[p2].dy = dy;
      ctd_projectiles[p2].cannon = idx;
      ctd_projectiles[p2].ptype = PROJECTILE_FIREBALL;
      ctd_projectiles[p2].big = false;
      ctd_projectiles[p2].travel_dist = 0.0;
      ctd_drawProjectile(tft, ctd_projectiles[p2]);
      break;
    }
    ctd_fireball_shots_left--;
    return;
  }
  if (ctd_zigzag_shots_left>0) {
    for (int p=0;p<MAX_PROJECTILES;p++) if (!ctd_projectiles[p].active) {
      int bx = ctd_cannon_x[idx] + cos(angle)*CANNON_BARREL_L;
      int by = CANNON_Y - CANNON_BASE_H/2 + sin(angle)*CANNON_BARREL_L;
      float dx = cos(angle)*ZIGZAG_SPEED;
      float dy = sin(angle)*ZIGZAG_SPEED;
      ctd_projectiles[p].active = true;
      ctd_projectiles[p].x = bx;
      ctd_projectiles[p].y = by;
      ctd_projectiles[p].dx = dx;
      ctd_projectiles[p].dy = dy;
      ctd_projectiles[p].cannon = idx;
      ctd_projectiles[p].ptype = PROJECTILE_ZIGZAG;
      ctd_projectiles[p].big = false;
      ctd_projectiles[p].travel_dist = 0.0;
      ctd_projectiles[p].zigzag_angle = angle;
      ctd_projectiles[p].zigzag_phase = random(0, 10000);
      ctd_drawProjectile(tft, ctd_projectiles[p]);
      break;
    }
    ctd_zigzag_shots_left--;
    return;
  }
  for (int p=0;p<MAX_PROJECTILES;p++)
    if (!ctd_projectiles[p].active) {
      int bx = ctd_cannon_x[idx] + cos(angle)*CANNON_BARREL_L;
      int by = CANNON_Y - CANNON_BASE_H/2 + sin(angle)*CANNON_BARREL_L;
      float dx = cos(angle)*PROJECTILE_SPEED;
      float dy = sin(angle)*PROJECTILE_SPEED;
      ctd_projectiles[p].active = true;
      ctd_projectiles[p].x = bx;
      ctd_projectiles[p].y = by;
      ctd_projectiles[p].dx = dx;
      ctd_projectiles[p].dy = dy;
      ctd_projectiles[p].cannon = idx;
      if (ctd_big_bullets_left>0) {
        ctd_projectiles[p].big = true; ctd_big_bullets_left--;
        ctd_projectiles[p].ptype = PROJECTILE_BIG;
      } else {
        ctd_projectiles[p].big = false;
        ctd_projectiles[p].ptype = PROJECTILE_NORMAL;
      }
      ctd_projectiles[p].travel_dist = 0.0;
      ctd_drawProjectile(tft, ctd_projectiles[p]);
      break;
    }
}

static float ctd_getBarrelAngle() {
  int x = ctd_joyX-512;
  const int dead = 90;
  float norm = 0;
  if (x<-dead)      norm = (float)(x+dead)/(512-dead);
  else if (x>dead)  norm = (float)(x-dead)/(512-dead);
  else              norm = 0;
  if (norm<-1) norm = -1;
  if (norm>1) norm = 1;
  float ang = norm * ANGLE_RIGHT_LIMIT;
  return ang;
}

static void ctd_processProjectilesAndCollisions(TFT_eSPI &tft) {
  for (int p=0; p<MAX_PROJECTILES; ++p) {
    Projectile &proj = ctd_projectiles[p];
    if (!proj.active) continue;
    ctd_drawProjectile(tft, proj,true);

    if (proj.ptype == PROJECTILE_ZIGZAG) {
      proj.travel_dist += sqrt((proj.dx)*(proj.dx)+(proj.dy)*(proj.dy));
      float zigzag_dir = proj.zigzag_angle + M_PI/2;
      float flight = proj.travel_dist;
      float zigzag = ZIGZAG_AMPLITUDE * sinf( (flight+proj.zigzag_phase)/ZIGZAG_PERIOD );
      proj.x += proj.dx;
      proj.y += proj.dy;
      proj.x += cos(zigzag_dir) * zigzag;
      proj.y += sin(zigzag_dir) * zigzag;
    } else {
      proj.x += proj.dx;
      proj.y += proj.dy;
      proj.travel_dist += sqrt((proj.dx)*(proj.dx)+(proj.dy)*(proj.dy));
    }
    if (proj.x<0 || proj.x>=SCREEN_W || proj.y<32 || proj.y>=SCREEN_H) {
      proj.active = false; continue;
    }
    for (int e=0; e<MAX_ENEMIES; ++e) {
      Enemy &en = ctd_enemies[e];
      if (!en.active) continue;
      int w = (en.type==ENEMY_BOSS) ? BOSS_W : ENEMY_W;
      int h = (en.type==ENEMY_BOSS) ? BOSS_H : ENEMY_H;
      int phw = 0, phh = 0;
      if (proj.ptype == PROJECTILE_FIREBALL) {
        float size = FIREBALL_W + proj.travel_dist*0.085;
        phw = phh = int(size/2);
      } else if (proj.big)
        phw = BIG_PROJECTILE_W/2, phh = BIG_PROJECTILE_H/2;
      else
        phw = PROJECTILE_W/2, phh = PROJECTILE_H/2;
      if (abs((int)proj.x-en.x)<=(w/2+phw-4) && abs((int)proj.y-en.y)<=(h/2+phh-4)) {
        uint32_t now = millis();
        if (now - ctd_last_enemy_destroyed <= COMBO_INTERVAL) ctd_combo_count++;
        else ctd_combo_count = 1;
        ctd_last_enemy_destroyed = now;
        ctd_combo_multiplier = 1.0 + 0.5*(ctd_combo_count-1);
        if (ctd_combo_count >= 3) {
          ctd_combo_display_until = now + 1300;
          ctd_combo_shown = ctd_combo_count;
        }
        ctd_drawEnemy(tft, en, true);
        // (Optional: spawn explosion particles etc.)
        en.hp--;
        if (en.hp<=0) {
          en.active=false;
          if (en.type == ENEMY_BOSS) {
            ctd_boss_active = false;
            ctd_bosses_killed++;
            if (ctd_bosses_killed >= ctd_next_life_boss_score) {
              ctd_lives++;
              ctd_next_life_boss_score += 5;
              ctd_extra_life_message_timer = millis() + 1100;
            }
            for (int k=0; k<BOSS_DROPS; k++)
              ctd_spawnPowerup(tft, en.x-(BOSS_W/3)+k*(BOSS_W/2), en.y);
            ctd_score += 100 * ctd_combo_multiplier;
            ctd_wave++; ctd_kills_this_wave = 0;
            ctd_enemy_speed_normal += ENEMY_SPEED_INC_PER_WAVE;
            ctd_enemy_speed_fast += ENEMY_SPEED_INC_PER_WAVE;
          } else {
            ctd_spawnPowerup(tft, en.x, en.y);
            ctd_score += 10 * ctd_combo_multiplier;
            ctd_kills_this_wave++;
          }
        } else {
          ctd_drawEnemy(tft, en, false);
        }
        proj.active=false;
        ctd_drawUI(tft);
        goto nextProj;
      }
    }
    for (int i=0; i<MAX_POWERUPS; i++) {
      Powerup &pw = ctd_powerups[i];
      if (!pw.active) continue;
      int phw = (proj.ptype == PROJECTILE_FIREBALL)
          ? int((FIREBALL_W + proj.travel_dist*0.085)/2)
          : (proj.big ? BIG_PROJECTILE_W/2 : PROJECTILE_W/2);
      int phh = phw;
      if (abs((int)proj.x-pw.x)<=POWERUP_W/2+phw-3 && abs((int)proj.y-pw.y)<=POWERUP_H/2+phh-3) {
        ctd_drawPowerup(tft, pw, true);
        pw.active = false;
        if (pw.type==POWERUP_BOMB) {
          ctd_bomb_available = true;
          ctd_score += 50;
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.drawString("Bomb REFILLED!", SCREEN_W/2-60, 50, 2);
        } else if (pw.type==POWERUP_BIGBULLETS) {
          ctd_big_bullets_left = 5;
          ctd_score += 30;
          tft.setTextColor(TFT_CYAN, TFT_BLACK);
          tft.drawString("BIG BULLETS!", SCREEN_W/2-60, 50, 2);
        } else if (pw.type==POWERUP_FIREBALL) {
          ctd_fireball_shots_left = FIREBALL_SHOTS_PER_POWERUP;
          ctd_score += 35;
          tft.setTextColor(TFT_RED, TFT_BLACK);
          tft.drawString("FIREBALLS!", SCREEN_W/2-60, 50, 2);
        } else if (pw.type==POWERUP_ZIGZAG) {
          ctd_zigzag_shots_left = ZIGZAG_SHOTS_PER_POWERUP;
          ctd_score += 30;
          tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
          tft.drawString("ZIGZAG BULLETS!", SCREEN_W/2-80, 50, 2);
        }
        delay(450); ctd_drawUI(tft);
        proj.active = false;
        goto nextProj;
      }
    }
    ctd_drawProjectile(tft, proj);
    nextProj:;
  }
}
static void ctd_processEnemies(TFT_eSPI &tft) {
  for (int e=0; e<MAX_ENEMIES; ++e) {
    Enemy &en = ctd_enemies[e];
    if (!en.active) continue;
    ctd_drawEnemy(tft, en,true);
    if (en.type == ENEMY_BOSS) {
      en.y += BOSS_SPEED;
      en.boss_zig_counter += 1;
      if (en.boss_zig_counter % 10 == 0) {
        en.x += en.boss_zig_dir * 16;
        if (en.x-BOSS_W/2<0+10 || en.x+BOSS_W/2>SCREEN_W-10)
          en.boss_zig_dir *= -1;
      }
      if (en.y >= CANNON_Y - CANNON_BASE_H/2 - BOSS_H/2) {
        ctd_lives = 0;
        ctd_drawUI(tft);
        // Game Over drawing/behavior can be added here
        return;
      }
      ctd_drawEnemy(tft, en,false);
    } else {
      if (en.type == ENEMY_NORMAL) en.y += ctd_enemy_speed_normal;
      else en.y += ctd_enemy_speed_fast;
      if (en.y >= CANNON_Y - CANNON_BASE_H/2 - ENEMY_H/2) {
        for (int i=0;i<NUM_CANNONS;++i) {
          if (abs(en.x-ctd_cannon_x[i])<=CANNON_BASE_W/2+ENEMY_W/2) {
            en.active = false; ctd_lives--; ctd_drawUI(tft);
            if (ctd_lives<=0) {
              // Game Over logic here if desired
              return;
            }
          }
        }
        en.active=false;
        continue;
      }
      ctd_drawEnemy(tft, en, false);
    }
  }
}
static void ctd_processPowerups(TFT_eSPI &tft) {
  uint32_t now = millis();
  for (int i=0; i<MAX_POWERUPS; i++) {
    Powerup &pw = ctd_powerups[i];
    if (!pw.active) continue;
    ctd_drawPowerup(tft, pw, true);
    pw.y += pw.vy;
    if (pw.y > SCREEN_H - 10 || now > pw.expire_ts) {
      pw.active = false;
      continue;
    }
    ctd_drawPowerup(tft, pw, false);
  }
}

static void ctd_processExplosionParticles(TFT_eSPI &tft) {
  for (int i=0; i<BIG_EXPLOSION_PARTICLES; ++i) {
    ExplosionParticle &e = ctd_explosions[i];
    if (!e.active) continue;
    ctd_drawExplosionParticle(tft, e, true);
    e.x += e.dx; e.y += e.dy;
    e.dx *= 0.88; e.dy *= 0.88;
    if (--e.ttl <= 0) { e.active = false; continue;}
    ctd_drawExplosionParticle(tft, e, false);
  }
}
static void ctd_processBomb(TFT_eSPI &tft) {
  if (!ctd_bomb_effect) return;
  tft.fillRect(0,0,SCREEN_W,SCREEN_H-30,TFT_ORANGE);
  delay(120);
  tft.fillRect(0,0,SCREEN_W,SCREEN_H-30,TFT_BLACK);
  for (int i=0;i<MAX_ENEMIES;i++) {
    if (ctd_enemies[i].active) {
      if (ctd_enemies[i].type == ENEMY_BOSS) {
        ctd_boss_active = false;
        ctd_bosses_killed++;
        if (ctd_bosses_killed >= ctd_next_life_boss_score) {
          ctd_lives++;
          ctd_next_life_boss_score += 5;
          ctd_extra_life_message_timer = millis() + 1100;
        }
        for (int k=0; k<BOSS_DROPS; k++)
          ctd_spawnPowerup(tft, ctd_enemies[i].x-(BOSS_W/3)+k*(BOSS_W/2), ctd_enemies[i].y);
        ctd_score += 100;
        ctd_wave++;
        ctd_kills_this_wave = 0;
        ctd_enemy_speed_normal += ENEMY_SPEED_INC_PER_WAVE;
        ctd_enemy_speed_fast += ENEMY_SPEED_INC_PER_WAVE;
      }
      ctd_enemies[i].active=false;
    }
  }
  ctd_bomb_effect = false; ctd_bomb_available=false;
  ctd_drawUI(tft);
}

static void ctd_spawnEnemy(TFT_eSPI &tft) {
  if (ctd_kills_this_wave >= 10 && !ctd_boss_active && !ctd_boss_incoming) {
    ctd_boss_incoming = true;
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.fillRect(0,SCREEN_H/2-40,SCREEN_W,60,TFT_BLACK);
    tft.drawString("*** BOSS INCOMING! ***", SCREEN_W/2, SCREEN_H/2, 4);
    ctd_boss_warning_until = millis() + 2000;
    return;
  }
  if (ctd_boss_incoming && millis() < ctd_boss_warning_until) return;
  if (ctd_boss_incoming && millis() >= ctd_boss_warning_until) {
    tft.fillRect(0, SCREEN_H/2-40, SCREEN_W, 60, TFT_BLACK);
    for (int i=0;i<MAX_ENEMIES;i++) if (!ctd_enemies[i].active) {
      ctd_enemies[i].active = true;
      ctd_enemies[i].x = SCREEN_W/2;
      ctd_enemies[i].y = 40+BOSS_H/2;
      ctd_enemies[i].type = ENEMY_BOSS;
      ctd_enemies[i].hp = BOSS_HITS;
      ctd_enemies[i].boss_zig_dir = 1;
      ctd_enemies[i].boss_zig_counter = 0;
      ctd_boss_active = true;
      ctd_boss_incoming = false;
      break;
    }
    return;
  }
  if (ctd_boss_active || ctd_kills_this_wave>=10) return;
  int attempts = 0;
  while (attempts++<10) {
    int rx = random(ENEMY_W/2+12,SCREEN_W-ENEMY_W/2-10);
    bool ok=true;
    for (int i=0;i<MAX_ENEMIES;i++)
      if (ctd_enemies[i].active && ctd_enemies[i].type!=ENEMY_BOSS && abs(ctd_enemies[i].x-rx)<ENEMY_W)
        ok=false;
    if (!ok) continue;
    for (int i=0;i<MAX_ENEMIES;i++)
      if (!ctd_enemies[i].active) {
        ctd_enemies[i].active=true;
        ctd_enemies[i].x=rx;
        ctd_enemies[i].y=56+ENEMY_H/2;
        ctd_enemies[i].type = (random(0,5)==0? ENEMY_FAST : ENEMY_NORMAL);
        ctd_enemies[i].hp = ctd_enemies[i].type==ENEMY_FAST ? 1 : ((random(0,5)==0)? 2 : 1);
        ctd_drawEnemy(tft, ctd_enemies[i]);
        return;
      }
    break;
  }
}

static void ctd_setupGame(TFT_eSPI &tft) {
  ctd_setupCannons();
  ctd_barrel_angle = -M_PI/2;
  ctd_score = 0;
  ctd_lives = 3;
  ctd_bomb_available = true;
  ctd_bomb_effect = false;
  ctd_last_enemy_spawn = 0;
  ctd_next_life_boss_score = 5;
  ctd_big_bullets_left = 0;
  ctd_fireball_shots_left = 0;
  ctd_zigzag_shots_left = 0;
  ctd_enemy_speed_normal = ENEMY_SPEED_NORM_BASE;
  ctd_enemy_speed_fast = ENEMY_SPEED_FAST_BASE;
  ctd_bosses_killed = 0;
  ctd_kills_this_wave = 0;
  ctd_wave = 1;
  ctd_boss_incoming = false;
  ctd_boss_active = false;
  for(int i=0;i<BIG_EXPLOSION_PARTICLES;++i) ctd_explosions[i].active=false;
  for(int i=0;i<MAX_PROJECTILES;i++) ctd_projectiles[i].active=false;
  for(int i=0;i<MAX_ENEMIES;i++) ctd_enemies[i].active=false;
  for(int i=0;i<MAX_POWERUPS;i++) ctd_powerups[i].active=false;
}

// ========== Entry point for menu ==============

void run_CannonTrioDefender(TFT_eSPI &tft, Adafruit_seesaw &ss)
{
  ctd_setupGame(tft);
  tft.setTextSize(1); 
  // Show welcome/title
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Cannon Trio Defender!", SCREEN_W/2, 36, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Joystick: Aim all turrets", SCREEN_W/2, 78, 2);
  tft.drawString("X/Y/A: Fire Left/Mid/Right Cannon", SCREEN_W/2, 102, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Powerup Key:", SCREEN_W/2, 128, 2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("B  Bomb: One-use, clears all enemies instantly", SCREEN_W/2, 152, 2);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawString("L  Big Bullets: Next 5 shots are big damage", SCREEN_W/2, 176, 2);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("F  Fireball: Next 3 shots are red/yellow spread", SCREEN_W/2, 200, 2);
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.drawString("Z  Zigzag: Next 3 shots zig-zag for wide hit", SCREEN_W/2, 224, 2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Collect Powerups for various damage effects!", SCREEN_W/2, 256, 2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("B = Bomb, Left/Right = X/A, Center = Y", SCREEN_W/2, 276, 2);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("Press any button to start!", SCREEN_W/2, 305, 2);  
  do { ctd_updateJoywingInputs(ss); delay(9); }
  while (!ctd_btnA && !ctd_btnB && !ctd_btnX && !ctd_btnY && !ctd_btnSelect);
  do { ctd_updateJoywingInputs(ss); delay(9); }
  while (ctd_btnA || ctd_btnB || ctd_btnX || ctd_btnY || ctd_btnSelect);

  // Main game loop
  while (1) {
    ctd_updateJoywingInputs(ss);
    if (ctd_btnSelect) break;

    float angle = ctd_getBarrelAngle() - M_PI/2;
    if (fabs(angle-ctd_barrel_angle) > 0.01) {
      ctd_drawAllCannons(tft, ctd_barrel_angle, true);
      ctd_barrel_angle = angle;
      ctd_drawAllCannons(tft, ctd_barrel_angle, false);
    }
    if (ctd_btnX && !ctd_prevBtnX) ctd_fireCannon(tft, 0, ctd_barrel_angle);
    if (ctd_btnY && !ctd_prevBtnY) ctd_fireCannon(tft, 1, ctd_barrel_angle);
    if (ctd_btnA && !ctd_prevBtnA) ctd_fireCannon(tft, 2, ctd_barrel_angle);
    if (ctd_btnB && !ctd_prevBtnB) if(ctd_bomb_available&&!ctd_bomb_effect) ctd_bomb_effect=true;

    ctd_prevBtnA = ctd_btnA; ctd_prevBtnB = ctd_btnB; ctd_prevBtnX = ctd_btnX; ctd_prevBtnY = ctd_btnY; ctd_prevBtnSelect = ctd_btnSelect;

    uint32_t now = millis();
    if (!ctd_boss_incoming || now >= ctd_boss_warning_until)
      if (now-ctd_last_enemy_spawn>ENEMY_SPAWN_INTERVAL) {
        ctd_spawnEnemy(tft);
        ctd_last_enemy_spawn = now;
      }

    ctd_processProjectilesAndCollisions(tft);
    ctd_processEnemies(tft);
    ctd_processExplosionParticles(tft);
    ctd_processPowerups(tft);
    ctd_processBomb(tft);
    ctd_drawUI(tft);
    delay(18);
  }
  tft.fillScreen(TFT_BLACK);
}