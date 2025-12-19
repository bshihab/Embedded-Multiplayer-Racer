#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "GameObjects.h"
#include <stdint.h>
#include <stdio.h>
#include "../inc/ST7735.h"

void ShowTitleScreen(void);
void ShowLanguageSelect(void);
void ShowCutscene(void);
void RunCountdown(void);
void TriggerGameOver(void);

void ST7735_DrawSprite_TransparentCustom(int16_t x, int16_t y, const uint16_t *sprite, uint16_t w, uint16_t h);
void DrawCheckeredWall(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void DrawTrackWalls(void);
void DrawFinishLine(void);
void RedrawFinishLine(void);
uint16_t GetBackgroundPixel(int16_t x, int16_t y);
uint16_t GetTerrainColorUnderCar(void);

void DrawJeep(void);
void EraseJeepBackground(void);
void EraseJeepOverObject(int16_t objX, int16_t objY, uint8_t objW, uint8_t objH, const uint16_t *objImage);

void DrawEnemyCar(void);
void EraseEnemyCarBackground(void);

void RedrawAllObjects(void);
void HUD_Draw(void);
void HUD_DrawIcon(int x, int y, uint8_t item);

#endif