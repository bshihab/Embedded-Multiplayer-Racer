// Lab9Main.c
// Runs on MSPM0G3507
// Lab 9 ECE319K
// Prarthana Balaji and Bilal Shihab
// Last Modified: December 18, 2025

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <string.h>

// Hardware Drivers
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "../inc/JoyStick.h"
#include "../inc/ADC.h"
#include "../inc/FIFO1.h"
#include "../inc/Switch.h"
#include "../ECE319K_Lab8/UART1.h"
#include "../ECE319K_Lab8/UART2.h"
#include "LED.h"
#include "Sound.h"

// Refactored Modules
#include "GameConstants.h"
#include "GameObjects.h"
#include "Graphics.h"
#include "SoundData.h"
#include "images/images.h"

// ----------------------------------------------------------------------------
// LOCAL GLOBALS (Specific to Main Loop / Interrupts)
// ----------------------------------------------------------------------------

volatile uint8_t GameTick;
GameState_t CurrentState = STATE_TITLE;
uint8_t LanguageMode = 0;   // 0 = English, 1 = Spanish

// Raw Analog inputs
uint32_t x_pos;
uint32_t y_pos;

// Logic Flags
uint8_t lastUseItemState = 0;

// Dimensions used for local collision math
int16_t jeep_width = 14;
int16_t jeep_height = 14;

// UART / Networking Variables
uint8_t rxState = 0;
uint8_t rxIndex = 0;
char rxBuf[8];

// ----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// ----------------------------------------------------------------------------
void FlashCollision(void);
uint8_t CheckCollisions(void);
void JoystickMovement(void);
void WallCollision(void);
void CheckLapTrigger(void);
void CheckEnemyCollision(void);
void GameStateMachine(void);
void RandomSafeSpawn(int16_t* outX, int16_t* outY);
void PLL_Init(void);

// ----------------------------------------------------------------------------
// LOGIC IMPLEMENTATIONS
// ----------------------------------------------------------------------------

void FlashCollision(void){
    for(int i=0; i<2; i++){
        LED_All_On();         
        Clock_Delay1ms(50);   
        LED_All_Off();        
        Clock_Delay1ms(50);   
    }
}

// Kept in Main as it relies on specific map bounds not fully exported to helpers
void RandomSafeSpawn(int16_t* outX, int16_t* outY){
   while(1){
       int16_t x = (Random() % 120) + 4;
       int16_t y = (Random() % 150) + 5;

       if(!(x <= OUT_LEFT+3 || x >= OUT_RIGHT-12)){
        if(!(y <= OUT_TOP+12 || y >= OUT_BOTTOM-3)){
        if(!(x >= IN_LEFT && x <= IN_RIGHT &&
            y <= IN_BOTTOM && y >= IN_TOP)){
        if(!(x >= Collidables[0].x-12 &&
            x <= Collidables[0].x + Collidables[0].width + 12 &&
            y <= Collidables[0].y &&
            y >= Collidables[0].y - Collidables[0].height)){
        if(!( (y > 120 && y < 150) ||
            (y > 20 && y < 50) )){

            *outX = x;
            *outY = y;
            return;
        }
        }
}
}
}
}
}

