// PART 1 OF 4
#ifndef GORDOS_BIG_ADVENTURE_H
#define GORDOS_BIG_ADVENTURE_H

#include <TFT_eSPI.h>
#include <SD.h>
#include <SPI.h>

namespace GordoGame {
    // Global pointers to hardware
    static TFT_eSPI* g_tft = nullptr;
    static Adafruit_seesaw* g_ss = nullptr;
    
    // Constants (button pins use macros from other headers)
    const int PIN_SD_CS = 13;
    const int PIN_SD_MOSI = 15;
    const int PIN_SD_MISO = 2;
    const int PIN_SD_SCLK = 14;
    // BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y, BUTTON_SELECT already defined as macros
    const int FRAME_W = 32;
    const int FRAME_H = 32;
    const int GORDO_ANIM_FRAMES = 4;
    const int BIRD_ANIM_FRAMES = 3;
    
    const int gordo_x_offsets[4] = {0, 32, 64, 96};
    const int gordo_widths[4] = {32, 32, 32, 32};
    const int gordo_row_ys[2] = {0, 43};
    
    // Static game state variables
    static TFT_eSprite* grassTile = nullptr;
    static TFT_eSprite* gordoSheet = nullptr;
    static TFT_eSprite* gordoFrame = nullptr;
    static TFT_eSprite* gordoSplash = nullptr;
    static int gordoSplash_w = 0, gordoSplash_h = 0;
    static TFT_eSprite* tree1 = nullptr;
    static int tree1_w = 0, tree1_h = 0;
    static TFT_eSprite* rock1 = nullptr;
    static int rock1_w = 0, rock1_h = 0;
    static TFT_eSprite* rock2 = nullptr;
    static int rock2_w = 0, rock2_h = 0;
    static TFT_eSprite* rock3 = nullptr;
    static int rock3_w = 0, rock3_h = 0;
    static TFT_eSprite* rock4 = nullptr;
    static int rock4_w = 0, rock4_h = 0;
    static TFT_eSprite* rock5 = nullptr;
    static int rock5_w = 0, rock5_h = 0;
    static TFT_eSprite* blueSheet = nullptr;
    static TFT_eSprite* blueFrame = nullptr;
    static TFT_eSprite* cardinalSheet = nullptr;
    static TFT_eSprite* cardinalFrame = nullptr;
    static TFT_eSprite* bunnyFrame = nullptr;
    static TFT_eSprite* eagleSheet = nullptr;
    static TFT_eSprite* eagleFrame = nullptr;
    
    static int eagle_x = 0, eagle_y = 0, eagle_vx = 0, eagle_vy = 0, eagle_anim = 0;
    static unsigned long lastEagleAnim = 0;
    static bool eagle_active = false;
    static unsigned long lastEagleAttack = 0;
    static int eagle_direction = 0;
    
    static int eagle2_x = 0, eagle2_y = 0, eagle2_vx = 0, eagle2_vy = 0, eagle2_anim = 0;
    static unsigned long lastEagle2Anim = 0;
    static bool eagle2_active = false;
    static unsigned long lastEagle2Attack = 0;
    static int eagle2_direction = 0;
    
    enum { DIR_RIGHT=0, DIR_LEFT=1 };
    static int gordo_x = 0, gordo_y = 0, gordo_dir = 0, gordo_anim_frame = 0, gordo_vx = 0, gordo_vy = 0;
    static unsigned long lastGordoAnim = 0;
    static int last_gordo_x = -1, last_gordo_y = -1;
    static int blue_x = 0, blue_y = 0, blue_vx = 0, blue_anim = 0, last_blue_x = 0, last_blue_y = 0;
    static int cardinal_x = 0, cardinal_y = 0, cardinal_vx = 0, cardinal_anim = 0, last_cardinal_x = 0, last_cardinal_y = 0;
    static bool cardinal_active = false;
    static uint32_t last_cardinal_time = 0, lastBlueAnim = 0, lastCardinalAnim = 0;
    
    static int rock1_x = 0, rock1_y = 0;
    static int rock2_x = 0, rock2_y = 0;
    static int rock3_x = 0, rock3_y = 0;
    static int rock4_x = 0, rock4_y = 0;
    static int rock5_x = 0, rock5_y = 0;
    
    enum GameState { STATE_SPLASH, STATE_SPLASH2, STATE_SCENE, STATE_END };
    static GameState gameState = STATE_SPLASH;
    static GameState prevGameState = STATE_SPLASH;
    
    static unsigned long splash2StartTime = 0;
    static int splash2Phase = 0;
    
    enum ConversationState { CONV_NONE, CONV_MSG1, CONV_MSG2, CONV_MSG3 };
    static ConversationState conversationState = CONV_NONE;
    static bool gateVisible = false;
    static bool nearBunny = false;
    static bool lastButtonAState = false;
    
    static bool gateOpening = false;
    static unsigned long gateOpenStartTime = 0;
    static int gateOpenAmount = 0;
    
    static int currentLevel = 1;
    static bool transitioning = false;
    static unsigned long transitionStartTime = 0;
    
    static int house_x = 0, house_y = 0, house_w = 0, house_h = 0;
    
    static bool isDigging = false;
    static unsigned long digStartTime = 0;
    static int dig_hole_x = 0;
    static int dig_hole_y = 0;
    
    static int carrot_x = 0, carrot_y = 0;
    static bool carrot_visible = false;
    static bool speedBoostActive = false;
    static unsigned long speedBoostStartTime = 0;
    static int carrot_w = 16, carrot_h = 20;
    
    static bool initialized = false;
    
    // Helper functions
    uint16_t rainbowColor(int i) {
        i %= 768; int r=0,g=0,b=0;
        if(i<256){r=255;g=i;b=0;}
        else if(i<512){r=511-i;g=255;b=0;}
        else{r=0;g=767-i;b=i-512;}
        return g_tft->color565(r,g,b);
    }
    
    bool loadBMPToSprite(const char* fname, TFT_eSprite* sprite) {
        File f=SD.open(fname); 
        if (!f) return false;
        f.seek(10);
        uint32_t pixelOffset = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        if (!f.seek(pixelOffset)) { f.close(); return false; }
        int w=sprite->width(), h=sprite->height();
        for(int y=0;y<h;y++) { 
            int spriteY=h-1-y;
            for(int x=0;x<w;x++) { 
                uint8_t b=f.read(),g=f.read(),r=f.read();
                sprite->drawPixel(x,spriteY,g_tft->color565(r,g,b)); 
            } 
        }
        f.close(); 
        return true;
    }
    
    bool loadBMPProperly(const char* fname, TFT_eSprite* sprite) {
        File f=SD.open(fname); 
        if (!f) return false;
        if (!f.seek(54)) { f.close(); return false; }
        int w=sprite->width(),h=sprite->height();
        for(int y=0;y<h;y++) { 
            int spriteY=h-1-y;
            for(int x=0;x<w;x++) { 
                uint8_t b=f.read(),g=f.read(),r=f.read();
                if(r>220&&g>220&&b>220)
                    sprite->drawPixel(x,spriteY,0x0001);
                else 
                    sprite->drawPixel(x,spriteY,g_tft->color565(r,g,b)); 
            } 
        }
        f.close(); 
        return true;
    }
    
    bool loadBMPSkipRows(const char* fname, TFT_eSprite* sprite, int skipRows) {
        File f = SD.open(fname); 
        if (!f) return false;
        f.seek(10);
        uint32_t pixelOffset = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        f.seek(18);
        int32_t bmpWidth = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        int32_t bmpHeight = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        int w = sprite->width(), h = sprite->height();
        for(int y = 0; y < h; y++) { 
            int bmpRow = skipRows + y;
            long rowPos = pixelOffset + (bmpHeight - 1 - bmpRow) * bmpWidth * 3;
            f.seek(rowPos);
            int spriteY = y;
            for(int x = 0; x < w; x++) { 
                uint8_t b = f.read(), g = f.read(), r = f.read();
                if(r > 220 && g > 220 && b > 220)
                    sprite->drawPixel(x, spriteY, 0x0001);
                else 
                    sprite->drawPixel(x, spriteY, g_tft->color565(r, g, b)); 
            } 
        }
        f.close(); 
        return true;
    }
    
    bool loadBMPWithTransparentCrop(const char* fname, TFT_eSprite* sprite) {
        File f=SD.open(fname); 
        if (!f) return false;
        if (!f.seek(54)) { f.close(); return false; }
        int w=sprite->width(), h=sprite->height();
        for(int y=0;y<h;y++) { 
            int spriteY=h-1-y;
            for(int x=0;x<w;x++) { 
                uint8_t b=f.read(),g=f.read(),r=f.read();
                if((r>220&&g>220&&b>220)||(r>240&&g<30&&b>240))
                    sprite->drawPixel(x,spriteY,0x0001);
                else 
                    sprite->drawPixel(x,spriteY,g_tft->color565(r,g,b)); 
            } 
        }
        f.close(); 
        return true;
    }
    
