// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// Period =  80000000/32/Freq=2500000/Freq
#define C1   1194   // 2093 Hz
#define B1   1265   // 1975.5 Hz
#define BF1  1341   // 1864.7 Hz
#define A1   1420   // 1760 Hz
#define AF1  1505   // 1661.2 Hz
#define G1   1594   // 1568 Hz
#define GF1  1689   // 1480 Hz
#define F1   1790   // 1396.9 Hz
#define E1   1896   // 1318.5 Hz
#define EF1  2009   // 1244.5 Hz
#define D1   2128   // 1174.7 Hz
#define DF1  2255   // 1108.7 Hz
#define C    2389   // 1046.5 Hz
#define B    2531   // 987.8 Hz
#define BF   2681   // 932.3 Hz
#define A    2841   // 880 Hz
#define AF   3010   // 830.6 Hz
#define G    3189   // 784 Hz
#define GF  3378   // 740 Hz
#define F   3579   // 698.5 Hz
#define E   3792   // 659.3 Hz
#define EF  4018   // 622.3 Hz
#define D   4257   // 587.3 Hz
#define DF  4510   // 554.4 Hz
#define C0  4778   // 523.3 Hz
#define B0  5062   // 493.9 Hz
#define BF0 5363   // 466.2 Hz
#define A0  5682   // 440 Hz
#define AF0 6020   // 415.3 Hz
#define G0  6378   // 392 Hz
#define GF0 6757   // 370 Hz
#define F0  7159   // 349.2 Hz
#define E0  7584   // 329.6 Hz
#define EF0 8035   // 311.1 Hz
#define D0  8513   // 293.7 Hz
#define DF0 9019   // 277.2 Hz
#define C7  9556   // 261.6 Hz
#define B7  10124   // 246.9 Hz
#define BF7 10726   // 233.1 Hz
#define A7  11364   // 220 Hz
#define AF7 12039   // 207.7 Hz
#define G7  12755   // 196 Hz
#define GF7 13514   // 185 Hz
#define F7  14317   // 174.6 Hz
#define E7  15169   // 164.8 Hz
#define EF7 16071   // 155.6 Hz
#define D7  17026   // 146.8 Hz
#define DF7 18039   // 138.6 Hz
#define C6  19111   // 130.8 Hz


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
  while(1){                
    // input from keys to select tone
		// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
		// 0x04 is key 2 pressed, 0x08 is key 3 pressed
		//EnableInterrupts();
		unsigned long piano = Piano_In();
		if (piano == 0x01) {
		  Sound_Tone(C0*2);
		} else if (piano == 0x02) {
		  Sound_Tone(D*2);
		} else if (piano == 0x04) {
		  Sound_Tone(E*2);
		} else if (piano == 0x08) {
		  Sound_Tone(G*2);
		} else {
		  Sound_Off();
		}
		delay(10);
  }            
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


