#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

#define SPOOKY_MAZE_W 15
#define SPOOKY_MAZE_H 11
#define SPOOKY_TILE_SIZE 28
#define SPOOKY_MAZE_X0 ((480 - (SPOOKY_MAZE_W*SPOOKY_TILE_SIZE))/2)
#define SPOOKY_MAZE_Y0 ((320 - (SPOOKY_MAZE_H*SPOOKY_TILE_SIZE))/2)
#define SPOOKY_MAX_GHOSTS 8

enum { SPOOKY_TILE_EMPTY=0, SPOOKY_TILE_WALL=1, SPOOKY_TILE_COIN=2, SPOOKY_TILE_TREASURE=3 };

struct SpookyGhost {
    int x, y;
    int start_x, start_y;
    int dir;
    unsigned long last_move = 0;
};

int spooky_maze[SPOOKY_MAZE_H][SPOOKY_MAZE_W];
int spooky_player_x, spooky_player_y;
int spooky_coins_left;
int spooky_ghosts_count;
SpookyGhost spooky_ghosts[SPOOKY_MAX_GHOSTS];
int spooky_lives;
int spooky_level;
int spooky_treasure_x = -1, spooky_treasure_y = -1;
uint16_t spooky_col_wall = TFT_BLUE;
unsigned long spooky_invincible_until = 0;

uint16_t wall_colors[] = {
    TFT_BLUE, TFT_PURPLE, TFT_GREEN, TFT_ORANGE, TFT_NAVY, TFT_BROWN, TFT_RED, TFT_CYAN
};
const int n_wall_colors = sizeof(wall_colors)/sizeof(wall_colors[0]);

uint16_t spooky_col_coin = TFT_YELLOW;
uint16_t spooky_col_player = TFT_GREEN;
uint16_t spooky_col_ghost = TFT_RED;
uint16_t spooky_col_treasure = TFT_GOLD;

void spooky_draw_tile(TFT_eSPI &tft, int x, int y, int tile) {
    int px = SPOOKY_MAZE_X0 + x*SPOOKY_TILE_SIZE;
    int py = SPOOKY_MAZE_Y0 + y*SPOOKY_TILE_SIZE;
    if (tile == SPOOKY_TILE_WALL) {
        tft.fillRect(px, py, SPOOKY_TILE_SIZE, SPOOKY_TILE_SIZE, spooky_col_wall);
        tft.drawRect(px, py, SPOOKY_TILE_SIZE, SPOOKY_TILE_SIZE, TFT_WHITE);
    } else if (tile == SPOOKY_TILE_COIN) {
        tft.fillRect(px, py, SPOOKY_TILE_SIZE, SPOOKY_TILE_SIZE, TFT_BLACK);
        tft.fillCircle(px+SPOOKY_TILE_SIZE/2, py+SPOOKY_TILE_SIZE/2, SPOOKY_TILE_SIZE/5, spooky_col_coin);
    } else if (tile == SPOOKY_TILE_TREASURE) {
        tft.fillRect(px, py, SPOOKY_TILE_SIZE, SPOOKY_TILE_SIZE, TFT_BLACK);
        int cx = px+SPOOKY_TILE_SIZE/2, cy = py+SPOOKY_TILE_SIZE/2;
        tft.fillTriangle(cx, cy-7, cx-6, cy, cx+6, cy, TFT_GOLD);
        tft.fillTriangle(cx, cy+7, cx-6, cy, cx+6, cy, TFT_GOLD);
        tft.fillTriangle(cx-7, cy, cx, cy-6, cx, cy+6, TFT_GOLD);
        tft.fillTriangle(cx+7, cy, cx, cy-6, cx, cy+6, TFT_GOLD);
        tft.drawRect(px+4, py+4, SPOOKY_TILE_SIZE-8, SPOOKY_TILE_SIZE-8, TFT_YELLOW);
    } else {
        tft.fillRect(px, py, SPOOKY_TILE_SIZE, SPOOKY_TILE_SIZE, TFT_BLACK);
    }
}

