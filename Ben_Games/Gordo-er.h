#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

// ----------- Grid and layout constants -------------
#define GORDOER_SCREEN_W 480
#define GORDOER_SCREEN_H 320
#define GORDOER_GRID_W 13
#define GORDOER_GRID_H 12
#define GORDOER_CELL 28
#define GORDOER_GRID_W (GORDOER_SCREEN_W/GORDOER_CELL)  // 17
//#define GORDOER_X0 ((GORDOER_SCREEN_W - (GORDOER_GRID_W*GORDOER_CELL))/2)
#define GORDOER_X0 0
#define GORDOER_Y0 24

#define GORDOER_MAX_LIVES 3
#define GORDOER_GOAL_ROW 0
#define GORDOER_LOG_START_ROW 1
#define GORDOER_LOG_ROWS 4
#define GORDOER_SAFE_ROW (GORDOER_LOG_START_ROW + GORDOER_LOG_ROWS)
#define GORDOER_CAR_START_ROW (GORDOER_SAFE_ROW+1)
#define GORDOER_CAR_ROWS 4
#define GORDOER_START_ROW (GORDOER_CAR_START_ROW + GORDOER_CAR_ROWS)
#define GORDOER_GOAL_SPOTS 5

enum { GORDOER_EMPTY=0, GORDOER_CAR=1, GORDOER_LOG=2, GORDOER_SAFE=3, GORDOER_GOAL=4, GORDOER_TRUCK=5, GORDOER_CARROT=6 };

struct GordoerVehicle {
    int x, len;
    int color;
    bool truck;
};

struct GordoerLog {
    int x, len;
};

struct GordoerLane {
    int type;
    int dir;
    int speed;
    int timer;
    uint16_t color;
    GordoerVehicle vehicles[3];
    int n_vehicles;
    GordoerLog logs[2];
    int n_logs;
};

struct GordoerGameState {
    int gordo_x, gordo_y;
    int gordo_lives;
    int level;
    int score;
    bool dead;
    bool win;
    unsigned long death_time;
    GordoerLane lanes[GORDOER_GRID_H];
    bool goal_filled[GORDOER_GOAL_SPOTS];
    int carrot_lane, carrot_pos;
    unsigned long carrot_timer;
};

const int goal_x[GORDOER_GOAL_SPOTS] = {1, 4, 8, 12, 15};

TFT_eSPI *gordoer_tft = nullptr;
Adafruit_seesaw *gordoer_ss = nullptr;

// --- Drawing helpers ---

void gordoer_draw_gordo_face(TFT_eSPI &tft, int cx, int cy, int face_r, bool erase=false) {
    if (erase) {
        tft.fillRect(cx-face_r-2, cy-face_r-2, 2*face_r+4, 2*face_r+4, TFT_BLACK);
        return;
    }
    tft.fillEllipse(cx, cy+face_r/4, face_r, face_r, TFT_WHITE);
    tft.fillEllipse(cx, cy, face_r-2, face_r-4, 0x39A7);
    tft.fillEllipse(cx, cy, face_r-4, face_r-7, TFT_WHITE);
    tft.fillEllipse(cx-face_r/2, cy+face_r/7, face_r/2+2, face_r/2, TFT_DARKGREY);
    tft.fillEllipse(cx+face_r/2, cy+face_r/7, face_r/2+2, face_r/2, TFT_DARKGREY);
    tft.fillTriangle(cx, cy-face_r/2, cx-face_r/4, cy+face_r/2, cx+face_r/4, cy+face_r/2, TFT_WHITE);
    tft.fillEllipse(cx-face_r/2, cy-face_r, face_r/6, face_r/1.2, TFT_DARKGREY);
    tft.fillEllipse(cx+face_r/2, cy-face_r, face_r/6, face_r/1.2, TFT_DARKGREY);
    tft.fillEllipse(cx-face_r/2, cy-face_r, face_r/10, face_r/2, TFT_PINK);
    tft.fillEllipse(cx+face_r/2, cy-face_r, face_r/10, face_r/2, TFT_PINK);
    tft.fillCircle(cx-face_r/5, cy, face_r/7, TFT_BLACK);
    tft.fillCircle(cx+face_r/5, cy, face_r/7, TFT_BLACK);
    tft.fillCircle(cx-face_r/6, cy-face_r/10, face_r/15, TFT_WHITE);
    tft.fillCircle(cx+face_r/6, cy-face_r/10, face_r/15, TFT_WHITE);
    tft.fillTriangle(cx, cy+face_r/4, cx-face_r/15, cy+face_r/3, cx+face_r/15, cy+face_r/3, TFT_PINK);
    tft.drawLine(cx, cy+face_r/3, cx, cy+face_r/2, TFT_PINK);
    tft.drawLine(cx, cy+face_r/2, cx-face_r/12, cy+face_r/2+face_r/10, TFT_PINK);
    tft.drawLine(cx, cy+face_r/2, cx+face_r/12, cy+face_r/2+face_r/10, TFT_PINK);
}

