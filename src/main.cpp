#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Arduino.h>
#include <FS.h>
#include<vector>
#include<cstdint>

/*
Version2.0
SCL 18
SDA 23
BLO RX2(16)
*/
// 显示屏引脚定义
#define TFT_CS        5    // 片选引脚
#define TFT_RST       4    // 复位引脚
#define TFT_DC        2    // 数据/命令引脚
// 背光控制引脚
#define BACKLIGHT_PIN 16
//摇杆
#define X_pin         15
#define Y_pin         13
#define SW            21
//砖块行列
#define row           3
#define col           8
// 初始化 ST7789 显示屏 (240x240 分辨率)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
//主界面
int selectedGame = 1;
bool menuUpdated = false;
bool fillblack = false;

//打砖块设定
int paddleX = 54;         // 板子初始位置
int paddleY = 200;        // 板子的垂直位置（固定在底部）
int paddleWidth = 40;     // 板子的宽度
int paddleHeight = 10;    // 板子的高度

int ballX = 64;           // 球的初始 X 坐标
int ballY = 30;           // 球的初始 Y 坐标
int ballSpeedX = 5;       // 球的水平速度
int ballSpeedY = 5;       // 球的垂直速度
int ballRadius = 5;     
bool gameOver = false;
int buttonState1;
int buttonState2;
int SWState;
unsigned long lastPressTime = 0;
bool first_set = false;
//砖块设定
int bricks[3][8];
int hei = 10;
int wid = 30;
//打方块的函数
void reset();
void drawGame();
void resetBricks();
void updateBall();
void updatePaddle();
void checkCollision();
void playGame1();

//飞机大战设定
int plane[8][9] = {
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1, 0, 0, 0, 1}
};
int enemy[6][7] = {
  {0, 0, 1, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1, 0},
  {1, 1, 0, 0, 0, 1, 1},
  {0, 1, 0, 0, 0, 1, 0},
  {0, 1, 1, 0, 1, 1, 0},
  {0, 0, 1, 1, 1, 0, 0}
};
int planeX;
int planeY;
int number_of_enemies;
int speedup = 1;
int heal;

class Bullet {
public:
  int16_t x, y;
  uint8_t speed;
  bool isActive;
  Bullet() : x(0) , y(0), isActive(false) {}
  Bullet(int16_t index_X, int16_t index_Y ,  uint8_t index_speed, bool isActive)
    :x(index_X), y(index_Y), speed(index_speed), isActive(isActive) {}

  void activate (int16_t index_X, int16_t index_Y ,  uint8_t index_speed){
    x = index_X;
    y = index_Y;
    speed = index_speed;
    isActive = true;
  }
  void deactive(){
    isActive = false;
    tft.fillRect(x,y, 3,4,ST77XX_BLACK);
  }
  void update(){
    if (isActive){
      tft.fillRect(x,y, 3,4,ST77XX_BLACK);
      y -= speed;
      tft.fillRect(x,y, 3,4,ST77XX_WHITE);

      if (y<6){
        deactive();
        dele();
      }
    }
    
  }
  void dele(){
    if (!isActive){
      tft.fillRect(x,y, 3,4,ST77XX_BLACK);
    }
  }
};

class Enemy{
public:
  int16_t x,y ;
  uint8_t speed;
  int health;
  bool isActive;
  int lrm;
  float angle;

  Enemy(): x(0), y(0), speed(1), health(1), isActive(false){}
  Enemy(int16_t index_x, int16_t index_y, 
        uint8_t speed, int health, bool isActive)
    :x(index_x), y(index_y), speed(speed), health(health),
    isActive(isActive){}
  void activate(int16_t index_x, int16_t index_y, 
      uint8_t ispeed, int ihealth){
    x = index_x;
    y = index_y;
    speed = ispeed;
    health = ihealth;
    isActive = true;    
    lrm ;
    angle = 0;
  }

