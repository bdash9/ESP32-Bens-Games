#include <TFT_eSPI.h>
#include <Wire.h>
#include <Adafruit_seesaw.h>
#include <SPI.h>
#include <SD.h>

TFT_eSPI tft = TFT_eSPI();
Adafruit_seesaw ss;

#define JOY_ADDR      0x49
#define JOY_SDA_PIN   21
#define JOY_SCL_PIN   22
#define BUTTON_A      6
#define BUTTON_B      7
#define BUTTON_X      9
#define BUTTON_Y      10
#define BUTTON_SELECT 14
#define JOY_X         3
#define JOY_Y         2
#define SCREEN_W      480
#define SCREEN_H      320
#define BAT_ADC_PIN   35

float readBatteryVoltage() {
    int raw = analogRead(BAT_ADC_PIN);
    return (raw / 4095.0f) * 2.0f * 3.3f;
}
int batteryPercent(float vbatt) {
    if (vbatt > 4.2) vbatt = 4.2;
    if (vbatt < 3.3) vbatt = 3.3;
    int pct = (int)(100.0 * (vbatt - 3.3) / (4.2 - 3.3));
    if (pct > 100) pct = 100;
    if (pct < 0) pct = 0;
    return pct;
}

// ----------- Game Forward Declarations ---------------
void run_GordoKong(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_GalaGordo(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_Gordoer(TFT_eSPI &tft, Adafruit_seesaw &ss); 
void run_GordoGachi(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_AmazeBallz(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_CannonTrioDefender(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_BenOTron(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_ScottsTangerineDream(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_GordosBigAdventure(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_SpookyMaze(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_AsciiStarWars_SD(TFT_eSPI &tft, Adafruit_seesaw &ss);

// ------------ Menu Items ---------------
const char* game_titles[] = {
    "Gordo Kong",               // NEW, now first
    "Gala-Gordo",
    "Gord-ogger",
    "Gordo-Gachi",
    "Amaze-Ballz",
    "Cannon-Trio-Defender",
    "Ben-O-Tron",
    "Scott's Tangerine Dream",
    "Gordo's Big Adventure",
    "Spooky Maze",
    "Ascii Star Wars (17 min)"
};
enum {
    GORDOKONG_INDEX = 0,        // NEW, now first
    GALAGORDO_INDEX,
    GORDOER_INDEX,
    GORDO_INDEX,
    AMAZE_INDEX,
    CANNON_INDEX,
    BENOTRON_INDEX,
    TANGERINE_INDEX,
    GORDOS_ADVENTURE_INDEX,
    SPOOKY_MAZE_INDEX,
    ASCII_STARWARS_SD_INDEX,
    NUM_GAMES
};
int selected_game = 0;

#define MENU_VISIBLE 6  // Now showing 6 at a time
// Small "Gordo" bunny for menu
void drawMenuGordo(TFT_eSPI &tft, int x, int y) {
    tft.fillEllipse(x, y, 14, 15, TFT_WHITE); // body
    tft.fillEllipse(x-7, y+8, 7, 8, TFT_DARKGREY); // left foot
    tft.fillEllipse(x+7, y+8, 7, 8, TFT_DARKGREY); // right foot
    tft.fillEllipse(x, y-6, 9, 10, TFT_WHITE); // face
    tft.fillEllipse(x-5, y-14, 3, 10, TFT_DARKGREY); // left ear
    tft.fillEllipse(x+5, y-14, 3, 10, TFT_DARKGREY); // right ear
    //tft.fillEllipse(x-2, y-9, 2, 4, TFT_PINK); // left ear in
    //tft.fillEllipse(x+2, y-9, 2, 4, TFT_PINK); // right ear in
    tft.fillCircle(x-3, y-4, 1, TFT_BLACK); // left eye
    tft.fillCircle(x+3, y-4, 1, TFT_BLACK); // right eye
    // Small pink triangle nose, below the eyes
    tft.fillTriangle(x-2, y, x+2, y, x, y+4, TFT_PINK);
}

// Pixel Space Invader for menu
void drawMenuSpaceInvader(TFT_eSPI &tft, int x, int y) {
    // Draws an 11x8 classic invader (each pixel 3x3)
    uint16_t c = TFT_GREEN;
    int px = x-16, py = y-12, s = 3;
const uint16_t sprite[8] = {
    0b00111000000,
    0b01111100000,
    0b11111110000,
    0b11011011000,
    0b11111110000,
    0b00100100000,
    0b01000010000,
    0b10000001000
};
    for (int row=0; row<8; row++)
        for (int col=0; col<11; col++)
            if (sprite[row] & (1<<(10-col)))
                tft.fillRect(px+col*s, py+row*s, s, s, c);
}

void drawMenu(int selected) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    // --- Arcade-style Title ---
    int titleY = 30;
    drawMenuGordo(tft, SCREEN_W/2-195, titleY+8); // Small Gordo left
    drawMenuSpaceInvader(tft, SCREEN_W-30 , titleY+6); 

    tft.setTextFont(2);
    tft.setTextSize(3);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Ben's Game Menu", SCREEN_W/2, titleY);

    // Determine which items to show (scroll)
    int start_idx = selected - MENU_VISIBLE/2;
    if (start_idx < 0) start_idx = 0;
    if (start_idx > NUM_GAMES - MENU_VISIBLE) start_idx = NUM_GAMES - MENU_VISIBLE;
    if (NUM_GAMES <= MENU_VISIBLE) start_idx = 0;

    tft.setTextFont(2);
    tft.setTextSize(1);

    for (int i = 0; i < MENU_VISIBLE; i++) {
        int item_idx = start_idx + i;
        int ypos = 90 + i * 33;
        if (item_idx == selected) {
            tft.setTextColor(TFT_RED, TFT_DARKGREY);
            tft.fillRoundRect(SCREEN_W/2-180, ypos-20, 360, 43, 12, TFT_DARKGREY);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(game_titles[item_idx], SCREEN_W/2, ypos);
    }

    // ---- BATTERY BAR ----
    float vbat = readBatteryVoltage();
    int pct = batteryPercent(vbat);
    int bw = 50, bh = 11;
    int x0 = SCREEN_W - 58, y0 = 8;
    tft.drawRect(x0, y0, bw, bh, TFT_WHITE);
    tft.fillRect(x0+2, y0+2, (bw-4)*pct/100, bh-4, TFT_GREEN);

    tft.setTextFont(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("A/Start: Play   SELECT: Quit to Menu", SCREEN_W/2, SCREEN_H-18);
    tft.setTextSize(2);
}

void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(1);
    Wire.begin(JOY_SDA_PIN, JOY_SCL_PIN);
    ss.begin(JOY_ADDR);
    ss.pinMode(BUTTON_A, INPUT_PULLUP);
    ss.pinMode(BUTTON_B, INPUT_PULLUP);
    ss.pinMode(BUTTON_X, INPUT_PULLUP);
    ss.pinMode(BUTTON_Y, INPUT_PULLUP);
    ss.pinMode(BUTTON_SELECT, INPUT_PULLUP);
    drawMenu(selected_game);
}

void loop() {
    static int prevJoyY = 512;
    static bool needRedraw = false;
    static bool prevAPressed = false;

    int joyY = ss.analogRead(JOY_Y);
    bool aPressed = !ss.digitalRead(BUTTON_A);

    delay(60);

    // Menu navigation
    if (joyY < 400 && prevJoyY >= 400) {
        selected_game--;
        if(selected_game < 0) selected_game = NUM_GAMES-1;
        needRedraw = true;
    }
    if (joyY > 600 && prevJoyY <= 600) {
        selected_game++;
        if(selected_game >= NUM_GAMES) selected_game = 0;
        needRedraw = true;
    }

    prevJoyY = joyY;

    if (needRedraw) {
        drawMenu(selected_game);
        needRedraw = false;
    }

    // Launch game ONLY when A is pressed *now* and was *not* pressed last time
    if (aPressed && !prevAPressed) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(2);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Loading...", SCREEN_W/2, SCREEN_H/2);

        switch (selected_game) {
            case GORDOKONG_INDEX:
                run_GordoKong(tft, ss); break;
            case GALAGORDO_INDEX:
                run_GalaGordo(tft, ss); break;
            case GORDOER_INDEX:
                run_Gordoer(tft, ss); break;
            case GORDO_INDEX:
                run_GordoGachi(tft, ss); break;
            case AMAZE_INDEX:
                run_AmazeBallz(tft, ss); break;
            case CANNON_INDEX:
                run_CannonTrioDefender(tft, ss); break;
            case BENOTRON_INDEX:
                run_BenOTron(tft, ss); break;
            case TANGERINE_INDEX:
                run_ScottsTangerineDream(tft, ss); break;
            case GORDOS_ADVENTURE_INDEX: 
                run_GordosBigAdventure(tft, ss); break;
            case SPOOKY_MAZE_INDEX:
                run_SpookyMaze(tft, ss); break;
            case ASCII_STARWARS_SD_INDEX:
                run_AsciiStarWars_SD(tft, ss); break;
        }
        drawMenu(selected_game);
    }
    prevAPressed = aPressed;
}

// Game includes
#include "Gordo_Kong.h"          // NEW!
#include "Gala-Gordo.h"  
#include "Gordo-er.h"         
#include "Gordo-gachi.h"
#include "Amaze-Ballz.h"
#include "Cannon-Trio-Defender.h"
#include "Ben-O-Tron.h"
#include "Scott-Tangerine-dream.h"
#include "Gordos_Big_Adventure.h"
#include "Spooky_Maze.h"
#include "Ascii_StarWars_SD.h"