void gordoer_draw_safe_row(TFT_eSPI &tft, int gy) {
    int py = GORDOER_Y0 + gy*GORDOER_CELL;
    tft.fillRect(0, py, GORDOER_SCREEN_W, GORDOER_CELL, TFT_DARKGREEN);
    tft.drawRect(0, py, GORDOER_SCREEN_W, GORDOER_CELL, TFT_GREEN);
}

void gordoer_draw_goal(TFT_eSPI &tft, int gx, int gy, bool filled) {
    int px = GORDOER_X0 + gx*GORDOER_CELL;
    int py = GORDOER_Y0 + gy*GORDOER_CELL;
    if (!filled) {
        tft.fillRect(px+2, py+2, GORDOER_CELL-4, GORDOER_CELL-4, TFT_BROWN); // brown burrow
        tft.drawRect(px+3, py+3, GORDOER_CELL-6, GORDOER_CELL-6, TFT_WHITE);
    }
    if (filled) {
        tft.fillRect(px+2, py+2, GORDOER_CELL-4, GORDOER_CELL-4, TFT_DARKGREEN);
        tft.drawRect(px+3, py+3, GORDOER_CELL-6, GORDOER_CELL-6, TFT_WHITE);
        gordoer_draw_gordo_face(tft, px+GORDOER_CELL/2, py+GORDOER_CELL/2, GORDOER_CELL/2-7, false);
    }
}

int gordoer_goal_idx(int gx, int gy) {
    if (gy != GORDOER_GOAL_ROW) return -1;
    // Place goals at (2, 6, 8, 10, 14) for 17 columns, evenly spaced
    int goal_x[GORDOER_GOAL_SPOTS] = {1, 4, 8, 12, 15};
    for (int i=0; i<GORDOER_GOAL_SPOTS; i++) {
        if (gx == goal_x[i]) return i;
    }
    return -1;
}

void gordoer_draw_water_row(TFT_eSPI &tft, int gy) {
    int py = GORDOER_Y0 + gy*GORDOER_CELL;
    tft.fillRect(0, py, GORDOER_SCREEN_W, GORDOER_CELL, TFT_BLUE);
}

void gordoer_draw_log(TFT_eSPI &tft, int gx, int gy, int len) {
    int px = GORDOER_X0 + gx*GORDOER_CELL;
    int py = GORDOER_Y0 + gy*GORDOER_CELL;
    int lw = GORDOER_CELL*len-6;
    int lh = GORDOER_CELL-14;
    // Only draw if any part is on screen
    if (gx + len > 0 && gx < GORDOER_GRID_W) {
        tft.fillRoundRect(px+3, py+10, lw, lh, 8, TFT_BROWN);
        tft.fillCircle(px+8, py+GORDOER_CELL-8, 4, TFT_ORANGE);
        tft.fillCircle(px+lw-8, py+GORDOER_CELL-8, 4, TFT_ORANGE);
    }
}