  void deactivate(){
    isActive = false;

    for (int row3 = 0; row3<6 ; row3++){
      for (int col3 = 0; col3<7 ; col3++){
        if (enemy[row3][col3] ==1 ){
          tft.fillRect(x +col3*2, y+row3*2, 
                  2,2,ST77XX_BLACK);
          }
        }
      }    
    x = 0;
    y = 0;
  }

  void update(){
    if (isActive){
      
      for (int row3 = 0; row3<6 ; row3++){
        for (int col3 = 0; col3<7 ; col3++){
          if (enemy[row3][col3] ==1 ){
            tft.fillRect(x +col3*2, y+row3*2, 
                    2,2,ST77XX_BLACK);
          }
        }
      }
    
//三种类型敌人，一种偏向向左，一种偏向右，一种左右摇摆
    int ran = random(0,10);
    int middle_ran = random(0,2);
    if (lrm == 0|| lrm == 1){

      if(ran == 0 || ran == 1){
        if (x + speed >210){
          x = 210;
        }
        else{
        x += 1.5*speed;
        }       
      }
      else {
        if (x - speed <30){
          x = 30;
        }
        else{
        x -= 1.5*speed;
        }
      }

    }

    else if(lrm == 3|| lrm == 4){
      if(ran >=0 && ran <= 7){
        if (x + speed >210){
          x = 210;
        }
        else{
        x += 1.5*speed;
        }       
      }
      else {
        if (x - speed <30){
          x = 30;
        }
        else{
        x -= 1.5*speed;
        }
      }
    }

    else if(lrm == 2){
      if(middle_ran == 0){
        if (x + speed >210){
          x = 210;
        }
        else{
        x += 1.5*speed;
        }       
      }
      else{
        if (x - speed <30){
          x = 30;
        }
        else{
        x -= 1.5*speed;
        }
      }        
    }
    else if (lrm == 5){
      angle += 0.1; // 控制水平摆动频率
      x = 120 + 80 * sin(angle); 
    }
    y += speed;
    if (y >220){
      gameOver = true;
    }
      for (int row3 = 0 ; row3<6 ; row3++){
        for (int col3 = 0; col3<7 ; col3++){
          if (enemy[row3][col3] ==1 ){
            tft.fillRect(x +col3*2, y+row3*2, 
                        2,2,ST77XX_WHITE);
          }
        }
      }      

    }//if active
  }//update

};//class

class Score{
public:
  int score;
  bool changed = false;
  bool firstseted = false;
  void firstset(){
    if (!firstseted){
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(100, 10);
      tft.println("Score: " + String(score));  
      firstseted = true;  
    }
  }
  void display_score(){
    if (changed){
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(100, 10);
      tft.fillRect(100, 10, 150, 40, ST77XX_BLACK); 
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(100, 10);
      tft.println("Score: " + String(score)); 
      changed = false;   
      }
  }
  void add_score(){
    score +=1;
    changed = true;
  }

  void reset_score(){
    score = 0;
  }
};
Score score;


std::vector<Bullet> bulletpool(100);
std::vector<Enemy> enemypool(50);
//飞机大战函数
void resetAll(int block_size);
void update_plane(int block_size);
void update_enemies();
void update_bullets();
bool checkCollision2(int bulletX, int bulletY, int enemyX, int enemyY);
void checkCollisions2();
void playGame2();


// 预声明 setBacklightBrightness 函数
void setBacklightBrightness(uint8_t brightness);

//主界面
void MainmenuInput();
void MainUI();
bool first_menuUpdate = false;

//定义游戏状态
enum GameState { Menu, Game1, Game2};
GameState state = Menu;


void setup() {
  // 初始化串口监视器
  //Serial.begin(115200);
  //Serial.println("ST7789 240x240 Display with Backlight Control");

  pinMode(X_pin, INPUT);
  pinMode(Y_pin, INPUT);
  pinMode(SW, INPUT_PULLUP);


  // 初始化显示屏
  tft.init(240, 240); 
  tft.setRotation(0);  // 显示方向 (0-3)
  tft.fillScreen(ST77XX_BLACK);  

  // 初始化背光控制的 PWM 频道
  ledcSetup(0, 5000, 8); // 0 号频道，频率 5kHz，分辨率 8 位

  ledcAttachPin(BACKLIGHT_PIN, 0); // 将引脚连接到 PWM 频道
  // 设置背光亮度为 50% (128/255)
  setBacklightBrightness(128);
  state = Menu;
}

