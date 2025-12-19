/*
 * LED.h
 * Drivers for PA28 (Red), PA27 (Yellow), PA26 (Green 1), PA25 (Green 2)
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>

// Initialize all LEDs
void LED_Init(void);

// --- RED LED (PA28) ---
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Red_Toggle(void);

// --- YELLOW LED (PA27) ---
void LED_Yellow_On(void);
void LED_Yellow_Off(void);
void LED_Yellow_Toggle(void);

// --- GREEN 1 LED (PA26) ---
void LED_Green1_On(void);
void LED_Green1_Off(void);
void LED_Green1_Toggle(void);

// --- GREEN 2 LED (PA25) ---
void LED_Green2_On(void);
void LED_Green2_Off(void);
void LED_Green2_Toggle(void);

// --- ALL LEDs (For Blinking) ---
void LED_All_On(void);
void LED_All_Off(void);
void LED_All_Toggle(void);

#endif /* LED_H_ */