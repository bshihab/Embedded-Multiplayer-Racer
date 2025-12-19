#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <stdint.h>
#include "GameConstants.h"





// Data Structures
typedef struct Objects{
  int16_t x;            
  int16_t y;              
  uint8_t width;
  uint8_t height;
  uint8_t velocity;
  uint8_t active;    
  ObjectType_t type;     
  const uint16_t *image;
} Collider_t;

typedef struct Jeep {
 uint16_t x;        
 uint16_t y;
 uint16_t length;
 uint16_t width;        
 int16_t currentSpeed;  
 uint8_t angle;
 const uint16_t *image;    
} Jeep_t;

typedef struct {
   uint16_t x;
   uint16_t y;
   uint8_t angle;
} RemotePlayer_t;

// Global Variable Declarations (extern)
extern Jeep_t playerCar;
extern RemotePlayer_t enemyCar;
extern Collider_t Collidables[MAX_OBJECTS];

extern uint8_t treasureCount;
extern uint8_t chestA_collected;
extern uint8_t chestB_collected;
extern uint8_t lapCount;
extern uint8_t enemyLapCount;
extern uint8_t bananaCount;
extern uint8_t bombCount;
extern uint8_t ldCount;
extern uint8_t inventory[3];
extern int8_t invTop;
extern uint8_t questionUses;
extern uint8_t gameOver;

extern uint8_t ledFlashTimer;
extern uint8_t bouncing;
extern uint8_t bounceFrames;
extern uint8_t bananaSpinMode; 
extern uint8_t bananaSpinFrame; 
extern uint8_t bananaSpinCycles;  
extern uint8_t bananaImmunity;
extern int8_t waymoDir[7];

extern uint8_t treasureRespawnTimer[MAX_OBJECTS];
extern uint8_t questionRespawnTimer[MAX_OBJECTS];
extern uint8_t bananaActivationTimer[MAX_OBJECTS];
extern uint8_t bombActivationTimer[MAX_OBJECTS];

// Networking Globals
extern uint8_t sendBomb;
extern uint8_t sendBanana;
extern uint8_t sendLapDec;
extern uint8_t global_bx;
extern uint8_t global_by;
extern uint8_t global_box;
extern uint8_t global_boy;

extern const int16_t QuestionSpawnX[NUM_Q_SPAWNS];
extern const int16_t QuestionSpawnY[NUM_Q_SPAWNS];

// Sprite Arrays
extern const unsigned short *JeepSprites[16];
extern const unsigned short *EnemySprites[16];

// Function Prototypes
void InitJeep(void);
void InitLevel(void);
void Random_Init(uint32_t seed);
uint32_t Random(void);
uint8_t RandomItem(void);
int8_t FindFreeObjectSlot(void);
void SpawnBanana(void);
void SpawnBomb(int16_t x, int16_t y);
void UpdateWaymo(uint8_t index, int8_t dirArray[]);
void Movement(uint32_t gas_pressed, uint32_t brake_pressed);

#endif