uint8_t CheckCollisions(void){
  for(uint8_t i = 0; i < MAX_OBJECTS; i++){

    // Skip inactive objects
    if(Collidables[i].active == 0) {
        continue;
    }

    int16_t jeep_left   = playerCar.x;
    int16_t jeep_right  = playerCar.x + jeep_width;
    int16_t jeep_top    = playerCar.y - jeep_height + 1;
    int16_t jeep_bottom = playerCar.y - 1;

    int16_t col_left   = Collidables[i].x;
    int16_t col_right  = Collidables[i].x + Collidables[i].width - 1;
    int16_t col_top    = Collidables[i].y - Collidables[i].height;
    int16_t col_bottom = Collidables[i].y - 1;
 
    if(jeep_right < col_left  ||
       jeep_left  > col_right ||
       jeep_bottom < col_top  ||
       jeep_top > col_bottom){
          continue;
    }

    // WATER COLLISIONS
    if(Collidables[i].type == OBJ_WATER){
        // water slow down
        playerCar.currentSpeed -= 1;
        if(playerCar.currentSpeed < 0){
               playerCar.currentSpeed = 0;
        }
         
        ST7735_DrawSprite_TransparentCustom(
            Collidables[i].x,
            Collidables[i].y,
            Collidables[i].image,
            Collidables[i].width,
            Collidables[i].height
        );

        DrawJeep();
        DrawEnemyCar();

        EraseJeepOverObject(  
            Collidables[i].x,
            Collidables[i].y,
            Collidables[i].width,
            Collidables[i].height,
            Collidables[i].image
        );
    }

    // TREASURE COLLISION
    else if(Collidables[i].type == OBJ_TREASURE){
       Sound_Start(treasureChime, 11025);

        if(treasureRespawnTimer[i] > 0){
            continue;
        }

        if(i == 1) chestA_collected = 1;
        if(i == 2) chestB_collected = 1;

        treasureRespawnTimer[i] = 30;
        Collidables[i].active = 0;

        // Erase chest manually to update background immediately
        for(int row = 0; row < Collidables[i].height; row++){
            for(int col = 0; col < Collidables[i].width; col++){
                int16_t px = Collidables[i].x + col;
                int16_t py = Collidables[i].y - row;

                if(px < 0 || px >= 128) continue;
                if(py < 0 || py >= 160) continue;

                int16_t bgY = 159 - py;
                uint16_t bgColor = OvalTrack[(bgY * 128) + px];
                ST7735_DrawPixel(px, py, bgColor);
            }
        }
    }

    // QUESTION COLLISION
    else if(Collidables[i].type == OBJ_QUESTION){

        if(i != 3) continue;

        ledFlashTimer = 12;

        EraseJeepOverObject(
            Collidables[i].x,
            Collidables[i].y,
            Collidables[i].width,
            Collidables[i].height,
            Collidables[i].image
        );

        EraseJeepBackground();

        // for preventing colliding again
        playerCar.x += 5;
        playerCar.y -= 5;

        Collidables[i].active = 0;
        questionRespawnTimer[i] = 500;
       
        // Manual Erase
        for(int row = 0; row < Collidables[i].height; row++){
            for(int col = 0; col < Collidables[i].width; col++){
                int16_t px = Collidables[i].x + col;
                int16_t py = Collidables[i].y - row;

                if(px < 0 || px >= 128) continue;
                if(py < 0 || py >= 160) continue;

                int16_t bgY = 159 - py;
                uint16_t bgColor = OvalTrack[(bgY*128)+px];
                ST7735_DrawPixel(px, py, bgColor);
            }
        }

        if(questionUses < MAX_QUESTION_USES){
            questionUses++;  
        }

        if(invTop < 2 && questionUses <= MAX_QUESTION_USES){
            uint8_t item = RandomItem();
            invTop++;
            inventory[invTop] = item;

            if(item == 0) bananaCount++;
            else if(item == 1) bombCount++;
            else ldCount++;
        }
    }

    // PIPE COLLISIONS
    else if(Collidables[i].type == OBJ_PIPE) {

        int16_t px1 = Collidables[i].x;
        int16_t px2 = px1 + Collidables[i].width;
        int16_t py1 = Collidables[i].y - Collidables[i].height;
        int16_t py2 = Collidables[i].y;

        int16_t cx1 = playerCar.x;
        int16_t cx2 = playerCar.x + jeep_width;
        int16_t cy1 = playerCar.y - jeep_height;
        int16_t cy2 = playerCar.y;

        int16_t overlapLeft   = cx2 - px1;
        int16_t overlapRight  = px2 - cx1;
        int16_t overlapTop    = cy2 - py1;
        int16_t overlapBottom = py2 - cy1;

        int16_t min = overlapLeft;
        uint8_t side = 0; // 0=L,1=R,2=T,3=B

        if(overlapRight < min){ min = overlapRight; side = 1; }
        if(overlapTop < min){ min = overlapTop; side = 2; }
        if(overlapBottom < min){ min = overlapBottom; side = 3; }

        EraseJeepBackground();

        switch(side){
            case 0: 
                playerCar.x = px1 - jeep_width - 2;
                playerCar.angle = (24 - playerCar.angle) % 16;
                break;
            case 1: // hit right of pipe
                playerCar.x = px2 + 2;
                playerCar.angle = (24 - playerCar.angle) % 16;
                break;
            case 2: // hit top of pipe
                playerCar.y = py1 - 2;
                playerCar.angle = (16 - playerCar.angle) % 16;
                break;
            case 3: // hit bottom of pipe
                playerCar.y = py2 + jeep_height + 2;
                playerCar.angle = (16 - playerCar.angle) % 16;
                break;
        }

        // bouncing
        bouncing = 1;
        bounceFrames = 2;
        playerCar.currentSpeed = 3;

        ST7735_DrawSprite_TransparentCustom(
            Collidables[i].x,
            Collidables[i].y,
            Collidables[i].image,
            Collidables[i].width,
            Collidables[i].height
        );
        DrawJeep();
        DrawEnemyCar();
    }
    // WAYMO COLLISIONS
    else if(Collidables[i].type == OBJ_WAYMO){
        
        int16_t wx1 = Collidables[i].x;
        int16_t wx2 = wx1 + Collidables[i].width;
        int16_t wy1 = Collidables[i].y - Collidables[i].height;
        int16_t wy2 = Collidables[i].y;

        int16_t cx1 = playerCar.x;
        int16_t cx2 = playerCar.x + jeep_width;
        int16_t cy1 = playerCar.y - jeep_height;
        int16_t cy2 = playerCar.y;

        int16_t overlapLeft   = cx2 - wx1;
        int16_t overlapRight  = wx2 - cx1;
        int16_t overlapTop    = cy2 - wy1;
        int16_t overlapBottom = wy2 - cy1;

        int16_t min = overlapLeft;
        uint8_t side = 0; 

        if(overlapRight < min){ min = overlapRight; side = 1; }
        if(overlapTop < min){ min = overlapTop; side = 2; }
        if(overlapBottom < min){ min = overlapBottom; side = 3; }

        EraseJeepBackground();

        switch(side){
            case 0: // Hit Waymo's Left Side
                playerCar.x = wx1 - jeep_width - 2;
                playerCar.angle = (24 - playerCar.angle) % 16;
                break;
            case 1: // Hit Waymo's Right Side
                playerCar.x = wx2 + 2;
                playerCar.angle = (24 - playerCar.angle) % 16;
                break;
            case 2: // Hit Waymo's Top
                playerCar.y = wy1 - 2;
                playerCar.angle = (16 - playerCar.angle) % 16;
                break;
            case 3: // Hit Waymo's Bottom
                playerCar.y = wy2 + jeep_height + 2;
                playerCar.angle = (16 - playerCar.angle) % 16;
                break;
        }

        // bouncing effect
        bouncing = 1;
        bounceFrames = 2;
        playerCar.currentSpeed = 3;

        ST7735_DrawSprite_TransparentCustom(
            Collidables[i].x,
            Collidables[i].y,
            Collidables[i].image,
            Collidables[i].width,
            Collidables[i].height
        );
        DrawJeep();
        DrawEnemyCar();
    }

    else if(Collidables[i].type == OBJ_BANANA){
        if(bananaImmunity == 0){
            // spin
            bananaSpinMode   = 1;
            bananaSpinCycles = 0;
            bananaSpinFrame  = 0;
            bananaImmunity   = 60;

            if(playerCar.currentSpeed < 2){
                playerCar.currentSpeed = 3;
            }
            bouncing = 0;
        }
        return 1; 
    }

    // bomb collisions
    else if(Collidables[i].type == OBJ_BOMB){

       if(bombActivationTimer[i] > 0){
           continue;
       }

        int jeep_cx = playerCar.x + (jeep_width / 2);
        int jeep_cy = playerCar.y - (jeep_height / 2);
        int bomb_cx = Collidables[i].x + (Collidables[i].width / 2);
        int bomb_cy = Collidables[i].y - (Collidables[i].height / 2);

        int dx = jeep_cx - bomb_cx;
        int dy = jeep_cy - bomb_cy;

        if(dx*dx + dy*dy < 150){  
           continue;
        }

       for(int row = 0; row < Collidables[i].height; row++){
           for(int col = 0; col < Collidables[i].width; col++){
               int16_t px = Collidables[i].x + col;
               int16_t py = Collidables[i].y - row;

               if(px < 0 || px >= 128 || py < 0 || py >= 160) continue;

               int trackRow = 159 - py;
               uint16_t bg = OvalTrack[(trackRow * 128) + px];
               ST7735_DrawPixel(px, py, bg);
           }
       }

       Collidables[i].active = 0;

       UART1_OutChar('>');
       UART1_OutChar(0);  
       UART1_OutChar(0);  
       UART1_OutChar(3);   

       TriggerGameOver();
    }
  }

  HUD_Draw();

  uint16_t terrain = GetTerrainColorUnderCar();
  if(terrain == BACKGROUND_GREEN){
    if(playerCar.currentSpeed > 0){
          playerCar.currentSpeed -= 2;
        if(playerCar.currentSpeed < 0) playerCar.currentSpeed = 0;
      }
  }
  return 0;
}

