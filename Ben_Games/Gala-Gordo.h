#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>



#define SCREEN_W 480
#define SCREEN_H 320
#define GORDO_SHIP_Y (SCREEN_H - 38)
#define GALA_ENEMY_ROWS 4
#define GALA_ENEMY_COLS 8
#define GALA_ENEMY_START_Y 40
#define GALA_ENEMY_SPACING_X 44
#define GALA_ENEMY_SPACING_Y 34
#define GALA_ENEMY_START_X 30
#define GALA_MAX_CARROTS 2
#define GALA_MAX_ENEMY_SHOTS 10
#define GALA_MAX_LIVES 3
#define GALA_MAX_ENEMIES (GALA_ENEMY_ROWS*GALA_ENEMY_COLS)
#define GALA_STAR_COUNT 32

struct GalaStar { int x, y; };

void gala_draw_gordo(TFT_eSPI &tft, int x, int y, int r=14, bool all_dark=false, bool small_cheeks=false) {
    uint16_t face_gray = all_dark ? TFT_DARKGREY : 0x39A7;
    tft.fillEllipse(x, y+5*r/14, r, r, TFT_WHITE);
    tft.fillEllipse(x, y-2*r/14, r-2, r-6, face_gray);
    int cheek_r = small_cheeks ? r/3 : r/2+2;
    int cheek_h = small_cheeks ? r/3 : r/2;
    tft.fillEllipse(x-8*r/14, y+8*r/14, cheek_r, cheek_h, TFT_DARKGREY);
    tft.fillEllipse(x+8*r/14, y+8*r/14, cheek_r, cheek_h, TFT_DARKGREY);
    tft.fillTriangle(x, y-12*r/14, x-8*r/14, y+10*r/14, x+8*r/14, y+10*r/14, TFT_WHITE);
    tft.fillEllipse(x-10*r/14, y-15*r/14, 4*r/14, 12*r/14, TFT_DARKGREY);
    tft.fillEllipse(x+10*r/14, y-15*r/14, 4*r/14, 12*r/14, TFT_DARKGREY);
    tft.fillEllipse(x-10*r/14, y-15*r/14, 2*r/14, 6*r/14, TFT_PINK);
    tft.fillEllipse(x+10*r/14, y-15*r/14, 2*r/14, 6*r/14, TFT_PINK);
    tft.fillCircle(x-5*r/14, y-2*r/14, 2*r/14, TFT_BLACK);
    tft.fillCircle(x+5*r/14, y-2*r/14, 2*r/14, TFT_BLACK);
    tft.fillCircle(x-5*r/14, y-2*r/14, 1*r/14, TFT_WHITE);
    tft.fillCircle(x+5*r/14, y-2*r/14, 1*r/14, TFT_WHITE);
    tft.fillTriangle(x, y+6*r/14, x-2*r/14, y+10*r/14, x+2*r/14, y+10*r/14, TFT_PINK);
}

void gala_draw_carrot_big(TFT_eSPI &tft, int x, int y, bool upsideDown=false) {
    if (!upsideDown) {
        tft.fillTriangle(x, y-40, x-10, y+10, x+10, y+10, TFT_ORANGE);
        tft.drawLine(x, y+10, x, y+30, TFT_GREEN);
        tft.drawLine(x, y+15, x-7, y+27, TFT_GREEN);
        tft.drawLine(x, y+15, x+7, y+27, TFT_GREEN);
    } else {
        tft.fillTriangle(x, y+40, x-10, y-10, x+10, y-10, TFT_ORANGE);
        tft.drawLine(x, y-10, x, y-30, TFT_GREEN);
        tft.drawLine(x, y-15, x-7, y-27, TFT_GREEN);
        tft.drawLine(x, y-15, x+7, y-27, TFT_GREEN);
    }
}

void gala_draw_carrot(TFT_eSPI &tft, int x, int y) {
    tft.fillTriangle(x, y-8, x-2, y, x+2, y, TFT_ORANGE);
    tft.drawLine(x, y, x, y+5, TFT_GREEN);
    tft.drawLine(x, y+2, x-3, y+7, TFT_GREEN);
    tft.drawLine(x, y+2, x+3, y+7, TFT_GREEN);
}

