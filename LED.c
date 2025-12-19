/*
 * LED.c
 * Runs on MSPM0G3507
 * Implements PA28 (Red), PA27 (Yellow), PA26 (Green1), PA25 (Green2)
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"

// PIN Bit Definitions for Port A
#define PIN_RED     (1 << 28)
#define PIN_YELLOW  (1 << 27)
#define PIN_GREEN1  (1 << 26)
#define PIN_GREEN2  (1 << 25)
#define PIN_ALL     (PIN_RED | PIN_YELLOW | PIN_GREEN1 | PIN_GREEN2)

void LED_Init(void){
    // 1. Configure PINCM indices for GPIO Output (Value 0x00000081)
    // We use the indices defined in your LaunchPad.h
    IOMUX->SECCFG.PINCM[PA25INDEX] = 0x00000081; // PA25 (Green 2)
    IOMUX->SECCFG.PINCM[PA26INDEX] = 0x00000081; // PA26 (Green 1)
    IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00000081; // PA27 (Yellow)
    IOMUX->SECCFG.PINCM[PA28INDEX] = 0x00000081; // PA28 (Red)

    // 2. Enable Digital Output on Port A (Bits 28, 27, 26, 25)
    GPIOA->DOE31_0 |= PIN_ALL;
    
    // 3. Start with all LEDs OFF
    GPIOA->DOUTCLR31_0 = PIN_ALL;
}

// --- RED (PA28) ---
void LED_Red_On(void){     GPIOA->DOUTSET31_0 = PIN_RED; }
void LED_Red_Off(void){    GPIOA->DOUTCLR31_0 = PIN_RED; }
void LED_Red_Toggle(void){ GPIOA->DOUTTGL31_0 = PIN_RED; }

// --- YELLOW (PA27) ---
void LED_Yellow_On(void){     GPIOA->DOUTSET31_0 = PIN_YELLOW; }
void LED_Yellow_Off(void){    GPIOA->DOUTCLR31_0 = PIN_YELLOW; }
void LED_Yellow_Toggle(void){ GPIOA->DOUTTGL31_0 = PIN_YELLOW; }

// --- GREEN 1 (PA26) ---
void LED_Green1_On(void){     GPIOA->DOUTSET31_0 = PIN_GREEN1; }
void LED_Green1_Off(void){    GPIOA->DOUTCLR31_0 = PIN_GREEN1; }
void LED_Green1_Toggle(void){ GPIOA->DOUTTGL31_0 = PIN_GREEN1; }

// --- GREEN 2 (PA25) ---
void LED_Green2_On(void){     GPIOA->DOUTSET31_0 = PIN_GREEN2; }
void LED_Green2_Off(void){    GPIOA->DOUTCLR31_0 = PIN_GREEN2; }
void LED_Green2_Toggle(void){ GPIOA->DOUTTGL31_0 = PIN_GREEN2; }

// --- ALL LEDs ---
void LED_All_On(void){     GPIOA->DOUTSET31_0 = PIN_ALL; }
void LED_All_Off(void){    GPIOA->DOUTCLR31_0 = PIN_ALL; }
void LED_All_Toggle(void){ GPIOA->DOUTTGL31_0 = PIN_ALL; }