void gordoer_draw_vehicle(TFT_eSPI &tft, int gx, int gy, int len, uint16_t color, bool truck=false) {
    int px = GORDOER_X0 + gx*GORDOER_CELL;
    int py = GORDOER_Y0 + gy*GORDOER_CELL;
    int w = GORDOER_CELL*len-4;
    int h = GORDOER_CELL-12;
    tft.fillRect(px+2, py+10, w, h, color);
    tft.fillRect(px+6, py+6, w-12, 10, TFT_WHITE);
    tft.fillCircle(px+8, py+GORDOER_CELL-6, 4, TFT_DARKGREY);
    tft.fillCircle(px+w-8, py+GORDOER_CELL-6, 4, TFT_DARKGREY);
    if (truck) {
        tft.fillRect(px+w-16, py+10, 14, h, TFT_DARKGREY);
    }
}

void gordoer_draw_carrot(TFT_eSPI &tft, int gx, int gy) {
    int px = GORDOER_X0 + gx*GORDOER_CELL + GORDOER_CELL/2;
    int py = GORDOER_Y0 + gy*GORDOER_CELL + GORDOER_CELL/2;
    tft.fillTriangle(px, py+8, px-5, py-6, px+5, py-6, TFT_ORANGE);
    tft.drawLine(px, py-6, px, py-12, TFT_GREEN);
    tft.drawLine(px, py-10, px-3, py-13, TFT_GREEN);
    tft.drawLine(px, py-10, px+3, py-13, TFT_GREEN);
}

