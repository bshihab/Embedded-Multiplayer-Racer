#include "GameObjects.h"
#include "images/images.h" // For sprite references
#include "../inc/ST7735.h" // For drawing calls inside spawn functions
#include "Graphics.h"      // For transparent draw

// Global Definitions
Jeep_t playerCar;
RemotePlayer_t enemyCar = {0, 0, 0};
Collider_t Collidables[MAX_OBJECTS];

uint8_t treasureCount = 0;
uint8_t chestA_collected = 0;
uint8_t chestB_collected = 0;
uint8_t lapCount = 0;
uint8_t enemyLapCount = 0;
uint8_t bananaCount = 0;
uint8_t bombCount = 0;
uint8_t ldCount = 0;  
uint8_t inventory[3];
int8_t invTop = -1;  
uint8_t questionUses = 0;
uint8_t gameOver = 0;

uint8_t ledFlashTimer = 0;
uint8_t bouncing = 0;
uint8_t bounceFrames = 0;
uint8_t bananaSpinMode = 0; 
uint8_t bananaSpinFrame = 0; 
uint8_t bananaSpinCycles = 0;  
uint8_t bananaImmunity = 0;
int8_t waymoDir[7] = {0,0,0,0,0, +1, -1};

uint8_t treasureRespawnTimer[MAX_OBJECTS] = {0};
uint8_t questionRespawnTimer[MAX_OBJECTS] = {0};
uint8_t bananaActivationTimer[MAX_OBJECTS] = {0};
uint8_t bombActivationTimer[MAX_OBJECTS] = {0};

uint8_t sendBomb = 0;
uint8_t sendBanana = 0;
uint8_t sendLapDec = 0;
uint8_t global_bx = 0;
uint8_t global_by = 0;
uint8_t global_box = 0;
uint8_t global_boy = 0;

const int16_t QuestionSpawnX[NUM_Q_SPAWNS] = {20, 40, 95, 105, 30, 15};
const int16_t QuestionSpawnY[NUM_Q_SPAWNS] = {50, 120, 30,  40, 130, 90};

// Sprite Arrays
const unsigned short *JeepSprites[16] = {
 Car_0, Car_1, Car_1, Car_1, Car_4, Car_5, Car_5, Car_5,
 Car_8, Car_3, Car_3, Car_3, Car_2, Car_7, Car_7, Car_7
};

const unsigned short *EnemySprites[16] = {
 Enemy_0, Enemy_1, Enemy_1, Enemy_1, Enemy_4, Enemy_5, Enemy_5, Enemy_5,
 Enemy_8, Enemy_3, Enemy_3, Enemy_3, Enemy_2, Enemy_7, Enemy_7, Enemy_7
};

// --- Logic Implementation ---

static uint32_t M = 1;

void Random_Init(uint32_t seed){
   if(seed == 0) seed = 1; 
   M = seed;
}

uint32_t Random32(void){
   M = 1664525*M + 1013904223;
   return M;
}

uint32_t Random(void){
   return (Random32() >> 16);
}

uint8_t RandomItem(void){
   return Random() % 3;
}

void InitJeep(void) {
    playerCar.x = 90;
    playerCar.y = 125;
    playerCar.length = 14;
    playerCar.width = 14;
    playerCar.currentSpeed = 0; 
    playerCar.angle = 4; 
    playerCar.image = Car_4;

    enemyCar.x = 115;    
    enemyCar.y = 125;
    enemyCar.angle = 4; 
}

void InitLevel(void){
    // WATER OBJECT
    Collidables[0].x = 104; Collidables[0].y = 70;
    Collidables[0].width = 8; Collidables[0].height = 15;
    Collidables[0].velocity = 0; Collidables[0].active = 1;  
    Collidables[0].type = OBJ_WATER; Collidables[0].image = water_collision;

    // TREASURE OBJECT 1
    Collidables[1].x = 30; Collidables[1].y = 20;
    Collidables[1].width = 9; Collidables[1].height = 9;
    Collidables[1].active = 1; Collidables[1].type = OBJ_TREASURE;
    Collidables[1].image = treasure_box;

    // TREASURE OBJECT 2
    Collidables[2].x = 50; Collidables[2].y = 150;
    Collidables[2].width = 9; Collidables[2].height = 9;
    Collidables[2].active = 1; Collidables[2].type = OBJ_TREASURE;
    Collidables[2].image = treasure_box;

    // QUESTION OBJECT
    Collidables[3].x = QuestionSpawnX[0]; Collidables[3].y = QuestionSpawnY[0];
    Collidables[3].width  = 9; Collidables[3].height = 9;
    Collidables[3].active = 1; Collidables[3].type   = OBJ_QUESTION;
    Collidables[3].image  = question;

    // PIPE OBJECT
    Collidables[4].x = 22; Collidables[4].y = 88;
    Collidables[4].width = 9; Collidables[4].height = 12;
    Collidables[4].active = 1; Collidables[4].type = OBJ_PIPE;
    Collidables[4].image = pipe;

    // WAYMO A
    Collidables[5].x = 20; Collidables[5].y = 142;        
    Collidables[5].width = 11; Collidables[5].height = 8;
    Collidables[5].velocity = 3; Collidables[5].active = 1;
    Collidables[5].type = OBJ_WAYMO; Collidables[5].image = waymo;

    // WAYMO B
    Collidables[6].x = 20; Collidables[6].y = 35;    
    Collidables[6].width = 11; Collidables[6].height = 8;
    Collidables[6].velocity = 2 ; Collidables[6].active = 1;
    Collidables[6].type = OBJ_WAYMO; Collidables[6].image = waymo;
}