void loop() {
  switch (state) {
    case Menu:
      MainUI();
      MainmenuInput();
      break;
    case Game1:
      playGame1();
      if (!gameOver){
        updateBall();
        updatePaddle();
        checkCollision();
        drawGame();
        score.firstset();
        score.display_score();
        delay(50);
      }else{
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(60, 120);
        tft.println("Game Over");
        delay(2000);
        tft.fillScreen(ST77XX_BLACK);
        state = Menu;
        gameOver = false;
        first_set = false;
        fillblack = false;
        menuUpdated = false;
        score.firstseted = false;
        score.reset_score();
      }
      break;
    case Game2:
      playGame2();
      if (!gameOver){
        update_plane(3);
        update_bullets();
        update_enemies();
        checkCollisions2();
        score.firstset();
        score.display_score();
        delay(50);
      }else{
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(60, 120);
        tft.println("Game Over");
        resetAll(2);
        delay(2000);
        tft.fillScreen(ST77XX_BLACK);
        state = Menu;
        gameOver = false;
        first_set = false;
        fillblack = false;
        menuUpdated = false;
        score.firstseted = false;
        score.reset_score();
      }
      break;
  }


}

// 设置背光亮度的函数 (亮度范围: 0-255)
void setBacklightBrightness(uint8_t brightness) {
  ledcWrite(0, brightness);
}

void resetBricks() {
  for (int r = 0; r< row; r++) {
    for (int c = 0; c< col; c++) {
      bricks[r][c] = 1;  // 1 表示砖块存在
    }
  }
}


void drawGame() {
  
  for (int r = 0; r< row ; r++){
    for (int c = 0; c < col ; c++){
      if (bricks[r][c] == 1){
        int brickX = c * wid;
        int brickY = r * hei;
        tft.fillRect(brickX,brickY, wid -3, hei -3 , ST77XX_WHITE);
      }
      else{
        int brickX = c * wid;
        int brickY = r * hei;
        tft.fillRect(brickX,brickY, wid -3, hei -3 , ST77XX_BLACK);
      }
    }
  }
tft.fillCircle(ballX, ballY, ballRadius, ST77XX_WHITE);

}

void updateBall() {

  tft.fillCircle(ballX, ballY, ballRadius, ST77XX_BLACK);



  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // 碰到左右边界反弹
  if (ballX <= ballRadius || ballX >= 240 - 2*ballRadius) {
    ballSpeedX = -ballSpeedX;
  }
  
  // 碰到上边界反弹
  if (ballY <= ballRadius) {
    ballSpeedY = -ballSpeedY;
  }
  
  // 碰到下边界，游戏结束
  if (ballY >= 240 - ballRadius) {
    gameOver = true;
  }

  if (ballY>=240 - ballRadius){
    ballSpeedY = -ballSpeedY;
  }

  if (ballY + ballRadius >= paddleY && ballX >+ paddleX && ballX <= paddleX + paddleWidth){
    ballSpeedY = -ballSpeedY;
    ballY -= 5;
  }
  tft.fillCircle(ballX, ballY,ballRadius, ST77XX_WHITE);
}

void updatePaddle(){
  tft.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, ST77XX_BLACK);
  buttonState1 = analogRead(X_pin);
  buttonState2 = analogRead(Y_pin);


  if (buttonState1 >3200){
    paddleX -= 10;
    if (paddleX < 0){
      paddleX=0;
    }
  }
  if (buttonState1 <2900){
    paddleX += 10;
    if (paddleX > 200){
      paddleX=200;
    }
  }
  tft.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, ST77XX_WHITE);

}