void spooky_draw_player(TFT_eSPI &tft, int x, int y, bool erase=false, bool invincible=false) {
    int px = SPOOKY_MAZE_X0 + x*SPOOKY_TILE_SIZE + SPOOKY_TILE_SIZE/2;
    int py = SPOOKY_MAZE_Y0 + y*SPOOKY_TILE_SIZE + SPOOKY_TILE_SIZE/2;
    if (erase) {
        spooky_draw_tile(tft, x, y, spooky_maze[y][x]);
        return;
    }
    if (invincible) {
        tft.fillRect(px-SPOOKY_TILE_SIZE/2+3, py-SPOOKY_TILE_SIZE/2+3, SPOOKY_TILE_SIZE-6, SPOOKY_TILE_SIZE-6, TFT_BLACK);
        tft.drawCircle(px, py, SPOOKY_TILE_SIZE/2-3, TFT_GREEN);
    } else {
        tft.fillCircle(px, py, SPOOKY_TILE_SIZE/2-3, spooky_col_player);
    }
    // Scared face
    tft.fillCircle(px-5, py-4, 2, TFT_WHITE);
    tft.fillCircle(px+5, py-4, 2, TFT_WHITE);
    tft.fillCircle(px-5, py-4, 1, TFT_BLACK);
    tft.fillCircle(px+5, py-4, 1, TFT_BLACK);
    tft.fillEllipse(px, py+7, 5, 3, TFT_BLACK);
}

void spooky_draw_ghost(TFT_eSPI &tft, int x, int y, bool erase=false) {
    int px = SPOOKY_MAZE_X0 + x*SPOOKY_TILE_SIZE + SPOOKY_TILE_SIZE/2;
    int py = SPOOKY_MAZE_Y0 + y*SPOOKY_TILE_SIZE + SPOOKY_TILE_SIZE/2;
    if (erase) {
        spooky_draw_tile(tft, x, y, spooky_maze[y][x]);
        return;
    }
    tft.fillCircle(px, py, SPOOKY_TILE_SIZE/2-3, spooky_col_ghost);
    tft.fillCircle(px-5, py-4, 2, TFT_WHITE);
    tft.fillCircle(px+5, py-4, 2, TFT_WHITE);
    tft.fillCircle(px-5, py-4, 1, TFT_BLACK);
    tft.fillCircle(px+5, py-4, 1, TFT_BLACK);
    // Fangs (just below face, not at bottom)
    int fang_base_y = py + 5; // About under the mouth
    tft.fillTriangle(px-6, fang_base_y, px-3, fang_base_y, px-4, fang_base_y+5, TFT_WHITE); // left fang
    tft.fillTriangle(px+6, fang_base_y, px+3, fang_base_y, px+4, fang_base_y+5, TFT_WHITE); // right fang
}

void spooky_draw_maze(TFT_eSPI &tft) {
    for(int y=0;y<SPOOKY_MAZE_H;y++)
        for(int x=0;x<SPOOKY_MAZE_W;x++)
            spooky_draw_tile(tft, x, y, spooky_maze[y][x]);
}