void gala_draw_enemy(TFT_eSPI &tft, int x, int y, int type) {
    uint16_t color = (type==0)?TFT_GREEN : (type==1)?TFT_RED : (type==2)?TFT_BLUE : TFT_YELLOW;
    int scale = 10;
    tft.fillTriangle(x-scale, y, x-2*scale, y-6, x-scale/2, y+scale/2, color);
    tft.fillTriangle(x+scale, y, x+2*scale, y-6, x+scale/2, y+scale/2, color);
    tft.fillEllipse(x, y, scale, 8, color);
    tft.fillRect(x-scale+2, y, 2*scale-4, 6, color);
    tft.fillEllipse(x-4, y-2, 2, 4, TFT_WHITE);
    tft.fillEllipse(x+4, y-2, 2, 4, TFT_WHITE);
    tft.fillCircle(x-4, y-2, 1, TFT_BLACK);
    tft.fillCircle(x+4, y-2, 1, TFT_BLACK);
    tft.fillTriangle(x, y+2, x-2, y+6, x+2, y+6, TFT_ORANGE);
    tft.fillTriangle(x, y+7, x-2, y+12, x+2, y+12, color);
}

void gala_draw_enemy_shot(TFT_eSPI &tft, int x, int y) {
    tft.fillRect(x-1, y-3, 3, 8, TFT_WHITE);
}

void gala_draw_boss(TFT_eSPI &tft, int x, int y) {
    uint16_t rainbow[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_BLUE, TFT_PURPLE};
    for (int i=0; i<6; i++)
        tft.fillEllipse(x, y-i*8, 32-i*4, 24-i*3, rainbow[i]);
    tft.fillTriangle(x-40, y, x-80, y-20, x-12, y+16, TFT_CYAN);
    tft.fillTriangle(x+40, y, x+80, y-20, x+12, y+16, TFT_CYAN);
    tft.fillEllipse(x-12, y-5, 7, 14, TFT_WHITE);
    tft.fillEllipse(x+12, y-5, 7, 14, TFT_WHITE);
    tft.fillCircle(x-12, y-5, 3, TFT_BLACK);
    tft.fillCircle(x+12, y-5, 3, TFT_BLACK);
    tft.fillTriangle(x, y+20, x-4, y+28, x+4, y+28, TFT_ORANGE);
}

void gala_erase_boss(TFT_eSPI &tft, int x, int y) {
    tft.fillRect(x-90, y-70, 180, 110, TFT_BLACK);
}

void gala_draw_boss_explosion(TFT_eSPI &tft, int x, int y) {
    for (int r=0; r<65; r+=12) {
        uint16_t c = (r/12==0)?TFT_RED : (r/12==1)?TFT_ORANGE : (r/12==2)?TFT_YELLOW : (r/12==3)?TFT_GREEN : (r/12==4)?TFT_BLUE : TFT_PURPLE;
        tft.drawCircle(x, y, r, c);
    }
    for (int i=0; i<22; i++) {
        int ex = x+random(-68,68), ey = y+random(-38,38);
        tft.drawLine(x, y, ex, ey, TFT_WHITE);
    }
}

void gala_gordo_splash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    gala_draw_carrot_big(tft, SCREEN_W/2 - 100, 190, true);
    gala_draw_carrot_big(tft, SCREEN_W/2 + 100, 190, true);
    gala_draw_gordo(tft, SCREEN_W/2, 190, 48, true, true);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(4);
    tft.drawString("Gala-Gordo", SCREEN_W/2, 60);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Press any button to play", SCREEN_W/2, 290);
    bool btnSel, btnAny;
    int dx, dy;
    do {
        dx=dy=0;
        btnSel = !ss.digitalRead(14);
        btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || btnSel);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        delay(10);
    } while (!btnAny);
    do {
        dx=dy=0;
        btnSel = !ss.digitalRead(14);
        btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || btnSel);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        delay(10);
    } while (btnAny);
    tft.fillScreen(TFT_BLACK);
}

struct GalaEnemy {
    int x, y;
    int prev_x, prev_y;
    int type;
    bool alive;
    bool diving;
    int dive_timer;
    int orig_x, orig_y;
    int dive_dir;
    bool capturing;
    int wave_y;
    int prev_wave_y;
    bool has_gordo;
    unsigned long wave_hold_timer;
    bool wave_retracting;
    bool waiting_with_gordo;
    bool wave_active;
};

void erase_gordo(TFT_eSPI &tft, int x, int y, int r=14) {
    tft.fillRect(x-24, y-30, 48, 54, TFT_BLACK);
}
void erase_mini_gordo(TFT_eSPI &tft, int x, int y, int r=12) {
    tft.fillRect(x-16, y-26, 32, 40, TFT_BLACK);
}
void erase_enemy(TFT_eSPI &tft, int x, int y) {
    tft.fillRect(x-22, y-18, 44, 36, TFT_BLACK);
}
void erase_carrot(TFT_eSPI &tft, int x, int y) {
    int er_y = y-8;
    if (er_y < 0) er_y = 0;
    tft.fillRect(x-6, er_y, 13, 22, TFT_BLACK);
}
void erase_wave(TFT_eSPI &tft, int x, int y, int wave_y) {
    for (int w = 0; w < 12; w++) {
        int wx = x + w*2 - 12;
        int wy = wave_y + abs(w-6)*4;
        tft.drawLine(x, y+10, wx, wy, TFT_BLACK);
    }
}