    bool loadBunnyFrameFromSD(int frameX, int frameY, int frameWidth, int frameHeight) {
        File f = SD.open("/bunny-hop-spritesheet.bmp");
        if (!f) return false;
        f.seek(10);
        uint32_t pixelOffset = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        f.seek(18);
        int32_t sheetWidth = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        int32_t sheetHeight = f.read() | (f.read()<<8) | (f.read()<<16) | (f.read()<<24);
        bunnyFrame->fillSprite(0x0001);
        for(int y = 0; y < frameHeight; y++) {
            for(int x = 0; x < frameWidth; x++) {
                int sheetX = frameX + x;
                int sheetY = frameY + y;
                long pixelPos = pixelOffset + ((sheetHeight - 1 - sheetY) * sheetWidth + sheetX) * 3;
                f.seek(pixelPos);
                uint8_t b = f.read(), g = f.read(), r = f.read();
                if(r > 250 && g > 250 && b > 250) {
                    bunnyFrame->drawPixel(x, y, 0x0001);
                } else {
                    bunnyFrame->drawPixel(x, y, g_tft->color565(r, g, b));
                }
            }
        }
        f.close();
        return true;
    }
    
    bool joywingButtonPressed() {
        g_ss->digitalReadBulk((uint32_t)1<<BUTTON_A|(uint32_t)1<<BUTTON_B|(uint32_t)1<<BUTTON_X|
            (uint32_t)1<<BUTTON_Y|(uint32_t)1<<BUTTON_SELECT);
        return !g_ss->digitalRead(BUTTON_A)||!g_ss->digitalRead(BUTTON_B)||!g_ss->digitalRead(BUTTON_X)||
               !g_ss->digitalRead(BUTTON_Y)||!g_ss->digitalRead(BUTTON_SELECT);
    }
    
    void drawCarrot(int x, int y) {
        uint16_t orange = g_tft->color565(255, 140, 0);
        uint16_t darkOrange = g_tft->color565(200, 100, 0);
        uint16_t green = g_tft->color565(0, 180, 0);
        for(int i = 0; i < 16; i++) {
            int width = 12 - (i / 2);
            if(width < 2) width = 2;
            uint16_t color = (i % 2 == 0) ? orange : darkOrange;
            g_tft->drawFastHLine(x + 8 - width/2, y + 4 + i, width, color);
        }
        g_tft->fillRect(x + 6, y, 4, 5, green);
        g_tft->fillRect(x + 4, y + 1, 2, 3, green);
        g_tft->fillRect(x + 10, y + 1, 2, 3, green);
    }
    
    void drawGrassTiled(int x0,int y0,int w,int h,TFT_eSprite* tile) { 
        for(int yy=0;yy<h;yy+=32)
            for(int xx=0;xx<w;xx+=32) 
                tile->pushSprite(x0+xx,y0+yy);
    }
    
    void drawClouds() {
        int sky_y = g_tft->height()*2/3;
        g_tft->fillEllipse(55, sky_y/3, 34, 13, g_tft->color565(230,230,245));
        g_tft->fillEllipse(110,sky_y/4, 25, 8, g_tft->color565(230,230,230));
        g_tft->fillEllipse(200,8+sky_y/7, 22, 8, g_tft->color565(210,220,240));
        g_tft->fillEllipse(270,sky_y/2, 25, 10, g_tft->color565(230,255,255));
        g_tft->fillEllipse(350,sky_y/2-3, 18, 5, g_tft->color565(250,250,255));
    }
    
    void drawSun(int sunX, int sunY) {
        int radius = 16;
        for(int r=radius; r>=0; --r) {
            uint8_t red=255, green=200-(radius-r)*4, blue=40-(radius-r)*3;
            if(green<140) green=140;
            if(blue<0 || blue>255) blue=0;
            g_tft->drawCircle(sunX, sunY, r, g_tft->color565(red, green, blue));
        }
        g_tft->fillCircle(sunX, sunY, radius-4, g_tft->color565(254,219,70));
    }
    
    void drawHole(int x, int y) {
        uint16_t veryDarkBrown = g_tft->color565(40, 25, 10);
        uint16_t darkBrown = g_tft->color565(60, 35, 15);
        uint16_t medBrown = g_tft->color565(80, 50, 25);
        uint16_t lightBrown = g_tft->color565(100, 65, 35);
        uint16_t rimBrown = g_tft->color565(120, 80, 40);
        g_tft->fillEllipse(x + 16, y + 24, 16, 12, veryDarkBrown);
        g_tft->fillEllipse(x + 16, y + 22, 15, 11, darkBrown);
        g_tft->fillEllipse(x + 16, y + 20, 14, 10, medBrown);
        g_tft->fillEllipse(x + 16, y + 18, 12, 8, lightBrown);
        g_tft->fillEllipse(x + 16, y + 16, 10, 6, rimBrown);
        g_tft->drawEllipse(x + 16, y + 16, 16, 11, g_tft->color565(60, 40, 20));
        g_tft->drawEllipse(x + 16, y + 16, 15, 10, g_tft->color565(50, 30, 15));
    }

} // namespace GordoGame
// PART 2 OF 4
namespace GordoGame {
    
    void loadTreeAndRocks() {
        const int PUR_R=124,PUR_G=75,PUR_B=129,PUR_RANGE=22;
        File f=SD.open("/tree1.bmp");
        if (f && f.seek(18)) {
            int w=f.read()|(f.read()<<8)|(f.read()<<16)|(f.read()<<24);
            int h=f.read()|(f.read()<<8)|(f.read()<<16)|(f.read()<<24); 
            f.seek(54);
            TFT_eSprite temp(g_tft); 
            temp.createSprite(w,h);
            for (int y=0; y<h; ++y) {
                int spriteY=h-1-y;
                for (int x=0; x<w; ++x) {
                    uint8_t b=f.read(),g=f.read(),r=f.read();
                    bool white=(r>220&&g>220&&b>220),black=(r<35&&g<35&&b<35),purple=(abs((int)r-PUR_R)<PUR_RANGE)&&(abs((int)g-PUR_G)<PUR_RANGE)&&(abs((int)b-PUR_B)<PUR_RANGE);
                    bool isTrunkish=(r>g+15&&g>b+10&&b<70&&r>59);
                    bool isLeafish=(g>100 && g>b+10 && g>r);
                    if (white||black||purple) temp.drawPixel(x,spriteY,0x0001);
                    else if (isTrunkish) temp.drawPixel(x,spriteY,g_tft->color565(117,67,32));
                    else if (isLeafish) temp.drawPixel(x,spriteY,g_tft->color565(90,180,60));
                    else temp.drawPixel(x,spriteY,g_tft->color565(r,g,b));
                }
            }
            f.close();
            int minX=w,minY=h,maxX=-1,maxY=-1;
            for (int y=0;y<h;++y)
                for(int x=0;x<w;++x)
                    if(temp.readPixel(x,y)!=0x0001){
                        if(x<minX)minX=x;if(x>maxX)maxX=x;if(y<minY)minY=y;if(y>maxY)maxY=y;
                    }
            int cropW = maxX-minX+1, cropH=maxY-minY+1;
            if(tree1) delete tree1;
            tree1=new TFT_eSprite(g_tft); 
            tree1->createSprite(cropW,cropH);
            for(int y=0; y<cropH;++y)
                for(int x=0;x<cropW;++x)
                    tree1->drawPixel(x,y,temp.readPixel(minX+x,minY+y));
            tree1_w=cropW;
            tree1_h=cropH;
        }
        rock1=new TFT_eSprite(g_tft); rock1->createSprite(54,23); rock1->fillSprite(0x0001); rock1_w=54; rock1_h=23;
        for(int y=0;y<23;++y)
            for(int x=0;x<54;++x){
                int dx=x-27,dy=y-11;
                if((dx*dx)*3+(dy*dy)*6<33*30)
                    rock1->drawPixel(x,y,g_tft->color565(96,92,80));
                else if((dx*dx)*3+(dy*dy)*6<36*34)
                    rock1->drawPixel(x,y,g_tft->color565(120,108,100));
            }
        rock2=new TFT_eSprite(g_tft); rock2->createSprite(36,15); rock2->fillSprite(0x0001); rock2_w=36; rock2_h=15;
        for(int y=0;y<15;++y)
            for(int x=0;x<36;++x){
                int dx=x-18,dy=y-7;
                if((dx*dx)*3+(dy*dy)*6<12*27)
                    rock2->drawPixel(x,y,g_tft->color565(120,100,100));
                else if((dx*dx)*3+(dy*dy)*6<14*29)
                    rock2->drawPixel(x,y,g_tft->color565(144,128,128));
            }
        rock3=new TFT_eSprite(g_tft); rock3->createSprite(36,15); rock3->fillSprite(0x0001); rock3_w=36; rock3_h=15;
        for(int y=0;y<15;++y)
            for(int x=0;x<36;++x){
                int dx=x-18,dy=y-7;
                if ((dx*dx)*3+(dy*dy)*6<12*27) 
                    rock3->drawPixel(x,y,g_tft->color565(120,100,100));
                else if ((dx*dx)*3+(dy*dy)*6<14*29) 
                    rock3->drawPixel(x,y,g_tft->color565(144,128,128));
            }
        rock4=new TFT_eSprite(g_tft); rock4->createSprite(60,26); rock4->fillSprite(0x0001); rock4_w=60; rock4_h=26;
        for(int y=0;y<26;++y)
            for(int x=0;x<60;++x){
                int dx=x-30,dy=y-13;
                if((dx*dx)*3+(dy*dy)*6<36*33)
                    rock4->drawPixel(x,y,g_tft->color565(96,92,80));
                else if((dx*dx)*3+(dy*dy)*6<39*36)
                    rock4->drawPixel(x,y,g_tft->color565(120,108,100));
            }
        rock5=new TFT_eSprite(g_tft); rock5->createSprite(40,18); rock5->fillSprite(0x0001); rock5_w=40; rock5_h=18;
        for(int y=0;y<18;++y)
            for(int x=0;x<40;++x){
                int dx=x-20,dy=y-9;
                if((dx*dx)*3+(dy*dy)*6<14*30)
                    rock5->drawPixel(x,y,g_tft->color565(120,100,100));
                else if((dx*dx)*3+(dy*dy)*6<16*32)
                    rock5->drawPixel(x,y,g_tft->color565(144,128,128));
            }
    }
    
