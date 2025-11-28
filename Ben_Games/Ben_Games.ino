#include <TFT_eSPI.h>
#include <Wire.h>
#include <Adafruit_seesaw.h>
#include <SPI.h>
#include <SD.h>

// ----------- Hardware Objects ---------------
TFT_eSPI tft = TFT_eSPI();
Adafruit_seesaw ss;

// --- JoyWing controls ---
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

// ----- BATTERY SENSE PIN -----
#define BAT_ADC_PIN 35    // Most TTGO/ESP32 boards; change for yours if needed

float readBatteryVoltage() {
    int raw = analogRead(BAT_ADC_PIN);
    // For 100k/100k divider, 3.3V ADC ref
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
void run_GordoGachi(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_AmazeBallz(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_CannonTrioDefender(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_BenOTron(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_ScottsTangerineDream(TFT_eSPI &tft, Adafruit_seesaw &ss);
void run_GordosBigAdventure(TFT_eSPI &tft, Adafruit_seesaw &ss); // *** NEW ***

// ------------ Menu Items ---------------
const char* game_titles[] = {
    "Gordo-Gachi",
    "Amaze-Ballz",
    "Cannon-Trio-Defender",
    "Ben-O-Tron",
    "Scott's Tangerine Dream",
    "Gordo's Big Adventure"  // *** NEW ***
};
enum {
    GORDO_INDEX = 0,
    AMAZE_INDEX,
    CANNON_INDEX,
    BENOTRON_INDEX,
    TANGERINE_INDEX,
    GORDOS_ADVENTURE_INDEX,  // *** NEW ***
    NUM_GAMES
};
int selected_game = 0;

void drawMenu(int selected) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    // --- EXPLICITLY SET FONT FOR TITLE ---
    tft.setTextFont(2);
    tft.setTextSize(3);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Ben's Game Menu", SCREEN_W/2, 30);

    tft.setTextFont(2);
    tft.setTextSize(1);

    for (int i = 0; i < NUM_GAMES; i++) {
        int ypos = 100 + i * 33;
        if (i == selected) {
            tft.setTextColor(TFT_RED, TFT_DARKGREY);
            tft.fillRoundRect(SCREEN_W/2-180, ypos-20, 360, 43, 12, TFT_DARKGREY);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(game_titles[i], SCREEN_W/2, ypos);
    }

    // ---- BATTERY BAR, Left of right margin, no percent ----
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
            case GORDO_INDEX:
                run_GordoGachi(tft, ss);
                break;
            case AMAZE_INDEX:
                run_AmazeBallz(tft, ss);
                break;
            case CANNON_INDEX:
                run_CannonTrioDefender(tft, ss);
                break;
            case BENOTRON_INDEX:
                run_BenOTron(tft, ss);
                break;
            case TANGERINE_INDEX:
                run_ScottsTangerineDream(tft, ss); 
                break;
            case GORDOS_ADVENTURE_INDEX: 
                run_GordosBigAdventure(tft, ss);
                break;
        }
        drawMenu(selected_game);
    }
    prevAPressed = aPressed;
}

// Game includes
#include "Gordo-gachi.h"
#include "Amaze-Ballz.h"
#include "Cannon-Trio-Defender.h"
#include "Ben-O-Tron.h"
#include "Scott-Tangerine-dream.h"
#include "Gordos_Big_Adventure.h" 