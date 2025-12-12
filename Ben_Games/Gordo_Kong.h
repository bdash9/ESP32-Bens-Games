#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

#define GK_SCREEN_W 480
#define GK_SCREEN_H 320

// Girders (top to bottom)
const int GK_GIRDER_Y[4] = { 70, 130, 190, 250 };
const int GK_GIRDER_X0[4] = { 30, 60, 30, 60 };
const int GK_GIRDER_X1[4] = { 450, 420, 450, 420 };

// Ladders: x, y0 (top), y1 (bottom)
const int GK_LADDERS[][3] = {
    {80, 70, 130}, {240, 70, 130},
    {120, 130, 190}, {280, 130, 190},
    {60, 190, 250}, {220, 190, 250}, {380, 190, 250}
};
const int NUM_LADDERS = sizeof(GK_LADDERS)/sizeof(GK_LADDERS[0]);

// Girder color palette
const uint16_t GK_GIRDER_COLORS[] = {
    TFT_NAVY,
    TFT_RED,
    TFT_DARKGREEN,
    TFT_PURPLE,
    TFT_ORANGE,
    TFT_CYAN,
    TFT_BROWN,
    TFT_MAGENTA
};
const int NUM_GIRDER_COLORS = sizeof(GK_GIRDER_COLORS)/sizeof(GK_GIRDER_COLORS[0]);

struct GKBarrel {
    int x, y, dir, girder, timer;
    bool alive;
    bool dropping;
    int drop_ladder_x;
    int drop_y_start, drop_y_end;
    int drop_frame;
    bool face_right;
    bool scored_jump;
    bool is_small_fox; // true for the moving fox on the 3rd girder
};

// --- Drawing functions (in order) ---

void gordokong_draw_girder(TFT_eSPI &tft, int x, int y, int w, int h, uint16_t girder_color = TFT_NAVY) {
    tft.fillRect(x, y, w, h, girder_color);
    for (int i = 0; i < w-20; i += 32)
        tft.fillEllipse(x+12+i, y+h/2, 8, 8, TFT_PURPLE);
}

void gordokong_draw_ladder(TFT_eSPI &tft, int x, int y0, int y1) {
    int ytop = min(y0, y1), ybot = max(y0, y1);
    tft.drawLine(x-5, ytop, x-5, ybot, TFT_BROWN);
    tft.drawLine(x+5, ytop, x+5, ybot, TFT_BROWN);
    for (int y = ytop; y <= ybot; y += 12)
        tft.drawLine(x-5, y, x+5, y, TFT_BROWN);
}

// Small carrot hammer, point down, no trail
void gordokong_draw_carrot_hammer(TFT_eSPI &tft, int x, int y) {
    tft.fillRect(x-10, y-25, 20, 35, TFT_BLACK); // erase region
    tft.fillTriangle(x, y+12, x-7, y-8, x+7, y-8, TFT_ORANGE);
    tft.drawLine(x, y-8, x, y-18, TFT_GREEN);
    tft.drawLine(x, y-8, x-4, y-14, TFT_GREEN);
    tft.drawLine(x, y-8, x+4, y-14, TFT_GREEN);
}

// --- Mother Fox at top left (smaller, upright)
void gordokong_draw_mother_fox(TFT_eSPI &tft, int x, int y) {
    float scale = 0.75; // 50% smaller than before
    // Tail (white tip)
    tft.fillEllipse(x-int(10*scale), y+int(36*scale), int(12*scale), int(8*scale), 0xFC40); // orange
    tft.fillEllipse(x-int(18*scale), y+int(40*scale), int(6*scale), int(4*scale), TFT_WHITE);
    // Hind legs (black)
    tft.fillRect(x-int(12*scale), y+int(42*scale), int(6*scale), int(13*scale), TFT_BLACK);
    tft.fillRect(x+int(7*scale), y+int(43*scale), int(6*scale), int(13*scale), TFT_BLACK);
    // Body
    tft.fillEllipse(x, y+int(32*scale), int(19*scale), int(19*scale), 0xFC40); // body
    // Chest (white)
    tft.fillEllipse(x+int(5*scale), y+int(24*scale), int(13*scale), int(11*scale), TFT_WHITE);
    // Head (orange)
    tft.fillEllipse(x+int(8*scale), y+int(6*scale), int(19*scale), int(17*scale), 0xFC40);
    // Cheeks (white)
    tft.fillEllipse(x+int(22*scale), y+int(15*scale), int(9*scale), int(7*scale), TFT_WHITE);
    tft.fillEllipse(x-2, y+int(15*scale), int(7*scale), int(7*scale), TFT_WHITE);
    // Eyes (black)
    tft.fillCircle(x+int(19*scale), y+int(11*scale), int(3*scale), TFT_BLACK);
    tft.fillCircle(x+int(7*scale), y+int(11*scale), int(3*scale), TFT_BLACK);
    // Nose (black)
    tft.fillEllipse(x+int(13*scale), y+int(20*scale), int(2*scale), int(2*scale), TFT_BLACK);
    // Smile
    tft.drawLine(x+int(13*scale), y+int(22*scale), x+int(17*scale), y+int(24*scale), TFT_BLACK);
    tft.drawLine(x+int(13*scale), y+int(22*scale), x+int(9*scale), y+int(24*scale), TFT_BLACK);
    // Ears (orange triangles, black tips)
    tft.fillTriangle(x+int(3*scale), y-int(8*scale), x-int(7*scale), y-int(25*scale), x+int(12*scale), y+int(4*scale), 0xFC40);
    tft.fillTriangle(x+int(23*scale), y-int(8*scale), x+int(33*scale), y-int(25*scale), x+int(14*scale), y+int(4*scale), 0xFC40);
    tft.fillTriangle(x-int(7*scale), y-int(25*scale), x+int(4*scale), y-int(1*scale), x+int(3*scale), y-int(8*scale), TFT_BLACK);
    tft.fillTriangle(x+int(33*scale), y-int(25*scale), x+int(22*scale), y-int(1*scale), x+int(23*scale), y-int(8*scale), TFT_BLACK);
}

