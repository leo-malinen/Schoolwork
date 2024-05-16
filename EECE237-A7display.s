; main.s
; Runs on any Cortex M processor
; Activity_05
; MEMORY OPERATIONS
;
; Author: Place your name here
; Date Created: 
; Date Modified:
; Insert a brief description of your program solution here 
; Lab number and lab partner if applicable 
; list all inputs and outputs here.

; we align 32 bit variables to 32-bits
; we align op codes to 16 bits

;------ DO NOT MODIFY OR REMOVE THIS SECTION ---------------      
	   THUMB
       AREA    DATA, ALIGN=4
;       GET tm4c123gh6pm.s
	   ALIGN 
;-----------------------------------------------------------
ONESEC             DCD 5333333      ; approximately 1s delay at ~16 MHz clock
QUARTERSEC         DCD 1333333      ; approximately 0.25s delay at ~16 MHz clock
FIFTHSEC           DCD 1066666      ; approximately 0.2s delay at ~16 MHz clock
	
; memory addresses for Port F control registers. 

GPIO_PORTF_DATA_R       EQU   0x400253FC
GPIO_PORTF_DIR_R        EQU   0x40025400
GPIO_PORTF_DEN_R        EQU   0x4002551C

; System control register use either one to turn on the clock for any port

SYSCTL_SCGC2_R          EQU   0x400FE118
	
SYSCTL_RCGCGPIO_R       EQU   0x400FE608
	
;; Delcare global variables here if needed
   

;;------------ DO NOT MODIFY OR REMOVE THIS SECTION------------
       AREA    |.text|, CODE, READONLY, ALIGN=2
 ;      GET tm4c123gh6pm.s
	   EXPORT  Start
;;------------------------------------------------------------


Start	proc		; DO NOT MODIFY THIS LINE. THIS MARKS THE START OF YOUR ASSEMBLY PROGRAM.

; board initialization here 
; insert all pins initilization code here before entering the main endless loop.

	LDR R0, =SYSCTL_RCGCGPIO_R;
	LDR R1, [R0];
	ORR R1, #0x20;
	STR R1, [R0];

	NOP
	NOP
	NOP
	NOP

	LDR R0, =GPIO_PORTF_DIR_R;
	LDR R1, [R0];
	ORR R1, #0x0E;
	STR R1, [R0];
	
	LDR R0, =GPIO_PORTF_DEN_R;
	LDR R1, [R0];
	ORR R1, #0x0E;
	STR R1, [R0];

loop   
	; main program loop
	; insert your code here
	LDR R0, =GPIO_PORTF_DATA_R;
	LDR R4, =0x03;
	
	LSL R4, R4, #2;
	STR R4, [R0];

	BL delay;

	LSR R4, R4, #1;
	STR R4, [R0];
	
	BL delay

	LSL R4, R4, #2;
	STR R4, [R0];

	BL delay;

	LSR R4, R4, #3;
	STR R4, [R0];

	BL delay;
	
	LSR R4, R4, #1;
	STR R4, [R0];

	BL delay;
	
	
	B   loop		; do not modify or delete this line. 


;-------------DO NOT MODIFY ANY LINES BELOW THIS SECTION------------
;------------delay------------
; Delay function for testing, which delays about 3*count cycles.
; Input: R0  count
; Output: none

delay
    LDR	 R3, = ONESEC
	LDR	 R5, [R3]
d	SUBS R5, R5, #1                 ; R0 = R0 - 1 (count = count - 1)
    BNE  d                           ; if count (R0) != 0, skip to 'd'
    BX   LR                          ; return

       ALIGN      
       ENDP 
       END 