void JoystickMovement(void){

   static uint8_t driftAngle = 0;
   
   // --- Drift Mode (Banana Hit) ---
   if(bananaSpinMode){
     
       if(bananaSpinCycles == 0 && bananaSpinFrame == 0){
           driftAngle = playerCar.angle;
       }

       EraseJeepBackground();
       RedrawAllObjects();

       bananaSpinFrame++;
       if(bananaSpinFrame >= 2){
           bananaSpinFrame = 0;
           playerCar.angle = (playerCar.angle + 1) % 16;
           bananaSpinCycles++;
       }

       uint8_t v = playerCar.currentSpeed;
       if(v > 0){
           switch(driftAngle){
               case 0:  playerCar.x += v; break;
               case 1:  playerCar.x += v; playerCar.y -= (v>>1); break;
               case 2:  playerCar.x += v; playerCar.y -= v; break;
               case 3:  playerCar.x += (v>>1); playerCar.y -= v; break;
               case 4:  playerCar.y -= v; break;
               case 5:  playerCar.x -= (v>>1); playerCar.y -= v; break;
               case 6:  playerCar.x -= v; playerCar.y -= v; break;
               case 7:  playerCar.x -= v; playerCar.y -= (v>>1); break;
               case 8:  playerCar.x -= v; break;
               case 9:  playerCar.x -= v; playerCar.y += (v>>1); break;
               case 10: playerCar.x -= v; playerCar.y += v; break;
               case 11: playerCar.x -= (v>>1); playerCar.y += v; break;
               case 12: playerCar.y += v; break;
               case 13: playerCar.x += (v>>1); playerCar.y += v; break;
               case 14: playerCar.x += v; playerCar.y += v; break;
               case 15: playerCar.x += v; playerCar.y += (v>>1); break;
           }

           if(bananaSpinCycles % 8 == 0 && playerCar.currentSpeed > 0){
                playerCar.currentSpeed--;
           }
       }
       if(playerCar.x < OUT_LEFT + 2) playerCar.x = OUT_LEFT + 2;
       if(playerCar.x > OUT_RIGHT - 15) playerCar.x = OUT_RIGHT - 15;
       if(playerCar.y < OUT_TOP + 15) playerCar.y = OUT_TOP + 15;
       if(playerCar.y > OUT_BOTTOM - 2) playerCar.y = OUT_BOTTOM - 2;
       
       if(playerCar.x > IN_LEFT - 14 && playerCar.x < IN_RIGHT && 
          playerCar.y > IN_TOP && playerCar.y < IN_BOTTOM + 14) {

            if(playerCar.x < (IN_LEFT + IN_RIGHT)/2) playerCar.x = IN_LEFT - 14; 
            else playerCar.x = IN_RIGHT;
       }

       DrawJeep();
       DrawEnemyCar();

       if(bananaSpinCycles >= 32){
           bananaSpinMode = 0;
           bananaSpinCycles = 0;
       }
       return;
   }

    // --- Bouncing Mode (Collisions) ---
    if(bouncing){
        EraseJeepBackground();
        RedrawAllObjects();

        uint8_t v = 3;

        // Move FORWARD in deflected direction
        switch(playerCar.angle){
            case 0:  playerCar.x += v; break;
            case 1:  playerCar.x += v; playerCar.y -= (v>>1); break;
            case 2:  playerCar.x += v; playerCar.y -= v; break;
            case 3:  playerCar.x += (v>>1); playerCar.y -= v; break;
            case 4:  playerCar.y -= v; break;
            case 5:  playerCar.x -= (v>>1); playerCar.y -= v; break;
            case 6:  playerCar.x -= v; playerCar.y -= v; break;
            case 7:  playerCar.x -= v; playerCar.y -= (v>>1); break;
            case 8:  playerCar.x -= v; break;
            case 9:  playerCar.x -= v; playerCar.y += (v>>1); break;
            case 10: playerCar.x -= v; playerCar.y += v; break;
            case 11: playerCar.x -= (v>>1); playerCar.y += v; break;
            case 12: playerCar.y += v; break;
            case 13: playerCar.x += (v>>1); playerCar.y += v; break;
            case 14: playerCar.x += v; playerCar.y += v; break;
            case 15: playerCar.x += v; playerCar.y += (v>>1); break;
        }

        DrawJeep();
        DrawEnemyCar();

        if(bounceFrames > 0) bounceFrames--;
        if(bounceFrames == 0) bouncing = 0;
        return;
    }

    uint32_t gasPedal = Switch_Up();
    uint32_t brakePedal = Switch_Down();
    uint32_t useItem = JoyStick_InButton();

    if(useItem && !lastUseItemState){
        if(invTop >= 0){
            uint8_t item = inventory[invTop];
            invTop--;

            if(item == 0) bananaCount--;
            else if(item == 1) bombCount--;
            else ldCount--;

            if(item == 0){
                SpawnBanana();
                sendBanana = 1;
                bananaImmunity = 15;
            }
            else if(item == 1){
                SpawnBomb(playerCar.x, playerCar.y);
                sendBomb = 1;
            }
            else { // LD
                lapCount++;
                if (enemyLapCount > 0){
                    enemyLapCount--;
                }
            }
            sendLapDec = 1;
        }
    }
    lastUseItemState = useItem;

    uint32_t sumX = 0;
    uint32_t sumY = 0;
    uint8_t i;

    for(i=0; i<16; i++){
      JoyStick_In(&x_pos, &y_pos);
      sumX += x_pos;
      sumY += y_pos;
    }
    uint32_t avgX = (uint16_t)(sumX >> 4);
    uint32_t avgY = (uint16_t)(sumY >> 4);

    Movement(gasPedal, brakePedal);
    uint32_t v = playerCar.currentSpeed;

    if(v > 0){
   
      EraseJeepBackground();
      RedrawAllObjects();

      // If not inside deadzone:
      if (avgX > 2148 || avgX < 1948 || avgY > 2148 || avgY < 1948 ){

          uint32_t dx, dy;

          // quadrant 1 (+,+)
          if (avgX > 2048 && avgY > 2048){
            dx = avgX - 2048; dy = avgY - 2048;
            if (5*dy < dx){    
              playerCar.angle = 0; // E
            }
            else if (3*dy < 2*dx){
              playerCar.angle = 1; // ENE
            }
            else if (2*dy < 3*dx){
              playerCar.angle = 2; // NE
            }
            else if (dy < 5*dx){
              playerCar.angle = 3; // NNE
            }
            else{              
              playerCar.angle = 4; // N
            }
          }
          // quadrant 2 (-,+)
          else if (avgX <= 2048 && avgY > 2048){
            dx = 2048 - avgX; dy = avgY - 2048;
            if (5*dy < dx){    
              playerCar.angle = 8; // W
            }
            else if (3*dy < 2*dx){
              playerCar.angle = 7; // WNW
            }
            else if (2*dy < 3*dx){
              playerCar.angle = 6; // NW
            }
            else if (dy < 5*dx){
              playerCar.angle = 5; // NNW
            }
            else {
              playerCar.angle = 4; // N
            }
          }
          // quadrant 3 3 (+,-)
          else if (avgX > 2048 && avgY <= 2048){
            dx = avgX - 2048; dy = 2048 - avgY;
            if (5*dy < dx){
              playerCar.angle = 0; // E
            }
            else if (3*dy < 2*dx){
              playerCar.angle = 15; // ESE
            }
            else if (2*dy < 3*dx){
              playerCar.angle = 14; // SE
            }
            else if (dy < 5*dx){
              playerCar.angle = 13; // SSE
            }
            else{playerCar.angle = 12; // S
          }
          }
          // quadrant 4 (-,-)
          else if (avgX <= 2048 && avgY <= 2048){
            dx = 2048 - avgX; dy = 2048 - avgY;
            if (5*dy < dx){
              playerCar.angle = 8; // W
            }
            else if (3*dy < 2*dx){
              playerCar.angle = 9; // WSW
            }
            else if (2*dy < 3*dx){
              playerCar.angle = 10; // SW
            }
            else if (dy < 5*dx){
              playerCar.angle = 11; // SSW
            }
            else{
              playerCar.angle = 12; // S
            }
          }
      }
   
      // for movement
      if (v != 0){
      if (playerCar.angle == 0) {       // East
          playerCar.x += v;
      }
      else if (playerCar.angle == 1) {  // ENE
          playerCar.x += v;
          playerCar.y -= (v >> 1);
      }
      else if (playerCar.angle == 2) {  // NE
          playerCar.x += v;
          playerCar.y -= v;
      }
      else if (playerCar.angle == 3) {  // NNE
          playerCar.x += (v >> 1);
          playerCar.y -= v;
      }
      else if (playerCar.angle == 4) {  // North
          playerCar.y -= v;
      }
      else if (playerCar.angle == 5) {  // NNW
          playerCar.x -= (v >> 1);
          playerCar.y -= v;
      }
      else if (playerCar.angle == 6) {  // NW
          playerCar.x -= v;
          playerCar.y -= v;
      }
      else if (playerCar.angle == 7) {  // WNW
          playerCar.x -= v;
          playerCar.y -= (v >> 1);
      }
      else if (playerCar.angle == 8) {  // West
          playerCar.x -= v;
      }
      else if (playerCar.angle == 9) {  // WSW
          playerCar.x -= v;
          playerCar.y += (v >> 1);
      }
      else if (playerCar.angle == 10) { // SW
          playerCar.x -= v;
          playerCar.y += v;
      }
      else if (playerCar.angle == 11) { // SSW
          playerCar.x -= (v >> 1);
          playerCar.y += v;
      }
      else if (playerCar.angle == 12) { // South
          playerCar.y += v;
      }
      else if (playerCar.angle == 13) { // SSE
          playerCar.x += (v >> 1);
          playerCar.y += v;
      }
      else if (playerCar.angle == 14) { // SE
          playerCar.x += v;
          playerCar.y += v;
      }
      else if (playerCar.angle == 15) { // ESE
          playerCar.x += v;
          playerCar.y += (v >> 1);
      }
      }

      // If speed reversed (bounce), push the car backwards
      if(playerCar.currentSpeed < 0){
        uint32_t v = -playerCar.currentSpeed;

        if (playerCar.angle == 0) playerCar.x -= v;
        else if (playerCar.angle == 1){ playerCar.x -= v; playerCar.y += (v>>1); }
        else if (playerCar.angle == 2){ playerCar.x -= v; playerCar.y += v; }
        else if (playerCar.angle == 3){ playerCar.x -= (v>>1); playerCar.y += v; }
        else if (playerCar.angle == 4) playerCar.y += v;
        else if (playerCar.angle == 5){ playerCar.x += (v>>1); playerCar.y += v; }
        else if (playerCar.angle == 6){ playerCar.x += v; playerCar.y += v; }
        else if (playerCar.angle == 7){ playerCar.x += v; playerCar.y += (v>>1); }
        else if (playerCar.angle == 8) playerCar.x += v;
        else if (playerCar.angle == 9){ playerCar.x += v; playerCar.y -= (v>>1); }
        else if (playerCar.angle == 10){ playerCar.x += v; playerCar.y -= v; }
        else if (playerCar.angle == 11){ playerCar.x += (v>>1); playerCar.y -= v; }
        else if (playerCar.angle == 12) playerCar.y -= v;
        else if (playerCar.angle == 13){ playerCar.x -= (v>>1); playerCar.y -= v; }
        else if (playerCar.angle == 14){ playerCar.x -= v; playerCar.y -= v; }
        else if (playerCar.angle == 15){ playerCar.x -= v; playerCar.y -= (v>>1); }

        playerCar.currentSpeed = 0;  // stop after bounce
      }

      DrawJeep();
      DrawEnemyCar();
    }
    else {
        DrawJeep();
        DrawEnemyCar();
    }
}