// Baby fox as barrel or moving fox
void gordokong_draw_baby_fox(TFT_eSPI &tft, int x, int y, bool right, bool dropping=false, int drop_frame=0) {
    int dir = right ? 1 : -1;
    int offset = dropping ? ((drop_frame%2)*2-1)*2 : 0;
    tft.fillEllipse(x-dir*7+offset, y+6, 4, 2, TFT_WHITE);
    tft.fillEllipse(x-dir*5+offset, y+5, 5, 2, 0xFC40);
    tft.fillEllipse(x+offset, y+3, 8, 5, 0xFC40);
    tft.fillEllipse(x+dir*7+offset, y-2, 5, 4, 0xFC40);
    tft.fillEllipse(x+dir*8+offset, y, 2, 2, TFT_WHITE);
    tft.fillEllipse(x+dir*11+offset, y-1, 1, 1, TFT_BLACK);
    tft.fillTriangle(x+dir*6+offset, y-5, x+dir*4+offset, y-10, x+dir*8+offset, y-3, 0xFC40);
    tft.fillTriangle(x+dir*8+offset, y-5, x+dir*10+offset, y-10, x+dir*9+offset, y-3, 0xFC40);
    tft.fillEllipse(x+dir*9+offset, y-3, 1, 1, TFT_BLACK);
}