    void drawScaledTree(int posX, int posY, int scale) {
        if(!tree1) return;
        for(int yy=0; yy<tree1_h; ++yy)
            for(int xx=0; xx<tree1_w; ++xx) {
                uint16_t c = tree1->readPixel(xx, yy);
                if(c != 0x0001) 
                    g_tft->fillRect(posX + xx*scale, posY + yy*scale, scale, scale, c);
            }
    }
    
    void drawScaledTreeUpperRight() {
        if(!tree1 || currentLevel != 1) return;
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        int grass_y = screenH * 2 / 3;
        int scale = 3;
        int destX = screenW - tree1_w * scale;
        int destY = grass_y - tree1_h * scale + 12;
        if(destY < 0) destY = 0;
        drawScaledTree(destX, destY, scale);
    }
    
    void drawRocks() {
        int screenW = g_tft->width(), screenH = g_tft->height();
        int grass_y = screenH * 2 / 3;
        if(currentLevel == 1) {
            int bottom_y = screenH;
            rock1_x = screenW/2 - rock1_w - 26;
            rock1_y = bottom_y - rock1_h - 8;
            rock2_x = rock1_x - 65;
            rock2_y = bottom_y - rock2_h - 18;
            rock3_x = rock1_x + rock1_w - (rock3_w*3)/4;
            rock3_y = rock1_y + rock1_h - (rock3_h*3)/4;
            if (rock1) rock1->pushSprite(rock1_x, rock1_y, 0x0001);
            if (rock2) rock2->pushSprite(rock2_x, rock2_y, 0x0001);
            if (rock3) rock3->pushSprite(rock3_x, rock3_y, 0x0001);
        } else if(currentLevel == 2) {
            rock4_x = screenW/3;
            rock4_y = grass_y + 20;
            rock5_x = screenW/2 + 40;
            rock5_y = grass_y + 35;
            if (rock4) rock4->pushSprite(rock4_x, rock4_y, 0x0001);
            if (rock5) rock5->pushSprite(rock5_x, rock5_y, 0x0001);
        }
    }
    
    bool checkRockCollision(int x, int y, int w, int h) {
        if(currentLevel == 1) {
            if (rock1 && x < rock1_x + rock1_w && x + w > rock1_x &&
                y < rock1_y + rock1_h && y + h > rock1_y) return true;
            if (rock2 && x < rock2_x + rock2_w && x + w > rock2_x &&
                y < rock2_y + rock2_h && y + h > rock2_y) return true;
            if (rock3 && x < rock3_x + rock3_w && x + w > rock3_x &&
                y < rock3_y + rock3_h && y + h > rock3_y) return true;
        } else if(currentLevel == 2) {
            if (rock4 && x < rock4_x + rock4_w && x + w > rock4_x &&
                y < rock4_y + rock4_h && y + h > rock4_y) return true;
            if (rock5 && x < rock5_x + rock5_w && x + w > rock5_x &&
                y < rock5_y + rock5_h && y + h > rock5_y) return true;
        }
        return false;
    }
    
   // PART 2 OF 4 (CONTINUED FROM drawFence)
    void drawFence(int gapAmount) {
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        int grass_y = screenH * 2 / 3;
        uint16_t white = TFT_WHITE;
        uint16_t gray = g_tft->color565(180, 180, 180);
        int fenceX = screenW - 12;
        int fenceTop = grass_y + 15;
        int fenceBottom = screenH - 25;
        int fenceHeight = fenceBottom - fenceTop;
        int gapCenter = fenceTop + fenceHeight / 2;
        int rail1Y = fenceTop + fenceHeight / 3;
        int rail2Y = fenceTop + (fenceHeight * 2) / 3;
        if(gapAmount > 0) {
            if(gapCenter - gapAmount/2 > rail1Y + 3) {
                g_tft->fillRect(fenceX - 8, rail1Y, 8, 3, white);
                g_tft->drawLine(fenceX - 8, rail1Y + 2, fenceX, rail1Y + 2, gray);
            }
            if(gapCenter + gapAmount/2 < fenceBottom) {
                int startY = gapCenter + gapAmount/2;
                if(startY < rail1Y + 3) startY = rail1Y + 3;
                g_tft->fillRect(fenceX - 8, startY, 8, 3, white);
                g_tft->drawLine(fenceX - 8, startY + 2, fenceX, startY + 2, gray);
            }
        } else {
            g_tft->fillRect(fenceX - 8, rail1Y, 8, 3, white);
            g_tft->drawLine(fenceX - 8, rail1Y + 2, fenceX, rail1Y + 2, gray);
        }
        if(gapAmount > 0) {
            if(gapCenter - gapAmount/2 > rail2Y + 3) {
                g_tft->fillRect(fenceX - 8, rail2Y, 8, 3, white);
                g_tft->drawLine(fenceX - 8, rail2Y + 2, fenceX, rail2Y + 2, gray);
            }
            if(gapCenter + gapAmount/2 < fenceBottom) {
                int startY = gapCenter + gapAmount/2;
                if(startY < rail2Y + 3) startY = rail2Y + 3;
                g_tft->fillRect(fenceX - 8, startY, 8, 3, white);
                g_tft->drawLine(fenceX - 8, startY + 2, fenceX, startY + 2, gray);
            }
        } else {
            g_tft->fillRect(fenceX - 8, rail2Y, 8, 3, white);
            g_tft->drawLine(fenceX - 8, rail2Y + 2, fenceX, rail2Y + 2, gray);
        }
        int picketSpacing = 7;
        for(int y = fenceTop; y <= fenceBottom; y += picketSpacing) {
            if(gapAmount > 0 && y >= gapCenter - gapAmount/2 && y <= gapCenter + gapAmount/2) continue;
            g_tft->fillRect(fenceX - 3, y, 3, 5, white);
            g_tft->drawLine(fenceX - 2, y, fenceX - 2, y + 5, gray);
            g_tft->drawPixel(fenceX - 3, y - 1, white);
            g_tft->drawPixel(fenceX - 2, y - 2, white);
        }
    }
    
    void drawBridge(int startX, bool leftHalf) {
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        uint16_t darkBrown = g_tft->color565(101, 67, 33);
        uint16_t medBrown = g_tft->color565(139, 90, 43);
        uint16_t lightBrown = g_tft->color565(160, 110, 60);
        int bridgeBaseY = screenH - 30;
        int halfWidth = 50;
        int plankHeight = 35;
        g_tft->fillRect(startX, bridgeBaseY - plankHeight, 3, plankHeight, darkBrown);
        g_tft->fillRect(startX + halfWidth - 3, bridgeBaseY - plankHeight, 3, plankHeight, darkBrown);
        for(int y = 0; y < plankHeight; y += 5) {
            int plankY = bridgeBaseY - y;
            g_tft->fillRect(startX, plankY, halfWidth, 3, medBrown);
            g_tft->drawLine(startX, plankY, startX + halfWidth, plankY, darkBrown);
        }
        g_tft->drawLine(startX + 5, bridgeBaseY - plankHeight + 5, startX + halfWidth - 5, bridgeBaseY - 5, lightBrown);
        g_tft->drawLine(startX + 5, bridgeBaseY - 5, startX + halfWidth - 5, bridgeBaseY - plankHeight + 5, lightBrown);
        g_tft->fillRect(startX, bridgeBaseY - plankHeight, halfWidth, 2, darkBrown);
    }
    
    void drawStonePath(int startX, bool rightSide) {
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        uint16_t stoneDark = g_tft->color565(80, 80, 80);
        uint16_t stoneMed = g_tft->color565(120, 120, 120);
        uint16_t stoneLit = g_tft->color565(150, 150, 150);
        int pathY = screenH - 35;
        int pathWidth = 60;
        int pathHeight = 30;
        
        // Main stone path
        g_tft->fillRect(startX, pathY, pathWidth, pathHeight, stoneMed);
        g_tft->drawRect(startX, pathY, pathWidth, pathHeight, stoneDark);
        
        // Stone blocks/tiles
        int blockSize = 15;
        for(int y = 0; y < pathHeight; y += blockSize) {
            for(int x = 0; x < pathWidth; x += blockSize) {
                g_tft->drawRect(startX + x, pathY + y, blockSize, blockSize, stoneDark);
                // Shading on top-left
                g_tft->drawLine(startX + x, pathY + y, startX + x + blockSize - 1, pathY + y, stoneLit);
                g_tft->drawLine(startX + x, pathY + y, startX + x, pathY + y + blockSize - 1, stoneLit);
            }
        }
        
        // Directional arrow pointing to next level
        int arrowX = startX + pathWidth / 2;
        int arrowY = pathY + pathHeight / 2;
        
        if(rightSide) {
            // Arrow pointing right
            g_tft->fillTriangle(arrowX - 5, arrowY - 8, arrowX + 8, arrowY, arrowX - 5, arrowY + 8, stoneLit);
            g_tft->drawTriangle(arrowX - 5, arrowY - 8, arrowX + 8, arrowY, arrowX - 5, arrowY + 8, stoneDark);
        } else {
            // Arrow pointing left
            g_tft->fillTriangle(arrowX + 5, arrowY - 8, arrowX - 8, arrowY, arrowX + 5, arrowY + 8, stoneLit);
            g_tft->drawTriangle(arrowX + 5, arrowY - 8, arrowX - 8, arrowY, arrowX + 5, arrowY + 8, stoneDark);
        }
    }