void run_GalaGordo(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    GalaStar stars[GALA_STAR_COUNT];
    for (int i = 0; i < GALA_STAR_COUNT; i++) {
        stars[i].x = random(0, SCREEN_W);
        stars[i].y = random(22, SCREEN_H-10);
    }
    int starfield_timer = 0;

    while (true) {
        gala_gordo_splash(tft, ss);

        int gordo_x = SCREEN_W/2;
        int gordo_y = GORDO_SHIP_Y;
        int gordo_prev_x = gordo_x;
        int gordo_prev_y = gordo_y;
        int gordo_lives = GALA_MAX_LIVES;
        int score = 0;
        int level = 1;
        bool has_double_gordo = false;
        int double_gordo_offset = 22;

        bool is_boss_level = false;

while (true) { // Level loop

    is_boss_level = (level % 2 == 0);

if (is_boss_level) {
    int boss_x = SCREEN_W/2, boss_y = 110, boss_prev_x = boss_x, boss_prev_y = boss_y;
    int boss_dir = 1;
    int boss_hp = 10 * (level/2);
    bool boss_alive = true;
    int boss_bomb_timer = 0;

    struct { int x, y; bool active; } carrots[GALA_MAX_CARROTS];
    for (int i=0; i<GALA_MAX_CARROTS; i++) carrots[i].active = false;
    struct { int x, y; bool active; } boss_bombs[GALA_MAX_ENEMY_SHOTS];
    for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) boss_bombs[i].active = false;

    tft.fillRect(0, 0, SCREEN_W, SCREEN_H, TFT_BLACK);

    bool quit_to_menu = false;
    bool gameover = false;
    int fire_cooldown = 0;

    while (!quit_to_menu && !gameover && boss_alive) {
        for (int i = 0; i < GALA_STAR_COUNT; i++)
            tft.drawPixel(stars[i].x, stars[i].y, TFT_WHITE);

        int joy_x = ss.analogRead(3);
        int dx = 0;
        if (joy_x < 512-100) dx = -1;
        if (joy_x > 512+100) dx = 1;
        bool btnA = !ss.digitalRead(6);
        bool btnSel = !ss.digitalRead(14);

        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }

        int old_gordo_x = gordo_x;
        int old_gordo_y = gordo_y;

        gordo_x += dx*8;
        if (gordo_x < 18) gordo_x = 18;
        if (gordo_x > SCREEN_W-18) gordo_x = SCREEN_W-18;

        for (int i=0; i<GALA_MAX_CARROTS; i++) {
            if (carrots[i].active) {
                erase_carrot(tft, carrots[i].x, carrots[i].y);
                carrots[i].y -= 3;
                if (carrots[i].y < 0) carrots[i].active = false;
            }
        }

        for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
            if (boss_bombs[i].active) {
                tft.fillRect(boss_bombs[i].x-3, boss_bombs[i].y-11, 7, 19, TFT_BLACK);
                boss_bombs[i].y += 5;
                if (boss_bombs[i].y > SCREEN_H) boss_bombs[i].active = false;
            }
        }

        if (btnA && fire_cooldown == 0) {
            for (int i=0; i<GALA_MAX_CARROTS; i++) {
                if (!carrots[i].active) {
                    if (has_double_gordo) {
                        carrots[i].x = gordo_x-12;
                        carrots[i].y = gordo_y-16;
                        carrots[i].active = true;
                        fire_cooldown = 16;
                        if (i+1<GALA_MAX_CARROTS) {
                            carrots[i+1].x = gordo_x+12;
                            carrots[i+1].y = gordo_y-16;
                            carrots[i+1].active = true;
                            fire_cooldown = 16;
                            break;
                        }
                    } else {
                        carrots[i].x = gordo_x;
                        carrots[i].y = gordo_y-16;
                        carrots[i].active = true;
                        fire_cooldown = 16;
                        break;
                    }
                }
            }
        }
        if (fire_cooldown > 0) fire_cooldown--;

        // ERASE previous positions (to prevent trails)
        if (has_double_gordo) {
            erase_gordo(tft, gordo_prev_x - double_gordo_offset, gordo_prev_y, 14);
            erase_gordo(tft, gordo_prev_x + double_gordo_offset, gordo_prev_y, 14);
        } else {
            erase_gordo(tft, gordo_prev_x, gordo_prev_y, 14);
        }
        gala_erase_boss(tft, boss_prev_x, boss_prev_y);

        boss_x += boss_dir * 8;
        if (boss_x < 90) { boss_x = 90; boss_dir = 1; }
        if (boss_x > SCREEN_W-90) { boss_x = SCREEN_W-90; boss_dir = -1; }
        boss_prev_x = boss_x;
        boss_prev_y = boss_y;

        boss_bomb_timer++;
        if (boss_bomb_timer > 18) {
            boss_bomb_timer = 0;
            for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
                if (!boss_bombs[i].active) {
                    boss_bombs[i].x = boss_x + random(-40,40);
                    boss_bombs[i].y = boss_y + 34;
                    boss_bombs[i].active = true;
                    if (random(0,2)==1) break;
                }
            }
        }

        // Draw Gordo and boss
        if (has_double_gordo) {
            gala_draw_gordo(tft, gordo_x - double_gordo_offset, gordo_y);
            gala_draw_gordo(tft, gordo_x + double_gordo_offset, gordo_y);
        } else {
            gala_draw_gordo(tft, gordo_x, gordo_y);
        }
        gala_draw_boss(tft, boss_x, boss_y);

        for (int i=0; i<GALA_MAX_CARROTS; i++)
            if (carrots[i].active) gala_draw_carrot(tft, carrots[i].x, carrots[i].y);

        for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++)
            if (boss_bombs[i].active) gala_draw_enemy_shot(tft, boss_bombs[i].x, boss_bombs[i].y);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(1);
        tft.setCursor(10, 10);
        tft.printf("Score: %d  ", score);
        tft.setCursor(SCREEN_W/2-45, 10);
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.printf("Boss HP: %d", boss_hp);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(SCREEN_W-100, 10);
        tft.printf("Lives: %d", gordo_lives);

        for (int i=0; i<GALA_MAX_CARROTS; i++) {
            if (!carrots[i].active) continue;
            if (abs(carrots[i].x - boss_x) < 48 && abs(carrots[i].y - boss_y) < 30) {
                boss_hp--;
                carrots[i].active = false;
                if (boss_hp <= 0) {
                    boss_alive = false;
                    score += 250 * (level/2);
                    break;
                }
            }
        }
        bool hit = false;
        for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
            if (boss_bombs[i].active &&
                ((has_double_gordo && (abs(boss_bombs[i].x - (gordo_x-double_gordo_offset)) < 13 ||
                                       abs(boss_bombs[i].x - (gordo_x+double_gordo_offset)) < 13) &&
                  abs(boss_bombs[i].y - gordo_y) < 16) ||
                 (!has_double_gordo && abs(boss_bombs[i].x - gordo_x) < 13 && abs(boss_bombs[i].y - gordo_y) < 16))) {
                boss_bombs[i].active = false;
                hit = true;
            }
        }
        if (hit) {
            gordo_lives--;
            for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
                if (boss_bombs[i].active) {
                    tft.fillRect(boss_bombs[i].x-3, boss_bombs[i].y-11, 7, 19, TFT_BLACK);
                    boss_bombs[i].active = false;
                }
            }
            delay(1000);
            has_double_gordo = false;
        }
        if (gordo_lives <= 0) {
            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.setTextSize(3);
            tft.drawString("Game Over", SCREEN_W/2, SCREEN_H/2-20);
            tft.setTextSize(2);
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.drawString("Score: " + String(score), SCREEN_W/2, SCREEN_H/2+20);
            delay(2000);
            return;
        }

        if (!boss_alive) {
            gala_erase_boss(tft, boss_x, boss_y);
            for (int i=0; i<9; i++) {
                gala_draw_boss_explosion(tft, boss_x, boss_y);
                delay(70);
            }
            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextSize(2);
            tft.drawString("Level " + String(level+1), SCREEN_W/2, SCREEN_H/2);
            delay(1000);
            level++;
            break;
        }

        if (starfield_timer == 0) {
            for (int i = 0; i < GALA_STAR_COUNT; i++) {
                tft.drawPixel(stars[i].x, stars[i].y, TFT_BLACK);
                stars[i].y += 2;
                if (stars[i].y > SCREEN_H-10) {
                    stars[i].x = random(0, SCREEN_W);
                    stars[i].y = 22;
                }
            }
        }
        starfield_timer = (starfield_timer+1)%4;
        gordo_prev_x = gordo_x;
        gordo_prev_y = gordo_y;
        delay(18);
    }
    continue;
}

    // ========== ORIGINAL BIRD WAVE CODE ==========
    GalaEnemy enemies[GALA_ENEMY_ROWS*GALA_ENEMY_COLS];
            int enemy_count = GALA_ENEMY_ROWS*GALA_ENEMY_COLS;
            int enemy_move_timer = 0, enemy_move_dir = 1, enemy_move_down = 0;
