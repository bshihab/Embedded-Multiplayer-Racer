#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include <stdint.h>
#include "../inc/ST7735.h"

// Colors
#define BACKGROUND_BROWN 0x118B
#define WALL_CYAN        ST7735_CYAN
#define WALL_YELLOW      ST7735_YELLOW
#define TRANSPARENT_COLOR 0xD11D
#define BACKGROUND_GREEN 0x4D84

// Game Logic Constants
#define NUM_Q_SPAWNS 6
#define MAX_QUESTION_USES 6
#define MAX_OBJECTS 12

// Map Boundaries
// Outer box
#define OUT_LEFT   2
#define OUT_RIGHT  126
#define OUT_TOP    9
#define OUT_BOTTOM 158

// Inner box 
#define IN_LEFT    40
#define IN_RIGHT   80
#define IN_TOP     55
#define IN_BOTTOM  105

// Finish Line
#define FINISH_X   107    
#define FINISH_Y   120  
#define FINISH_W   15    
#define FINISH_H   4      

// Waymo Bounds
#define WAYMO_LEFT_BOUND 10
#define WAYMO_RIGHT_BOUND 115

// Enums
typedef enum {
    STATE_TITLE,
    STATE_LANGUAGE_SELECT,
    STATE_CUTSCENE,  
    STATE_COUNTDOWN,
    STATE_RACING
} GameState_t;

typedef enum {
  OBJ_WATER,
  OBJ_TREASURE,
  OBJ_WAYMO,
  OBJ_PIPE,
  OBJ_BOOST,
  OBJ_QUESTION,
  OBJ_BANANA,
  OBJ_BOMB,
  OBJ_LAPDEC
} ObjectType_t;

#endif