    void copyBirdFrame(TFT_eSprite& sheet, TFT_eSprite& frame, int frameIdx, int row) {
        int sx=frameIdx*FRAME_W, sy=row*FRAME_H;
        frame.fillSprite(0x0001);
        for(int y=0;y<FRAME_H;y++)
            for(int x=0;x<FRAME_W;x++)
                frame.drawPixel(x,y,sheet.readPixel(sx+x,sy+y));
    }
    
    void copyEagleFrame(int frameIdx, int row, bool flipVertical) {
        int sx = frameIdx * FRAME_W;
        int sy = row * FRAME_H;
        eagleFrame->fillSprite(0x0001);
        if(flipVertical) {
            for(int y=0; y<FRAME_H; y++)
                for(int x=0; x<FRAME_W; x++)
                    eagleFrame->drawPixel(x, FRAME_H - 1 - y, eagleSheet->readPixel(sx+x, sy+y));
        } else {
            for(int y=0; y<FRAME_H; y++)
                for(int x=0; x<FRAME_W; x++)
                    eagleFrame->drawPixel(x, y, eagleSheet->readPixel(sx+x, sy+y));
        }
    }
    
    bool isGordoNearBunny(int bunnyX, int bunnyY) {
        int distance = 60;
        return (abs(gordo_x - bunnyX) < distance && abs(gordo_y - bunnyY) < distance);
    }
    
    bool isGordoSafe() {
        if(isDigging) return true;
        if(currentLevel == 1) {
            if(gordo_x >= rock1_x - 10 && gordo_x <= rock1_x + rock1_w + 10 &&
               gordo_y >= rock1_y - 10 && gordo_y <= rock1_y + rock1_h + 10) return true;
            if(gordo_x >= rock2_x - 10 && gordo_x <= rock2_x + rock2_w + 10 &&
               gordo_y >= rock2_y - 10 && gordo_y <= rock2_y + rock2_h + 10) return true;
            if(gordo_x >= rock3_x - 10 && gordo_x <= rock3_x + rock3_w + 10 &&
               gordo_y >= rock3_y - 10 && gordo_y <= rock3_y + rock3_h + 10) return true;
        } else if(currentLevel == 2) {
            if(gordo_x >= rock4_x - 10 && gordo_x <= rock4_x + rock4_w + 10 &&
               gordo_y >= rock4_y - 10 && gordo_y <= rock4_y + rock4_h + 10) return true;
            if(gordo_x >= rock5_x - 10 && gordo_x <= rock5_x + rock5_w + 10 &&
               gordo_y >= rock5_y - 10 && gordo_y <= rock5_y + rock5_h + 10) return true;
        }
        return false;
    }
    
    bool checkEagleCollision() {
        if(!eagle_active) return false;
        if(eagle_x < gordo_x + FRAME_W && eagle_x + FRAME_W > gordo_x &&
           eagle_y < gordo_y + FRAME_H && eagle_y + FRAME_H > gordo_y) {
            return true;
        }
        return false;
    }
    
    void drawConversationBubble(String line1, String line2, int bunnyX, int bunnyY) {
        int bubbleW = 150;
        int bubbleH = 45;
        int bubbleX = bunnyX - 51;
        int bubbleY = bunnyY - 60;
        g_tft->fillRoundRect(bubbleX, bubbleY, bubbleW, bubbleH, 8, TFT_WHITE);
        g_tft->drawRoundRect(bubbleX, bubbleY, bubbleW, bubbleH, 8, TFT_BLACK);
        int tailX = bunnyX + 24;
        g_tft->fillTriangle(tailX - 6, bubbleY + bubbleH, tailX + 6, bubbleY + bubbleH,
                         tailX, bubbleY + bubbleH + 8, TFT_WHITE);
        g_tft->drawLine(tailX - 6, bubbleY + bubbleH, tailX, bubbleY + bubbleH + 8, TFT_BLACK);
        g_tft->drawLine(tailX + 6, bubbleY + bubbleH, tailX, bubbleY + bubbleH + 8, TFT_BLACK);
        g_tft->setTextColor(TFT_BLACK, TFT_WHITE);
        g_tft->setTextSize(1);
        g_tft->setCursor(bubbleX + 8, bubbleY + 8);
        g_tft->println(line1);
        g_tft->setCursor(bubbleX + 8, bubbleY + 20);
        g_tft->println(line2);
        g_tft->setTextSize(1);
        g_tft->setCursor(bubbleX + bubbleW - 20, bubbleY + bubbleH - 12);
        g_tft->print("(A)");
    }
    
    void drawBunny(int x, int y) {
        bunnyFrame->pushSprite(x, y, 0x0001);
        if(nearBunny && conversationState == CONV_NONE) {
            int circleX = x + 24;
            int circleY = y - 8;
            int circleRadius = 8;
            uint16_t darkGreen = g_tft->color565(0, 100, 0);
            g_tft->fillCircle(circleX, circleY, circleRadius, darkGreen);
            g_tft->drawCircle(circleX, circleY, circleRadius, TFT_BLACK);
            g_tft->setTextColor(TFT_WHITE, darkGreen);
            g_tft->setTextDatum(MC_DATUM);
            g_tft->drawString("A", circleX, circleY, 2);
        }
    }
    
    void updateGordoControl() {
        if(isDigging) {
            gordo_vx = 0;
            gordo_vy = 0;
            return;
        }
        static unsigned long lastRead=0; 
        static int animDir=DIR_RIGHT;
        if(millis()-lastRead<25)return; 
        lastRead=millis();
        uint16_t joy_x=g_ss->analogRead(3), joy_y=g_ss->analogRead(2);
        int dz=60,new_vx=0,new_vy=0;
        if(joy_x<512-dz){ 
            new_vx=-1;
            animDir=DIR_LEFT;
        } else if(joy_x>512+dz){ 
            new_vx=1;
            animDir=DIR_RIGHT;
        } else{ 
            new_vx=0; 
        }
        if(joy_y<512-dz){ 
            new_vy=-1;
        } else if(joy_y>512+dz){ 
            new_vy=1;
        } else{ 
            new_vy=0; 
        }
        gordo_dir=animDir; 
        gordo_vx=new_vx; 
        gordo_vy=new_vy;
    }

  
    void drawRainbow() {
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        int grass_y = screenH * 2 / 3;
        int rainbowEndX = house_x + 30;
        int rainbowStartX = 40;
        int centerX = (rainbowStartX + rainbowEndX) / 2;
        int centerY = grass_y;
        uint16_t colors[7] = {
            g_tft->color565(255, 0, 0), g_tft->color565(255, 127, 0), g_tft->color565(255, 255, 0),
            g_tft->color565(0, 255, 0), g_tft->color565(0, 0, 255), g_tft->color565(75, 0, 130),
            g_tft->color565(148, 0, 211)
        };
        int baseRadius = 140;
        int arcWidth = 4;
        for(int i = 6; i >= 0; i--) {
            int radius = baseRadius - (i * arcWidth);
            for(float angle = 3.14159; angle >= 0; angle -= 0.05) {
                int x1 = centerX + radius * cos(angle);
                int y1 = centerY - radius * sin(angle);
                int x2 = centerX + radius * cos(angle - 0.05);
                int y2 = centerY - radius * sin(angle - 0.05);
                if(y1 >= 0 && y1 < grass_y && y2 >= 0 && y2 < grass_y) {
                    for(int w = 0; w < arcWidth; w++) {
                        g_tft->drawLine(x1, y1 - w, x2, y2 - w, colors[i]);
                    }
                }
            }
        }
    }
    
    void drawFlowers() {
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        int grass_y = screenH * 2 / 3;
        uint16_t flowerColors[5] = {
            g_tft->color565(255, 0, 0), g_tft->color565(255, 192, 203), g_tft->color565(255, 255, 0),
            g_tft->color565(148, 0, 211), g_tft->color565(255, 165, 0)
        };
        uint16_t stemGreen = g_tft->color565(34, 139, 34);
        uint16_t centerYellow = g_tft->color565(255, 215, 0);
        int flowerPositions[12][2] = {
            {60, grass_y + 25}, {120, grass_y + 40}, {180, grass_y + 30}, {90, grass_y + 50},
            {150, grass_y + 55}, {210, grass_y + 45}, {240, grass_y + 35}, {270, grass_y + 50},
            {300, grass_y + 28}, {330, grass_y + 42}, {360, grass_y + 38}, {390, grass_y + 48}
        };
        for(int i = 0; i < 12; i++) {
            int x = flowerPositions[i][0];
            int y = flowerPositions[i][1];
            if(currentLevel == 4 && x > screenW - 80) continue;
            uint16_t petalColor = flowerColors[i % 5];
            g_tft->drawLine(x, y, x, y + 8, stemGreen);
            int petalRadius = 3;
            for(int p = 0; p < 5; p++) {
                float angle = p * 3.14159 * 2 / 5;
                int petalX = x + 5 * cos(angle);
                int petalY = y + 5 * sin(angle);
                g_tft->fillCircle(petalX, petalY, petalRadius, petalColor);
            }
            g_tft->fillCircle(x, y, 2, centerYellow);
        }
    }
    
