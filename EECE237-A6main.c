
// This program is provided to enable smoother debugging of assembly programs
// Do not modify or change this program. 
// it provides an entry to the assembly program, to be completed for the assignment. 



#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PortF_Init.h"




// Defined in display.s

void Start(void);
// define global variables here




int main(void) {

	PortF_Init();	
	
	Start();				//do not modify or delete this line.
	
	return 0;
}