void WallCollision(void){
 uint8_t collision = 0;
  // Define Car AABB
 int16_t carLeft   = playerCar.x;
 int16_t carRight  = playerCar.x + jeep_width;
 int16_t carTop    = playerCar.y - jeep_height;
 int16_t carBottom = playerCar.y;

 // outer wall collisions!
 if (carLeft <= OUT_LEFT + 1) {
     EraseJeepBackground();
     playerCar.x = OUT_LEFT + 2;
     playerCar.angle = (24 - playerCar.angle) % 16;
     collision = 1;
     DrawCheckeredWall(OUT_LEFT, OUT_TOP, OUT_LEFT, OUT_BOTTOM); 
 }
 else if (carRight >= OUT_RIGHT - 1) {
     EraseJeepBackground();
     playerCar.x = OUT_RIGHT - jeep_width - 2;
     playerCar.angle = (24 - playerCar.angle) % 16;
     collision = 1;
 }
 else if (carTop <= OUT_TOP + 1) {
     EraseJeepBackground(); 
     playerCar.y = OUT_TOP + jeep_height + 2;
     playerCar.angle = (16 - playerCar.angle) % 16;
     collision = 1;
 }
 else if (carBottom >= OUT_BOTTOM - 1) {
     EraseJeepBackground(); 
     playerCar.y = OUT_BOTTOM - 2;
     playerCar.angle = (16 - playerCar.angle) % 16;
     collision = 1;
 }

 // inner wall collisions
 int16_t wallLeft   = IN_LEFT - 1;
 int16_t wallRight  = IN_RIGHT + 1;
 int16_t wallTop    = IN_TOP - 1;
 int16_t wallBottom = IN_BOTTOM + 1;

 if (carRight > wallLeft && carLeft < wallRight &&
     carBottom > wallTop && carTop < wallBottom) {
 
    int16_t overlapLeft   = carRight - wallLeft;
    int16_t overlapRight  = wallRight - carLeft;
    int16_t overlapTop    = carBottom - wallTop;
    int16_t overlapBottom = wallBottom - carTop;

    int16_t minOverlap = overlapLeft;
    if (overlapRight < minOverlap) minOverlap = overlapRight;
    if (overlapTop < minOverlap)   minOverlap = overlapTop;
    if (overlapBottom < minOverlap) minOverlap = overlapBottom;
   
    EraseJeepBackground();

     if (minOverlap == overlapLeft) {
         playerCar.x = wallLeft - jeep_width - 2;
         playerCar.angle = (24 - playerCar.angle) % 16;
     }
     else if (minOverlap == overlapRight) {
         playerCar.x = wallRight + 2;
         playerCar.angle = (24 - playerCar.angle) % 16;
     }
     else if (minOverlap == overlapTop) {
         playerCar.y = wallTop - 2;
         playerCar.angle = (16 - playerCar.angle) % 16;
     }
     else if (minOverlap == overlapBottom) {
          playerCar.y = wallBottom + jeep_height + 2;
          playerCar.angle = (16 - playerCar.angle) % 16;
     }
     collision = 1;
 }

 if(collision){
     bouncing = 1;
     bounceFrames = 2;
     playerCar.currentSpeed = 3;
 }
}