void gordoer_draw_hud(TFT_eSPI &tft, int lives, int level, int score) {
    tft.fillRect(0, 0, GORDOER_SCREEN_W, 24, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    String livesStr = "Lives: " + String(lives);
    String levelStr = "Level: " + String(level);
    String scoreStr = "Score: " + String(score);
    int total_w = tft.textWidth(livesStr) + tft.textWidth(levelStr) + tft.textWidth(scoreStr) + 100;
    int x = (GORDOER_SCREEN_W - total_w)/2;
    tft.setCursor(x, 4);
    tft.print(livesStr);
    tft.setCursor(x + tft.textWidth(livesStr) + 50, 4);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print(levelStr);
    tft.setCursor(x + tft.textWidth(livesStr) + tft.textWidth(levelStr) + 100, 4);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.print(scoreStr);
}

// --- Erase Gordo by redrawing the underlying tile ---
void gordoer_redraw_tile_under_gordo(GordoerGameState &gs, int gx, int gy) {
    if (gy == GORDOER_START_ROW || gy == GORDOER_SAFE_ROW) {
        gordoer_draw_safe_row(*gordoer_tft, gy);
    } else if (gy == GORDOER_GOAL_ROW) {
        int idx = gordoer_goal_idx(gx, gy);
        gordoer_draw_goal(*gordoer_tft, gx, gy, (idx>=0) ? gs.goal_filled[idx] : false);
    } else if (gy >= GORDOER_LOG_START_ROW && gy < GORDOER_LOG_START_ROW+GORDOER_LOG_ROWS) {
        gordoer_draw_water_row(*gordoer_tft, gy);
        for (int i=0; i<gs.lanes[gy].n_logs; i++)
            gordoer_draw_log(*gordoer_tft, gs.lanes[gy].logs[i].x, gy, gs.lanes[gy].logs[i].len);
    } else if (gy >= GORDOER_CAR_START_ROW && gy < GORDOER_CAR_START_ROW+GORDOER_CAR_ROWS) {
        gordoer_tft->fillRect(0, GORDOER_Y0 + gy*GORDOER_CELL, GORDOER_SCREEN_W, GORDOER_CELL, TFT_BLACK);
        for (int i=0; i<gs.lanes[gy].n_vehicles; i++)
            gordoer_draw_vehicle(*gordoer_tft, gs.lanes[gy].vehicles[i].x, gy, gs.lanes[gy].vehicles[i].len, gs.lanes[gy].vehicles[i].color, gs.lanes[gy].vehicles[i].truck);
    }
}

// --- Draw Gordo with new erase logic
void gordoer_draw_gordo(TFT_eSPI &tft, GordoerGameState &gs, int gx, int gy, bool erase=false) {
    int px = GORDOER_X0 + gx*GORDOER_CELL + GORDOER_CELL/2;
    int py = GORDOER_Y0 + gy*GORDOER_CELL + GORDOER_CELL/2;
    if (erase) {
        gordoer_redraw_tile_under_gordo(gs, gx, gy);
        return;
    }
    gordoer_draw_gordo_face(tft, px, py, GORDOER_CELL/2-3, false);
}

// --- Check if Gordo is on a log (handles wrap-around and logs at edges) ---
bool gordoer_on_log(GordoerGameState &gs, int gx, int gy) {
    if (gy < GORDOER_LOG_START_ROW || gy >= GORDOER_LOG_START_ROW + GORDOER_LOG_ROWS) return false;
    for (int i = 0; i < gs.lanes[gy].n_logs; i++) {
        int lx = gs.lanes[gy].logs[i].x;
        int llen = gs.lanes[gy].logs[i].len;
        // Safe buffer: extend log by 1 on each side
        for (int l = -1; l < llen+1; l++) {
            int log_cell = lx + l;
            if (log_cell == gx) return true;
        }
    }
    return false;
}

bool gordoer_in_water(GordoerGameState &gs, int gx, int gy) {
    // Now: true if not on any log
    return !gordoer_on_log(gs, gx, gy);
}

// --- Grid drawing ---
void gordoer_draw_grid(GordoerGameState &gs) {
    TFT_eSPI &tft = *gordoer_tft;
    for (int i=0; i<GORDOER_GOAL_SPOTS; i++) {
        gordoer_draw_goal(tft, goal_x[i], GORDOER_GOAL_ROW, gs.goal_filled[i]);
    }
    for (int x=0; x<GORDOER_GRID_W; x++) {
        bool isGoal = false;
        for(int i=0; i<GORDOER_GOAL_SPOTS; i++)
            if (x == goal_x[i]) isGoal = true;
        if (!isGoal)
            tft.fillRect(GORDOER_X0 + x*GORDOER_CELL, GORDOER_Y0 + GORDOER_GOAL_ROW*GORDOER_CELL, GORDOER_CELL, GORDOER_CELL, TFT_BLACK);
    }
    for (int y=GORDOER_LOG_START_ROW; y<GORDOER_LOG_START_ROW+GORDOER_LOG_ROWS; y++) {
        gordoer_draw_water_row(tft, y);
        for (int i=0; i<gs.lanes[y].n_logs; i++) {
            gordoer_draw_log(tft, gs.lanes[y].logs[i].x, y, gs.lanes[y].logs[i].len);
        }
        if (gs.carrot_lane == y && gs.carrot_pos >= 0)
            gordoer_draw_carrot(tft, gs.carrot_pos, y);
    }
    gordoer_draw_safe_row(tft, GORDOER_SAFE_ROW);
    for (int y=GORDOER_CAR_START_ROW; y<GORDOER_CAR_START_ROW+GORDOER_CAR_ROWS; y++) {
        tft.fillRect(0, GORDOER_Y0 + y*GORDOER_CELL, GORDOER_SCREEN_W, GORDOER_CELL, TFT_BLACK);
        for (int i=0; i<gs.lanes[y].n_vehicles; i++) {
            gordoer_draw_vehicle(tft, gs.lanes[y].vehicles[i].x, y, gs.lanes[y].vehicles[i].len, gs.lanes[y].vehicles[i].color, gs.lanes[y].vehicles[i].truck);
        }
    }
    gordoer_draw_safe_row(tft, GORDOER_START_ROW);
}

void gordoer_reset_lanes(GordoerGameState &gs, int level) {
    for (int i=0; i<GORDOER_GOAL_SPOTS; i++) gs.goal_filled[i] = false;
    for (int y=0; y<GORDOER_GRID_H; y++) {
        gs.lanes[y].type = GORDOER_EMPTY;
        gs.lanes[y].dir = 0;
        gs.lanes[y].speed = 0;
        gs.lanes[y].timer = 0;
        gs.lanes[y].color = TFT_BLACK;
        gs.lanes[y].n_vehicles = 0;
        gs.lanes[y].n_logs = 0;
    }
    for (int y=GORDOER_LOG_START_ROW; y<GORDOER_LOG_START_ROW+GORDOER_LOG_ROWS; y++) {
        gs.lanes[y].type = GORDOER_LOG;
        gs.lanes[y].dir = ((y%2)==0) ? 1 : -1;
        gs.lanes[y].speed = 4+random(1,3) - (level>1?level/2:0); // FASTER logs each level
        if (gs.lanes[y].speed < 2) gs.lanes[y].speed = 2;
        gs.lanes[y].n_logs = 1 + random(0,2);
        for (int i=0; i<gs.lanes[y].n_logs; i++) {
            int len = 2 + random(0,2);
            int x = random(0, GORDOER_GRID_W-len);
            gs.lanes[y].logs[i] = {x, len};
        }
    }
    gs.lanes[GORDOER_SAFE_ROW].type = GORDOER_SAFE;
    uint16_t car_colors[4] = {TFT_RED, TFT_YELLOW, TFT_BLUE, TFT_ORANGE};
    for (int y=GORDOER_CAR_START_ROW; y<GORDOER_CAR_START_ROW+GORDOER_CAR_ROWS; y++) {
        gs.lanes[y].type = GORDOER_CAR;
        gs.lanes[y].dir = ((y%2)==0) ? 1 : -1;
        gs.lanes[y].speed = 3+random(1,3) - (level>1?level/2:0); // FASTER cars/trucks
        if (gs.lanes[y].speed < 1) gs.lanes[y].speed = 1;
        gs.lanes[y].n_vehicles = 1 + random(1,2);
        for (int i=0; i<gs.lanes[y].n_vehicles; i++) {
            bool truck = (random(0,3)==0);
            int len = truck ? random(2,4) : 1;
            int x = random(0, GORDOER_GRID_W-len);
            gs.lanes[y].vehicles[i] = {x, len, car_colors[y-GORDOER_CAR_START_ROW], truck};
        }
    }
    gs.carrot_lane = GORDOER_LOG_START_ROW + random(0, GORDOER_LOG_ROWS);
    gs.carrot_pos = random(0, GORDOER_GRID_W);
    gs.carrot_timer = millis() + 9000;
}

void gordoer_advance_lanes(GordoerGameState &gs) {
    for (int y=GORDOER_LOG_START_ROW; y<GORDOER_LOG_START_ROW+GORDOER_LOG_ROWS; y++) {
        gs.lanes[y].timer++;
        if (gs.lanes[y].timer >= gs.lanes[y].speed) {
            gs.lanes[y].timer = 0;
            for (int i=0; i<gs.lanes[y].n_logs; i++) {
                gs.lanes[y].logs[i].x += gs.lanes[y].dir;
                if (gs.lanes[y].logs[i].x < -gs.lanes[y].logs[i].len) gs.lanes[y].logs[i].x = GORDOER_GRID_W-1;
                if (gs.lanes[y].logs[i].x > GORDOER_GRID_W-1) gs.lanes[y].logs[i].x = -gs.lanes[y].logs[i].len;
            }
        }
    }
    for (int y=GORDOER_CAR_START_ROW; y<GORDOER_CAR_START_ROW+GORDOER_CAR_ROWS; y++) {
        gs.lanes[y].timer++;
        if (gs.lanes[y].timer >= gs.lanes[y].speed) {
            gs.lanes[y].timer = 0;
            for (int i=0; i<gs.lanes[y].n_vehicles; i++) {
                gs.lanes[y].vehicles[i].x += gs.lanes[y].dir;
                if (gs.lanes[y].vehicles[i].x < -gs.lanes[y].vehicles[i].len) gs.lanes[y].vehicles[i].x = GORDOER_GRID_W-1;
                if (gs.lanes[y].vehicles[i].x > GORDOER_GRID_W-1) gs.lanes[y].vehicles[i].x = -gs.lanes[y].vehicles[i].len;
            }
        }
    }
}

void gordoer_splash_screen(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    int px = GORDOER_SCREEN_W/2, py = 120, r = 68;
    gordoer_draw_gordo_face(tft, px, py, r, false);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(4);
    tft.drawString("Gord-ogger", px, py+r+32);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Press a button to play", px, 290);
    bool btnSel, btnAny;
    int dx,dy;
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

void run_Gordoer(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    gordoer_tft = &tft;
    gordoer_ss = &ss;

    while (true) {
        gordoer_splash_screen(tft, ss);
        GordoerGameState gs;
        gs.gordo_lives = GORDOER_MAX_LIVES;
        gs.level = 1;
        gs.score = 0;
        bool quit_to_menu = false;

        while (!quit_to_menu) {
            gs.gordo_x = GORDOER_GRID_W/2;
            gs.gordo_y = GORDOER_START_ROW;
            gs.dead = false;
            gs.win = false;
            gordoer_reset_lanes(gs, gs.level);

            tft.fillScreen(TFT_BLACK);
            gordoer_draw_hud(tft, gs.gordo_lives, gs.level, gs.score);
            gordoer_draw_grid(gs);
            gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, false);

            unsigned long frame = 0;
            int last_log_timer[GORDOER_LOG_ROWS] = {0,0,0,0};
            while (!gs.dead && !gs.win) {
                int dx=0, dy=0;
                bool btnSel = !ss.digitalRead(14);
                bool btnA = !ss.digitalRead(6);
                int joy_x = ss.analogRead(3);
                int joy_y = ss.analogRead(2);
                int dz = 120;
                if (joy_x < 512-dz) dx = -1;
                if (joy_x > 512+dz) dx = 1;
                if (joy_y < 512-dz) dy = -1;
                if (joy_y > 512+dz) dy = 1;
                if (btnA) dy = -1;

                if (btnSel) { tft.fillScreen(TFT_BLACK); return; }

                int next_x = gs.gordo_x + dx;
                int next_y = gs.gordo_y + dy;
                if ((dx != 0 || dy != 0) &&
                    next_x >= 0 && next_x < GORDOER_GRID_W &&
                    next_y >= 0 && next_y < GORDOER_GRID_H) {
                    gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, true);
                    gs.gordo_x = next_x;
                    gs.gordo_y = next_y;
                    gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, false);
                    delay(80);
                }

                // Animate lanes
                if (frame % 3 == 0) {
                    gordoer_advance_lanes(gs);
                    gordoer_draw_grid(gs);
                    gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, false);
                }
                // Move with log if on log and log just advanced
if (gs.gordo_y >= GORDOER_LOG_START_ROW && gs.gordo_y < GORDOER_LOG_START_ROW+GORDOER_LOG_ROWS) {
    int log_lane = gs.gordo_y-GORDOER_LOG_START_ROW;
    if (gs.lanes[gs.gordo_y].timer == 0 && last_log_timer[log_lane] != 0) {
        if (gordoer_on_log(gs, gs.gordo_x, gs.gordo_y)) {
            int newx = gs.gordo_x + gs.lanes[gs.gordo_y].dir;
            // Only move if newx is still on screen
            if (newx >= 0 && newx < GORDOER_GRID_W) {
                gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, true);
                gs.gordo_x = newx;
                gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, false);
            } else {
                // Gordo would fall off the edge, so he dies
                gs.dead = true;
            }
        }
        // After moving with the log, check if still on log
        if (!gordoer_on_log(gs, gs.gordo_x, gs.gordo_y)) {
            gs.dead = true;
        }
    }
    last_log_timer[log_lane] = gs.lanes[gs.gordo_y].timer;
}
                // Carrot treat
                if (gs.gordo_y == gs.carrot_lane && gs.gordo_x == gs.carrot_pos) {
                    gs.score += 50;
                    gs.carrot_pos = -1;
                }
                // Carrot respawn
                if (millis() > gs.carrot_timer) {
                    gs.carrot_lane = GORDOER_LOG_START_ROW + random(0, GORDOER_LOG_ROWS);
                    gs.carrot_pos = random(0, GORDOER_GRID_W);
                    gs.carrot_timer = millis() + 9000;
                }
                // Win: reach empty goal
                int goal_idx = gordoer_goal_idx(gs.gordo_x, gs.gordo_y);
                if (goal_idx >= 0 && !gs.goal_filled[goal_idx]) {
                    gs.goal_filled[goal_idx] = true;
                    gs.score += 100 * gs.level;
                    gordoer_draw_grid(gs);
                    gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, false);
                    tft.setTextDatum(MC_DATUM);
                    tft.setTextColor(TFT_GREEN, TFT_BLACK);
                    tft.setTextSize(3);
                    tft.drawString("Nice!", GORDOER_SCREEN_W/2, GORDOER_SCREEN_H/2);
                    delay(900);
                    gs.gordo_x = GORDOER_GRID_W/2;
                    gs.gordo_y = GORDOER_START_ROW;
                    gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, false);
                }
                // Win level: all goals filled
                bool all_filled = true;
                for (int i=0;i<GORDOER_GOAL_SPOTS;i++) if (!gs.goal_filled[i]) all_filled=false;
                if (all_filled) {
                    gs.win = true;
                    gordoer_draw_hud(tft, gs.gordo_lives, gs.level, gs.score);
                    tft.setTextDatum(MC_DATUM);
                    tft.setTextColor(TFT_GREEN, TFT_BLACK);
                    tft.setTextSize(3);
                    tft.drawString("Great!", GORDOER_SCREEN_W/2, GORDOER_SCREEN_H/2-24);
                    tft.setTextSize(2);
                    tft.drawString("Level " + String(gs.level+1), GORDOER_SCREEN_W/2, GORDOER_SCREEN_H/2+16);
                    delay(1200);
                    gs.level++;
                    break;
                }
                // If Gordo is in the goal row but NOT in a goal, he dies!