void spooky_generate_maze(int level) {
    int simple[SPOOKY_MAZE_H][SPOOKY_MAZE_W] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,2,0,0,0,1,0,0,0,1,2,0,0,2,1},
        {1,0,1,1,0,1,0,1,0,1,0,1,1,0,1},
        {1,0,1,2,0,0,0,1,0,0,0,2,1,0,1},
        {1,0,1,1,0,1,0,0,0,1,1,1,1,0,1},
        {1,0,0,0,0,1,1,1,0,0,0,0,0,0,1},
        {1,0,1,1,0,0,2,0,0,1,1,1,1,0,1},
        {1,2,1,2,0,1,1,1,0,2,1,2,1,2,1},
        {1,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
        {1,2,0,1,1,1,0,1,0,1,1,1,0,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    spooky_coins_left = 0;
    for(int y=0;y<SPOOKY_MAZE_H;y++)
        for(int x=0;x<SPOOKY_MAZE_W;x++) {
            spooky_maze[y][x] = simple[y][x];
            if (spooky_maze[y][x]==SPOOKY_TILE_COIN) spooky_coins_left++;
        }
    spooky_player_x = 1; spooky_player_y = 1;
    spooky_col_wall = wall_colors[level % n_wall_colors];
spooky_ghosts_count = 2 + (level-1);
if (spooky_ghosts_count > 3) spooky_ghosts_count = 3;
    for (int i=0; i<spooky_ghosts_count; ++i) {
        int gx, gy;
        if (i == 0) { gx = SPOOKY_MAZE_W-2; gy = 1; }
        else if (i == 1) { gx = SPOOKY_MAZE_W-2; gy = SPOOKY_MAZE_H-2; }
        else if (i == 2) { gx = SPOOKY_MAZE_W/2; gy = SPOOKY_MAZE_H/2; }
        else { gx = 1+(i%2)*((SPOOKY_MAZE_W-3)); gy = 1+(i/2)*((SPOOKY_MAZE_H-3)/(SPOOKY_MAX_GHOSTS-1)); }
        spooky_ghosts[i] = {gx, gy, gx, gy, (i%4)};
    }
    spooky_lives = 3;

    // Place treasures: 1 on level 1-2, 2 for level >=3
    int treasures_to_place = (level < 3) ? 1 : 2;
    int placed = 0;
    while (placed < treasures_to_place) {
        int tx = 1 + random(SPOOKY_MAZE_W-2);
        int ty = 1 + random(SPOOKY_MAZE_H-2);
        // Don't place on wall, don't overlap, not at player start
        if ((spooky_maze[ty][tx] == SPOOKY_TILE_EMPTY || spooky_maze[ty][tx] == SPOOKY_TILE_COIN)
            && !(tx==1 && ty==1) && spooky_maze[ty][tx] != SPOOKY_TILE_TREASURE) {
            if (spooky_maze[ty][tx] == SPOOKY_TILE_COIN) {
                spooky_coins_left--;  // Remove a coin if replaced
            }
            spooky_maze[ty][tx] = SPOOKY_TILE_TREASURE;
            placed++;
            spooky_treasure_x = tx;
            spooky_treasure_y = ty;
        }
    }
    spooky_invincible_until = 0;
}

void spooky_move_ghosts(TFT_eSPI &tft, int player_x, int player_y, int *prev_gx, int *prev_gy) {
    unsigned long now = millis();
    for(int g=0; g<spooky_ghosts_count; ++g) {
        SpookyGhost &gh = spooky_ghosts[g];
        prev_gx[g] = gh.x;
        prev_gy[g] = gh.y;
        if (now - gh.last_move < 280) continue;
        int nx = gh.x, ny = gh.y;
        int dx[4] = {0,1,0,-1}, dy[4] = {-1,0,1,0};
        int try_dir = gh.dir;
        if (random(0, 3) == 0) {
            if (abs(player_x-gh.x) > abs(player_y-gh.y))
                try_dir = (player_x > gh.x) ? 1 : 3;
            else
                try_dir = (player_y > gh.y) ? 2 : 0;
        }
        for(int tries=0; tries<4; ++tries) {
            int tx = gh.x + dx[try_dir], ty = gh.y + dy[try_dir];
            if (spooky_maze[ty][tx] != SPOOKY_TILE_WALL) { nx = tx; ny = ty; break; }
            try_dir = (try_dir+1)%4;
        }
        gh.x = nx; gh.y = ny; gh.dir = try_dir;
        gh.last_move = now;
    }
}

void spooky_poll_input(Adafruit_seesaw &ss, int &dx, int &dy, bool &btnSelect, bool &btnAny) {
    int joy_x = ss.analogRead(3);
    int joy_y = ss.analogRead(2);
    dx = dy = 0;
    int dz = 120;
    if (joy_x < 512-dz) dx = -1;
    if (joy_x > 512+dz) dx = 1;
    if (joy_y < 512-dz) dy = -1;
    if (joy_y > 512+dz) dy = 1;
    btnSelect = !ss.digitalRead(14);
    btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || !ss.digitalRead(14));
}