void CheckLapTrigger(void){
    int16_t carLeft   = playerCar.x;
    int16_t carRight  = playerCar.x + jeep_width;
    int16_t carTop    = playerCar.y - jeep_height;
    int16_t carBottom = playerCar.y;

    int16_t lineLeft   = FINISH_X;
    int16_t lineRight  = FINISH_X + FINISH_W;
    int16_t lineTop    = FINISH_Y - FINISH_H;
    int16_t lineBottom = FINISH_Y;

    static uint8_t wasOnLine = 0;

    uint8_t onLine =
        !(carRight  < lineLeft || carLeft > lineRight || carBottom < lineTop || carTop > lineBottom);

    if(onLine && !wasOnLine){
        if(chestA_collected && chestB_collected){
            lapCount++;
            chestA_collected = 0;
            chestB_collected = 0;
            treasureCount = 0;
        }
    }
    wasOnLine = onLine;

    if(lapCount >= 10){
        UART1_OutChar('>');
        UART1_OutChar(0);  
        UART1_OutChar(0);  
        UART1_OutChar(4);  

        ST7735_FillScreen(ST7735_BLACK);
        ST7735_SetCursor(4, 6);
        ST7735_SetTextColor(ST7735_GREEN);
        printf("YOU WON!");
        ST7735_SetCursor(1, 8);
        ST7735_SetTextColor(ST7735_WHITE);
        printf("You finished 10 laps");

        while(1){ }       
    }
}

