; main.s
; Runs on any Cortex M processor
; Activity_03 
; see a program run in C and assembly
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
	   ALIGN 
;-----------------------------------------------------------

;; Delcare global variables here if needed
   


;;------------ DO NOT MODIFY OR REMOVE THIS SECTION------------
       AREA    |.text|, CODE, READONLY, ALIGN=2
       GET tm4c123gh6pm.s
	   EXPORT  Start
;;------------------------------------------------------------

Start	proc		; DO NOT MODIFY THIS LINE. THIS MARKS THE START OF YOUR ASSEMBLY PROGRAM.

; board initialization here if needed
; insert all pins initilization code here before entering the main endless loop.


loop   
	; main program loop
	; insert your code here

   MOV R0, #0x00000064;
   MOV R1, #0x00000018;
   MOV R2, #0;
   MOV R3, #0;
   MOV R4, #0;
   MOV R5, #0;
   MOV R6, #0;
   MOV R7, #5;
   MOV R8, #9;
   MOV R9, #91;
   
   
   SUB R2, R0, #32;
   MUL R2, R2, R7;
   UDIV R2, R2, R8; Fahrenheit to Celsius

   MUL R3, R2, R8;
   UDIV R3, R3, R7;
   ADD R3, R3, #32; Compare TF

   SUB R5, R0, R3; TFdiff variable
   
   MUL R3, R1, R8;
   UDIV R3, R3, R7;
   ADD R3, R3, #32; Celsius to Fahrenheit   
   
   SUB R2, R3, #32;
   MUL R2, R2, R7;
   UDIV R2, R2, R8; Compare TC
   
   SUB R6, R1, R2; TCdiff variable
   
   ADD R4, R2, R9;
   ADD R4, R4, R9;
   ADD R4, R4, R9; Kelvin

	B   loop		; do not modify or delete this line. 

		

       ALIGN      
       ENDP 
       END 

