#pragma once
#include <TFT_eSPI.h>
#include <Adafruit_seesaw.h>

#define TFT_LIGHTPURPLE 0xB4FC
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
#define MAZE_SIZE 21
#define NUM_BADGUYS 5
#define ORANGE_IDX 2
#define BLUE_IDX   1
#define PURPLE_IDX 3
#define RED_IDX    0

struct BadGuy {
  int x, y;
  int spawn_x, spawn_y;
  uint16_t color;
  bool dead = false;
  unsigned long respawn_time = 0;
  int hp = 1;
  int size = 0;
  unsigned long freeze_until = 0;
};
struct Bullet {
  int x = -1, y = -1;
  int dir = 0;
  bool active = false;
};
enum Direction { DIR_NONE=0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
struct RepeatState { Direction dir = DIR_NONE; unsigned long held_since = 0; unsigned long last_repeat = 0; };

int maze[MAZE_SIZE][MAZE_SIZE];
BadGuy badguys[NUM_BADGUYS];
const uint16_t badGuyColors[NUM_BADGUYS] = {TFT_RED, TFT_BLUE, TFT_ORANGE, TFT_LIGHTPURPLE, TFT_YELLOW};
const int spawnLocs[NUM_BADGUYS][2] = {
  {1,MAZE_SIZE-2},
  {MAZE_SIZE-2,1},
  {1,1 + (MAZE_SIZE-3)/2},
  {MAZE_SIZE-2,MAZE_SIZE-2},
  {(MAZE_SIZE-1)/2,1},
};
int px = 1, py = 1;
bool hasWon = false;
int level = 1;
const uint16_t playerColors[] = {TFT_GREEN, TFT_CYAN, TFT_MAGENTA, TFT_ORANGE, TFT_PINK, TFT_BLUE, TFT_YELLOW};
const int NUM_PLAYER_COLORS = sizeof(playerColors)/sizeof(playerColors[0]);
const uint16_t wallColors[] = {TFT_WHITE, TFT_SKYBLUE, TFT_NAVY, TFT_ORANGE, TFT_OLIVE, TFT_BROWN};
const int NUM_WALL_COLORS = sizeof(wallColors)/sizeof(wallColors[0]);
uint16_t curPlayerColor = TFT_GREEN, curWallColor = TFT_WHITE;
int cell_w = 0, cell_h = 0;
int joyX = 512, joyY = 512;
bool btnA = false, btnB = false, btnX = false, btnY = false, btnSelect = false;
bool prevBtnA = false, prevBtnB = false, prevBtnX = false, prevBtnY = false, prevBtnSelect = false;
RepeatState rs;
Bullet playerBullet;
Bullet purpleBullet;
unsigned long purpleLastShoot = 0;
unsigned long purpleNextShoot = 0;
int playerExplosiveShots = 0;
bool explosiveBulletJustHitThisShot = false;
unsigned long lastBadGuyMove = 0;
const unsigned long BADGUY_MOVE_MS = 200;

void updateJoywingInputs(TFT_eSPI &tft, Adafruit_seesaw &ss) {
  joyX = ss.analogRead(JOY_X);
  joyY = ss.analogRead(JOY_Y);
  btnA      = !ss.digitalRead(BUTTON_A);
  btnB      = !ss.digitalRead(BUTTON_B);
  btnX      = !ss.digitalRead(BUTTON_X);
  btnY      = !ss.digitalRead(BUTTON_Y);
  btnSelect = !ss.digitalRead(BUTTON_SELECT);
}
void waitForAllButtonsReleased(TFT_eSPI &tft, Adafruit_seesaw &ss) {
  do { updateJoywingInputs(tft, ss); delay(10);}
  while (btnSelect || btnA || btnB || btnX || btnY);
  prevBtnA = prevBtnB = prevBtnX = prevBtnY = prevBtnSelect = false;
}
void waitForButtonRelease(TFT_eSPI &tft, Adafruit_seesaw &ss) {
  updateJoywingInputs(tft, ss);
  while (btnSelect || btnA || btnB || btnX || btnY) {
    updateJoywingInputs(tft, ss); delay(10);
  }
  prevBtnA = prevBtnB = prevBtnX = prevBtnY = prevBtnSelect = false;
}
void showLevelScreen(TFT_eSPI &tft, int llevel) {
  tft.fillScreen(TFT_BLACK); tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(MC_DATUM); tft.setTextSize(1);
  String msg = "LEVEL " + String(llevel);
  tft.drawString(msg, tft.width()/2, tft.height()/2, 4); delay(1200);
}
void showBootSplash(TFT_eSPI &tft, Adafruit_seesaw &ss) {
  tft.fillScreen(TFT_BLACK); tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM); tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("                              AMAZE-BALLZ!", 16, 8, 4);
  struct Entry { uint16_t color; const char* text; };
  Entry entries[] = {
    {TFT_YELLOW,"BOOM!"},
    {TFT_ORANGE,"Grow 3X"},
    {TFT_BLUE,"3 sec freeze"},
    {TFT_RED,"3X Portals"},
    {TFT_LIGHTPURPLE,"Shoots back"}
  };
  const int R = 15, n_entries = sizeof(entries)/sizeof(entries[0]), virtLineH = 38;
  int y0 = 12+10+2*virtLineH, centerX = tft.width()/2; tft.setTextDatum(ML_DATUM);
  for(int i=0; i < n_entries; i++) {
    int y = y0 + i*virtLineH, dotX = centerX-85;
    tft.fillCircle(dotX, y, R, entries[i].color);
    tft.setTextColor(entries[i].color, TFT_BLACK);
    tft.drawString("     =  ", dotX+R+10, y-R+8, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(entries[i].text, dotX+R+10+80, y-R+8, 4);
  }
  tft.setTextDatum(MC_DATUM); tft.setTextSize(1); tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Press any button...", tft.width()/2, tft.height()-30, 2);
  do{updateJoywingInputs(tft,ss);if(btnSelect)return;delay(10);}while(!btnA&&!btnB&&!btnX&&!btnY&&!btnSelect);
  waitForButtonRelease(tft,ss);
}
void clearMazeWalls() {
  for(int y=0;y<MAZE_SIZE;y++) for(int x=0;x<MAZE_SIZE;x++) maze[y][x]=1;
  maze[1][1]=0; maze[MAZE_SIZE-2][MAZE_SIZE-2]=0;
}
void randomSeedFromNoise() { randomSeed(micros() ^ millis()); }
void setSpawnBlocks() {
  for(int i=0;i<NUM_BADGUYS;++i){
    int sx=spawnLocs[i][0],sy=spawnLocs[i][1];
    for(int dy=-1;dy<=1;++dy) for(int dx=-1;dx<=1;++dx){
      int xx=sx+dx,yy=sy+dy; if(xx>0&&xx<MAZE_SIZE-1&&yy>0&&yy<MAZE_SIZE-1) maze[yy][xx]=0;}
    maze[sy][sx]=2+i; badguys[i].spawn_x=sx; badguys[i].spawn_y=sy; badguys[i].color=badGuyColors[i];
  }
}
void generateRandomMaze() {
  clearMazeWalls(); bool visited[MAZE_SIZE][MAZE_SIZE]={false};
  int sx=1,sy=1; visited[sy][sx]=true; maze[sy][sx]=0;
  struct cell{int x,y;}; cell stack[MAZE_SIZE*MAZE_SIZE]; int sp=0; stack[sp++]={sx,sy};
  while(sp){
    cell curr=stack[sp-1]; int dirs[4][2]={{0,-2},{2,0},{0,2},{-2,0}};
    for(int i=0;i<4;++i){int j=random(0,4),tx=dirs[i][0],ty=dirs[i][1];dirs[i][0]=dirs[j][0];dirs[i][1]=dirs[j][1];dirs[j][0]=tx;dirs[j][1]=ty;}
    bool done=false;
    for(int i=0;i<4;++i){
      int nx=curr.x+dirs[i][0],ny=curr.y+dirs[i][1];
      if(nx>0&&nx<MAZE_SIZE-1&&ny>0&&ny<MAZE_SIZE-1&&!visited[ny][nx]){
        maze[curr.y+dirs[i][1]/2][curr.x+dirs[i][0]/2]=0;maze[ny][nx]=0;
        visited[ny][nx]=true;stack[sp++]={nx,ny};done=true;break;
      }
    }if(!done)sp--;
  }maze[1][1]=0;maze[MAZE_SIZE-2][MAZE_SIZE-2]=0;setSpawnBlocks();
}
void drawMazeFull(TFT_eSPI &tft) {
  tft.fillScreen(TFT_BLACK);
  cell_w = tft.width()/MAZE_SIZE; cell_h = tft.height()/MAZE_SIZE;
  for(int y=0;y<MAZE_SIZE;y++) for(int x=0;x<MAZE_SIZE;x++)
    if(maze[y][x]==1)tft.fillRect(x*cell_w,y*cell_h,cell_w,cell_h,curWallColor);
      else if(maze[y][x]>=2&&maze[y][x]<2+NUM_BADGUYS)
        tft.fillRect(x*cell_w,y*cell_h,cell_w,cell_h,badGuyColors[maze[y][x]-2]);
  tft.fillRect((MAZE_SIZE-2)*cell_w,(MAZE_SIZE-2)*cell_h,cell_w,cell_h,TFT_YELLOW);
}
void drawPlayer(TFT_eSPI &tft,int x, int y, bool erase=false){
  uint16_t col=erase?(maze[y][x]==1?curWallColor:(maze[y][x]>=2&&maze[y][x]<2+NUM_BADGUYS?badGuyColors[maze[y][x]-2]:TFT_BLACK)):curPlayerColor;
  int cx=x*cell_w+cell_w/2,cy=y*cell_h+cell_h/2;tft.fillCircle(cx,cy,min(cell_w,cell_h)/2-3,col);
}
void drawBadGuy(TFT_eSPI &tft,int i,bool erase=false){
  if(badguys[i].dead&&!erase)return;BadGuy&bg=badguys[i];
  uint16_t col=erase?(maze[bg.y][bg.x]==1?curWallColor:(maze[bg.y][bg.x]>=2&&maze[bg.y][bg.x]<2+NUM_BADGUYS?badGuyColors[maze[bg.y][bg.x]-2]:TFT_BLACK)):bg.color;
  int size=0;if(i==ORANGE_IDX)size=badguys[i].size;int cx=bg.x*cell_w+cell_w/2,cy=bg.y*cell_h+cell_h/2;tft.fillCircle(cx,cy,min(cell_w,cell_h)/2-5+size,col);
}
void drawAllBadGuys(TFT_eSPI &tft,bool erase=false){for(int i=0;i<NUM_BADGUYS;i++)if(!badguys[i].dead||erase)drawBadGuy(tft,i,erase);}
void drawBullet(TFT_eSPI &tft,int x,int y,uint16_t color,bool erase=false){
  uint16_t col=erase?(maze[y][x]==1?curWallColor:(maze[y][x]>=2&&maze[y][x]<2+NUM_BADGUYS?badGuyColors[maze[y][x]-2]:TFT_BLACK)):color;
  int cx=x*cell_w+cell_w/2,cy=y*cell_h+cell_h/2;tft.fillCircle(cx,cy,min(cell_w,cell_h)/6,col);
}
void erasePlayerBullet(TFT_eSPI &tft){if(playerBullet.active)drawBullet(tft,playerBullet.x,playerBullet.y,TFT_WHITE,true);}
void erasePurpleBullet(TFT_eSPI &tft){if(purpleBullet.active)drawBullet(tft,purpleBullet.x,purpleBullet.y,TFT_LIGHTPURPLE,true);}
void drawExplosion(TFT_eSPI &tft,int ex,int ey){
  float radius=1.5;for(int dy=-2;dy<=2;dy++)for(int dx=-2;dx<=2;dx++){
    int xx=ex+dx,yy=ey+dy;if(xx>0&&xx<MAZE_SIZE-1&&yy>0&&yy<MAZE_SIZE-1){
      if(sqrt(dx*dx+dy*dy)<=radius)tft.fillRect(xx*cell_w,yy*cell_h,cell_w,cell_h,TFT_YELLOW);
    }
  }
}
void drawMessage(TFT_eSPI &tft,const char* msg,uint16_t fgcol,uint16_t bgcol,int ms,int fontSize=4){
  tft.fillScreen(bgcol);tft.setTextColor(fgcol,bgcol);tft.setTextDatum(MC_DATUM);tft.setTextSize(1);tft.drawString(msg,tft.width()/2,tft.height()/2,fontSize);
  delay(ms);
}
void placeBadGuys() {
  for(int i=0;i<NUM_BADGUYS;i++){
    badguys[i].x=badguys[i].spawn_x;badguys[i].y=badguys[i].spawn_y;badguys[i].dead=false;badguys[i].respawn_time=0;
    if(i==ORANGE_IDX){badguys[i].hp=3;badguys[i].size=0;}if(i==BLUE_IDX)badguys[i].freeze_until=0;
  }purpleBullet.active=false;
}
void moveBadGuys(TFT_eSPI &tft){
  unsigned long now=millis();if(now-lastBadGuyMove<BADGUY_MOVE_MS)return;lastBadGuyMove=now;
  for(int i=0;i<NUM_BADGUYS;i++){
    BadGuy&b=badguys[i];
    if(b.dead){if(millis()>=b.respawn_time){b.x=b.spawn_x;b.y=b.spawn_y;b.dead=false;if(i==ORANGE_IDX){b.hp=3;b.size=0;}if(i==BLUE_IDX)b.freeze_until=0;drawBadGuy(tft,i,false);}continue;}
    if(i==BLUE_IDX&&millis()<b.freeze_until)continue;
    int dirs[4][2]={{0,-1},{1,0},{0,1},{-1,0}},opts[4],nopts=0;
    for(int d=0;d<4;d++){
      int nx=b.x+dirs[d][0],ny=b.y+dirs[d][1];
      if(nx>0&&nx<MAZE_SIZE-1&&ny>0&&ny<MAZE_SIZE-1&&(maze[ny][nx]==0||maze[ny][nx]==(2+i)))opts[nopts++]=d;
    }if(nopts==0)continue;int dir=opts[random(nopts)];drawBadGuy(tft,i,true);b.x+=dirs[dir][0];b.y+=dirs[dir][1];drawBadGuy(tft,i,false);
  }
}
bool playerInExplosion(int ex,int ey){return sqrt((px-ex)*(px-ex)+(py-ey)*(py-ey))<=1.5;}
bool checkBadGuyCollision(){for(int i=0;i<NUM_BADGUYS;i++)if(!badguys[i].dead&&badguys[i].x==px&&badguys[i].y==py)return true;return false;}
void respawnPlayer(TFT_eSPI &tft,Adafruit_seesaw &ss){
  drawPlayer(tft,px,py,true);drawMessage(tft,"GOT YA!",TFT_RED,TFT_BLACK,700,6);drawMazeFull(tft);drawAllBadGuys(tft,false);erasePlayerBullet(tft);erasePurpleBullet(tft);
  px=1;py=1;drawPlayer(tft,px,py,false);playerBullet.active=false;purpleBullet.active=false;playerExplosiveShots=0;waitForButtonRelease(tft,ss);
}
Direction getHeldDirection(){
  int deadzone=120;if(joyY<(512-deadzone))return DIR_UP;if(joyY>(512+deadzone))return DIR_DOWN;if(joyX<(512-deadzone))return DIR_LEFT;if(joyX>(512+deadzone))return DIR_RIGHT;return DIR_NONE;
}
void tryMovePlayer(TFT_eSPI &tft,int dx,int dy){
  int nx=px+dx,ny=py+dy;
  if(maze[ny][nx]==0||(maze[ny][nx]>=2&&maze[ny][nx]<2+NUM_BADGUYS)){
    drawPlayer(tft,px,py,true);px=nx;py=ny;drawPlayer(tft,px,py,false);
    if(px==MAZE_SIZE-2&&py==MAZE_SIZE-2)hasWon=true;
  }
}
void levelSetup(){
  curPlayerColor=playerColors[((level-1)%NUM_PLAYER_COLORS)];curWallColor=wallColors[(level-1)%NUM_WALL_COLORS];randomSeedFromNoise();generateRandomMaze();
  px=1;py=1;hasWon=false;placeBadGuys();playerBullet.active=false;purpleBullet.active=false;playerExplosiveShots=0;purpleNextShoot=0;purpleLastShoot=millis();
}
void getNextCellForDirection(Direction dir,int x,int y,int*nx,int*ny){
  *nx=x;*ny=y;switch(dir){case DIR_RIGHT:(*nx)++;break;case DIR_DOWN:(*ny)++;break;case DIR_UP:(*ny)--;break;case DIR_LEFT:(*nx)--;break;default:break;}
}
void firePlayerBullet(TFT_eSPI &tft,Direction dir){
  if(playerBullet.active)return;int bx=px,by=py;
  switch(dir){case DIR_RIGHT:bx=px+1;by=py;break;case DIR_DOWN:bx=px;by=py+1;break;case DIR_UP:bx=px;by=py-1;break;case DIR_LEFT:bx=px-1;by=py;break;default:return;}
  if(maze[by][bx]==1&&playerExplosiveShots<=0)return;
  playerBullet.x=bx;playerBullet.y=by;playerBullet.dir=dir;playerBullet.active=true;
  drawBullet(tft,playerBullet.x,playerBullet.y,(playerExplosiveShots>0?TFT_ORANGE:TFT_WHITE),false);
}
bool advancePlayerBullet(TFT_eSPI &tft,Adafruit_seesaw &ss){
  if(!playerBullet.active)return false;uint16_t color=(playerExplosiveShots>0?TFT_ORANGE:TFT_WHITE);
  drawBullet(tft,playerBullet.x,playerBullet.y,color,true);int nxx,nyy;
  getNextCellForDirection((Direction)playerBullet.dir,playerBullet.x,playerBullet.y,&nxx,&nyy);explosiveBulletJustHitThisShot=false;
  if(nxx>0&&nxx<MAZE_SIZE-1&&nyy>0&&nyy<MAZE_SIZE-1){
    if(maze[nyy][nxx]==1&&playerExplosiveShots>0){maze[nyy][nxx]=0;tft.fillRect(nxx*cell_w,nyy*cell_h,cell_w,cell_h,TFT_BLACK);playerExplosiveShots--;explosiveBulletJustHitThisShot=true;playerBullet.active=false;return false;}
    if(maze[nyy][nxx]==1&&playerExplosiveShots<=0){playerBullet.active=false;return false;}
  }
  playerBullet.x=nxx;playerBullet.y=nyy;
  if(playerBullet.x<=0||playerBullet.x>=MAZE_SIZE-1||playerBullet.y<=0||playerBullet.y>=MAZE_SIZE-1){playerBullet.active=false;return false;}
  for(int i=0;i<NUM_BADGUYS;i++){
    if(!badguys[i].dead&&badguys[i].x==playerBullet.x&&badguys[i].y==playerBullet.y){
      if(i==NUM_BADGUYS-1){
        int ex=badguys[i].x,ey=badguys[i].y;drawBadGuy(tft,i,true);drawExplosion(tft,ex,ey);delay(200);float radius=1.5;
        for(int dy=-2;dy<=2;dy++)for(int dx=-2;dx<=2;dx++){int xx=ex+dx,yy=ey+dy;if(xx>0&&xx<MAZE_SIZE-1&&yy>0&&yy<MAZE_SIZE-1){if(sqrt(dx*dx+dy*dy)<=radius){if(maze[yy][xx]==1){maze[yy][xx]=0;tft.fillRect(xx*cell_w,yy*cell_h,cell_w,cell_h,TFT_BLACK);}}}}
        if(playerInExplosion(ex,ey)){respawnPlayer(tft,ss);playerBullet.active=false;return false;}
        badguys[i].dead=true;badguys[i].respawn_time=millis()+500;
      }else if(i==ORANGE_IDX){badguys[i].hp--;if(badguys[i].hp>0){badguys[i].size=2*(3-badguys[i].hp);drawBadGuy(tft,i,false);playerBullet.active=false;return false;}
        else{drawBadGuy(tft,i,true);badguys[i].dead=true;badguys[i].respawn_time=millis()+500;badguys[i].size=0;}}
      else if(i==BLUE_IDX){badguys[i].freeze_until=millis()+3000;drawBadGuy(tft,i,false);playerBullet.active=false;return false;}
      else if(i==RED_IDX){drawBadGuy(tft,i,true);badguys[i].dead=true;badguys[i].respawn_time=millis()+500;playerExplosiveShots=3;}
      else{drawBadGuy(tft,i,true);badguys[i].dead=true;badguys[i].respawn_time=millis()+500;}
      playerBullet.active=false;return false;
    }
  }
  drawBullet(tft,playerBullet.x,playerBullet.y,(playerExplosiveShots>0?TFT_ORANGE:TFT_WHITE),false);
  return true;
}
Direction getDirectionTowardsPlayer(int bx,int by){
  int dx=px-bx,dy=py-by;if(abs(dx)>abs(dy)){if(dx>0)return DIR_RIGHT;else if(dx<0)return DIR_LEFT;}else if(abs(dy)>0){if(dy>0)return DIR_DOWN;else if(dy<0)return DIR_UP;}
  return DIR_NONE;
}
void maybeFirePurpleBullet(TFT_eSPI &tft){
  if(badguys[PURPLE_IDX].dead)return;unsigned long now=millis();if(purpleBullet.active)return;
  if(purpleNextShoot==0){purpleLastShoot=now;purpleNextShoot=now+1000+random(0,2000);return;}
  if(now>=purpleNextShoot){
    int bx=badguys[PURPLE_IDX].x,by=badguys[PURPLE_IDX].y;Direction d=getDirectionTowardsPlayer(bx,by);
    if(d!=DIR_NONE){
      int tx=bx,ty=by;
      switch(d){case DIR_RIGHT:tx=bx+1;break;case DIR_LEFT:tx=bx-1;break;case DIR_UP:ty=by-1;break;case DIR_DOWN:ty=by+1;break;default:break;}
      if(tx>0&&tx<MAZE_SIZE-1&&ty>0&&ty<MAZE_SIZE-1&&maze[ty][tx]!=1){
        purpleBullet.x=tx;purpleBullet.y=ty;purpleBullet.dir=d;purpleBullet.active=true;drawBullet(tft,purpleBullet.x,purpleBullet.y,TFT_LIGHTPURPLE,false);}
    }purpleLastShoot=now;purpleNextShoot=now+1000+random(0,2000);
  }
}
bool advancePurpleBullet(TFT_eSPI &tft,Adafruit_seesaw &ss){
  if(!purpleBullet.active)return false;drawBullet(tft,purpleBullet.x,purpleBullet.y,TFT_LIGHTPURPLE,true);
  switch(purpleBullet.dir){
    case DIR_RIGHT:purpleBullet.x++;break;case DIR_DOWN:purpleBullet.y++;break;case DIR_UP:purpleBullet.y--;break;case DIR_LEFT:purpleBullet.x--;break;default:purpleBullet.active=false;return false;
  }
  if(purpleBullet.x<=0||purpleBullet.x>=MAZE_SIZE-1||purpleBullet.y<=0||purpleBullet.y>=MAZE_SIZE-1||maze[purpleBullet.y][purpleBullet.x]==1){purpleBullet.active=false;return false;}
  if(purpleBullet.x==px&&purpleBullet.y==py){respawnPlayer(tft,ss);purpleBullet.active=false;return false;}
  drawBullet(tft,purpleBullet.x,purpleBullet.y,TFT_LIGHTPURPLE,false);return true;
}

// ================= MAIN GAME ENTRY ==================
void run_AmazeBallz(TFT_eSPI &tft, Adafruit_seesaw &ss)
{
  randomSeedFromNoise(); showBootSplash(tft, ss); level = 1;
levelsetup:
  levelSetup(); showLevelScreen(tft, level);
  drawMazeFull(tft); drawAllBadGuys(tft, false); drawPlayer(tft, px, py, false);
  unsigned long lastPlayerBulletMove = 0, lastPurpleBulletMove = 0;
  hasWon = false;
  while(true) {
    updateJoywingInputs(tft, ss); unsigned long now = millis();
    if(btnSelect) break;
    if (btnA && !prevBtnA) firePlayerBullet(tft, DIR_RIGHT);
    if (btnB && !prevBtnB) firePlayerBullet(tft, DIR_DOWN);
    if (btnX && !prevBtnX) firePlayerBullet(tft, DIR_LEFT);
    if (btnY && !prevBtnY) firePlayerBullet(tft, DIR_UP);
    prevBtnA = btnA; prevBtnB = btnB; prevBtnX = btnX; prevBtnY = btnY;
    if (playerBullet.active && (now - lastPlayerBulletMove) > 40) {lastPlayerBulletMove = now;advancePlayerBullet(tft, ss);}
    if (purpleBullet.active && (now - lastPurpleBulletMove) > 40) {lastPurpleBulletMove = now;advancePurpleBullet(tft, ss);}
    maybeFirePurpleBullet(tft);
    Direction d = getHeldDirection();
    if (d != DIR_NONE && !hasWon) {
      if (rs.dir == DIR_NONE || rs.dir != d) {
        rs.dir = d;rs.held_since = now;rs.last_repeat = now;
        switch(d) {
          case DIR_UP:    tryMovePlayer(tft, 0, -1); break;
          case DIR_DOWN:  tryMovePlayer(tft, 0,  1); break;
          case DIR_LEFT:  tryMovePlayer(tft, -1, 0); break;
          case DIR_RIGHT: tryMovePlayer(tft, 1,  0); break;
          default: break;
        }
        if (checkBadGuyCollision()) {respawnPlayer(tft, ss); continue;}
      } else {
        const unsigned long repeat_delay = 300, repeat_rate  = 80;
        if (now - rs.held_since > repeat_delay && now - rs.last_repeat > repeat_rate) {
          switch(d) {
            case DIR_UP:    tryMovePlayer(tft, 0, -1); break;
            case DIR_DOWN:  tryMovePlayer(tft, 0,  1); break;
            case DIR_LEFT:  tryMovePlayer(tft, -1, 0); break;
            case DIR_RIGHT: tryMovePlayer(tft, 1,  0); break;
            default: break;
          }
          if (checkBadGuyCollision()) { respawnPlayer(tft, ss); continue;}
          rs.last_repeat = now;
        }
      }
    } else {rs.dir = DIR_NONE;}

    moveBadGuys(tft);
    if (checkBadGuyCollision()) {respawnPlayer(tft, ss); continue;}
    if (btnSelect && !prevBtnSelect && !hasWon) { waitForAllButtonsReleased(tft, ss); break; }
    prevBtnSelect = btnSelect;
    if (hasWon) { drawMessage(tft, "LEVEL COMPLETED", TFT_YELLOW, TFT_BLACK, 1000); level++; waitForButtonRelease(tft, ss); goto levelsetup; continue;}
    delay(14);
  }
  tft.fillScreen(TFT_BLACK);
}