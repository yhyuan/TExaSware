// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 11, 2014

// Lab 8
//      Jon Valvano and Ramesh Yerraballi
//      November 21, 2013

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
unsigned long In;  // input from PE0
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(unsigned long time);  // Delay100ms
void PortE_Init(void);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1);  // activate grader and set system clock to 80 MHz
  PortE_Init();
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
     Delay100ms(1);
     In = GPIO_PORTE_DATA_R&0x01;   // read PE0 into In
     if (In == 0x01) {
         GPIO_PORTE_DATA_R = GPIO_PORTE_DATA_R ^ 0x02; // toggle the LED
     } else {
         GPIO_PORTE_DATA_R |= 0x02;         // the LED ON (make PE1 =1). 
     }

  }
  
}

// Subroutine to initialize port E pins for input and output
// PF4 is input SW1 and PF2 is output Blue LED
// Inputs: None
// Outputs: None
// Notes: ...
void PortE_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) E clock
  delay = SYSCTL_RCGC2_R;           // delay

  //GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  //GPIO_PORTF_CR_R |= 0x1F;           // allow changes to PF4-0 
  // only PF0 needs to be unlocked, other bits can't be locked

  GPIO_PORTE_AMSEL_R &= ~0x03;        // 3) disable analog function ON PE0, PE1
  GPIO_PORTE_PCTL_R &= ~0x000000FF;   // 4) GPIO clear bit PCTL on PE0, PE1
  GPIO_PORTE_DIR_R &= ~0x01;          // 5.1) PE0 input, 
  GPIO_PORTE_DIR_R |= 0x02;          // 5.2) PE1 output  
  GPIO_PORTE_AFSEL_R &= ~0x03;        // 6) no alternate function ON PE0, PE1
  //GPIO_PORTE_PUR_R |= 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTE_DEN_R |= 0x03;          // 7) enable digital pins PE0, PE1
  GPIO_PORTE_DATA_R |= 0x02;         // The system starts with the LED ON (make PE1 =1). 
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