    void drawHouse() {
        int screenW = g_tft->width();
        int screenH = g_tft->height();
        int grass_y = screenH * 2 / 3;
        uint16_t tan = g_tft->color565(210, 180, 140);
        uint16_t darkTan = g_tft->color565(180, 150, 110);
        uint16_t stone = g_tft->color565(120, 120, 120);
        uint16_t darkStone = g_tft->color565(80, 80, 80);
        uint16_t roofBrown = g_tft->color565(101, 67, 33);
        uint16_t blueDoor = g_tft->color565(70, 130, 180);
        uint16_t windowColor = g_tft->color565(135, 206, 250);
        uint16_t windowFrame = g_tft->color565(60, 40, 20);
        house_w = 120;
        house_h = 140;
        house_x = screenW - 60;
        house_y = grass_y - 60;
        int foundationH = 40;
        for(int y = 0; y < foundationH; y += 8) {
            for(int x = 0; x < 60; x += 12) {
                uint16_t color = (y % 16 < 8) ? stone : darkStone;
                g_tft->fillRect(house_x + x, grass_y + y, 10, 6, color);
            }
        }
        g_tft->fillRect(house_x, house_y, 60, 60, tan);
        for(int y = house_y; y < house_y + 60; y += 15) {
            g_tft->drawLine(house_x, y, house_x + 60, y, darkTan);
        }
        int doorW = 25, doorH = 40;
        int doorX = house_x + 8;
        int doorY = grass_y - doorH + 40;
        g_tft->fillRect(doorX, doorY, doorW, doorH, blueDoor);
        g_tft->drawRect(doorX, doorY, doorW, doorH, darkStone);
        g_tft->fillCircle(doorX + doorW - 5, doorY + doorH / 2, 2, g_tft->color565(212, 175, 55));
        int windowW = 18, windowH = 18;
        int windowX = doorX + 4;
        int windowY = house_y + 15;
        g_tft->fillRect(windowX, windowY, windowW, windowH, windowColor);
        g_tft->drawRect(windowX, windowY, windowW, windowH, windowFrame);
        g_tft->drawLine(windowX + windowW/2, windowY, windowX + windowW/2, windowY + windowH, windowFrame);
        g_tft->drawLine(windowX, windowY + windowH/2, windowX + windowW, windowY + windowH/2, windowFrame);
        int roofY = house_y - 20;
        for(int i = 0; i < 25; i++) {
            g_tft->drawLine(house_x, roofY + i, house_x + 60 + i, roofY + i, roofBrown);
        }
        int dormerX = house_x + 15, dormerY = house_y - 15, dormerW = 15, dormerH = 12;
        g_tft->fillRect(dormerX, dormerY, dormerW, dormerH, tan);
        g_tft->fillRect(dormerX + 2, dormerY + 2, dormerW - 4, dormerH - 6, windowColor);
        g_tft->drawRect(dormerX + 2, dormerY + 2, dormerW - 4, dormerH - 6, windowFrame);
        g_tft->fillTriangle(dormerX - 2, dormerY, dormerX + dormerW + 2, dormerY,
                         dormerX + dormerW/2, dormerY - 8, roofBrown);
    }
  void updateGordoMoveAndDraw() {
    int screenW=g_tft->width(),screenH=g_tft->height(),grass_y=screenH*2/3;
    
    if(last_gordo_x>=0&&last_gordo_y>=0) {
        if(last_gordo_y < grass_y) {
            g_tft->fillRect(last_gordo_x, last_gordo_y, FRAME_W, FRAME_H, g_tft->color565(140,198,236));
        } else {
            int startTileX = (last_gordo_x / 32) * 32;
            int startTileY = ((last_gordo_y / 32) * 32);
            
            for(int ty = startTileY; ty < last_gordo_y + FRAME_H; ty += 32) {
                for(int tx = startTileX; tx < last_gordo_x + FRAME_W; tx += 32) {
                    if(ty >= grass_y) {
                        grassTile->pushSprite(tx, ty);
                    }
                }
            }
            
            if(tree1 && currentLevel == 1) {
                int tree_x = screenW - tree1_w*3;
                int tree_y = grass_y - tree1_h*3 + 12;
                if(tree_y < 0) tree_y = 0;
                
                if(last_gordo_x < tree_x + tree1_w*3 && last_gordo_x + FRAME_W > tree_x &&
                   last_gordo_y < tree_y + tree1_h*3 && last_gordo_y + FRAME_H > tree_y) {
                    drawScaledTreeUpperRight();
                }
            }
            
            if(currentLevel == 1) {
                if (rock1) rock1->pushSprite(rock1_x, rock1_y, 0x0001);
                if (rock2) rock2->pushSprite(rock2_x, rock2_y, 0x0001);
                if (rock3) rock3->pushSprite(rock3_x, rock3_y, 0x0001);
            } else if(currentLevel == 2) {
                if (rock4) rock4->pushSprite(rock4_x, rock4_y, 0x0001);
                if (rock5) rock5->pushSprite(rock5_x, rock5_y, 0x0001);
            }
            
            if(currentLevel == 1) {
                drawFence(gateVisible ? 60 : gateOpenAmount);
            } else if(currentLevel == 2) {
                drawBridge(screenW - 50, true);
            } else if(currentLevel == 3) {
                drawBridge(0, false);
                drawStonePath(screenW - 60, true);
            } else if(currentLevel == 4) {
                if(last_gordo_y < grass_y) {
                    drawRainbow();
                }
                drawHouse();
            }
            
            int bunnyX = (screenW * 2) / 3;
            int bunnyY = screenH - 32 - 20;
            if(currentLevel == 1 && last_gordo_x < bunnyX + 48 && last_gordo_x + FRAME_W > bunnyX &&
               last_gordo_y < bunnyY + 32 && last_gordo_y + FRAME_H > bunnyY) {
                drawBunny(bunnyX, bunnyY);
            }
        }
    }
    
    int speedMultiplier = speedBoostActive ? 4 : 1;
    int new_gordo_x = gordo_x + gordo_vx * speedMultiplier;
    int new_gordo_y = gordo_y + gordo_vy * speedMultiplier;
    
    new_gordo_x = constrain(new_gordo_x, 0, screenW - FRAME_W);
    int grass_y_bot = screenH - FRAME_H;
    new_gordo_y = constrain(new_gordo_y, grass_y + 10, grass_y_bot);
    
    if (!checkRockCollision(new_gordo_x, new_gordo_y, FRAME_W, FRAME_H)) {
        gordo_x = new_gordo_x;
        gordo_y = new_gordo_y;
    }

    if((gordo_vx||gordo_vy)&&millis()-lastGordoAnim>150){
        gordo_anim_frame=(gordo_anim_frame+1)%GORDO_ANIM_FRAMES;
        lastGordoAnim=millis();
    } else if(!gordo_vx && !gordo_vy) 
        gordo_anim_frame=0;

    int row = (gordo_dir == DIR_RIGHT) ? 0 : 1;
    int frame = gordo_anim_frame;
    int sx = gordo_x_offsets[frame];
    int sy = gordo_row_ys[row];
    
    gordoFrame->fillSprite(0x0001);
    for(int y=0; y<FRAME_H; y++)
        for(int x=0; x<FRAME_W; x++)
            gordoFrame->drawPixel(x, y, gordoSheet->readPixel(sx + x, sy + y));
    
    gordoFrame->pushSprite(gordo_x, gordo_y, 0x0001);
    last_gordo_x=gordo_x; 
    last_gordo_y=gordo_y;
}

void drawSplashScreen() {
    g_tft->fillScreen(TFT_BLACK);
    g_tft->setTextSize(1);  // ← ADD THIS LINE to reset text size
    int screenW = g_tft->width(), screenH = g_tft->height();
    String title = "Gordo's Big Adventure";
    int fontHeight = 4, charWidth = 21;
    int startX = (screenW-title.length()*charWidth)/2, yTitle=18;
    int rainbowStep=768/title.length();
    for(unsigned i=0;i<title.length();++i) {
        uint16_t c=rainbowColor(i*rainbowStep);
        g_tft->setTextColor(c,TFT_BLACK);
        g_tft->drawCentreString(String(title[i]), startX + i*charWidth, yTitle, fontHeight);
    }
    if(gordoSplash) {
        int gx = (screenW-128)/2;
        int gy = screenH/2 - 64;
        for(int y=0; y<32; y++) {
            for(int x=0; x<32; x++) {
                uint16_t color = gordoSplash->readPixel(x, y);
                if(color != 0x0001) {
                    g_tft->fillRect(gx + x*4, gy + y*4, 4, 4, color);
                }
            }
        }
    }
    g_tft->setTextColor(TFT_WHITE,TFT_BLACK); 
    g_tft->setTextDatum(MC_DATUM);
    g_tft->drawString("Help Gordo find his way home",screenW/2, screenH/2+84, 4);
    g_tft->setTextColor(TFT_YELLOW,TFT_BLACK); 
    g_tft->drawString("Press any button to continue",screenW/2, screenH-37,2);
}

