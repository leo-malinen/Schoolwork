// Documentation Section 
// main.c

// Runs on TM4C123 LaunchPad
// Input From: 
// Output to:
//
// 
// Author: Enter your name here
// Date: 
// Last modified: 

// 1. Pre-processor Directives Section
// Constant declarations to access port registers using 
// symbolic names instead of addresses
// include directives
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "delay.h"


// 2. Declarations Section
//   Global Variables
//   Insert Function Prototypes here
void PortD_Init(void);
void PortE_Init(void);
uint32_t digit_to_sevenseg(uint32_t i);

// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable

int main(void){
// Declare variables here
	uint32_t i = 0; // counter i = 0, 1, 2, ... 9, 0, 1, 2, ...
// Initialize the ports here
	
	PortD_Init();		// initialize port D, 
	PortE_Init();   // initialize port E, 
	
	while(1){
			
			// Insert your code here
	    uint32_t seven_seg_code = digit_to_sevenseg(i % 10);
      GPIO_PORTD_DATA_R = (seven_seg_code & 0x0F); 
      GPIO_PORTE_DATA_R = ((seven_seg_code >> 3) & 0x0E); 	
			//Delay(436344);		
			Delay(700000);									
			i++;
  }
}
void PortD_Init(void){
    //Insert PortD initialization and configuration here
    SYSCTL_RCGCGPIO_R |= 0x08; //activate port d
    while((SYSCTL_PRGPIO_R&0x08) == 0){};
    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    GPIO_PORTD_CR_R = 0x0F;           
    GPIO_PORTD_AMSEL_R = 0x00;        
    GPIO_PORTD_PCTL_R = 0x00000000;   
    GPIO_PORTD_DIR_R = 0x0F;          
    GPIO_PORTD_AFSEL_R = 0x00;        
    GPIO_PORTD_DEN_R = 0x0F;         
}

void PortE_Init(void){
    //Insert PortE initialization and configuration here
    SYSCTL_RCGCGPIO_R |= 0x10; //activate port e
    while((SYSCTL_PRGPIO_R&0x10) == 0){}; 
    GPIO_PORTE_LOCK_R = 0x4C4F434B;   
    GPIO_PORTE_CR_R = 0x0E;
    GPIO_PORTE_AMSEL_R = 0x00;
    GPIO_PORTE_PCTL_R = 0x00000000;
    GPIO_PORTE_DIR_R = 0x0E;
    GPIO_PORTE_AFSEL_R = 0x00;
    GPIO_PORTE_DEN_R = 0x0E;
}
uint32_t digit_to_sevenseg(uint32_t i){
    uint32_t sevenseg_codes[10] = {
        0x7F, //0
        0x06, //1
        0xAB, //2
        0x8F, //3
        0xCE, //4
        0x8F, //5
        0xED, //6
        0x07, //7
        0xFF, //8
        0xCF  //9
    };
    return sevenseg_codes[i];
}

