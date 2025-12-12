// This code doesn't blink. it only redraws the charcters that change 
// and not the whole screen

#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>
#include <SD.h>

const int PIN_SD_CS   = 13;
const int PIN_SD_MOSI = 15;
const int PIN_SD_MISO = 2;
const int PIN_SD_SCLK = 14;

void run_AsciiStarWars_SD(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    Serial.begin(115200);
    delay(1000);
    Serial.println("[StarWars] Starting...");
    SPI.begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    Serial.println("[StarWars] SPI.begin done.");

    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("[StarWars] SD.begin FAILED!");
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_WHITE);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("SD CARD ERROR!", tft.width()/2, tft.height()/2, 4);
        delay(3000);
        return;
    }
    Serial.println("[StarWars] SD.begin succeeded.");

    File f = SD.open("/sw1.txt", "r");
    if (!f) {
        Serial.println("[StarWars] SD.open /sw1.txt FAILED!");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setTextFont(2);
        tft.drawString("Can't open sw1.txt on SD!", 20, 40);
        delay(2000);
        return;
    }
    Serial.println("[StarWars] SD.open /sw1.txt succeeded.");

    char line[128];
    String frame_lines[24];
    const int max_cols = 80;
    const int max_rows = 24;

    tft.setTextFont(1); // 6x8 mono
    int char_w = 6; // Font 1 is 6x8 for TFT_eSPI
    int char_h = 8;
    int total_w = char_w * max_cols;
    int total_h = char_h * max_rows;
    int x0 = (tft.width()  - total_w) / 2 + 40; // right-shift by 40 pixels
    int y0 = tft.height() - total_h - 16;
    if (y0 < 0) y0 = 0;

    int frame_count = 0;
    while (true) {
        if (!ss.digitalRead(14)) {
            tft.fillScreen(TFT_BLACK);
            f.close();
            Serial.println("[StarWars] SELECT pressed, exiting.");
            return;
        }

        // --- Read frame delay ---
        int frame_delay = 66; // default
        while (f.available()) {
            int len = f.readBytesUntil('\n', line, sizeof(line)-1);
            line[len] = '\0';
            String s = String(line);
            s.trim();
            if (s.length() == 0) continue; // skip blank
            bool just_number = true;
            for (size_t i = 0; i < s.length(); ++i) {
                if (!isDigit(s[i])) { just_number = false; break; }
            }
            if (just_number && s.length() > 0) {
                // Use the number as the frame delay (in ticks, e.g. 16 ms per tick) Larger number is slower Ie 24
                int n = s.toInt();
                frame_delay = n * 62;
                if (frame_delay < 16) frame_delay = 16; // minimum delay
                Serial.print("[StarWars] New frame delay (ms): "); Serial.println(frame_delay);
                break;
            }
        }

        // --- Read frame lines ---
        int lines_in_frame = 0;
        while (f.available() && lines_in_frame < max_rows) {
            int len = f.readBytesUntil('\n', line, sizeof(line)-1);
            line[len] = '\0';
            String s = String(line);
            s.trim();
            // Check for next frame's number (separator)
            bool just_number = true;
            for (size_t i = 0; i < s.length(); ++i) {
                if (!isDigit(s[i])) { just_number = false; break; }
            }
            if (just_number && s.length() > 0) {
                // Seek back so we read this number as the next frame's delay
                int backseek = len + 1; // +1 for the newline
                f.seek(f.position() - backseek);
                break;
            }
            if (s.length() == 0) continue; // skip blank lines
            frame_lines[lines_in_frame++] = String(line);
        }
        // Always fill all lines
        for (int i = lines_in_frame; i < max_rows; i++) frame_lines[i] = "";

        int y = y0;
        for (int row = 0; row < max_rows; row++) {
            String ln = frame_lines[row];
            for (int col = 0; col < max_cols; col++) {
                char c = (col < ln.length()) ? ln[col] : ' ';
                tft.drawChar(x0 + col * char_w, y, c, TFT_WHITE, TFT_BLACK, 1);
            }
            y += char_h;
        }

        Serial.print("[StarWars] Drew frame #");
        Serial.print(frame_count++);
        Serial.print(", delay: "); Serial.println(frame_delay);

        delay(frame_delay);

        if (!f.available()) {
            Serial.println("[StarWars] End of file, looping.");
            f.seek(0); // loop movie
        }
    }
}