void spooky_draw_hud(TFT_eSPI &tft, int lives, int coins_left, int level, bool invincible, unsigned long invincible_left_ms) {
    tft.fillRect(0, 0, 480, 32, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    int lives_x = SPOOKY_MAZE_X0 + 4;
    int lives_y = 4;
    tft.setCursor(lives_x, lives_y);
    tft.print("Lives: ");
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print(lives);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    int coins_x = SPOOKY_MAZE_X0 + SPOOKY_MAZE_W*SPOOKY_TILE_SIZE - 110;
    int coins_y = 4;
    tft.setCursor(coins_x, coins_y);
    tft.print("Coins: ");
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print(coins_left);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(SPOOKY_MAZE_X0+SPOOKY_MAZE_W*SPOOKY_TILE_SIZE/2-30, 4);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.print("Level: ");
    tft.print(level);
    if (invincible) {
        tft.setTextColor(TFT_GOLD, TFT_BLACK);
        tft.setCursor(SPOOKY_MAZE_X0 + SPOOKY_MAZE_W*SPOOKY_TILE_SIZE/2-50, 14); // raised up a tickle
        tft.print("INVINCIBLE: ");
        tft.print(invincible_left_ms/1000+1);
        tft.print("s");
    }
}

// --- Splash screen: solid green face, ghosts with fangs ---
void spooky_splash_screen(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    int y_center = 100;
    int px = 240;
    int py = y_center;
    int rad = 44;
    // Player (filled green)
    tft.fillCircle(px, py, rad, TFT_GREEN);
    // Scared face
    tft.fillCircle(px-10, py-12, 5, TFT_WHITE);
    tft.fillCircle(px+10, py-12, 5, TFT_WHITE);
    tft.fillCircle(px-10, py-12, 2, TFT_BLACK);
    tft.fillCircle(px+10, py-12, 2, TFT_BLACK);
    tft.fillEllipse(px, py+18, 14, 8, TFT_BLACK);

    // Ghosts (big, left/right, with fangs)
    for (int dir = -1; dir <= 1; dir += 2) {
        int gx = px + dir*90;
        tft.fillCircle(gx, py, rad, TFT_RED);
        tft.fillCircle(gx-13, py-13, 6, TFT_WHITE);
        tft.fillCircle(gx+13, py-13, 6, TFT_WHITE);
        tft.fillCircle(gx-13, py-13, 3, TFT_BLACK);
        tft.fillCircle(gx+13, py-13, 3, TFT_BLACK);
        // Fangs
        int fang_y = py + 18;
        tft.fillTriangle(gx-13, fang_y, gx-9, fang_y, gx-11, fang_y+10, TFT_WHITE); // left fang
        tft.fillTriangle(gx+13, fang_y, gx+9, fang_y, gx+11, fang_y+10, TFT_WHITE); // right fang
    }

    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(4);
    tft.drawString("Spooky Maze", 240, y_center+rad+32);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Press a button to play", 240, 260);

    // Wait for button
    bool btnSel, btnAny;
    int dx,dy;
    do {
        spooky_poll_input(ss, dx, dy, btnSel, btnAny);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        delay(10);
    } while (!btnAny);
    // Wait for release
    do {
        spooky_poll_input(ss, dx, dy, btnSel, btnAny);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        delay(10);
    } while (btnAny);
    tft.fillScreen(TFT_BLACK);
}

// --- Reset ghosts to their start positions ---
void spooky_reset_ghosts(TFT_eSPI &tft) {
    for(int g=0;g<spooky_ghosts_count;++g) {
        spooky_draw_ghost(tft, spooky_ghosts[g].x, spooky_ghosts[g].y, true);
        spooky_ghosts[g].x = spooky_ghosts[g].start_x;
        spooky_ghosts[g].y = spooky_ghosts[g].start_y;
        spooky_ghosts[g].last_move = 0;
        spooky_draw_ghost(tft, spooky_ghosts[g].x, spooky_ghosts[g].y, false);
    }
}

void spooky_respawn_player(TFT_eSPI &tft) {
    spooky_draw_player(tft, spooky_player_x, spooky_player_y, true);
    spooky_player_x = 1;
    spooky_player_y = 1;
    spooky_draw_player(tft, spooky_player_x, spooky_player_y, false, false);
    spooky_reset_ghosts(tft);
}

void run_SpookyMaze(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    while (true) {
        spooky_splash_screen(tft, ss);
        spooky_level = 1;
        while (true) {
            spooky_generate_maze(spooky_level);
            tft.fillScreen(TFT_BLACK);
            spooky_draw_maze(tft);
            spooky_draw_player(tft, spooky_player_x, spooky_player_y, false, false);
            for(int g=0;g<spooky_ghosts_count;++g) spooky_draw_ghost(tft, spooky_ghosts[g].x, spooky_ghosts[g].y, false);
            spooky_draw_hud(tft, spooky_lives, spooky_coins_left, spooky_level, false, 0);

            bool gameover = false, win = false;
            unsigned long last_move = 0;
            int prev_px = spooky_player_x;
            int prev_py = spooky_player_y;
            int prev_gx[SPOOKY_MAX_GHOSTS], prev_gy[SPOOKY_MAX_GHOSTS];

            while (!gameover && !win) {
                int dx=0, dy=0;
                bool btnSelect = false, btnAny = false;
                spooky_poll_input(ss, dx, dy, btnSelect, btnAny);
                if (btnSelect) { tft.fillScreen(TFT_BLACK); return; } // <-- RETURN to menu

                unsigned long now = millis();
                bool invincible = (now < spooky_invincible_until);
                prev_px = spooky_player_x;
                prev_py = spooky_player_y;

                if ((dx != 0 || dy != 0) && (now-last_move > 110)) {
                    int nx = spooky_player_x + dx, ny = spooky_player_y + dy;
                    if (nx>=0 && nx<SPOOKY_MAZE_W && ny>=0 && ny<SPOOKY_MAZE_H && spooky_maze[ny][nx] != SPOOKY_TILE_WALL) {
                        spooky_player_x = nx; spooky_player_y = ny;
                        if (spooky_maze[spooky_player_y][spooky_player_x] == SPOOKY_TILE_COIN) {
                            spooky_maze[spooky_player_y][spooky_player_x] = SPOOKY_TILE_EMPTY;
                            spooky_coins_left--;
                        }
                        if (spooky_maze[spooky_player_y][spooky_player_x] == SPOOKY_TILE_TREASURE) {
                            spooky_maze[spooky_player_y][spooky_player_x] = SPOOKY_TILE_EMPTY;
                            spooky_invincible_until = millis() + 5000;
                        }
                        last_move = now;
                    }
                }

                spooky_move_ghosts(tft, spooky_player_x, spooky_player_y, prev_gx, prev_gy);

                spooky_draw_player(tft, prev_px, prev_py, true, false);
                for(int g=0;g<spooky_ghosts_count;++g)
                    spooky_draw_ghost(tft, prev_gx[g], prev_gy[g], true);

                spooky_draw_player(tft, spooky_player_x, spooky_player_y, false, invincible);
                for(int g=0;g<spooky_ghosts_count;++g)
                    spooky_draw_ghost(tft, spooky_ghosts[g].x, spooky_ghosts[g].y, false);

                bool hit = false;
                for(int g=0;g<spooky_ghosts_count;++g) {
                    if (spooky_ghosts[g].x == spooky_player_x && spooky_ghosts[g].y == spooky_player_y)
                        hit = true;
                }
                if (hit && !invincible) {
                    spooky_lives--;
                    spooky_draw_hud(tft, spooky_lives, spooky_coins_left, spooky_level, invincible, spooky_invincible_until > now ? (spooky_invincible_until-now) : 0);
                    if (spooky_lives <= 0) {
                        gameover = true;
                    } else {
                        for(int i=0;i<2;i++) {
                            spooky_draw_player(tft, spooky_player_x, spooky_player_y, true, false);
                            delay(120);
                            spooky_draw_player(tft, spooky_player_x, spooky_player_y, false, invincible);
                            delay(120);
                        }
                        spooky_respawn_player(tft);
                    }
                }

                spooky_draw_hud(tft, spooky_lives, spooky_coins_left, spooky_level, invincible, spooky_invincible_until > now ? (spooky_invincible_until-now) : 0);

                if (spooky_coins_left == 0) win = true;
                delay(18);
            }

            tft.fillScreen(TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.setTextSize(3);
            if (gameover)
                tft.setTextColor(TFT_RED, TFT_BLACK), tft.drawString("GAME OVER", tft.width()/2, tft.height()/2);
            else
                tft.setTextColor(TFT_GREEN, TFT_BLACK), tft.drawString("LEVEL UP!", tft.width()/2, tft.height()/2);
            delay(1200);
            if (gameover) break;
            else spooky_level++;
        }
    }
}