    void drawScene() {
        int screenW = g_tft->width(), screenH = g_tft->height(), grass_y = screenH*2/3;
        g_tft->fillScreen(g_tft->color565(140,198,236));
        drawGrassTiled(0, grass_y, screenW, screenH-grass_y, grassTile);
        int last_tile_x = (screenW/32)*32, last_tile_y = (screenH/32)*32;
        for(int xx=last_tile_x; xx<screenW; xx+=32)
            for(int yy=last_tile_y; yy<screenH; yy+=32) 
                grassTile->pushSprite(xx, yy);
        drawClouds();
        if(currentLevel == 3) {
            drawSun(screenW/2, 24);
        } else if(currentLevel == 4) {
            drawSun(24, 24);
        } else {
            drawSun(screenW - 24, 24);
            if(currentLevel == 1) {
                drawScaledTreeUpperRight();
            }
        }
        drawRocks();
        if(currentLevel == 3 && carrot_visible) {
            drawCarrot(carrot_x, carrot_y);
        }
        if(currentLevel == 4) {
            drawRainbow(); 
            drawFlowers();
            drawHouse();
        }
        if(currentLevel == 1) {
            drawFence(gateVisible ? 60 : 0);
        } else if(currentLevel == 2) {
            drawBridge(screenW - 50, true);
        } else if(currentLevel == 3) {
            drawBridge(0, false);
            drawStonePath(screenW - 60, true); 
        }
        int row = (gordo_dir == DIR_RIGHT) ? 0 : 1;
        int sx = gordo_x_offsets[0];
        int sy = gordo_row_ys[row];
        gordoFrame->fillSprite(0x0001);
        for(int y=0; y<FRAME_H; y++)
            for(int x=0; x<FRAME_W; x++)
                gordoFrame->drawPixel(x, y, gordoSheet->readPixel(sx + x, sy + y));
        gordoFrame->pushSprite(gordo_x, gordo_y, 0x0001);
        if(currentLevel == 1) {
            int bunnyX = (screenW * 2) / 3;
            int bunnyY = screenH - 32 - 20;
            drawBunny(bunnyX, bunnyY);
        }
    }

} // END namespace GordoGame
// PART 3 OF 4 - FIXED with GordoGame:: prefixes
void run_GordosBigAdventure(TFT_eSPI &tft, Adafruit_seesaw &ss) {
    using namespace GordoGame;
    
    g_tft = &tft;
    g_ss = &ss;
    
    if (!initialized) {
        Serial.println("\n=== GORDO'S BIG ADVENTURE ===");
        
        SPI.begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
        if (!SD.begin(PIN_SD_CS)) {
            Serial.println("SD Card failed!");
            tft.fillScreen(TFT_RED);
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("SD CARD ERROR!", 240, 160, 4);
            delay(3000);
            return;
        }
        
        Serial.println("Loading sprites...");
        gordoSheet = new TFT_eSprite(&tft);
        gordoSheet->createSprite(128, 85); 
        loadBMPWithTransparentCrop("/Gordo2.bmp", gordoSheet);
        
        gordoFrame = new TFT_eSprite(&tft);
        gordoFrame->createSprite(32,32);
        
        gordoSplash = new TFT_eSprite(&tft); 
        gordoSplash->createSprite(32,32);
        loadBMPProperly("/Gordo1.bmp", gordoSplash);
        
        grassTile = new TFT_eSprite(&tft);
        grassTile->createSprite(32,32); 
        loadBMPToSprite("/Grass.bmp", grassTile);
        
        loadTreeAndRocks();
        
        blueSheet = new TFT_eSprite(&tft);
        blueSheet->createSprite(96,256); 
        blueFrame = new TFT_eSprite(&tft);
        blueFrame->createSprite(32,32); 
        loadBMPProperly("/bird.bmp", blueSheet);
        
        cardinalSheet = new TFT_eSprite(&tft);
        cardinalSheet->createSprite(96,256); 
        cardinalFrame = new TFT_eSprite(&tft);
        cardinalFrame->createSprite(32,32); 
        loadBMPProperly("/bird_2_cardinal.bmp", cardinalSheet);
        
        Serial.println("Loading eagle sprite...");
        eagleSheet = new TFT_eSprite(&tft);
        eagleSheet->createSprite(96, 128);
        eagleFrame = new TFT_eSprite(&tft);
        eagleFrame->createSprite(32, 32); 
        loadBMPSkipRows("/eagle.bmp", eagleSheet, 0);
        
        Serial.println("Loading bunny frame...");
        bunnyFrame = new TFT_eSprite(&tft);
        bunnyFrame->createSprite(48, 32);
        loadBunnyFrameFromSD(0, 0, 48, 32);
        
        int screenW=tft.width(),screenH=tft.height();
        GordoGame::gordo_x = screenW/4; 
        GordoGame::gordo_y = screenH*2/3+13; 
        GordoGame::gordo_dir = GordoGame::DIR_RIGHT; 
        GordoGame::gordo_anim_frame = 0; 
        GordoGame::gordo_vx = GordoGame::gordo_vy = 0;
        GordoGame::last_gordo_x = GordoGame::last_gordo_y = -1;
        
        blue_x=screenW-FRAME_W; 
        blue_y=screenH/3; 
        blue_vx=-2; 
        blue_anim=0;
        
        cardinal_x=-FRAME_W; 
        cardinal_y=blue_y-FRAME_H-8; 
        if(cardinal_y<0)cardinal_y=0; 
        cardinal_vx=2; 
        cardinal_anim=0; 
        cardinal_active=false; 
        last_cardinal_time=millis();
        
        carrot_x = screenW / 4;
        carrot_y = screenH * 2 / 3 + 30;
        carrot_visible = false;
        
        Serial.println("Setup complete!");
        drawSplashScreen();
        initialized = true;
    }
    
    bool exitGame = false;
    static bool wasPressed = false;
    
    while (!exitGame) {
        // **CHECK SELECT BUTTON TO EXIT**
        if (!ss.digitalRead(BUTTON_SELECT)) {
            Serial.println("SELECT pressed - returning to menu");
            delay(200);
            
            // Reset game state
            initialized = false;
            gameState = STATE_SPLASH;
            prevGameState = STATE_SPLASH;
            currentLevel = 1;
            conversationState = CONV_NONE;
            gateVisible = false;
            gateOpening = false;
            transitioning = false;
            carrot_visible = false;
            speedBoostActive = false;
            eagle_active = false;
            eagle2_active = false;
            isDigging = false;
            
            int screenW = tft.width();
            int screenH = tft.height();
            GordoGame::gordo_x = screenW/4; 
            GordoGame::gordo_y = screenH*2/3 + 13;
            GordoGame::gordo_dir = GordoGame::DIR_RIGHT; 
            GordoGame::gordo_anim_frame = 0; 
            GordoGame::gordo_vx = 0;
            GordoGame::gordo_vy = 0;
            GordoGame::last_gordo_x = -1;
            GordoGame::last_gordo_y = -1;
            
            tft.fillScreen(TFT_BLACK);
            break;
        }
        
    if (gameState == STATE_SPLASH) {
    if (prevGameState != STATE_SPLASH) { 
        drawSplashScreen(); 
        prevGameState = STATE_SPLASH; 
    }
    bool pressed = joywingButtonPressed();
    if (pressed && !wasPressed) { 
        delay(200); 
        gameState = STATE_SCENE;
        // Don't set prevGameState here - let the scene draw on next loop
    }
    wasPressed = pressed; 
    delay(20); 
    continue;
}
        
        if (prevGameState != STATE_SCENE) { 
            Serial.println("Drawing initial scene");
            drawScene(); 
            prevGameState = STATE_SCENE; 
        }
        
        int screenW=tft.width(),screenH=tft.height();
        int bunnyX = (screenW * 2) / 3;
        int bunnyY = screenH - 32 - 20;
        
        nearBunny = isGordoNearBunny(bunnyX, bunnyY);
        
        ss.digitalReadBulk((uint32_t)1<<BUTTON_A|(uint32_t)1<<BUTTON_B|(uint32_t)1<<BUTTON_X|
            (uint32_t)1<<BUTTON_Y|(uint32_t)1<<BUTTON_SELECT);
        
        bool buttonAPressed = !ss.digitalRead(BUTTON_A);
        
        if(buttonAPressed && !lastButtonAState) {
            if(currentLevel == 1 && nearBunny) {
                if(conversationState == CONV_NONE) {
                    conversationState = CONV_MSG1;
                } else if(conversationState == CONV_MSG1) {
                    conversationState = CONV_MSG2;
                } else if(conversationState == CONV_MSG2) {
                    conversationState = CONV_MSG3;
} else if(conversationState == CONV_MSG3) {
    conversationState = CONV_NONE;
    gateOpening = true;
    gateOpenStartTime = millis();
    gateOpenAmount = 0;
    drawScene();  // ← ADD THIS to clear the bubble
}
            } else if(!isDigging && !nearBunny) {
                Serial.println("Gordo is digging!");
                isDigging = true;
                digStartTime = millis();
                dig_hole_x = GordoGame::gordo_x;
                dig_hole_y = GordoGame::gordo_y;
                drawHole(dig_hole_x, dig_hole_y);
            }
        }
        lastButtonAState = buttonAPressed;
        
        if(isDigging) {
            unsigned long elapsed = millis() - digStartTime;
            if(elapsed >= 1500) {
                Serial.println("Gordo emerges from hole!");
                isDigging = false;
                int grass_y = screenH * 2 / 3;
                int startTileX = (dig_hole_x / 32) * 32;
                int startTileY = ((dig_hole_y / 32) * 32);
                for(int ty = startTileY; ty < dig_hole_y + FRAME_H; ty += 32) {
                    for(int tx = startTileX; tx < dig_hole_x + FRAME_W; tx += 32) {
                        if(ty >= grass_y) {
                            grassTile->pushSprite(tx, ty);
                        }
                    }
                }
                if(currentLevel == 1) {
                    if (rock1) rock1->pushSprite(rock1_x, rock1_y, 0x0001);
                    if (rock2) rock2->pushSprite(rock2_x, rock2_y, 0x0001);
                    if (rock3) rock3->pushSprite(rock3_x, rock3_y, 0x0001);
                } else if(currentLevel == 2) {
                    if (rock4) rock4->pushSprite(rock4_x, rock4_y, 0x0001);
                    if (rock5) rock5->pushSprite(rock5_x, rock5_y, 0x0001);
                }
                GordoGame::last_gordo_x = -1;
                GordoGame::last_gordo_y = -1;
            }
        }
        
        if(currentLevel == 1 && gateVisible && !transitioning && GordoGame::gordo_x >= screenW - 60) {
            Serial.println("*** STARTING TRANSITION TO LEVEL 2! ***");
            transitioning = true;
            transitionStartTime = millis();
        }
        
        if(currentLevel == 2 && !transitioning && GordoGame::gordo_x >= screenW - 50) {
            Serial.println("*** STARTING TRANSITION TO LEVEL 3! ***");
            transitioning = true;
            transitionStartTime = millis();
        }
        
        if(currentLevel == 3 && !transitioning && GordoGame::gordo_x >= screenW - 60) {
            Serial.println("*** STARTING TRANSITION TO LEVEL 4! ***");
            transitioning = true;
            transitionStartTime = millis();
        }
        
        if(transitioning) {
            unsigned long elapsed = millis() - transitionStartTime;
            if(elapsed < 800) {
                tft.fillScreen(TFT_BLACK);
                delay(20);
                continue;
            } else {
                if(currentLevel == 1) {
                    Serial.println("Loading Level 2...");
                    currentLevel = 2;
                } else if(currentLevel == 2) {
                    Serial.println("Loading Level 3...");
                    currentLevel = 3;
                    carrot_visible = true;
                } else if(currentLevel == 3) {
                    Serial.println("Loading Level 4...");
                    currentLevel = 4;
                }
                GordoGame::gordo_x = 20;
                GordoGame::gordo_y = screenH * 2 / 3 + 13;
                GordoGame::gordo_dir = GordoGame::DIR_RIGHT;
                GordoGame::gordo_anim_frame = 0;
                GordoGame::gordo_vx = 0;
                GordoGame::gordo_vy = 0;
                GordoGame::last_gordo_x = -1;
                GordoGame::last_gordo_y = -1;
                eagle_active = false;
                eagle2_active = false;
                lastEagleAttack = millis();
                lastEagle2Attack = millis() + 2500;
                transitioning = false;
                drawScene();
                delay(100);
            }
        }
        
if(transitioning) {
    continue;
}

updateGordoControl();
updateGordoMoveAndDraw();  // ← ADD THIS LINE

if(currentLevel == 3 && carrot_visible) {
            if(GordoGame::gordo_x < carrot_x + carrot_w && GordoGame::gordo_x + FRAME_W > carrot_x &&
               GordoGame::gordo_y < carrot_y + carrot_h && GordoGame::gordo_y + FRAME_H > carrot_y) {
                Serial.println("Gordo ate the carrot! SPEED BOOST!");
                carrot_visible = false;
                speedBoostActive = true;
                speedBoostStartTime = millis();
            }
            if(carrot_visible) {
                drawCarrot(carrot_x, carrot_y);
            }
        }
        
        if(speedBoostActive && millis() - speedBoostStartTime > 10000) {
            Serial.println("Speed boost ended");
            speedBoostActive = false;
        }
        
        if(currentLevel == 4 && GordoGame::gordo_x >= screenW - 70) {
            Serial.println("*** GORDO REACHED HOME! ***");
            gameState = STATE_END;
            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("Gordo made it home!", screenW/2, 70, 4);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.drawString("Press SELECT to return to menu", screenW/2, screenH - 20, 2);
            delay(3000);
            continue;
        }
        
        if(currentLevel == 1) {
            uint16_t skyColor=tft.color565(140,198,236);
            tft.fillRect(last_blue_x,last_blue_y,FRAME_W,FRAME_H,skyColor);
            if(cardinal_active) tft.fillRect(last_cardinal_x,last_cardinal_y,FRAME_W,FRAME_H,skyColor);
            blue_x+=blue_vx; 
            if(blue_x<-FRAME_W)blue_x=tft.width();
            if(millis()-lastBlueAnim>80){ 
                blue_anim=(blue_anim+1)%BIRD_ANIM_FRAMES; 
                lastBlueAnim=millis();
            }
            copyBirdFrame(*blueSheet, *blueFrame, blue_anim, 0); 
            blueFrame->pushSprite(blue_x,blue_y,0x0001);
            last_blue_x=blue_x; 
            last_blue_y=blue_y;
            if(!cardinal_active&&millis()-last_cardinal_time>4200){
                cardinal_active=true;
                cardinal_x=-FRAME_W;
            }
            if(cardinal_active){
                cardinal_x+=cardinal_vx;
                if(millis()-lastCardinalAnim>100){
                    cardinal_anim=(cardinal_anim+1)%BIRD_ANIM_FRAMES; 
                    lastCardinalAnim=millis();
                }
                copyBirdFrame(*cardinalSheet, *cardinalFrame, cardinal_anim, 3); 
                cardinalFrame->pushSprite(cardinal_x,cardinal_y,0x0001);
                last_cardinal_x=cardinal_x; 
                last_cardinal_y=cardinal_y;
                if(cardinal_x>tft.width()){
                    cardinal_active=false; 
                    last_cardinal_time=millis();
                }
            }
        }
        // Eagle logic (Level 2 and 3 only)
        if(currentLevel == 2 || currentLevel == 3) {
            if(!eagle_active && millis() - lastEagleAttack > 5000) {
                eagle_active = true;
                eagle_anim = 0;
                
                int side = random(0, 2);
                int grass_y = screenH * 2 / 3;
                int sky_height = grass_y - 40;
                
                if(side == 0) {
                    eagle_x = -FRAME_W;
                    eagle_y = random(sky_height / 3, sky_height / 2);
                    eagle_vx = 3;
                    eagle_vy = 0;
                    eagle_direction = 3;
                } else {
                    eagle_x = screenW;
                    eagle_y = random(sky_height / 3, sky_height / 2);
                    eagle_vx = -3;
                    eagle_vy = 0;
                    eagle_direction = 0;
                }
            }
            
            if(eagle_active) {
                int grass_y = screenH * 2 / 3;
                bool aboveGordo = (eagle_x > GordoGame::gordo_x - 30 && eagle_x < GordoGame::gordo_x + 30 && eagle_y < grass_y - 20);
                
                if(eagle_direction == 3 || eagle_direction == 0) {
                    if(aboveGordo) {
                        eagle_vx = 0;
                        eagle_vy = 3;
                        eagle_direction = 1;
                    }
                } else if(eagle_direction == 1) {
                    if(eagle_x < GordoGame::gordo_x - 5) {
                        eagle_vx = 1;
                    } else if(eagle_x > GordoGame::gordo_x + 5) {
                        eagle_vx = -1;
                    } else {
                        eagle_vx = 0;
                    }
                } else if(eagle_direction == 2) {
                    eagle_vx = 0;
                }
                if(eagle_active) {
                int old_eagle_x = eagle_x;  // ← ADD THIS
                int old_eagle_y = eagle_y;  // ← ADD THIS
                int grass_y = screenH * 2 / 3;
                bool aboveGordo = (eagle_x > GordoGame::gordo_x - 30 && eagle_x < GordoGame::gordo_x + 30 && eagle_y < grass_y - 20);
                
                if(eagle_direction == 3 || eagle_direction == 0) {
                    if(aboveGordo) {
                        eagle_vx = 0;
                        eagle_vy = 3;
                        eagle_direction = 1;
                    }
                } else if(eagle_direction == 1) {
                    if(eagle_x < GordoGame::gordo_x - 5) {
                        eagle_vx = 1;
                    } else if(eagle_x > GordoGame::gordo_x + 5) {
                        eagle_vx = -1;
                    } else {
                        eagle_vx = 0;
                    }
                } else if(eagle_direction == 2) {
                    eagle_vx = 0;
                }
                
// *** ADD THIS BLOCK to clear old position ***
                if(old_eagle_y < grass_y) {
                    // Eagle was in sky - fill with sky color and redraw clouds
                    tft.fillRect(old_eagle_x, old_eagle_y, FRAME_W, FRAME_H, tft.color565(140,198,236));
                    drawClouds();  // Redraw clouds in case eagle erased them
                    // Redraw sun appropriate to current level
                    if(currentLevel == 2) {
                        drawSun(screenW - 24, 24);
                    } else if(currentLevel == 3) {
                        drawSun(screenW/2, 24);
                    }
                    
                    // Also redraw top grass tiles in case eagle bounced near the boundary
                    int startTileX = ((old_eagle_x - 32) / 32) * 32;
                    if(startTileX < 0) startTileX = 0;
                    for(int tx = startTileX; tx <= old_eagle_x + FRAME_W + 32; tx += 32) {
                        if(tx < screenW) {
                            grassTile->pushSprite(tx, grass_y);
                        }
                    }
                } else {
                    // Eagle was over grass - redraw grass tiles generously
                    int startTileX = ((old_eagle_x - 32) / 32) * 32;
                    int startTileY = ((old_eagle_y - 32) / 32) * 32;
                    if(startTileX < 0) startTileX = 0;
                    if(startTileY < grass_y) startTileY = grass_y;
                    
                    for(int ty = startTileY; ty <= old_eagle_y + FRAME_H + 32; ty += 32) {
                        for(int tx = startTileX; tx <= old_eagle_x + FRAME_W + 32; tx += 32) {
                            if(ty >= grass_y && tx < screenW) {
                                grassTile->pushSprite(tx, ty);
                            }
                        }
                    }
                    
                    // Redraw background elements that might have been erased
                    if(currentLevel == 2) {
                        if(rock4 && old_eagle_x < rock4_x + rock4_w + 32 && old_eagle_x + FRAME_W + 32 > rock4_x &&
                           old_eagle_y < rock4_y + rock4_h + 32 && old_eagle_y + FRAME_H + 32 > rock4_y) {
                            rock4->pushSprite(rock4_x, rock4_y, 0x0001);
                        }
                        if(rock5 && old_eagle_x < rock5_x + rock5_w + 32 && old_eagle_x + FRAME_W + 32 > rock5_x &&
                           old_eagle_y < rock5_y + rock5_h + 32 && old_eagle_y + FRAME_H + 32 > rock5_y) {
                            rock5->pushSprite(rock5_x, rock5_y, 0x0001);
                        }
                    }
                }
                // *** END OF CLEARING CODE ***
                

                eagle_x += eagle_vx;
                eagle_y += eagle_vy;
                
                if(eagle_direction == 1) {
                    if(isDigging && 
                       eagle_x < dig_hole_x + FRAME_W && eagle_x + FRAME_W > dig_hole_x &&
                       eagle_y < dig_hole_y + FRAME_H && eagle_y + FRAME_H > dig_hole_y) {
                        eagle_vy = -4;
                        eagle_vx = 0;
                        eagle_direction = 2;
                    }
                    
                    if(currentLevel == 2) {
                        if(rock4 && eagle_x < rock4_x + rock4_w && eagle_x + FRAME_W > rock4_x &&
                           eagle_y < rock4_y + rock4_h && eagle_y + FRAME_H > rock4_y) {
                            eagle_vy = -4;
                            eagle_vx = 0;
                            eagle_direction = 2;
                        }
                        if(rock5 && eagle_x < rock5_x + rock5_w && eagle_x + FRAME_W > rock5_x &&
                           eagle_y < rock5_y + rock5_h && eagle_y + FRAME_H > rock5_y) {
                            eagle_vy = -4;
                            eagle_vx = 0;
                            eagle_direction = 2;
                        }
                    }
                }
                
                if(millis() - lastEagleAnim > 100) {
                    eagle_anim = (eagle_anim + 1) % 3;
                    lastEagleAnim = millis();
                }
                
                bool flipVertical = (eagle_direction == 1);
                copyEagleFrame(eagle_anim, eagle_direction, flipVertical);
                eagleFrame->pushSprite(eagle_x, eagle_y, 0x0001);
                
                if(checkEagleCollision() && !isGordoSafe()) {
                    tft.fillScreen(TFT_RED);
                    delay(200);
                    tft.fillScreen(TFT_BLACK);
                    delay(300);
                    GordoGame::gordo_x = 20;
                    GordoGame::gordo_y = screenH * 2 / 3 + 13;
                    GordoGame::gordo_dir = GordoGame::DIR_RIGHT;
                    GordoGame::gordo_vx = 0;
                    GordoGame::gordo_vy = 0;
                    GordoGame::last_gordo_x = -1;
                    GordoGame::last_gordo_y = -1;
                    eagle_active = false;
                    lastEagleAttack = millis();
                    isDigging = false;
                    drawScene();
                }
                
                if(eagle_x < -FRAME_W - 20 || eagle_x > screenW + 20 || eagle_y < -FRAME_H * 2) {
                    eagle_active = false;
                    lastEagleAttack = millis();
                }
                
                if(eagle_y > screenH - FRAME_H && eagle_direction == 1) {
                    eagle_vy = -3;
                    eagle_vx = 0;
                    eagle_direction = 2;
                }
            }
        }
        }

        // Second eagle for level 3
    if(currentLevel == 3) {
        if(!eagle2_active && millis() - lastEagle2Attack > 6500) {
            eagle2_active = true;
            eagle2_anim = 0;
            
            int side = random(0, 2);
            int grass_y = screenH * 2 / 3;
            int sky_height = grass_y - 40;
            
            if(side == 0) {
                eagle2_x = -FRAME_W;
                eagle2_y = random(sky_height / 4, sky_height / 3);
                eagle2_vx = 2;
                eagle2_vy = 0;
                eagle2_direction = 3;
            } else {
                eagle2_x = screenW;
                eagle2_y = random(sky_height / 4, sky_height / 3);
                eagle2_vx = -2;
                eagle2_vy = 0;
                eagle2_direction = 0;
            }
        }
        
        if(eagle2_active) {
            int old_eagle2_x = eagle2_x;
            int old_eagle2_y = eagle2_y;
            int grass_y = screenH * 2 / 3;
            
            if(eagle2_direction == 3 || eagle2_direction == 0) {
                bool aboveGordo = (eagle2_x > GordoGame::gordo_x - 40 && eagle2_x < GordoGame::gordo_x + 40 && eagle2_y < grass_y - 20);
                if(aboveGordo) {
                    eagle2_vx = 0;
                    eagle2_vy = 2;
                    eagle2_direction = 1;
                }
            } else if(eagle2_direction == 1) {
                if(eagle2_x < GordoGame::gordo_x - 5) {
                    eagle2_vx = 1;
                } else if(eagle2_x > GordoGame::gordo_x + 5) {
                    eagle2_vx = -1;
                } else {
                    eagle2_vx = 0;
                }
            } else if(eagle2_direction == 2) {
                eagle2_vx = 0;
            }
            
            // Clear old position
            if(old_eagle2_y < grass_y) {
                tft.fillRect(old_eagle2_x, old_eagle2_y, FRAME_W, FRAME_H, tft.color565(140,198,236));
                drawClouds();
                drawSun(screenW/2, 24);
                int startTileX = ((old_eagle2_x - 32) / 32) * 32;
                if(startTileX < 0) startTileX = 0;
                for(int tx = startTileX; tx <= old_eagle2_x + FRAME_W + 32; tx += 32) {
                    if(tx < screenW) {
                        grassTile->pushSprite(tx, grass_y);
                    }
                }
            }
            
            eagle2_x += eagle2_vx;
            eagle2_y += eagle2_vy;
            
            if(eagle2_direction == 1) {
                if(isDigging && eagle2_x < dig_hole_x + FRAME_W && eagle2_x + FRAME_W > dig_hole_x &&
                   eagle2_y < dig_hole_y + FRAME_H && eagle2_y + FRAME_H > dig_hole_y) {
                    eagle2_vy = -3;
                    eagle2_vx = 0;
                    eagle2_direction = 2;
                }
            }
            
            if(millis() - lastEagle2Anim > 100) {
                eagle2_anim = (eagle2_anim + 1) % 3;
                lastEagle2Anim = millis();
            }
            
            bool flipVertical = (eagle2_direction == 1);
            copyEagleFrame(eagle2_anim, eagle2_direction, flipVertical);
            eagleFrame->pushSprite(eagle2_x, eagle2_y, 0x0001);
            
            if(eagle2_x < GordoGame::gordo_x + FRAME_W && eagle2_x + FRAME_W > GordoGame::gordo_x &&
               eagle2_y < GordoGame::gordo_y + FRAME_H && eagle2_y + FRAME_H > GordoGame::gordo_y && !isGordoSafe()) {
                tft.fillScreen(TFT_RED);
                delay(200);
                tft.fillScreen(TFT_BLACK);
                delay(300);
                GordoGame::gordo_x = 20;
                GordoGame::gordo_y = screenH * 2 / 3 + 13;
                GordoGame::gordo_vx = 0;
                GordoGame::gordo_vy = 0;
                GordoGame::last_gordo_x = -1;
                GordoGame::last_gordo_y = -1;
                eagle2_active = false;
                lastEagle2Attack = millis();
                isDigging = false;
                drawScene();
            }
            
            if(eagle2_x < -FRAME_W - 20 || eagle2_x > screenW + 20 || eagle2_y < -FRAME_H * 2) {
                eagle2_active = false;
                lastEagle2Attack = millis();
            }
            
            if(eagle2_y > screenH - FRAME_H && eagle2_direction == 1) {
                eagle2_vy = -2;
                eagle2_vx = 0;
                eagle2_direction = 2;
            }
        }
    }
        if(gateOpening) {
            unsigned long elapsed = millis() - gateOpenStartTime;
            gateOpenAmount = min(60, (int)(elapsed / 10));
            int grass_y = screenH * 2 / 3;
            int fenceX = screenW - 20;
            tft.fillRect(fenceX - 10, grass_y + 10, 20, screenH - grass_y - 35, tft.color565(98, 147, 23));
            drawFence(gateOpenAmount);
            if(gateOpenAmount >= 60) {
                gateOpening = false;
                gateVisible = true;
            }
        }
        
        if(currentLevel == 1) {
            int bunnyDrawX = (screenW * 2) / 3;
            int bunnyDrawY = screenH - 32 - 20;
            drawBunny(bunnyDrawX, bunnyDrawY);
            if(conversationState == CONV_MSG1) {
                drawConversationBubble("Watch out for", "Eagles!", bunnyX, bunnyY);
            } else if(conversationState == CONV_MSG2) {
                drawConversationBubble("Hide behind rocks", "or dig a hole", bunnyX, bunnyY);
            } else if(conversationState == CONV_MSG3) {
                drawConversationBubble("Good luck!", "", bunnyX, bunnyY);
            }
        }
        
        delay(16);
    }
    
    Serial.println("Game exited cleanly");
}

#endif // GORDOS_BIG_ADVENTURE_H