void CheckEnemyCollision(void){
    int16_t pLeft   = playerCar.x;
    int16_t pRight  = playerCar.x + 14; 
    int16_t pTop    = playerCar.y - 14; 
    int16_t pBottom = playerCar.y;

    int16_t eLeft   = enemyCar.x;
    int16_t eRight  = enemyCar.x + 14;
    int16_t eTop    = enemyCar.y - 14;
    int16_t eBottom = enemyCar.y;

    if(pRight < eLeft || pLeft > eRight || pBottom < eTop || pTop > eBottom){
        return; 
    }

    int16_t overlapLeft   = pRight - eLeft;
    int16_t overlapRight  = eRight - pLeft;
    int16_t overlapTop    = pBottom - eTop;
    int16_t overlapBottom = eBottom - pTop;
  
    int16_t minOverlap = overlapLeft;
    int8_t side = 0; 

    if(overlapRight < minOverlap) { minOverlap = overlapRight; side = 1; }
    if(overlapTop < minOverlap)   { minOverlap = overlapTop;   side = 2; }
    if(overlapBottom < minOverlap){ minOverlap = overlapBottom; side = 3; }
   
    EraseJeepBackground();

    switch(side){
        case 0: 
            playerCar.x = eLeft - 14 - 4; 
            if(playerCar.angle < 4 || playerCar.angle > 12){ 
                playerCar.angle = (24 - playerCar.angle) % 16;
            }
            break;
        case 1:
            playerCar.x = eRight + 4; 
            if(playerCar.angle > 4 && playerCar.angle < 12){ 
                playerCar.angle = (24 - playerCar.angle) % 16;
            }
            break;
        case 2: 
            playerCar.y = eTop - 4; 
            if(playerCar.angle > 8) 
                playerCar.angle = (16 - playerCar.angle) % 16;
            break;
        case 3: 
            playerCar.y = eBottom + 14 + 4; 
            if(playerCar.angle > 0 && playerCar.angle < 8){ 
                playerCar.angle = (16 - playerCar.angle) % 16;
            }
            break;
    }

    bouncing = 1;
    bounceFrames = 4;
    playerCar.currentSpeed = 4; 
}


