#ifndef __images_h
#define __images_h
#include <stdint.h>

// Backgrounds
extern const unsigned short OvalTrack[];
extern const unsigned short startline[];

// Obstacles & Items
extern const unsigned short water_collision[];
extern const unsigned short treasure_box[];
extern const unsigned short question[];
extern const unsigned short pipe[];
extern const unsigned short waymo[];
extern const unsigned short bomb[];
extern const unsigned short banana[];

// UI / HUD Icons
extern const unsigned short lapDec[];
extern const unsigned short smallBan[];
extern const unsigned short smallBom[];
extern const unsigned short questionInventory[];

// Player Car Sprites
extern const unsigned short Car_0[];
extern const unsigned short Car_1[];
extern const unsigned short Car_2[];
extern const unsigned short Car_3[];
extern const unsigned short Car_4[];
extern const unsigned short Car_5[];
extern const unsigned short Car_7[];
extern const unsigned short Car_8[];
// (Ensure you have definitions for Car_6, Car_9...Car_15 if your code uses them, 
// otherwise the linker will fail when JeepSprites[] tries to point to them).

// Enemy Car Sprites
extern const unsigned short Enemy_0[];
extern const unsigned short Enemy_1[];
extern const unsigned short Enemy_2[];
extern const unsigned short Enemy_3[];
extern const unsigned short Enemy_4[];
extern const unsigned short Enemy_5[];
extern const unsigned short Enemy_7[];
extern const unsigned short Enemy_8[];

#endif