void reset(){
  paddleX = 54;
  ballX = 64;
  ballY = 30;
  ballSpeedX = 5;
  ballSpeedY = 5;
  gameOver = false;
  resetBricks();
}

void checkCollision(){  
  for ( int r = 0 ; r < row; r++){
    for (int c = 0 ; c< col ; c++){
      if (bricks[r][c] == 1){
        int brickX = c * wid;
        int brickY = r * hei;
        if(ballX + ballRadius > brickX && 
            ballX - ballRadius < brickX + wid &&
            ballY + ballRadius > brickY && 
            ballY - ballRadius < brickY + hei){
          bricks[r][c] = 0;

          ballSpeedY = -ballSpeedY;
          return;
        }

      }
    }
  }
}

void playGame1(){

  if (first_set == false){
    tft.fillScreen(ST77XX_BLACK);  

    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println("Game Init");
    delay(1000);
    tft.fillScreen(ST77XX_BLACK);  
    resetBricks();
    drawGame();
    first_set = true;
  }

}


//游戏2
void playGame2(){
  if (first_set == false){
    tft.fillScreen(ST77XX_BLACK);  

    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println("Game Init");
    delay(1000);
    tft.fillScreen(ST77XX_BLACK);  
    resetAll(3);
    first_set = true;
  }

}
void resetAll(int block_size = 2){

  for (int row2 = 0; row2 < 8; row2++) {
    for (int col2 = 0; col2 < 9; col2++) {
      if (plane[row2][col2] == 1) {
        tft.fillRect(planeX + col2*block_size,
                     planeY + row2*block_size,
                     block_size, 
                     block_size, 
                     ST77XX_BLACK);
            }
        }
    }

  for (auto& singleBullet : bulletpool){
    singleBullet.isActive = false;
  }


  for (auto& singleEnemy : enemypool){
    singleEnemy.isActive = false;
  }
  planeX = 100;
  planeY = 200;
}


void update_plane(int block_size = 2){
  for (int row2 = 0; row2 < 8; row2++) {
    for (int col2 = 0; col2 < 9; col2++) {
      if (plane[row2][col2] == 1) {
        tft.fillRect(planeX + col2*block_size,
                     planeY + row2*block_size,
                     block_size, 
                     block_size, 
                     ST77XX_BLACK);
            }
        }
    }
  buttonState1 = analogRead(X_pin);
  buttonState2 = analogRead(Y_pin);


  if (buttonState1 >3200){
    planeX -= 10;
    if (planeX < 0){
      planeX=0;
    }
  }
  if (buttonState1 <2900){
    planeX += 10;
    if (planeX > 200){
      planeX=200;
    }
  }

  if (buttonState2 <2000 ){
    planeY -= 10;
    if (planeY < 0){
      planeY=0;
      gameOver = true;
    }   
  }else if( buttonState2>3100){
    planeY += 10;
    if (planeY > 200){
      planeY=200;
    }
  }

  for (int row2 = 0; row2 < 8; row2++) {
    for (int col2 = 0; col2 < 9; col2++) {
      if (plane[row2][col2] == 1) {
        tft.fillRect(planeX + col2*block_size,
                     planeY + row2*block_size,
                     block_size, 
                     block_size, 
                     ST77XX_WHITE);
      }
    }
  }
}

void update_bullets(){
  //子弹生成间隔
  unsigned long nowtime = millis();
  //只赋值第一次
  static unsigned long lasttime = 0;

  //子弹更新时间间隔
  unsigned long nowtime2 = millis();
  static unsigned long lasttime2 = 0;
  if (nowtime - lasttime >=400){
    //激活一个子弹
    for (auto& singleBullet : bulletpool){
      if (!singleBullet.isActive){
        singleBullet.activate(planeX+8, planeY -4,5);
        break;
      }
    }
    lasttime = nowtime;
  }

  if (nowtime2 - lasttime2 >=50){
    for (auto& singleBullet : bulletpool){
      if(singleBullet.isActive){
        singleBullet.update();
      }
    }
    
    lasttime2 = nowtime2;  
  }
}