void TIMG12_IRQHandler(void){
    uint16_t my_xPosition = playerCar.x;
    uint16_t my_yPosition = playerCar.y;
    uint8_t my_angle = playerCar.angle;

    if((TIMG12->CPU_INT.IIDX) == 1){ 
        GameTick = 1;

        if(sendBanana){
            sendBanana = 0;
            UART1_OutChar('>');
            UART1_OutChar(global_bx);
            UART1_OutChar(global_by);
            UART1_OutChar(0);      
        }
        else if(sendBomb){
            sendBomb = 0;
            UART1_OutChar('>');
            UART1_OutChar(global_box);
            UART1_OutChar(global_boy);
            UART1_OutChar(1);  
        }
        else if(sendLapDec){
            sendLapDec = 0;
            UART1_OutChar('>');
            UART1_OutChar(0);   
            UART1_OutChar(0);      
            UART1_OutChar(2);     
        }
        else{
            UART1_OutChar('<');
            UART1_OutChar(my_xPosition);
            UART1_OutChar(my_yPosition);
            UART1_OutChar(my_angle);
        }
    }
}

// _______________________________________________________________________________________________ //

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
    // Clock_Init40MHz(); // run this line for 40MHz
    Clock_Init80MHz(0);   // run this line for 80MHz
}

void GameStateMachine(void){
    switch(CurrentState){
        // title
        case STATE_TITLE:
            ShowTitleScreen();
            CurrentState = STATE_LANGUAGE_SELECT;
            break;
        // language select
        case STATE_LANGUAGE_SELECT:
            ShowLanguageSelect();
            if(Switch_Left()){          
                LanguageMode = 0;
                CurrentState = STATE_CUTSCENE;
            }
            else if(Switch_Right()){   
                LanguageMode = 1;
                CurrentState = STATE_CUTSCENE;
            }
            break;
        case STATE_CUTSCENE:
            ShowCutscene();
            if(Switch_Right()){
                CurrentState = STATE_COUNTDOWN;
            }
            break;
        case STATE_COUNTDOWN:
            RunCountdown();
            CurrentState = STATE_RACING;  
            break;
        case STATE_RACING:
            break;
        default:
            CurrentState = STATE_TITLE;
            break;
    }
}


