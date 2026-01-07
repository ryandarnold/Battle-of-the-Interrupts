; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
original 	  EQU 0
flexOriginal  EQU 4
ten1 		  EQU 8
lowRemainder1 EQU 12
lowRemainder2 EQU 16
lowRemainder3 EQU 20
countOfNums	  EQU 24
	
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

		
allNines DCD 9999
  

nines	DCD	9999  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
    ; copy/paste Lab 7 solution here
	
	;Binding
							
num			EQU		0			; This is a copy of the input
subtractee	EQU		4			; This is num/10 
result		EQU		8			; result = num - subtractee
D_0			EQU		12
D_1			EQU		16
D_2			EQU		20
D_3			EQU		24
D_4			EQU		28
D_5			EQU		32
D_6			EQU		36
D_7			EQU		40
D_8			EQU		44
D_9			EQU		48
flag		EQU 	52			; zero if all MSBs thus far have been 0
index		EQU 	53
ten			EQU		54
arrayFrame	EQU		55

n	RN	0
	PUSH	{R1, R2, R3, R4, R5, LR}
	
	;Allocation
	SUB		SP, #64
	
	MOV		R1, #0					; Initializing flag to 0
	STRB	R1, [SP, #flag]			; 0 means all MSBs thus far have been 0, 1 is if an actually significant bit has been displayed
	
	STRB	R1, [SP, #index]		; Initializing index to 0 (counts number of digits)
	
	MOV		R1, #12
	ADD		R1, SP
	STR		R1, [SP, #arrayFrame]	; Setting array frame to memory location of D_0	in stack		

	MOV		R1, #10					; Initializing ten to 10
	STRB	R1, [SP, #ten]
	
	STR		n, [SP, #num]			; Initializing num to R0 (input)
	
	STR		n, [SP, #subtractee]	; Initializing subtractee to R0 (input)
	
loopStoreArray
	LDR		R1, [SP, #num]				; subtractee = (n/10) * 10
	LDRB	R2, [SP, #ten]	
	UDIV	R1, R2
	MUL		R1, R2
	STR		R1, [SP, #subtractee]
	
	LDR		R2, [SP, #num]
	SUB		R3, R2, R1
	STR		R3, [SP, #result]			; result = num - subtractee (so now result has the digit[i] value)
	
	LDR		R1, [SP, #num]	
	LDRB	R2, [SP, #ten]	
	UDIV	R1, R2
	STR		R1, [SP, #num]				; n= n/10	(changing n for next loop)
	
	LDR		R3, [SP, #result]
	ADD		R3,	#0x30					; Adding 0x30 to result (to turn into ASCII)
	STR		R3, [SP, #result]
	
	;LDR		R0, [SP, #result]
	;BL		ST7735_OutChar
	
	LDR       	R1, [SP, #result]            ; Putting result into correct index of array
    LDR	    	R2, [SP, #arrayFrame]
    LDRB    	R3, [SP, #index]
    MOV        	R4, #4						 ; Multiply index by 4 because each digit value is 32 bits (increment 4 bytes)
    MUL        	R3, R4
    STR       	R1, [R2, R3]            
    
    LDRB    	R1, [SP, #index]            ;Incrementing index
    ADD        	R1, #1
    STRB    	R1, [SP, #index]
    CMP       	R1, #10						; Once index reaches 10, all values stored thusfar start displaying
    BNE        	loopStoreArray   			; If less then 10, then go to next digit
    
	
;DISPLAY CHARACTERS NOW BASED ON ITEMS STORED INTO ARRAY
    LDRB    	R0, [SP, #D_9]				
	CMP			R0, #0x30					; If MSB is ASCII zero, then display
	BEQ			Skip9
    BL        	ST7735_OutChar
	LDRB		R1, [SP, #flag]				; If bit is not zero, then set flag ie always display all following digits
	MOV			R1, #1						; (We just don't want Most significant zeroes because they have no significance)
	STRB		R1, [SP, #flag]
	
Skip9
    LDRB    	R0, [SP, #D_8]
	LDRB		R1, [SP, #flag]				; If flag is 1, ie 
	CMP			R1, #1						; If any previous digit was nonzero, display current digit, even if it is zero
	BEQ			noPass8
	CMP			R0, #0x30					; If flag not set (previous digits were all zero) and current digit is zero,
	BEQ			Skip8						; then don't display
noPass8
    BL      	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]
	
Skip8
    LDRB    	R0, [SP, #D_7]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass7
	CMP			R0, #0x30
	BEQ			Skip7
noPass7
    BL       	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]
	
Skip7	
    LDRB    	R0, [SP, #D_6]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass6
	CMP			R0, #0x30
	BEQ			Skip6
noPass6
    BL        	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]
	
Skip6
	LDRB    	R0, [SP, #D_5]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass5
	CMP			R0, #0x30
	BEQ			Skip5
noPass5
    BL       	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]

Skip5
    LDRB    	R0, [SP, #D_4]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass4
	CMP			R0, #0x30
	BEQ			Skip4
noPass4
	BL        	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]	
	
Skip4
    LDRB   		R0, [SP, #D_3]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass3
	CMP			R0, #0x30
	BEQ			Skip3
noPass3
    BL      	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]	
	
Skip3
    LDRB   		R0, [SP, #D_2]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass2
	CMP			R0, #0x30
	BEQ			Skip2
noPass2
    BL        	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]	

Skip2
	LDRB    	R0, [SP, #D_1]
	LDRB		R1, [SP, #flag]
	CMP			R1, #1
	BEQ			noPass1
	CMP			R0, #0x30
	BEQ			Skip1
noPass1
    BL        	ST7735_OutChar
	LDRB		R1, [SP, #flag]
	MOV			R1, #1
	STRB		R1, [SP, #flag]	
	
Skip1
	LDRB    	R0, [SP, #D_0]
	BL        	ST7735_OutChar

	ADD	SP, #64
	POP {R1, R2, R3, R4, R5, LR}	


      BX  LR
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
    ; copy/paste Lab 7 solution here
		
	PUSH {R4-R11, R12, LR}  ;pushes all used registers to stack because I need lots of local variables D:
	ADD SP, #-32
	
	
	STR R0, [SP, #original] ; stores the R0 into a local variable in the stack
	LDR R0, [SP, #original] ;reloads R0 as a local variable
	
	AND R1, #0x0   ;initializes R1 to zero to be used as a temp storage of R0 original input 
	ADD R1, R0, R1
	STR R1, [SP,#flexOriginal] ;stores R1 as a local variable in the stack
	LDR R1, [SP,#flexOriginal] ;reloads R1 as a local variale in the stack
	
	
	AND R2, #0x0     ;R2 is the number 10 that is divisor for the original input
	MOV R2, #10
	STR R2, [SP, #ten1]
	LDR R2, [SP, #ten1]
	
	AND R3, #0x0        
	STR R3, [SP, #lowRemainder1]
	LDR R3, [SP, #lowRemainder1]
	
	AND R4, #0x0
	STR R4, [SP, #lowRemainder2]
	LDR R4, [SP, #lowRemainder2]
	
	AND R5, #0x0
	STR R5, [SP, #lowRemainder3]
	LDR R5, [SP, #lowRemainder3]
	
	;test case - decimal
	;this tests if I can output the decimal point and it does
	;MOV R0, #0x2E    
	;B ST7735_OutChar
	;end of test case - decimal
	
	
checkIfInputGreaterThan9999
	LDR R1, =allNines
	LDR R2, [R1]
	CMP R0, R2
	BHI allStars
	LDR R2, [SP, #ten1]
	LDR R1, [SP, #flexOriginal]	
	B GreaterThanTen
	
OutPutDecimal
	PUSH {R0, R1, R3, LR}

	MOV R0, #0x2E ;moves ASCII value of decimal into R0
	AND R0, #0x0FF ;bitmask to want only 8 bits
	BL ST7735_OutChar
	LDR R0, [SP, #original]
	LDR R1, [SP, #flexOriginal]
	LDR R3, [SP, #lowRemainder1]
	
	POP {R0, R1, R3, PC}
	
GreaterThanTen  ; finds the lowest remainder bit (ie 1234 -->4) and stores it as a local variable
	UDIV R3, R1, R2 ; (1234 ->123) and R3 has value 123
	MUL R5, R3, R2 ;R5 is 1230
	SUB R4, R1, R5  ;R4 has the least significant digit (ie 4)
	AND R4, #0x0FF ;bitmask 
	ADD R4, R4, #0x30  ;converts the last bit (ie 4) into ASCII '4'
	STR R4, [SP, #lowRemainder1]  ;stores the ASCII value of lowest remainder in lowRemainder1
	B secondToLastBit
	
secondToLastBit
	LDR R0, [SP, #original] ;finds the second to lowest value of R0 and puts it as a local variable (1234-->3)
	UDIV R4, R0, R2 ;divides the original value by 10 to eventually get the second to last bit (ie 1234->123)
	UDIV R1, R4, R2 ;divides the divided by ten number by 10 again to get second to last digit (ie 123->12) and R1 has value 12
	MUL R2, R1, R2 ;R2 = 120
	SUB R3, R4, R2   ;R3 has second to last digit (ie 123-120 ->3) and R3 has value 3
	AND R3, R3, #0x0FF
	ADD R3, R3, #0x30 ; changes lowRemainder2 to ASCII value
	MOV R0, R3
	STR R0, [SP, #lowRemainder2] ;stores ASCII value of 2nd lowest remainder into lowRemainder2
	B thirdToLastBit
	
thirdToLastBit  ;finds the third to lowest bit value of R0 and puts it as a local variable (ie 1234->2)
	LDR R0, [SP, #original]
	LDR R2, [SP, #ten1]
	UDIV R1, R0, R2 ;divides the original by 10 (ie 1234->123) and R1 has 123
	UDIV R3, R1, R2 ;divides the divided by ten by 10 (ie 123->12) and R3 has 12
	UDIV R4, R3, R2 ;divides the divided by ten by ten by 10 (ie 12->1) and R4 has 1
	MUL R4, R2 ; (ie 1*10 - >10)
	SUB R5, R3, R4 ; R5 has the second highest value digit (ie 1234 -->2) and R5 has 2
	AND R5, #0x0FF ;bit masks the second highest value 
	ADD R5, #0x30  ;converts the second highest value to ASCII
	STR R5, [SP, #lowRemainder3]
	B highestBitValue
	
highestBitValue ;finds the highest bit value of R0 and stores it as a local variable in the stack (ie 1234-->1)
	LDR R0, [SP, #original]
	LDR R2, [SP, #ten1]
	UDIV R1, R0, R2 ;divides the original by 10 (ie 1234->123) and R1 has 123
	UDIV R3, R1, R2 ;divides the divided by ten by 10 (ie 123->12) and R3 has 12
	UDIV R4, R3, R2 ;divides the divided by ten by ten by 10 (ie 12->1) and R4 has 1
	MOV R0, R4
	AND R0, #0x0FF ;masks R0 to have only 8 bits for ASCII
	ADD R0, #0x30 ;converts the highest bit in number to ASCII
	BL ST7735_OutChar
	BL OutPutDecimal
	LDR R0, [SP, #lowRemainder3]
	BL ST7735_OutChar
	LDR R0, [SP, #lowRemainder2]
	BL ST7735_OutChar
	LDR R0, [SP, #lowRemainder1]
	BL ST7735_OutChar
	B done
	
	

allStars
	MOV R0, #0x2A  ;outputs star
	BL ST7735_OutChar
	MOV R0, #0x2E  ;outputs decimal
	BL ST7735_OutChar
	MOV R0, #0x2A  ;outputs star
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	B done

outPutZero
	MOV R0, #0x30
	BL ST7735_OutChar

done
	ADD SP, #32
	POP {R4-R11, R12,LR}	
	
     BX   LR
 
     
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