void update_enemies(){
  unsigned long nowtime_Enemy1 = millis();
  static unsigned long lasttime_Enemy1  = 0;

  unsigned long nowtime_Enemy2 = millis();
  static unsigned long lasttime_Enemy2 = 0;
  if(score.score >35){
    speedup = 2;
  }
  else if(score.score >20){
    speedup = 1.5;
  }
  else if(score.score >10){
    speedup = 1.2;
  }
  
if(nowtime_Enemy1- lasttime_Enemy1 >1000){
    for (auto& singleEnemy : enemypool){
      if (!singleEnemy.isActive){
        int16_t randomX = random(30, 210); 
        int16_t randomY = random(30, 60);
        if (score.score>5){
          heal = 2;
        }
        else {
          heal = 1;
        }
        singleEnemy.activate(randomX, randomY, speedup, heal);
        if(score.score>5){
          singleEnemy.lrm = 5;
        }
      /* else if(score.score >10 ){
          singleEnemy.lrm = (random(0,2)==3) ? 5 : random(0,5);
        }*/
        else if(score.score >0){
          singleEnemy.lrm = (random(0,2) == 0) ? 5: random(0,5);
        }

        break;
      }   
    }
  lasttime_Enemy1 = nowtime_Enemy1;
}
  if (nowtime_Enemy2 - lasttime_Enemy2 >=50){
    for (auto& singleEnemy : enemypool){
      if(singleEnemy.isActive){
        singleEnemy.update();
      }
    }
    lasttime_Enemy2 = nowtime_Enemy2;
  }
}

bool checkCollision2(int bulletX, int bulletY, int enemyX, int enemyY){
  return !(bulletX > enemyX +14 || 
            bulletX + 3< enemyX ||
            bulletY > enemyY +12||
            bulletY +4 <enemyY);
}

void checkCollisions2(){
  for(auto& singleBullet :bulletpool){
    if(!singleBullet.isActive) continue;
    for (auto& singleEnemy : enemypool){
      if (!singleEnemy.isActive) continue;
      if(checkCollision2(singleBullet.x, singleBullet.y,
                        singleEnemy.x, singleEnemy.y)){
        singleEnemy.health -=1;
        if(singleEnemy.health == 0){
          singleBullet.deactive();
          singleEnemy.deactivate();
          score.add_score();
          
          break;
        }
        singleBullet.deactive();

        break;
         }
    }
    
  }
}
//主界面函数
void MainUI(){
  if (fillblack == false){
    tft.fillScreen(ST77XX_BLACK);
    fillblack = true;
  }
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Select Game:");



  if (!menuUpdated){
    if (first_menuUpdate == false){



    }

    if (selectedGame == 1) {
      tft.setCursor(10, 40);
      tft.setTextColor(ST77XX_YELLOW);
      tft.println("1. Brick Breaker");
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(10, 70);
      tft.println("2. Plane Shooter");
    } 
    else if (selectedGame == 2) {
      tft.setCursor(10, 40);
      tft.setTextColor(ST77XX_WHITE);
      tft.println("1. Brick Breaker");
      tft.setTextColor(ST77XX_YELLOW);
      tft.setCursor(10, 70);
      tft.println("2. Plane Shooter");
    }
  }
  menuUpdated = true;
}

void MainmenuInput(){
  //前0后4000，中3000
  //左4000右0，中3100
  buttonState1 = analogRead(X_pin);
  buttonState2 = analogRead(Y_pin);
  if (buttonState2 <2000 &&selectedGame != 1){
    selectedGame = 1;
    menuUpdated = false;
  }else if( buttonState2>3100 && selectedGame !=2){
    selectedGame = 2;
    menuUpdated = false;
  }
  SWState = digitalRead(SW);

  if( SWState == LOW&& millis() - lastPressTime >200){
    
    lastPressTime = millis();
    if (selectedGame == 1){
      state  = Game1;
    }else if(selectedGame == 2){
      state = Game2;
    }
  }
}