if (gs.gordo_y == GORDOER_GOAL_ROW) {
    int goal_idx2 = gordoer_goal_idx(gs.gordo_x, gs.gordo_y);
    if (goal_idx2 < 0) {
        gs.dead = true; // Gordo is not in a goal box, so ouch!
    }
}
                // Death: car, water, log out of bounds
                if (gs.gordo_y >= GORDOER_CAR_START_ROW && gs.gordo_y < GORDOER_CAR_START_ROW+GORDOER_CAR_ROWS) {
                    for (int i=0; i<gs.lanes[gs.gordo_y].n_vehicles; i++) {
                        int vx = gs.lanes[gs.gordo_y].vehicles[i].x;
                        int vlen = gs.lanes[gs.gordo_y].vehicles[i].len;
                        if (gs.gordo_x >= vx && gs.gordo_x < vx+vlen)
                            gs.dead = true;
                    }
                }
if (gs.gordo_y >= GORDOER_LOG_START_ROW && gs.gordo_y < GORDOER_LOG_START_ROW + GORDOER_LOG_ROWS) {
    if (gordoer_in_water(gs, gs.gordo_x, gs.gordo_y)) gs.dead = true;
}                if (gs.dead) {
                    gordoer_draw_gordo(tft, gs, gs.gordo_x, gs.gordo_y, true);
                    tft.setTextDatum(MC_DATUM);
                    tft.setTextColor(TFT_RED, TFT_BLACK);
                    tft.setTextSize(3);
                    tft.drawString("Ouch!", GORDOER_SCREEN_W/2, GORDOER_SCREEN_H/2);
                    gs.gordo_lives--;
                    gordoer_draw_hud(tft, gs.gordo_lives, gs.level, gs.score);
                    delay(1200);
                    if (gs.gordo_lives == 0) {
                        tft.fillScreen(TFT_BLACK);
                        tft.setTextDatum(MC_DATUM);
                        tft.setTextColor(TFT_RED, TFT_BLACK);
                        tft.setTextSize(3);
                        tft.drawString("Game Over", GORDOER_SCREEN_W/2, GORDOER_SCREEN_H/2);
                        delay(1200);
                        quit_to_menu = true;
                    }
                    break;
                }
                gordoer_draw_hud(tft, gs.gordo_lives, gs.level, gs.score);
                frame++;
                delay(35);
            }
        }
    }
}