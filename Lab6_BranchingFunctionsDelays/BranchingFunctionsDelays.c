// BranchingFunctionsDelays.c Lab 6
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  
// This lab will use the hardware already built into the LaunchPad.
// Daniel Valvano, Jonathan Valvano
// January 8, 2014

/*
SYSTEM REQUIREMENTS

The system has one input switch and one output LED. Figure 6.1 shows the system when simulated as the switch is touched. A negative logic switch means the PF4 signal will be 1 (high, 3.3V) if the switch is not pressed, and the PF4 signal will be 0 (low, +0V) if the switch is pressed. A positive logic blue LED interface means if the software outputs a 1 to PF2 (high, +3.3V) the LED will turn ON, and if the software outputs a 0 to PF2 (low, 0V) the blue LED will be OFF. Here in Lab 6, you first debug in simulation and then run on the real board, but no external components on the protoboard will be required. The switch and LED are already built into the LaunchPad. However in Lab 8, you will attach a real switch and a real LED to your protoboard (solderless breadboard) and interface them to your microcontroller. Overall functionality of this system is described in the following rules.

1) Make PF2 an output and make PF4 an input (enable PUR for PF4). 
2) The system starts with the LED ON (make PF2 =1). 
3) Delay for about 100 ms
4) If the switch is pressed (PF4 is 0), then toggle the LED once, else turn the LED ON. 
5) Repeat steps 3 and 4 over and over.
*/
// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
//#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
//#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
	unsigned long In;  // input from PF4
  unsigned long i; // output to PF2 (blue LED)	
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(unsigned long time);  // Delay100ms

int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
  // initialization goes here
//volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  //GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  //GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R &= ~0x14;        // 3) disable analog on PF2 4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00;   // 4) PCTL GPIO on PF2 4
  GPIO_PORTF_DIR_R &= ~0x10;          // 5) PF4 in(0)
	GPIO_PORTF_DIR_R |= 0x04;          // 5) PF2 out(1)
  GPIO_PORTF_AFSEL_R &= ~0x14;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R |= 0x10;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R |= 0x14;          // 7) enable digital I/O on PF2, 4
  GPIO_PORTF_DATA_R |= 0x04;         // The system starts with the LED ON (make PF2 =1). 
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    // body goes here
		//for(i=0;i<2666667;i++); // Delay for about 100 ms 100 ms * 80 MHZ/3
		Delay100ms(1);
		In = GPIO_PORTF_DATA_R&0x10;   // read PF4 into Sw1
		if (In == 0) {
		   GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R ^ 0x04;
		} else {
		   GPIO_PORTF_DATA_R |= 0x04; 
		}
  }
}

void Delay100ms(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 1333333;  // this number means 100ms
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements every 100 ms
  }
}