// Gordo with gray cheeks (for splash only)
void gordokong_draw_gordo_side_graycheeks(TFT_eSPI &tft, int x, int y, bool facing_right, bool with_hammer, float scale=1.0, bool hop=false, bool tight_box=false) {
    int dir = facing_right ? 1 : -1;
    int hop_offset = hop ? -14 : 0;
    int back_foot_x = x+dir*int(8*scale);
    int front_foot_x = x-dir*int(8*scale);
    int foot_y = y+int(7*scale)+hop_offset;
    int erase_w, erase_h, erase_yoff;
if (tight_box) {
    erase_w = int(12*scale); // narrower
    erase_h = int(40*scale); // <--- was 62, now shorter for splash
    erase_yoff = int(29*scale); // <--- was 49, now starts lower
} else {
    erase_w = int(24*scale);
    erase_h = int(62*scale);
    erase_yoff = int(49*scale);
}
tft.fillRect(x-erase_w, y-erase_yoff, erase_w*2, erase_h, TFT_BLACK);

    tft.fillEllipse(x-dir*int(5*scale), y+hop_offset, int(8*scale), int(12*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(3*scale), y+hop_offset, int(9*scale), int(12*scale), TFT_WHITE);
    tft.fillEllipse(back_foot_x, foot_y, int(6*scale), int(6*scale), TFT_DARKGREY);
    tft.fillEllipse(back_foot_x, y+int(13*scale)+hop_offset, int(3*scale), int(1.2*scale), TFT_DARKGREY);
    tft.fillEllipse(front_foot_x, foot_y, int(3*scale), int(3*scale), TFT_DARKGREY);
    tft.fillCircle(back_foot_x, foot_y+int(6*scale)-2, int(2.0*scale), TFT_WHITE);
    tft.fillCircle(front_foot_x, foot_y+int(3*scale)-2, int(1.5*scale), TFT_WHITE);
    tft.fillEllipse(x+dir*int(5*scale), y+int(2*scale)+hop_offset, int(3.2*scale), int(2.7*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(11*scale), y+int(2*scale)+hop_offset, int(3.2*scale), int(2.7*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(8*scale), y-int(4*scale)+hop_offset, int(6*scale), int(7*scale), TFT_WHITE);
    tft.fillEllipse(x+dir*int(12*scale), y-int(13*scale)+hop_offset, int(3*scale), int(8*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(5*scale), y-int(13*scale)+hop_offset, int(3*scale), int(8*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(12*scale), y-int(19*scale)+hop_offset, int(3*scale), int(7*scale), TFT_DARKGREY); // left ear tip
    tft.fillEllipse(x+dir*int(5*scale), y-int(19*scale)+hop_offset, int(3*scale), int(7*scale), TFT_DARKGREY); // right ear tip
    tft.fillCircle(x+dir*int(7*scale), y-int(3*scale)+hop_offset, int(1.3*scale), TFT_BLACK);
    tft.fillCircle(x+dir*int(10*scale), y-int(3*scale)+hop_offset, int(1.3*scale), TFT_BLACK);
    int nose_cx = x+dir*int(8.5*scale);
    int nose_cy = y+int(0*scale)+hop_offset;
    tft.fillTriangle(nose_cx-2, nose_cy, nose_cx+2, nose_cy, nose_cx, nose_cy+4, TFT_PINK);
    tft.drawLine(x+dir*int(11*scale), y+int(4*scale)+hop_offset, x+dir*int(14*scale), y+int(5*scale)+hop_offset, TFT_DARKGREY);
    tft.drawLine(x+dir*int(11*scale), y+int(3*scale)+hop_offset, x+dir*int(15*scale), y+hop_offset, TFT_DARKGREY);
    if (with_hammer) gordokong_draw_carrot_hammer(tft, x-dir*int(15*scale), y-int(10*scale)+hop_offset);
}

void gordokong_draw_gordo_side(TFT_eSPI &tft, int x, int y, bool facing_right, bool with_hammer, float scale=1.0, bool hop=false) {
    int dir = facing_right ? 1 : -1;
    int hop_offset = hop ? -14 : 0;
    int back_foot_x = x+dir*int(8*scale);
    int front_foot_x = x-dir*int(8*scale);
    int foot_y = y+int(7*scale)+hop_offset;
    int erase_w = int(24*scale); // narrower than previous 36*scale
    int erase_h = int(62*scale);
    int erase_yoff = int(49*scale);
    tft.fillRect(x-erase_w, y-erase_yoff, erase_w*2, erase_h, TFT_BLACK);

    tft.fillEllipse(x-dir*int(5*scale), y+hop_offset, int(8*scale), int(12*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(3*scale), y+hop_offset, int(9*scale), int(12*scale), TFT_WHITE);
    tft.fillEllipse(back_foot_x, foot_y, int(6*scale), int(6*scale), TFT_DARKGREY);
    tft.fillEllipse(back_foot_x, y+int(13*scale)+hop_offset, int(3*scale), int(1.2*scale), TFT_DARKGREY);
    tft.fillEllipse(front_foot_x, foot_y, int(3*scale), int(3*scale), TFT_DARKGREY);
    tft.fillCircle(back_foot_x, foot_y+int(6*scale)-2, int(2.0*scale), TFT_WHITE);
    tft.fillCircle(front_foot_x, foot_y+int(3*scale)-2, int(1.5*scale), TFT_WHITE);
    tft.fillEllipse(x+dir*int(8*scale), y-int(4*scale)+hop_offset, int(6*scale), int(7*scale), TFT_WHITE);
    tft.fillEllipse(x+dir*int(12*scale), y-int(13*scale)+hop_offset, int(3*scale), int(8*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(5*scale), y-int(13*scale)+hop_offset, int(3*scale), int(8*scale), TFT_DARKGREY);
    tft.fillEllipse(x+dir*int(12*scale), y-int(19*scale)+hop_offset, int(3*scale), int(7*scale), TFT_DARKGREY); // left ear tip
    tft.fillEllipse(x+dir*int(5*scale), y-int(19*scale)+hop_offset, int(3*scale), int(7*scale), TFT_DARKGREY); // right ear tip
    tft.fillEllipse(x+dir*int(12*scale), y-int(13*scale)+hop_offset, int(1*scale), int(4*scale), TFT_PINK);
    tft.fillEllipse(x+dir*int(5*scale), y-int(13*scale)+hop_offset, int(1*scale), int(4*scale), TFT_PINK);
    tft.fillCircle(x+dir*int(7*scale), y-int(3*scale)+hop_offset, int(1.3*scale), TFT_BLACK);
    tft.fillCircle(x+dir*int(10*scale), y-int(3*scale)+hop_offset, int(1.3*scale), TFT_BLACK);
    int nose_cx = x+dir*int(8.5*scale);
    int nose_cy = y+int(0*scale)+hop_offset;
    tft.fillTriangle(nose_cx-2, nose_cy, nose_cx+2, nose_cy, nose_cx, nose_cy+4, TFT_PINK);
    tft.drawLine(x+dir*int(11*scale), y+int(4*scale)+hop_offset, x+dir*int(14*scale), y+int(5*scale)+hop_offset, TFT_DARKGREY);
    tft.drawLine(x+dir*int(11*scale), y+int(3*scale)+hop_offset, x+dir*int(15*scale), y+hop_offset, TFT_DARKGREY);
    if (with_hammer) gordokong_draw_carrot_hammer(tft, x-dir*int(15*scale), y-int(10*scale)+hop_offset);
}

void gordokong_draw_girl(TFT_eSPI &tft, int x, int y) {
    tft.fillEllipse(x, y, 13, 15, 0xC618); // gray
    tft.fillEllipse(x, y, 11, 13, TFT_WHITE); // white face
    tft.fillEllipse(x-7, y+8, 5, 6, TFT_WHITE);
    tft.fillEllipse(x+7, y+8, 5, 6, TFT_WHITE);
    tft.fillEllipse(x, y-5, 7, 8, TFT_WHITE);
    tft.fillEllipse(x-7, y-14, 4, 13, 0xC618);
    tft.fillEllipse(x+7, y-14, 4, 13, 0xC618);
    tft.drawLine(x-9, y-17, x, y-21, TFT_YELLOW);
    tft.drawLine(x+9, y-17, x, y-21, TFT_YELLOW);
    tft.drawLine(x-9, y-17, x+9, y-17, TFT_YELLOW);
    tft.fillCircle(x, y-21, 2, TFT_YELLOW);
    tft.fillCircle(x-5, y-18, 1, TFT_YELLOW);
    tft.fillCircle(x+5, y-18, 1, TFT_YELLOW);
    tft.fillCircle(x-4, y-3, 1, TFT_BLACK);
    tft.fillCircle(x+4, y-3, 1, TFT_BLACK);
    tft.drawLine(x-4, y-5, x-6, y-7, TFT_BLACK);
    tft.drawLine(x-4, y-5, x-3, y-8, TFT_BLACK);
    tft.drawLine(x+4, y-5, x+6, y-7, TFT_BLACK);
    tft.drawLine(x+4, y-5, x+3, y-8, TFT_BLACK);
    tft.drawLine(x-2, y+5, x, y+7, TFT_PINK);
    tft.drawLine(x+2, y+5, x, y+7, TFT_PINK);
    tft.fillEllipse(x, y+3, 2, 1, TFT_PINK);
}

// DK block letters
void gordokong_draw_dk_letter(TFT_eSPI &tft, char letter, int x, int y, int sz) {
    static const uint8_t G[7] = {
        0b0111110,
        0b1000001,
        0b1000000,
        0b1001111,
        0b1000001,
        0b1000001,
        0b0111110
    };
    static const uint8_t O[7] = {
        0b0111110,
        0b1000001,
        0b1000001,
        0b1000001,
        0b1000001,
        0b1000001,
        0b0111110
    };
    static const uint8_t R[7] = {
        0b1111110,
        0b1000001,
        0b1000001,
        0b1111110,
        0b1001000,
        0b1000100,
        0b1000010
    };
    static const uint8_t D[7] = {
        0b1111100,
        0b1000010,
        0b1000001,
        0b1000001,
        0b1000001,
        0b1000010,
        0b1111100
    };
    static const uint8_t K[7] = {
        0b1000001,
        0b1000010,
        0b1000100,
        0b1001000,
        0b1000100,
        0b1000010,
        0b1000001
    };
    static const uint8_t N[7] = {
        0b1000001,
        0b1100001,
        0b1010001,
        0b1001001,
        0b1000101,
        0b1000011,
        0b1000001
    };
    static const uint8_t blank[7] = {0,0,0,0,0,0,0};
    const uint8_t* map = blank;
    switch(letter) {
        case 'G': map = G; break;
        case 'O': map = O; break;
        case 'R': map = R; break;
        case 'D': map = D; break;
        case 'K': map = K; break;
        case 'N': map = N; break;
        default: map = blank; break;
    }
    for(int row=0;row<7;row++)
        for(int col=0;col<7;col++)
            if(map[row] & (1<<(6-col)))
                tft.fillRect(x+col*sz, y+row*sz, sz-1, sz-1, TFT_CYAN);
}

void gordokong_draw_dk_text(TFT_eSPI &tft, const char* text, int x, int y, int sz, int spacing) {
    int cx = x;
    for(int i=0;text[i];i++) {
        if(text[i]==' ') { cx += 8*sz+spacing; continue; }
        gordokong_draw_dk_letter(tft, text[i], cx, y, sz);
        cx += 8*sz+spacing;
    }
}

void gordokong_splash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    tft.fillScreen(TFT_BLACK);
    int sz = 10;
    int wordWidthG = 5*8*sz + 4*2;
    int wordWidthK = 4*8*sz + 3*2;
    int xG = (GK_SCREEN_W - wordWidthG) / 2;
    int xK = (GK_SCREEN_W - wordWidthK) / 2;
    gordokong_draw_dk_text(tft, "GORDO", xG, 40, sz, 2);
    gordokong_draw_dk_text(tft, "KONG", xK, 130, sz, 2); // moved down so O/N are visible
    gordokong_draw_gordo_side_graycheeks(tft, GK_SCREEN_W/2, 250, true, false, 0.8*2.2, false, true); // tight_box=true for splash
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Press any button to play", GK_SCREEN_W/2, GK_SCREEN_H-32);
    bool btnSel, btnAny;
    do {
        btnSel = !ss.digitalRead(14);
        btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || btnSel);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        delay(10);
    } while (!btnAny);
    do {
        btnSel = !ss.digitalRead(14);
        btnAny = (!ss.digitalRead(6) || !ss.digitalRead(7) || !ss.digitalRead(9) || !ss.digitalRead(10) || btnSel);
        if (btnSel) { tft.fillScreen(TFT_BLACK); return; }
        delay(10);
    } while (btnAny);
    tft.fillScreen(TFT_BLACK);
}

#define GK_NUM_BARRELS 4 // Now includes the moving fox on the 3rd girder
#define GK_BARREL_SPEED_START 2
#define GK_BARREL_RESPAWN 70
#define GK_GIRDER_H 12

void reset_barrels(GKBarrel barrels[GK_NUM_BARRELS]) {
    for(int i=0;i<3;i++) {
        barrels[i].x = GK_GIRDER_X0[0]+30;
        barrels[i].y = GK_GIRDER_Y[0]-8;
        barrels[i].dir = 1;
        barrels[i].girder = 0;
        barrels[i].timer = i*GK_BARREL_RESPAWN;
        barrels[i].alive = false;
        barrels[i].dropping = false;
        barrels[i].drop_frame = 0;
        barrels[i].face_right = true;
        barrels[i].scored_jump = false;
        barrels[i].is_small_fox = false;
    }

    // Moving small fox on the 3rd girder
    barrels[3].x = (GK_GIRDER_X0[2] + GK_GIRDER_X1[2]) / 2;
    barrels[3].y = GK_GIRDER_Y[2] - 8;
    barrels[3].dir = 1;
    barrels[3].girder = 2;
    barrels[3].timer = 0;
    barrels[3].alive = true;
    barrels[3].dropping = false;
    barrels[3].drop_frame = 0;
    barrels[3].face_right = true;
    barrels[3].scored_jump = false;
    barrels[3].is_small_fox = true;
}

void gordokong_place_carrot(int &carrot_x, int &carrot_y) {
    int girder = random(1, 4); // 1,2,3 (not the top)
    int x0 = GK_GIRDER_X0[girder] + 40;
    int x1 = GK_GIRDER_X1[girder] - 40;
    carrot_x = x0 + random(0, x1 - x0);
    carrot_y = GK_GIRDER_Y[girder] - 24;
}

void run_GordoKong(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    gordokong_splash(tft, ss);

start_game:
    int level = 1;
    int barrel_speed = GK_BARREL_SPEED_START;
    int score = 0;

    while (true) {
        tft.fillScreen(TFT_BLACK);

        uint16_t girder_color = GK_GIRDER_COLORS[(level-1) % NUM_GIRDER_COLORS];

        for (int i=0; i<4; i++)
            gordokong_draw_girder(tft, GK_GIRDER_X0[i], GK_GIRDER_Y[i], GK_GIRDER_X1[i]-GK_GIRDER_X0[i], GK_GIRDER_H, girder_color);
        for (int i=0; i<NUM_LADDERS; i++)
            gordokong_draw_ladder(tft, GK_LADDERS[i][0], GK_LADDERS[i][1], GK_LADDERS[i][2]);
        gordokong_draw_mother_fox(tft, 50, GK_GIRDER_Y[0]-32);
        gordokong_draw_girl(tft, 440, GK_GIRDER_Y[0]-14);


        int gordo_x = 80, gordo_y = GK_GIRDER_Y[3] - 8;
        int gordo_prev_x = gordo_x, gordo_prev_y = gordo_y;
        bool gordo_on_ground = true;
        bool gordo_facing_right = true;
        bool gordo_on_ladder = false;
        bool gordo_with_carrot = false;
        int carrot_time = 0;
        int lives = 3;
        bool gordo_hop = false;

        GKBarrel barrels[GK_NUM_BARRELS];
        reset_barrels(barrels);

        int carrot_on = 1;
        int carrot_x = 320;
        int carrot_y = GK_GIRDER_Y[1]-24;
        gordokong_place_carrot(carrot_x, carrot_y);

        bool game_over = false;
        unsigned long game_over_time = 0;
        bool ouch_visible = false;
        unsigned long ouch_time = 0;

        static int jump_phase = 0;
        static int jump_counter = 0;
        int hop_anim = 0;

        while (true) {
            int erase_w = int(24*0.8);
            int erase_h = int(62*0.8);
            int erase_yoff = int(49*0.8);
            tft.fillRect(gordo_prev_x-erase_w, gordo_prev_y-erase_yoff, erase_w*2, erase_h, TFT_BLACK);
            // Erase carrot hammer region if Gordo is holding carrot
if (gordo_with_carrot) {
    int dir = gordo_facing_right ? 1 : -1;
    float scale = 0.8;
    int carrot_hammer_x = gordo_prev_x - dir*int(15*scale);
    int carrot_hammer_y = gordo_prev_y - int(10*scale);
    tft.fillRect(carrot_hammer_x-10, carrot_hammer_y-25, 20, 35, TFT_BLACK);
}

            for (int i=0;i<GK_NUM_BARRELS;i++) {
                if (barrels[i].alive) {
                    if (barrels[i].dropping)
                        tft.fillRect(barrels[i].drop_ladder_x-16, min(barrels[i].y, barrels[i].drop_y_end)-16, 32, abs(barrels[i].y-barrels[i].drop_y_end)+32, TFT_BLACK);
                    else
                        tft.fillRect(barrels[i].x-16, barrels[i].y-16, 32, 32, TFT_BLACK);
                }
            }
            tft.fillRect(carrot_x-10, carrot_y-25, 20, 35, TFT_BLACK);

            for (int i=0; i<4; i++)
                gordokong_draw_girder(tft, GK_GIRDER_X0[i], GK_GIRDER_Y[i], GK_GIRDER_X1[i]-GK_GIRDER_X0[i], GK_GIRDER_H, girder_color);
            for (int i=0; i<NUM_LADDERS; i++)
                gordokong_draw_ladder(tft, GK_LADDERS[i][0], GK_LADDERS[i][1], GK_LADDERS[i][2]);
            gordokong_draw_mother_fox(tft, 50, GK_GIRDER_Y[0]-32);
            gordokong_draw_girl(tft, 440, GK_GIRDER_Y[0]-14);
// Draw "Gordo" over princess rabbit and clear after 4 seconds
static unsigned long gordo_msg_time = 0;
static bool gordo_msg_visible = false;
if (!gordo_msg_visible) {
tft.setTextDatum(TL_DATUM);
tft.setTextFont(2);
tft.setTextColor(TFT_ORANGE, TFT_BLACK);
tft.drawString("Gordo!", 375, GK_GIRDER_Y[0]-40);
    gordo_msg_time = millis();
    gordo_msg_visible = true;
} else if (gordo_msg_visible && millis() - gordo_msg_time > 4000) {
    tft.fillRect(410, GK_GIRDER_Y[0]-40, 60, 20, TFT_BLACK);
    gordo_msg_visible = false;
}


// Erase carrot region before drawing
tft.fillRect(carrot_x-10, carrot_y-25, 20, 35, TFT_BLACK);
            if (carrot_on)
                gordokong_draw_carrot_hammer(tft, carrot_x, carrot_y);

            for (int i=0;i<GK_NUM_BARRELS;i++)
                if (barrels[i].alive)
                    gordokong_draw_baby_fox(tft, barrels[i].dropping ? barrels[i].drop_ladder_x : barrels[i].x, barrels[i].y, barrels[i].face_right, barrels[i].dropping, barrels[i].drop_frame);

            gordokong_draw_gordo_side(tft, gordo_x, gordo_y, gordo_facing_right, gordo_with_carrot, 0.8, gordo_hop);

            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.setTextFont(2);
            tft.setTextSize(1);
            tft.setCursor(10, 8);
            tft.printf("Level:%d", level);
            tft.setCursor(180, 8);
            tft.printf("Lives:%d", lives);
            tft.setCursor(340, 8);
            tft.printf("Score:%d", score);
if (gordo_with_carrot) {
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setCursor(260, 30);
    tft.print("CARROT!");
} else {
    // Erase the word CARROT!
    tft.fillRect(260, 30, 90, 20, TFT_BLACK);
}

            if (ouch_visible && millis() - ouch_time > 1000) {
                tft.fillRect(GK_SCREEN_W/2-90, (GK_GIRDER_Y[1]+GK_GIRDER_Y[0])/2-20, 180, 40, TFT_BLACK);
                ouch_visible = false;
            }

            if (game_over) {
                if (millis() - game_over_time > 4000) {
                    gordokong_splash(tft, ss);
                    goto start_game;
                }
                delay(28);
                continue;
            }

            gordo_prev_x = gordo_x;
            gordo_prev_y = gordo_y;

            int joy_x = ss.analogRead(JOY_X), joy_y = ss.analogRead(JOY_Y);
            bool btnA = !ss.digitalRead(BUTTON_A);
            bool btnSel = !ss.digitalRead(BUTTON_SELECT);
            if (btnSel) { tft.fillScreen(TFT_BLACK); return; }

            int move_x = 0, move_y = 0;
            if (joy_x < 410) move_x = -1;
            if (joy_x > 610) move_x = 1;
            if (joy_y < 410) move_y = -1;
            if (joy_y > 610) move_y = 1;

            gordo_hop = false;
            if (!gordo_on_ladder && move_x != 0 && (gordo_on_ground || jump_phase)) {
                hop_anim++;
                if ((hop_anim/2)%2) gordo_hop = true;
            } else {
                hop_anim = 0;
            }

            bool climbing = false;
            for (int i=0; i<NUM_LADDERS; i++) {
                int lx = GK_LADDERS[i][0];
                int ly0 = GK_LADDERS[i][1], ly1 = GK_LADDERS[i][2];
                int ytop = min(ly0, ly1), ybot = max(ly0, ly1);
                if (abs(gordo_x - lx) <= 8 && gordo_y+8 >= ytop && gordo_y-8 <= ybot && move_y != 0) {
                    climbing = true;
                    gordo_y += move_y*8;
                    if (gordo_y < ytop) gordo_y = ytop;
                    if (gordo_y > ybot) gordo_y = ybot;
                    break;
                }
            }
            gordo_on_ladder = climbing;

            if (!climbing && move_x != 0) {
                gordo_x += move_x*8;
                if (gordo_x < 40) gordo_x = 40;
                if (gordo_x > GK_SCREEN_W-40) gordo_x = GK_SCREEN_W-40;
                gordo_facing_right = (move_x > 0);
            }

            int girder = 3;
            for (int i=3;i>=0;i--) if (gordo_y >= GK_GIRDER_Y[i]-10) { girder = i; break; }
            if (!climbing && jump_phase==0) {
                gordo_y = GK_GIRDER_Y[girder]-8;
                gordo_on_ground = true;
            } else {
                gordo_on_ground = false;
            }

            if (btnA && gordo_on_ground && !climbing && jump_phase==0) {
                jump_phase = 1; jump_counter = 0;
            }
            if (jump_phase == 1) {
                gordo_y -= 11;
                jump_counter++;
                if (jump_counter > 3) jump_phase = 2;
            } else if (jump_phase == 2) {
                gordo_y += 11;
                jump_counter--;
                if (jump_counter <= 0) { jump_phase = 0; gordo_on_ground = true; }
            }

            if (carrot_on && abs(gordo_x-carrot_x)<14 && abs(gordo_y-carrot_y)<14) {
                gordo_with_carrot = true;
                carrot_on = 0;
                carrot_time = 90;
            }
            if (gordo_with_carrot) {
                carrot_time--;
                if (carrot_time<=0) gordo_with_carrot = false;
            }

            for (int i=0;i<3;i++) { // Only regular barrels drop and respawn
                if (!barrels[i].alive) {
                    barrels[i].timer--;
                    if (barrels[i].timer<=0) {
                        barrels[i].alive = true;
                        barrels[i].girder = 0;
                        barrels[i].x = GK_GIRDER_X0[0]+30;
                        barrels[i].y = GK_GIRDER_Y[0]-8;
                        barrels[i].dir = 1;
                        barrels[i].timer = GK_BARREL_RESPAWN*GK_NUM_BARRELS;
                        barrels[i].dropping = false;
                        barrels[i].face_right = true;
                        barrels[i].scored_jump = false;
                    }
                    continue;
                }
                if (barrels[i].dropping) {
                    barrels[i].drop_frame++;
                    int sign = (barrels[i].drop_y_end > barrels[i].y) ? 1 : -1;
                    barrels[i].y += sign*4;
                    if ((sign > 0 && barrels[i].y >= barrels[i].drop_y_end) || (sign < 0 && barrels[i].y <= barrels[i].drop_y_end)) {
                        barrels[i].y = barrels[i].drop_y_end;
                        barrels[i].dropping = false;
                        barrels[i].x = barrels[i].drop_ladder_x;
                        barrels[i].girder++;
                        barrels[i].face_right = barrels[i].dir > 0;
                    }
                    continue;
                }
                if (barrels[i].girder == 3 && barrels[i].x > GK_SCREEN_W+20) {
                    barrels[i].alive = false;
                    barrels[i].timer = GK_BARREL_RESPAWN*GK_NUM_BARRELS;
                    continue;
                }
                barrels[i].x += barrels[i].dir*barrel_speed;
                if (barrels[i].x < GK_GIRDER_X0[barrels[i].girder]+20) barrels[i].dir = 1;
                if (barrels[i].x > GK_GIRDER_X1[barrels[i].girder]-20) barrels[i].dir = -1;
                barrels[i].face_right = barrels[i].dir > 0;
                for (int k=0;k<NUM_LADDERS;k++) {
                    if (abs(barrels[i].x-GK_LADDERS[k][0])<10 &&
                        barrels[i].girder < 3 && barrels[i].girder >= 0 &&
                        GK_LADDERS[k][1]==GK_GIRDER_Y[barrels[i].girder] &&
                        GK_LADDERS[k][2]==GK_GIRDER_Y[barrels[i].girder+1] &&
                        random(0, 40)==1) {
                        barrels[i].dropping = true;
                        barrels[i].drop_ladder_x = GK_LADDERS[k][0];
                        barrels[i].drop_y_start = barrels[i].y;
                        barrels[i].drop_y_end = GK_GIRDER_Y[barrels[i].girder+1]-8;
                        barrels[i].drop_frame = 0;
                        break;
                    }
                }
            }
            // Moving fox on girder 2 (barrels[3])
            GKBarrel &fox = barrels[3];
            if (fox.alive && !fox.dropping) {
                fox.x += fox.dir*2;
                if (fox.x < GK_GIRDER_X0[2]+20) { fox.dir = 1; fox.face_right = true; }
                if (fox.x > GK_GIRDER_X1[2]-20) { fox.dir = -1; fox.face_right = false; }
            }

            // Jump scoring
            for (int i=0;i<GK_NUM_BARRELS;i++) {
                if (!barrels[i].alive) continue;
                int bx = barrels[i].dropping ? barrels[i].drop_ladder_x : barrels[i].x;
                int by = barrels[i].y;
                bool above = (gordo_prev_y > by-20 && gordo_y < by-8) || (gordo_y > by-20 && gordo_prev_y < by-8);
                bool close_x = abs(gordo_x - bx) < 20;
                if (!barrels[i].scored_jump && above && close_x && jump_phase!=0) {
                    score += 100;
                    barrels[i].scored_jump = true;
                }
            }

            // Barrel/fox collision
            bool hit = false;
            for (int i=0;i<GK_NUM_BARRELS;i++) {
                if (!barrels[i].alive) continue;
                int bx = barrels[i].dropping ? barrels[i].drop_ladder_x : barrels[i].x;
                int by = barrels[i].y;
                if (abs(bx-gordo_x)<18 && abs(by-gordo_y)<14) {
                    if (gordo_with_carrot) {
                        tft.fillEllipse(bx, by, 13, 13, TFT_YELLOW);
                        score += 200;
                        barrels[i].alive = false;
                        barrels[i].timer = GK_BARREL_RESPAWN*GK_NUM_BARRELS;
                        barrels[i].dropping = false;
                    } else {
                        hit = true;
                        break;
                    }
                }
            }
            if (hit) {
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_RED, TFT_BLACK);
                tft.setTextFont(4);
                tft.setTextSize(2);
                tft.drawString("Ouch!", GK_SCREEN_W/2, (GK_GIRDER_Y[1]+GK_GIRDER_Y[0])/2);
                ouch_visible = true;
                ouch_time = millis();
                gordo_x = 80; gordo_y = GK_GIRDER_Y[3] - 8; gordo_with_carrot = false;
                carrot_time = 0;
                lives--;
                reset_barrels(barrels);
                if (lives <= 0) {
                    tft.setTextDatum(MC_DATUM);
                    tft.setTextColor(TFT_RED, TFT_BLACK);
                    tft.setTextFont(4);
                    tft.setTextSize(2);
                    tft.drawString("GAME OVER!", GK_SCREEN_W/2, (GK_GIRDER_Y[3]+GK_GIRDER_Y[2])/2);
                    game_over = true;
                    game_over_time = millis();
                }
            }

            if (gordo_y <= GK_GIRDER_Y[0]-16 && gordo_x > 410) {
                score += 300;
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_GREEN, TFT_BLACK);
                tft.setTextFont(4);
                tft.setTextSize(2);
                tft.drawString("YOU WIN!", GK_SCREEN_W/2, GK_SCREEN_H/2-24);
                tft.setTextFont(2);
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.drawString("Score: "+String(score), GK_SCREEN_W/2, GK_SCREEN_H/2+24);
                delay(1200);
                level++;
                barrel_speed++;
                break;
            }
            delay(28);
        }
    }
}