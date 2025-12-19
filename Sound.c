// Sound.c
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 

#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC.h"
#include "../inc/Timer.h"
#include "../inc/DAC5.h"

uint16_t currentIndex;
uint32_t maxIndex;
const uint16_t *currentSound;


void Sound_Init(void){
  // write this

  DAC_Init();          // Port B is DAC
  currentIndex = 0;
  SysTick->CTRL = 0;         // disable SysTick during setup
  SysTick->LOAD = 7272;  // reload value
  SysTick->VAL = 0;          // any write to current clears it
  SCB->SHP[1] = SCB->SHP[1]&(~0xC0000000)|0x40000000; // set priority = 1
  SysTick->CTRL = 0x0007;    // enable SysTick with core clock and interrupts

}

void SysTick_Handler(void){ // called at 11 kHz
    // output one value to DAC if a sound is active
  
    if (currentIndex >= maxIndex){
      currentIndex = 0;
    }
    else{
    currentIndex = (currentIndex + 1);
    DAC_Out(currentSound[currentIndex]);
    }
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement


// 16 for 12 bit, 8 for 5 bit
void Sound_Start(const uint16_t *pt, uint32_t count){
// write this
    currentSound = pt;
    maxIndex = count;
    currentIndex =0;
    SysTick->LOAD = 7272;
    
}