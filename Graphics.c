#include "Graphics.h"
#include "images/images.h"
#include "SoundData.h"
#include "../inc/Clock.h"
#include "LED.h"
#include "Sound.h"

// Access global state for HUD and Logic
extern uint8_t LanguageMode; 

void ShowTitleScreen(void){
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(1, 6);
    ST7735_SetTextColor(ST7735_GREEN);
    printf("CLANKERS\n");
    ST7735_SetCursor(1, 8);
    printf("ON THE LOOSE!");
    Clock_Delay1ms(1300); 
}

void ShowLanguageSelect(void){
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(1, 4);
    ST7735_SetTextColor(ST7735_YELLOW);
    printf("Welcome to");
    ST7735_SetCursor(1, 6);
    ST7735_SetTextColor(ST7735_GREEN);
    printf("Clankers on");
    ST7735_SetCursor(1, 7);
    ST7735_SetTextColor(ST7735_GREEN);
    printf("the Loose");
    ST7735_SetCursor(2, 9);
    ST7735_SetTextColor(ST7735_WHITE);
    printf("Select Language:");
    ST7735_SetCursor(2, 11);
    printf("Left for English");
    ST7735_SetCursor(2, 12);
    printf("Right for Spanish");
}

void ShowCutscene(void){
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetTextColor(ST7735_WHITE);

    if(LanguageMode == 0){
        ST7735_SetCursor(0, 3); printf("Robots have gone");
        ST7735_SetCursor(0, 4); printf("rogue on the track!");
        ST7735_SetCursor(0, 6); printf("Collect 2 chests,");
        ST7735_SetCursor(0, 7); printf("dodge bombs &");
        ST7735_SetCursor(0, 8); printf("bananas, and finish");
        ST7735_SetCursor(0, 9); printf("your laps first.");
    } else {
        ST7735_SetCursor(0, 3); printf("Los robots se han");
        ST7735_SetCursor(0, 4); printf("vuelto locos!");
        ST7735_SetCursor(0, 6); printf("Consigue 2 cofres,");
        ST7735_SetCursor(0, 7); printf("evita bombas y");
        ST7735_SetCursor(0, 8); printf("bananas y termina");
        ST7735_SetCursor(0, 9); printf("tus vueltas primero.");
    }
    ST7735_SetCursor(0, 13);
    ST7735_SetTextColor(ST7735_YELLOW);
    printf("Right switch to start");
}

void RunCountdown(void){
    ST7735_FillScreen(ST7735_BLACK);
    // ready
    LED_Red_On();
    Sound_Start(beepOne, 3354);
    ST7735_FillRect(34, 70, 60, 20, ST7735_BLACK); 
    ST7735_SetCursor(7, 7);
    ST7735_SetTextColor(ST7735_RED);
    printf("READY");
    Clock_Delay1ms(1000);
    LED_Red_Off();
    // set
    LED_Yellow_On();
    Sound_Start(beepOne, 3354);
    ST7735_FillRect(34, 70, 60, 20, ST7735_BLACK);
    ST7735_SetCursor(8, 7);
    ST7735_SetTextColor(ST7735_YELLOW);
    printf("SET");
    Clock_Delay1ms(1000);
    LED_Yellow_Off();
    // go
    LED_Green1_On();
    Sound_Start(beepTwo, 7886);
    ST7735_FillRect(34, 70, 60, 20, ST7735_BLACK);
    ST7735_SetCursor(9, 7);
    ST7735_SetTextColor(ST7735_GREEN);
    printf("GO!");
    Clock_Delay1ms(500);
    LED_Green1_Off();
}

void TriggerGameOver(void){
   gameOver = 1;
   ST7735_FillScreen(ST7735_BLACK);
   ST7735_SetCursor(3, 4);
   ST7735_SetTextColor(ST7735_RED);
   printf("GAME OVER");
   ST7735_SetCursor(2, 7);
   ST7735_SetTextColor(ST7735_WHITE);
   printf("YOU HIT A BOMB !!");
   while(1){}
}

void ST7735_DrawSprite_TransparentCustom(int16_t x, int16_t y, const uint16_t *sprite, uint16_t w, uint16_t h) {
    if(sprite == NULL || w == 0 || h == 0) return;
    for(uint16_t row = 0; row < h; row++){
        for(uint16_t col = 0; col < w; col++){
            uint32_t index = row*w + col;
            if(index >= (w*h)) return; 
            uint16_t color = sprite[index];
            if(color != TRANSPARENT_COLOR){
                int16_t px = x + col;
                int16_t py = y - row;
                if(px >= 0 && px < 128 && py >= 0 && py < 160){
                    ST7735_DrawPixel(px, py, color);
                }
            }
        }
    }
}