for (int r=0; r<GALA_ENEMY_ROWS; r++)
for (int c=0; c<GALA_ENEMY_COLS; c++) {
    int idx = r*GALA_ENEMY_COLS + c;
    enemies[idx].x = GALA_ENEMY_START_X + c*GALA_ENEMY_SPACING_X;
    enemies[idx].y = GALA_ENEMY_START_Y + r*GALA_ENEMY_SPACING_Y;
    enemies[idx].prev_x = enemies[idx].x;
    enemies[idx].prev_y = enemies[idx].y;
    enemies[idx].orig_x = enemies[idx].x;
    enemies[idx].orig_y = enemies[idx].y;
    enemies[idx].type = r%4;
    enemies[idx].alive = true;
    enemies[idx].diving = false;
    enemies[idx].dive_timer = 0;
    enemies[idx].dive_dir = 0;
    enemies[idx].capturing = false;
    enemies[idx].wave_y = 0;
    enemies[idx].prev_wave_y = 0;
    enemies[idx].has_gordo = false;
    enemies[idx].wave_hold_timer = 0;
    enemies[idx].wave_retracting = false;
    enemies[idx].waiting_with_gordo = false;
    enemies[idx].wave_active = false;
}

            struct { int x, y; bool active; } carrots[GALA_MAX_CARROTS];
            for (int i=0; i<GALA_MAX_CARROTS; i++) carrots[i].active = false;
            struct { int x, y; bool active; } enemyshots[GALA_MAX_ENEMY_SHOTS];
            for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) enemyshots[i].active = false;

            bool quit_to_menu = false;
            bool gameover = false;
            int fire_cooldown = 0;
            bool gordo_captured = false;
            int captured_bird = -1;
            static int rescued_gordo_x = -1, rescued_gordo_y = -1;
            bool rescued_gordo_falling = false;
            int rescued_gordo_top_x = -1, rescued_gordo_top_y = -1; // For erasing head after rescue

            tft.fillRect(0, 0, SCREEN_W, SCREEN_H, TFT_BLACK);

