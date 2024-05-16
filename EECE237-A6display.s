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
       GET tm4c123gh6pm.s
	   ALIGN 
;-----------------------------------------------------------
ONESEC             DCD 5333333      ; approximately 1s delay at ~16 MHz clock
QUARTERSEC         DCD 1333333      ; approximately 0.25s delay at ~16 MHz clock
FIFTHSEC           DCD 1066666      ; approximately 0.2s delay at ~16 MHz clock
;; Delcare global variables here if needed
   
Colors SPACE 16;
Pattern SPACE 6;

;;------------ DO NOT MODIFY OR REMOVE THIS SECTION------------
       AREA    |.text|, CODE, READONLY, ALIGN=2
 ;      GET tm4c123gh6pm.s
	   EXPORT  Start
;;------------------------------------------------------------


Start	proc		; DO NOT MODIFY THIS LINE. THIS MARKS THE START OF YOUR ASSEMBLY PROGRAM.

; board initialization here if needed
; insert all pins initilization code here before entering the main endless loop.

	LDR R0, =Colors;
	LDR R2, =0x400253FC;

	MOV R1, #0x0002; 
	STR R1, [R0];
	STR R1, [R2];

	MOV R1, #0x0004;
	STR R1, [R0, #2];
	STR R1, [R2];

	MOV R1, #0x0006;
	STR R1, [R0, #4];
	STR R1, [R2];

	MOV R1, #0x0008;
	STR R1, [R0, #6];
	STR R1, [R2];

	MOV R1, #0x000A;
	STR R1, [R0, #8];
	STR R1, [R2];

	MOV R1, #0x000C;
	STR R1, [R0, #10];	
	STR R1, [R2];

	MOV R1, #0x000E;
	STR R1, [R0, #12];
	STR R1, [R2];

	MOV R1, #0x0000;
	STR R1, [R2, #14];

loop   
	; main program loop
	; insert your code here

	LDR R5, =Pattern;	

	LDR R6, [R5];
	STR R6, [R2];
	BL delay;	
	LDR R6, [R5, #1];
	STR R6, [R2];
	BL delay;
	LDR R6, [R5, #2];
	STR R6, [R2];
	BL delay;
	LDR R6, [R5, #3];
	STR R6, [R2];
	BL delay;
	LDR R6, [R5, #4];
	STR R6, [R2];
	BL delay;
	LDR R6, [R5, #5];
	STR R6, [R2];
	BL delay;

		B   loop		; do not modify or delete this line. 


;-------------DO NOT MODIFY ANY LINES BELOW THIS SECTION------------
;------------delay------------
; Delay function for testing, which delays about 3*count cycles.
; Input: R0  count
; Output: none

delay
    LDR	 R3, = ONESEC
	LDR	 R4, [R3]
d	SUBS R4, R4, #1                 ; R0 = R0 - 1 (count = count - 1)
    BNE  d                           ; if count (R0) != 0, skip to 'd'
    BX   LR                          ; return

       ALIGN      
       ENDP 
       END 