int main(void) {
    __disable_irq();
    PLL_Init();
    LaunchPad_Init();
    ST7735_InitPrintf();
    Switch_Init();
    JoyStick_Init();
    UART1_Init(); 
    UART2_Init();   
    Fifo1_Init();   
    Sound_Init();

    uint32_t seed = 0;
    for(int i=0; i<50; i++){
        JoyStick_In(&x_pos, &y_pos);
        seed = (seed * 1315423911) ^ x_pos ^ (y_pos << 1);
    }
   
    Random_Init(seed);
    InitJeep();
    InitLevel();
    LED_Init();

    CurrentState = STATE_TITLE;

    while(CurrentState != STATE_RACING){
        GameStateMachine();
        Clock_Delay1ms(150);
    }

    ST7735_FillScreen(ST7735_WHITE);
    ST7735_DrawBitmap(0, 160, OvalTrack, 128, 160);
    DrawTrackWalls();
    DrawFinishLine();
    
    for(uint8_t i = 0; i < MAX_OBJECTS; i++){
        if(Collidables[i].active){
            ST7735_DrawSprite_TransparentCustom(
                Collidables[i].x,
                Collidables[i].y,
                Collidables[i].image,
                Collidables[i].width,
                Collidables[i].height
            );
        }
    }

    DrawJeep();

    TimerG12_IntArm(80000000/30, 2);
    __enable_irq();

    while(1){
        if(!gameOver){
            if(GameTick){
                GameTick = 0;

                // led stuff
                if(ledFlashTimer > 0){
                    ledFlashTimer--; 
                    if( (ledFlashTimer > 9) || (ledFlashTimer > 3 && ledFlashTimer <= 6) ){
                        LED_All_On();  
                    } else {
                        LED_All_Off();
                    }
                } else {
                    LED_All_Off();
                }

                EraseEnemyCarBackground();

                while(UART2_RxAvailable()){       
                    char first = UART2_InChar();     

                    if(first == '<'){
                        if(Fifo1_Size() < 3){
                            break;  
                        }
                        uint8_t rxX     = UART2_InChar();
                        uint8_t rxY     = UART2_InChar();
                        uint8_t rxAngle = UART2_InChar();

                        if(rxX > 127) rxX = 127;
                        if(rxY > 159) rxY = 159;
                        rxAngle &= 0x0F;

                        enemyCar.x     = rxX;
                        enemyCar.y     = rxY;
                        enemyCar.angle = rxAngle;
                    }
                    else if(first == '>'){
                        uint8_t objX    = UART2_InChar();
                        uint8_t objY    = UART2_InChar();
                        uint8_t objType = UART2_InChar();  

                        if(objX > 127) objX = 127;
                        if(objY > 159) objY = 159;

                        // banana
                        if(objType == 0){
                            int8_t idx = FindFreeObjectSlot();
                            if(idx >= 0){
                                Collidables[idx].x = objX;
                                Collidables[idx].y = objY;
                                Collidables[idx].width = 8;
                                Collidables[idx].height = 13;
                                Collidables[idx].type = OBJ_BANANA;
                                Collidables[idx].image = banana;
                                Collidables[idx].active = 0;   
                                bananaActivationTimer[idx] = 12;  
                                ST7735_DrawSprite_TransparentCustom(objX, objY, banana, 8, 13);
                            }
                        }
                        // bomb
                        else if(objType == 1){
                            int8_t idx = FindFreeObjectSlot();
                            if(idx >= 0){
                                Collidables[idx].x      = objX;
                                Collidables[idx].y      = objY;
                                Collidables[idx].width  = 13;
                                Collidables[idx].height = 10;
                                Collidables[idx].type   = OBJ_BOMB;
                                Collidables[idx].image  = bomb;
                                Collidables[idx].active      = 0;   
                                bombActivationTimer[idx]     = 50;
                                ST7735_DrawSprite_TransparentCustom(objX, objY, bomb, 13, 10);
                            }
                        }
                        else if(objType == 2){   
                            enemyLapCount++;       
                            if(lapCount > 0){
                                lapCount--;       
                            }
                            HUD_Draw();           
                        }
                        else if(objType == 3){
                            ST7735_FillScreen(ST7735_BLACK);
                            ST7735_SetCursor(2, 6);
                            ST7735_SetTextColor(ST7735_GREEN);
                            printf("YOU WON!");
                            ST7735_SetCursor(2, 7);
                            ST7735_SetTextColor(ST7735_GREEN);
                            printf("Your clanker");
                            ST7735_SetCursor(2, 8);
                            ST7735_SetTextColor(ST7735_GREEN);
                            printf("enemy hit a bomb");
                            while(1){}
                        }
                        // 10 laps done
                        else if(objType == 4){   
                            ST7735_FillScreen(ST7735_BLACK);
                            ST7735_SetCursor(3, 6);
                            ST7735_SetTextColor(ST7735_RED);
                            printf("YOU LOST!");
                            ST7735_SetCursor(1, 8);
                            ST7735_SetTextColor(ST7735_WHITE);
                            printf("Enemy finished first!");
                            while(1){}
                        }
                    }
                } // End UART

                if(bananaImmunity > 0){
                    bananaImmunity--;
                }

                UpdateWaymo(5, waymoDir);
                UpdateWaymo(6, waymoDir);

                RedrawAllObjects();

                JoystickMovement();
                WallCollision();
                CheckCollisions();
                CheckEnemyCollision();
                CheckLapTrigger();
                RedrawFinishLine();
                HUD_Draw();
                DrawEnemyCar();

                for(uint8_t t = 1; t <= 2; t++){
                    if(treasureRespawnTimer[t] > 0){
                        treasureRespawnTimer[t]--;
                        if(treasureRespawnTimer[t] == 0){
                            Collidables[t].active = 1;
                            ST7735_DrawSprite_TransparentCustom(
                                Collidables[t].x,
                                Collidables[t].y,
                                Collidables[t].image,
                                Collidables[t].width,
                                Collidables[t].height
                            );
                        }
                    }
                }

                if(questionRespawnTimer[3] > 0){
                    questionRespawnTimer[3]--;
                    if(questionRespawnTimer[3] == 0){
                        if(questionUses >= MAX_QUESTION_USES){
                            Collidables[3].active = 0;
                        } else{
                            uint8_t idx = Random() % NUM_Q_SPAWNS;
                            Collidables[3].x = QuestionSpawnX[idx];
                            Collidables[3].y = QuestionSpawnY[idx];
                            Collidables[3].active = 1;
                            ST7735_DrawSprite_TransparentCustom(
                                Collidables[3].x,
                                Collidables[3].y,
                                Collidables[3].image,
                                Collidables[3].width,
                                Collidables[3].height
                            );
                        }
                    }
                }

                for(uint8_t b = 0; b < MAX_OBJECTS; b++){
                    if(Collidables[b].type == OBJ_BANANA && !Collidables[b].active){
                        if(bananaActivationTimer[b] > 0){
                            bananaActivationTimer[b]--;
                        }
                        if(bananaActivationTimer[b] == 0){
                            Collidables[b].active = 1;
                        }
                    }
                }

                for(uint8_t b = 0; b < MAX_OBJECTS; b++){
                    if(Collidables[b].type == OBJ_BOMB && !Collidables[b].active){
                        if(bombActivationTimer[b] > 0){
                            bombActivationTimer[b]--;
                        }
                        if(bombActivationTimer[b] == 0){
                            Collidables[b].active = 1;
                        }
                    }
                }

                GetTerrainColorUnderCar();
            }
        } 
    }
}