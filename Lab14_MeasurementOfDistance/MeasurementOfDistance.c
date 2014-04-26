// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// April 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE1 and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10];
unsigned long Distance; // units 0.001 cm
unsigned long ADCdata;  // 12-bit 0 to 4095 sample
unsigned long Flag;     // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  //return 0;  // replace this line with real code
	
	//return ((500.1221*ADCdata)>>10)+0;
	return ((500*ADCdata)>>10)+1;
	//return long(2000 * ADCdata/4095.0);
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;           
  NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	GPIO_PORTF_DATA_R ^= 0x02;      //1) Toggle PF1 
	GPIO_PORTF_DATA_R ^= 0x02;      //2) Toggle PF1 again 
	ADCdata = ADC0_In();						//3) Sample the ADC, calling your ADC0_In() 
	Distance = Convert(ADCdata);		//4) Convert the sample to Distance, calling your Convert(), and storing the result into the global 
	Flag = 1;												//5) Set the Flag, signifying new data is ready 
	GPIO_PORTF_DATA_R ^= 0x02;      //6) Toggle PF1 a third time
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
  if(n < 10) {
		String[0] = '0';
		String[2] = '0';
		String[3] = '0';
		String[4] = n + '0';
	} else if (n < 100) {
		String[0] = '0';
		String[2] = '0';
		String[3] = n/10 + '0';
		String[4] = (n%10) + '0';
	} else if (n < 1000) {
		String[0] = '0';
		String[2] = n/100 + '0';
		String[3] = (n%100)/10 + '0';
		String[4] = (n%10) + '0';
	} else if (n < 10000) {
		String[0] = n/1000 + '0';
		String[2] = (n%1000)/100 + '0';
		String[3] = (n%100)/10 + '0';
		String[4] = (n%10) + '0';
	} else {
		String[0] = '*';
		String[2] = '*';
		String[3] = '*';
		String[4] = '*';
	}
	String[1] = '.';
	String[5] = ' ';
	String[6] = 'c';
	String[7] = 'm';
	String[8] = '\0';
}

void PF1_Init(void) {
	SYSCTL_RCGC2_R |= 0x00000020; // activate port F
  GPIO_PORTF_DIR_R |= 0x02;   // make PF1 output (PF1 built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x02;// disable alt funct on PF1
  GPIO_PORTF_DEN_R |= 0x02;   // enable digital I/O on PF1
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFFF0F)+0x00000000;
  GPIO_PORTF_AMSEL_R &= ~0x02;     // disable analog functionality on PF1
}

int main1(void){ 
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
// initialize ADC0, channel 1, sequencer 3
// initialize Nokia5110 LCD (optional)
// initialize SysTick for 40 Hz interrupts
// initialize profiling on PF1 (optional)
                                    //    wait for clock to stabilize

  EnableInterrupts();
// print a welcome message  (optional)
  while(1){ 
// read mailbox
// output to Nokia5110 LCD (optional)
  }
}

int main2(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
		//Distance = Convert(ADCdata);
  }
}
int main3(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    
  }
}

int main(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
	SysTick_Init(16000 * 25 * 5);  //initialize SysTick for 40 Hz interrupts
	PF1_Init();
  EnableInterrupts();
  while(1){ 
		Flag = 0; 			//Clear the Flag to 0 
    while(!Flag);		//Wait for the Flag to be set 
		UART_ConvertDistance(Distance);//Convert Distance to String, calling your UART_ConvertDistance() 
		Nokia5110_SetCursor(0, 0);//Optional: output the string 
		Nokia5110_OutString(String); //    a) Nokia5110_SetCursor(0, 0); Nokia5110_OutString(String); 
										//    b) UART_OutString(String); UART_OutChar('\n');		
  }
}
