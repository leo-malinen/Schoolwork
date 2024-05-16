
// this program uses integer operations to convert Temperatures from Celcius to Fahrenheit
// and from Fahrenheit to Celcius
// after each conversion it estimates the resulted difference in degrees from the integer 
// estimation. It also calculates Temperature in Kelvin. 
// Write the assembly program that will perform the same operations. 

#include <stdint.h>



// Defined in display.s

void Start(void);
// define global variables here



int main(void) {

	uint32_t TinF=100;
	uint32_t TinC=24;
	
	uint32_t TC, TF, TK, TFdiff, TCdiff;
	
	// convert Fahrenheit to Celcius
	
	 TC = ((TinF-32)*5)/9;
	// calculate the original value with the integer operation approximation 
	
		TF = ((TC * 9)/5)+32;
	// compare the two values
	
	TFdiff=TinF-TF; 
	
 // convert Celcius to Fahrenheit	
	
   TF= ((TinC * 9)/5)+32;
// calcualate the original value with integer approximation. 
	 
	 TC = ((TF-32)*5)/9;
	
	// compare the results
	
	TCdiff= TinC-TC;
	
	// get approximated Temperature in Kelvin
	
	TK=TC+273;
	
	Start();				//do not modify or delete this line.
	
	return 0;
}
