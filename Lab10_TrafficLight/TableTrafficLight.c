// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define SENSOR  (*((volatile unsigned long *)0x4002400C))
#define LIGHT   (*((volatile unsigned long *)0x400050FC))
// Linked data structure
struct State {
  unsigned long Out;
  unsigned long OutPF;
  unsigned long Time; 
  unsigned long Next[8];};
typedef const struct State STyp;
#define goW   0
#define waitW 1
#define goS   2
#define waitS 3
#define walk  4
#define notWalk  5
#define walkOff  6
#define notWalk2  7
#define walkOff2  8

/*STyp FSM[4]={
 {0x21,3000,{goN,waitN,goN,waitN}},
 {0x22, 500,{goE,goE,goE,goE}},
 {0x0C,3000,{goE,goE,waitE,waitE}},
 {0x14, 500,{goN,goN,goN,goN}}};
*/ 
STyp FSM[9]={
 {0x0C, 0x02, 100,{     goW,     goW,   waitW,   waitW,   waitW,   waitW,   waitW,  waitW}},  //goW
 {0x14, 0x02, 60, {   waitW,     goW,     goS,     goS,    walk,    walk,     goS,    goS}}, //waitW
 {0x21, 0x02, 100,{     goS,   waitS,     goS,   waitS,   waitS,   waitS,   waitS,  waitS}},  //goS
 {0x22, 0x02, 60, {   waitS,     goW,     goS,     goW,    walk,    walk,    walk,   walk}}, //waitS
 {0x24, 0x08, 100,{    walk, notWalk, notWalk, notWalk,    walk, notWalk, notWalk,notWalk}},  //walk
 {0x24, 0x02, 60, { notWalk, walkOff, walkOff, walkOff, walkOff, walkOff, walkOff,walkOff}},  //notWalk
 {0x24, 0x00, 60, {walkOff,notWalk2,notWalk2,notWalk2,notWalk2,notWalk2,notWalk2,notWalk2}},  //walkOff
 {0x24, 0x02, 60, {notWalk2,walkOff2,walkOff2,walkOff2,walkOff2,walkOff2,walkOff2,walkOff2}},  //notWalk2
 {0x24, 0x00, 60, {walkOff2,     goW,     goS,     goW,    walk,     goW,     goS,     goW}}};  //walkOff2

unsigned long S;  // index to the current state
unsigned long Input;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****
 /*
void PLL_Init(void){
  // 0) Use RCC2
  SYSCTL_RCC2_R |=  0x80000000;  // USERCC2
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |=  0x00000800;  // BYPASS2, PLL bypass
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0)   // clear XTAL field, bits 10-6
                 + 0x00000540;   // 10101, configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~0x00000070;  // configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~0x00002000;
  // 4) set the desired system divider
  SYSCTL_RCC2_R |= 0x40000000;   // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~ 0x1FC00000)  // clear system clock divider
                  + (4<<22);      // configure for 80 MHz clock
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&0x00000040)==0){};  // wait for PLLRIS bit
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~0x00000800;
}
*/
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}

void PortBEF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x32;      // 1) B E F
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
	//GPIO_PORTE_PUR_R |= 0x07; 
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0

  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0

  GPIO_PORTF_AMSEL_R &= ~0x0A;        // 3) disable analog on PF1, PF3
  GPIO_PORTF_PCTL_R &= ~0x0000F0F0;   // 4) PCTL GPIO on PF1, PF3
  GPIO_PORTF_DIR_R |= 0x0A;         // 5) PF3, PF1 out
  GPIO_PORTF_AFSEL_R &= ~0x0A;        // 6) disable alt funct on PF1, PF3
  GPIO_PORTF_DEN_R |= 0x0A;          // 7) enable digital I/O on PF1, PF3
}

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
  //PLL_Init();       // 80 MHz, Program 10.1
  SysTick_Init();   // Program 10.2
  PortBEF_Init();   // initialize PB,PE,PF
  S = goW;  
  EnableInterrupts();
  while(1){
	  LIGHT = FSM[S].Out;  // set lights
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & (~0x0A)) | FSM[S].OutPF; // set PF1 and PF3
		//GPIO_PORTF_DATA_R = FSM[S].OutPF;
    SysTick_Wait10ms(FSM[S].Time);
		Input = GPIO_PORTE_DATA_R;
    //Input = SENSOR;     // read sensors
    S = FSM[S].Next[Input];  
  }
}