int enemy_speed = max(20 - level, 6);

            while (!quit_to_menu && !gameover) {
                for (int i = 0; i < GALA_STAR_COUNT; i++)
                    tft.drawPixel(stars[i].x, stars[i].y, TFT_WHITE);

                int joy_x = ss.analogRead(3);
                int dx = 0;
                if (joy_x < 512-100) dx = -1;
                if (joy_x > 512+100) dx = 1;
                bool btnA = !ss.digitalRead(6);
                bool btnSel = !ss.digitalRead(14);

                if (btnSel) { tft.fillScreen(TFT_BLACK); return; }

                int old_gordo_x = gordo_x;
                int old_gordo_y = gordo_y;

                gordo_x += dx*8;
                if (gordo_x < 18) gordo_x = 18;
                if (gordo_x > SCREEN_W-18) gordo_x = SCREEN_W-18;

                for (int i=0; i<GALA_MAX_CARROTS; i++) {
                    if (carrots[i].active) {
                        erase_carrot(tft, carrots[i].x, carrots[i].y);
                        carrots[i].y -= 3;
                        if (carrots[i].y < 0) carrots[i].active = false;
                    }
                }

                enemy_move_timer++;
                if (enemy_move_timer > enemy_speed) {
                    enemy_move_timer = 0;
                    int min_x = SCREEN_W, max_x = 0;
                    for (int i=0; i<enemy_count; i++) {
                        if (enemies[i].alive && !enemies[i].diving && !enemies[i].capturing && !enemies[i].waiting_with_gordo) {
                            if (enemies[i].x < min_x) min_x = enemies[i].x;
                            if (enemies[i].x > max_x) max_x = enemies[i].x;
                        }
                    }
                    if ((enemy_move_dir==1 && max_x+14 >= SCREEN_W-8) ||
                        (enemy_move_dir==-1 && min_x-14 <= 8)) {
                        enemy_move_down = 1;
                        enemy_move_dir *= -1;
                    }
                    for (int i=0; i<enemy_count; i++) {
                        if (enemies[i].alive && !enemies[i].diving && !enemies[i].capturing && !enemies[i].waiting_with_gordo) {
                            enemies[i].x += enemy_move_dir*13;
                            if (enemy_move_down) enemies[i].y += 12;
                        }
                    }
                    if (enemy_move_down) enemy_move_down = 0;
                }

                // ====== ONLY LOWEST COLOR TYPE CAN DIVE ======
                int allowed_type = -1;
                for (int t = 3; t >= 0; t--) { // yellow (3), blue (2), red (1), green (0)
                    for (int i=0; i<enemy_count; i++) {
                        if (enemies[i].alive && enemies[i].type == t) {
                            allowed_type = t;
                            break;
                        }
                    }
                    if (allowed_type != -1) break;
                }

                // Only birds of allowed_type can start or continue diving; others reset!
                for (int i=0; i<enemy_count; i++) {
                    if (enemies[i].alive) {
                        if (enemies[i].type == allowed_type) {
                            if (!enemies[i].diving && !enemies[i].capturing && !enemies[i].waiting_with_gordo &&
                                random(0, 400-level*5) == 1) {
                                enemies[i].diving = true;
                                enemies[i].dive_timer = 0;
                                enemies[i].dive_dir = (random(0,2)==0) ? -1 : 1;
                            }
                            if (enemies[i].diving) {
                                enemies[i].x += 4 * enemies[i].dive_dir;
                                enemies[i].y += 3;
                                enemies[i].dive_timer++;
                                if (enemies[i].y > SCREEN_H-50 || enemies[i].dive_timer > 32 ||
                                    enemies[i].x < 0 || enemies[i].x > SCREEN_W) {
                                    enemies[i].diving = false;
                                    enemies[i].x = enemies[i].orig_x;
                                    enemies[i].y = enemies[i].orig_y;
                                    enemies[i].dive_dir = 0;
                                }
                            }
                        } else {
                            // If this color isn't allowed, forcibly reset any dives in progress
                            if (enemies[i].diving) {
                                enemies[i].diving = false;
                                enemies[i].x = enemies[i].orig_x;
                                enemies[i].y = enemies[i].orig_y;
                                enemies[i].dive_dir = 0;
                            }
                        }
                    }
                }

                // --- Green bird capture wave (top row green birds as before) ---
                if (!gordo_captured && captured_bird == -1) {
                    for (int i=0; i<enemy_count; i++) {
                        if (enemies[i].alive && !enemies[i].diving && !enemies[i].capturing && !enemies[i].waiting_with_gordo &&
                            enemies[i].type==0 && enemies[i].orig_y==GALA_ENEMY_START_Y &&
                            random(0, 600-level*8) == 1) {
                            enemies[i].capturing = true;
                            enemies[i].wave_y = enemies[i].y+10;
                            enemies[i].prev_wave_y = enemies[i].wave_y;
                            enemies[i].wave_hold_timer = 0;
                            enemies[i].wave_retracting = false;
                            captured_bird = i;
                            break;
                        }
                    }
                }

                // --- Green bird capture wave logic (unchanged) ---
                if (captured_bird != -1 && enemies[captured_bird].capturing) {
                    erase_wave(tft, enemies[captured_bird].x, enemies[captured_bird].y, enemies[captured_bird].prev_wave_y);
                    if (!enemies[captured_bird].wave_retracting) {
                        if (enemies[captured_bird].wave_y < SCREEN_H-18) {
                            enemies[captured_bird].wave_y += 6;
                        } else {
                            if (enemies[captured_bird].wave_hold_timer == 0)
                                enemies[captured_bird].wave_hold_timer = millis() + 2000;
                            if (millis() > enemies[captured_bird].wave_hold_timer) {
                                enemies[captured_bird].wave_retracting = true;
                            }
                        }
                    } else {
                        enemies[captured_bird].wave_y -= 6;
                        if (enemies[captured_bird].wave_y <= enemies[captured_bird].y + 10) {
                            if (enemies[captured_bird].has_gordo) {
                                enemies[captured_bird].waiting_with_gordo = true;
                            }
                            enemies[captured_bird].capturing = false;
                            enemies[captured_bird].wave_retracting = false;
                            enemies[captured_bird].wave_hold_timer = 0;
                            enemies[captured_bird].wave_y = 0;
                            enemies[captured_bird].prev_wave_y = 0;
                            enemies[captured_bird].y = enemies[captured_bird].orig_y;
                            enemies[captured_bird].x = enemies[captured_bird].orig_x;
                            captured_bird = -1;
                        }
                    }
                    for (int w = 0; w < 12; w++) {
                        int wx = enemies[captured_bird].x + w*2 - 12;
                        int wy = enemies[captured_bird].wave_y + abs(w-6)*4;
                        tft.drawLine(enemies[captured_bird].x, enemies[captured_bird].y+10, wx, wy, TFT_GREEN);
                    }
                    enemies[captured_bird].prev_wave_y = enemies[captured_bird].wave_y;
                    if (enemies[captured_bird].wave_y >= GORDO_SHIP_Y-8 &&
                        abs(gordo_x - enemies[captured_bird].x) < 16 &&
                        !gordo_captured) {
                        gordo_captured = true;
                        enemies[captured_bird].has_gordo = true;
                    }
                }

                for (int i=0; i<enemy_count; i++) {
                    if (enemies[i].alive && (enemies[i].capturing || enemies[i].waiting_with_gordo) && enemies[i].has_gordo) {
                        gala_draw_gordo(tft, enemies[i].x, enemies[i].y-16, 12, true, true);
                    }
                }

                for (int i=0; i<GALA_MAX_CARROTS; i++) {
                    if (!carrots[i].active) continue;
                    for (int j=0; j<enemy_count; j++) {
                        if (enemies[j].alive && enemies[j].has_gordo && enemies[j].waiting_with_gordo &&
                            abs(carrots[i].x - enemies[j].x) < 17 &&
                            abs(carrots[i].y - enemies[j].y) < 16) {
                            erase_enemy(tft, enemies[j].x, enemies[j].y);
                            enemies[j].alive = false;
                            enemies[j].waiting_with_gordo = false;
                            carrots[i].active = false;
                            score += 100;
                            rescued_gordo_x = enemies[j].x;
                            rescued_gordo_y = enemies[j].y-16;
                            rescued_gordo_falling = true;
                        }
                    }
                }
                if (rescued_gordo_falling) {
                    erase_mini_gordo(tft, rescued_gordo_x, rescued_gordo_y, 12);
                    if (rescued_gordo_y < GORDO_SHIP_Y) {
                        gala_draw_gordo(tft, rescued_gordo_x, rescued_gordo_y, 12, true, true);
                        rescued_gordo_y += 3;
                    } else {
                        has_double_gordo = true;
                        rescued_gordo_falling = false;
                    }
                }

                if (btnA && fire_cooldown == 0) {
                    for (int i=0; i<GALA_MAX_CARROTS; i++) {
                        if (!carrots[i].active) {
                            if (has_double_gordo) {
                                carrots[i].x = gordo_x-12;
                                carrots[i].y = gordo_y-16;
                                carrots[i].active = true;
                                fire_cooldown = 16;
                                if (i+1<GALA_MAX_CARROTS) {
                                    carrots[i+1].x = gordo_x+12;
                                    carrots[i+1].y = gordo_y-16;
                                    carrots[i+1].active = true;
                                    fire_cooldown = 16;
                                    break;
                                }
                            } else {
                                carrots[i].x = gordo_x;
                                carrots[i].y = gordo_y-16;
                                carrots[i].active = true;
                                fire_cooldown = 16;
                                break;
                            }
                        }
                    }
                }
                if (fire_cooldown > 0) fire_cooldown--;

                int bomb_chance = max(44 - level*2, 18);
                for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
                    if (!enemyshots[i].active && random(0, bomb_chance) == 1) {
                        int tries = 0;
                        while (tries < 8) {
                            int idx = random(0, enemy_count);
                            if (enemies[idx].alive && !enemies[idx].diving && !enemies[idx].capturing && !enemies[idx].waiting_with_gordo) {
                                enemyshots[i].x = enemies[idx].x;
                                enemyshots[i].y = enemies[idx].y + 12;
                                enemyshots[i].active = true;
                                break;
                            }
                            tries++;
                        }
                    }
                }
                for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
                    if (enemyshots[i].active) {
                        tft.fillRect(enemyshots[i].x-3, enemyshots[i].y-11, 7, 19, TFT_BLACK);
                        enemyshots[i].y += 2;
                        if (enemyshots[i].y > SCREEN_H) enemyshots[i].active = false;
                    }
                }

                if (has_double_gordo) {
                    erase_gordo(tft, gordo_prev_x - double_gordo_offset, gordo_prev_y, 14);
                    erase_gordo(tft, gordo_prev_x + double_gordo_offset, gordo_prev_y, 14);
                } else {
                    erase_gordo(tft, gordo_prev_x, gordo_prev_y, 14);
                }

                if (has_double_gordo) {
                    gala_draw_gordo(tft, gordo_x - double_gordo_offset, gordo_y);
                    gala_draw_gordo(tft, gordo_x + double_gordo_offset, gordo_y);
                } else {
                    gala_draw_gordo(tft, gordo_x, gordo_y);
                }
                gordo_prev_x = gordo_x;
                gordo_prev_y = gordo_y;

                for (int i=0; i<GALA_MAX_CARROTS; i++)
                    if (carrots[i].active) gala_draw_carrot(tft, carrots[i].x, carrots[i].y);

                for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++)
                    if (enemyshots[i].active) gala_draw_enemy_shot(tft, enemyshots[i].x, enemyshots[i].y);

                for (int i=0; i<enemy_count; i++) {
                    if (enemies[i].alive) {
                        if (enemies[i].x != enemies[i].prev_x || enemies[i].y != enemies[i].prev_y) {
                            erase_enemy(tft, enemies[i].prev_x, enemies[i].prev_y);
                            gala_draw_enemy(tft, enemies[i].x, enemies[i].y, enemies[i].type);
                            enemies[i].prev_x = enemies[i].x;
                            enemies[i].prev_y = enemies[i].y;
                        } else if (enemies[i].prev_x == -9999) {
                            gala_draw_enemy(tft, enemies[i].x, enemies[i].y, enemies[i].type);
                            enemies[i].prev_x = enemies[i].x;
                            enemies[i].prev_y = enemies[i].y;
                        }
                    } else {
                        if (enemies[i].prev_x != -9999) {
                            erase_enemy(tft, enemies[i].prev_x, enemies[i].prev_y);
                            enemies[i].prev_x = -9999;
                            enemies[i].prev_y = -9999;
                        }
                    }
                }

                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.setTextSize(1);
                tft.setCursor(10, 10);
                tft.printf("Score: %d  ", score);
                tft.setCursor(SCREEN_W/2-35, 10);
                tft.setTextColor(TFT_CYAN, TFT_BLACK);
                tft.printf("Level: %d", level);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.setCursor(SCREEN_W-100, 10);
                tft.printf("Lives: %d", gordo_lives);

                for (int i=0; i<GALA_MAX_CARROTS; i++) {
                    if (!carrots[i].active) continue;
                    for (int j=0; j<enemy_count; j++) {
                        if (enemies[j].alive &&
                            abs(carrots[i].x - enemies[j].x) < 17 &&
                            abs(carrots[i].y - enemies[j].y) < 16) {
                            erase_enemy(tft, enemies[j].x, enemies[j].y);
                            enemies[j].alive = false;
                            carrots[i].active = false;
                            score += 50;
                        }
                    }
                }

                bool hit = false;
                for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
                    if (enemyshots[i].active &&
                        ((has_double_gordo && (abs(enemyshots[i].x - (gordo_x-double_gordo_offset)) < 13 ||
                                               abs(enemyshots[i].x - (gordo_x+double_gordo_offset)) < 13) &&
                          abs(enemyshots[i].y - gordo_y) < 16) ||
                         (!has_double_gordo && abs(enemyshots[i].x - gordo_x) < 13 && abs(enemyshots[i].y - gordo_y) < 16))) {
                        enemyshots[i].active = false;
                        hit = true;
                    }
                }
                for (int i=0; i<enemy_count; i++) {
                    if (enemies[i].alive &&
                        ((has_double_gordo && (abs(enemies[i].x - (gordo_x-double_gordo_offset)) < 15 ||
                                               abs(enemies[i].x - (gordo_x+double_gordo_offset)) < 15) &&
                          abs(enemies[i].y - gordo_y) < 18) ||
                         (!has_double_gordo && abs(enemies[i].x - gordo_x) < 15 && abs(enemies[i].y - gordo_y) < 18))) {
                        hit = true;
                    }
                }
                if (hit) {
                    gordo_lives--;
                    for (int i=0; i<GALA_MAX_ENEMY_SHOTS; i++) {
                        if (enemyshots[i].active) {
                            tft.fillRect(enemyshots[i].x-3, enemyshots[i].y-11, 7, 19, TFT_BLACK);
                            enemyshots[i].active = false;
                        }
                    }
                    delay(1000);
                    has_double_gordo = false;
                }

                bool all_dead = true;
                for (int i=0; i<enemy_count; i++)
                    if (enemies[i].alive) all_dead = false;
                if (all_dead) {
                    tft.setTextDatum(MC_DATUM);
                    tft.setTextColor(TFT_GREEN, TFT_BLACK);
                    tft.setTextSize(2);
                    tft.drawString("Level " + String(level+1), SCREEN_W/2, SCREEN_H/2);
                    delay(1000);
                    level++;
                    break;
                }

                if (gordo_lives <= 0) {
                    tft.setTextDatum(MC_DATUM);
                    tft.setTextColor(TFT_RED, TFT_BLACK);
                    tft.setTextSize(3);
                    tft.drawString("Game Over", SCREEN_W/2, SCREEN_H/2-20);
                    tft.setTextSize(2);
                    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                    tft.drawString("Score: " + String(score), SCREEN_W/2, SCREEN_H/2+20);
                    bool btnSel, btnAny;
                    int dx,dy;
                    do {
                        dx=dy=0;
                        btnSel = !ss.digitalRead(14);
                        btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || btnSel);
                        delay(10);
                    } while (!btnAny);
                    do {
                        dx=dy=0;
                        btnSel = !ss.digitalRead(14);
                        btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || btnSel);
                        delay(10);
                    } while (btnAny);
                    gameover = true;
                    break;
                }

                if (starfield_timer == 0) {
                    for (int i = 0; i < GALA_STAR_COUNT; i++) {
                        tft.drawPixel(stars[i].x, stars[i].y, TFT_BLACK);
                        stars[i].y += 2;
                        if (stars[i].y > SCREEN_H-10) {
                            stars[i].x = random(0, SCREEN_W);
                            stars[i].y = 22;
                        }
                    }
                }
                starfield_timer = (starfield_timer+1)%4;
                delay(18);
            }
            if (gameover || quit_to_menu) break;
        }
    }
}