void DrawCheckeredWall(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    uint16_t color;
    int segLen = 4;
    if (abs(x2 - x1) > abs(y2 - y1)) {
       int start = (x1 < x2) ? x1 : x2;
       int end   = (x1 < x2) ? x2 : x1;
       int y     = y1;
       int count = 0;
       for (int i = start; i < end; i += segLen) {
        if (count % 2 == 0) color = WALL_CYAN; else color = WALL_YELLOW;
        int nextI = (i + segLen > end) ? end : i + segLen;
        ST7735_Line(i, y - 1, nextI, y - 1, color);
        ST7735_Line(i, y, nextI, y, color);
        ST7735_Line(i, y + 1, nextI, y + 1, color);
        count++;
       }
    } else {
       int start = (y1 < y2) ? y1 : y2;
       int end = (y1 < y2) ? y2 : y1;
       int x = x1;
       int count = 0;
       for (int j = start; j < end; j += segLen) {
        if (count % 2 == 0) color = WALL_CYAN; else color = WALL_YELLOW;
        int nextJ = (j + segLen > end) ? end : j + segLen;
        ST7735_Line(x - 1, j, x - 1, nextJ, color);
        ST7735_Line(x,     j, x,     nextJ, color);
        ST7735_Line(x + 1, j, x + 1, nextJ, color);
        count++;
       }
    }
}

void DrawTrackWalls(void) {
    // Outer
    DrawCheckeredWall(OUT_LEFT, OUT_TOP, OUT_RIGHT, OUT_TOP);       
    DrawCheckeredWall(OUT_RIGHT, OUT_TOP, OUT_RIGHT, OUT_BOTTOM);   
    DrawCheckeredWall(OUT_RIGHT, OUT_BOTTOM, OUT_LEFT, OUT_BOTTOM); 
    DrawCheckeredWall(OUT_LEFT, OUT_BOTTOM, OUT_LEFT, OUT_TOP);     
    // Inner
    DrawCheckeredWall(IN_LEFT, IN_TOP, IN_RIGHT, IN_TOP);       
    DrawCheckeredWall(IN_RIGHT, IN_TOP, IN_RIGHT, IN_BOTTOM);   
    DrawCheckeredWall(IN_RIGHT, IN_BOTTOM, IN_LEFT, IN_BOTTOM); 
    DrawCheckeredWall(IN_LEFT, IN_BOTTOM, IN_LEFT, IN_TOP);     
}

void DrawFinishLine(void){
    for(int row = 0; row < FINISH_H; row++){
        for(int col = 0; col < FINISH_W; col++){
            int16_t px = FINISH_X + col;
            int16_t py = FINISH_Y - row;  
            uint16_t color;
            if( ((row/2) + (col/2)) % 2 == 0 ) color = ST7735_WHITE; else color = ST7735_BLACK;
            if(px >= 0 && px < 128 && py >= 0 && py < 160){
                ST7735_DrawPixel(px, py, color);
            }
        }
    }
}

void RedrawFinishLine(void){
    DrawFinishLine();  
}