int8_t FindFreeObjectSlot(void){
   for(uint8_t i=7; i<MAX_OBJECTS; i++){
       if(Collidables[i].active == 0){
           return i;
       }
   }
   return -1;
}

void SpawnBanana(void){
    int8_t idx = FindFreeObjectSlot();
    if(idx < 0) return;

    int16_t bx = playerCar.x;
    int16_t by = playerCar.y;

    switch(playerCar.angle){
        case 0:  bx -= 10; break;  // E
        case 1:  bx -= 9;  by += 3; break;
        case 2:  bx -= 7;  by += 7; break;
        case 3:  bx -= 3;  by += 9; break;
        case 4:  by += 10; break;  // N
        case 5:  bx += 3;  by += 9; break;
        case 6:  bx += 7;  by += 7; break;
        case 7:  bx += 9;  by += 3; break;
        case 8:  bx += 10; break;  // W
        case 9:  bx += 9;  by -= 3; break;
        case 10: bx += 7;  by -= 7; break;
        case 11: bx += 3;  by -= 9; break;
        case 12: by -= 10; break;  // S
        case 13: bx -= 3;  by -= 9; break;
        case 14: bx -= 7;  by -= 7; break;
        case 15: bx -= 9;  by -= 3; break;
    }

    Collidables[idx].x = bx;
    Collidables[idx].y = by;
    global_bx = bx;
    global_by = by;

    Collidables[idx].width  = 8;
    Collidables[idx].height = 13;
    Collidables[idx].type = OBJ_BANANA;
    Collidables[idx].active = 0;        
    bananaActivationTimer[idx] = 12;   
    Collidables[idx].image = banana;

    ST7735_DrawSprite_TransparentCustom(bx, by, banana, 8, 13);
}

void SpawnBomb(int16_t x, int16_t y){
   int8_t idx = FindFreeObjectSlot();
   if(idx < 0) return;

   Collidables[idx].x = x;
   Collidables[idx].y = y;
   Collidables[idx].width  = 13;
   Collidables[idx].height = 10;

   global_box = x;
   global_boy = y;

   Collidables[idx].type = OBJ_BOMB;
   Collidables[idx].image = bomb;
   Collidables[idx].active = 0;      
   bombActivationTimer[idx] = 50;        

   ST7735_DrawSprite_TransparentCustom(x, y, bomb, 13, 10);
}

void Movement(uint32_t gas_pressed, uint32_t brake_pressed){
    static uint8_t coastCounter = 0;
    uint8_t max_speed = 3; 
    uint8_t acceleration = 1;
    uint8_t braking = 2; 

  if(brake_pressed){
      if(playerCar.currentSpeed > 0){
          playerCar.currentSpeed -= braking;
      }
      if(playerCar.currentSpeed < 0) playerCar.currentSpeed = 0;
  }
  else if(gas_pressed){
      playerCar.currentSpeed += acceleration;
      if(playerCar.currentSpeed > max_speed){
          playerCar.currentSpeed = max_speed;
      }
      coastCounter = 0;
  }
  else {
      coastCounter++;
      if(coastCounter >= 5) {
          coastCounter = 0; 
          if(playerCar.currentSpeed > 0){
              playerCar.currentSpeed -= 1; 
              if(playerCar.currentSpeed < 0){
                  playerCar.currentSpeed = 0;
              }
          }
      }
  }
}

void UpdateWaymo(uint8_t index, int8_t dirArray[]) {
   int16_t x = Collidables[index].x;
   int16_t y = Collidables[index].y;
   uint8_t w = Collidables[index].width;
   uint8_t h = Collidables[index].height;
   uint8_t v = Collidables[index].velocity;
   int8_t dir = dirArray[index];

   if(Collidables[index].active == 0) return;

   // Erase logic handled by Redraw calls usually, 
   // but specific waymo erase is essentially redrawing bg pixels
   // Assumes graphics are available via imports
   for(int row = 0; row < h; row++){
       for(int col = 0; col < w; col++){
           int16_t px = x + col;
           int16_t py = y - row;
           if(px >= 0 && px < 128 && py >= 0 && py < 160){
               uint16_t bgColor = GetBackgroundPixel(px, py);
               ST7735_DrawPixel(px, py, bgColor);
           }
       }
   }

   x += v * dir;

   if(x <= WAYMO_LEFT_BOUND){
       x = WAYMO_LEFT_BOUND;
       dir = +1;
   }
   if(x + w >= WAYMO_RIGHT_BOUND){
       x = WAYMO_RIGHT_BOUND - w;
       dir = -1;
   }

   Collidables[index].x = x;
   dirArray[index] = dir;

   ST7735_DrawSprite_TransparentCustom(x, y, Collidables[index].image, w, h);
}