uint16_t GetBackgroundPixel(int16_t x, int16_t y) {
    if (x < 0 || x >= 128 || y < 0 || y >= 160) return ST7735_BLACK;
    // Outer Walls logic
    if (x >= OUT_LEFT-1 && x <= OUT_LEFT+1 && y >= OUT_TOP && y <= OUT_BOTTOM) return ( ((y - OUT_TOP) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    if (x >= OUT_RIGHT-1 && x <= OUT_RIGHT+1 && y >= OUT_TOP && y <= OUT_BOTTOM) return ( ((y - OUT_TOP) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    if (y >= OUT_TOP-1 && y <= OUT_TOP+1 && x >= OUT_LEFT && x <= OUT_RIGHT) return ( ((x - OUT_LEFT) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    if (y >= OUT_BOTTOM-1 && y <= OUT_BOTTOM+1 && x >= OUT_LEFT && x <= OUT_RIGHT) return ( ((x - OUT_LEFT) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    // Inner Walls logic
    if (x >= IN_LEFT-1 && x <= IN_LEFT+1 && y >= IN_TOP && y <= IN_BOTTOM) return ( ((y - IN_TOP) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    if (x >= IN_RIGHT-1 && x <= IN_RIGHT+1 && y >= IN_TOP && y <= IN_BOTTOM) return ( ((y - IN_TOP) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    if (y >= IN_TOP-1 && y <= IN_TOP+1 && x >= IN_LEFT && x <= IN_RIGHT) return ( ((x - IN_LEFT) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );
    if (y >= IN_BOTTOM-1 && y <= IN_BOTTOM+1 && x >= IN_LEFT && x <= IN_RIGHT) return ( ((x - IN_LEFT) / 4 % 2 == 0) ? WALL_CYAN : WALL_YELLOW );

    int16_t bgY = 159 - y;
    if(bgY < 0 || bgY >= 160) return ST7735_BLACK; 
    return OvalTrack[(bgY * 128) + x];
}

uint16_t GetTerrainColorUnderCar(void) {
    int16_t centerX = playerCar.x + playerCar.width/2;
    int16_t centerY = playerCar.y - playerCar.length/2;
    if(centerX < 0 || centerX >= 128) return ST7735_BLACK;
    if(centerY < 0 || centerY >= 160) return ST7735_BLACK;
    int16_t bgY = 159 - centerY;
    if(bgY < 0 || bgY >= 160) return ST7735_BLACK;
    return OvalTrack[(bgY * 128) + centerX];
}

void DrawJeep(void) {
  uint8_t idx = playerCar.angle & 0x0F;  
  ST7735_DrawSprite_TransparentCustom(playerCar.x, playerCar.y, JeepSprites[idx], 14, 14);
}

void EraseJeepBackground(void) {
  const int jeepW = 14; const int jeepH = 14;
  for(int row = 0; row < jeepH; row++){
      for(int col = 0; col < jeepW; col++){
          int16_t px = playerCar.x + col;
          int16_t py = playerCar.y - row;
          if(px >= 0 && px < 128 && py >= 0 && py < 160){
            uint16_t bgColor = GetBackgroundPixel(px, py); 
            ST7735_DrawPixel(px, py, bgColor);
          }
      }
  }
}

void EraseJeepOverObject(int16_t objX, int16_t objY, uint8_t objW, uint8_t objH, const uint16_t *objImage) {
    const int jeepW = 14; const int jeepH = 14;
    for(int row = 0; row < jeepH; row++){
        for(int col = 0; col < jeepW; col++){
            int16_t px = playerCar.x + col;
            int16_t py = playerCar.y - row;
            if(px < 0 || px >= 128 || py < 0 || py >= 160) continue;
            // pixel overlap check
            if(px >= objX && px <  objX + objW && py <= objY && py >  objY - objH) {
                int objRow = objY - py;
                int objCol = px - objX;
                if(objRow >= 0 && objRow < objH && objCol >= 0 && objCol < objW) {
                    uint16_t objColor = objImage[objRow * objW + objCol];
                    if(objColor != TRANSPARENT_COLOR){
                        ST7735_DrawPixel(px, py, objColor);
                        continue;
                    }
                }
            }
            uint16_t bgColor = GetBackgroundPixel(px, py);
            ST7735_DrawPixel(px, py, bgColor);
        }
    }
}

void DrawEnemyCar(void){
    if(enemyCar.x > 127 || enemyCar.y > 159) return;
    uint8_t idx = enemyCar.angle & 0x0F;
    ST7735_DrawSprite_TransparentCustom(enemyCar.x, enemyCar.y, EnemySprites[idx], 14, 14);
}

void EraseEnemyCarBackground(void){
    const int enemyW = 14; const int enemyH = 14;
    for(int row = 0; row < enemyH; row++){
        for(int col = 0; col < enemyW; col++){
            int16_t px = enemyCar.x + col;
            int16_t py = enemyCar.y - row;
            if(px >= 0 && px < 128 && py >= 0 && py < 160){
                uint16_t bgColor = GetBackgroundPixel(px, py);
                ST7735_DrawPixel(px, py, bgColor);
            }
        }
    }
}

void RedrawAllObjects(void){
    for(uint8_t i = 0; i < MAX_OBJECTS; i++){
        if(Collidables[i].type == OBJ_BANANA){
            ST7735_DrawSprite_TransparentCustom(Collidables[i].x, Collidables[i].y, Collidables[i].image, Collidables[i].width, Collidables[i].height);
        } else {
            if(Collidables[i].active){
                ST7735_DrawSprite_TransparentCustom(Collidables[i].x, Collidables[i].y, Collidables[i].image, Collidables[i].width, Collidables[i].height);
            }
        }
    }
}

void HUD_DrawIcon(int x, int y, uint8_t item){
    const uint16_t *icon = NULL;
    switch(item){
        case 0: icon = smallBan;  break;
        case 1: icon = smallBom;  break;
        case 2: icon = lapDec;    break;
        default: icon = NULL;     break;
    }
    if(icon == NULL){
        ST7735_FillRect(x, y-7, 8, 8, ST7735_BLACK);
        return;
    }
    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){
            uint16_t color = icon[row*8 + col];
            int16_t px = x + col;
            int16_t py = y - row;
            if(px >= 0 && px < 128 && py >= 0 && py < 160){
                if(color == TRANSPARENT_COLOR) ST7735_DrawPixel(px, py, ST7735_BLACK);
                else ST7735_DrawPixel(px, py, color);
            }
        }
    }
}

void HUD_Draw(void){
    ST7735_FillRect(0, 0, 128, 10, ST7735_BLACK);
    int iconY = 9; int x = 4;      
    for(int i = 0; i < 3; i++){
        if(i <= invTop){
            HUD_DrawIcon(x, iconY, inventory[i]);
        } else {
            HUD_DrawIcon(x, iconY, 255);
        }
        x += 14;
    }
    ST7735_SetTextColor(ST7735_YELLOW);
    ST7735_SetCursor(9, 0);
    if(LanguageMode == 0) printf("Lap %d/10", lapCount);
    else printf("Vuelta %d/10", lapCount);
}