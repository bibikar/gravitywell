; Print.s
; Student names: Sneha Pendharkar, Sameer Bibikar
; Last modification date: 2017-03-24
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
		PRESERVE8

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

num EQU 0

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R0,LR} ; save R0 and LR since we need them in order to return!
	
	MOV R3,#10
	UDIV R1,R0,R3 ; R1 <- R0 / 10
	MUL R2,R1,R3
	SUB R0,R0,R2 ; R0 <- R0 % 10
	CMP R1,#0
	; If input / 10 = 0, we don't recurse.
	; If input is 7, for example, we don't recurse,
	; but we will still output 7.
	; If input is 17, we will put 7 in the local variable,
	; recurse, and have 1 in the recursion call, which
	; will output the 1, but recurse.
	BEQ nodecrec
	
	SUB SP,#4 ; allocate space for 32-bit local variable.
	STR R0,[SP,#num]
	
	; make sure R0 has the input for the recursive call
	MOV R0,R1
	BL LCD_OutDec
	
	LDR R0,[SP,#num] ; get local variable and put it in R0.
	; if we have skipped the local variable part, it should be in R0 already!
	ADD SP,#4 ; deallocate 32-bit local variable

	; now what we need to print is in R0.
nodecrec
	; R0 now contains the actual numerical digit we need to output
	; To convert this to ASCII we add 48.
	ADD R0,#48
	BL ST7735_OutChar

	POP {R0,PC} ; pop the R0 and LR->PC that we saved

;* * * * * * * * End of LCD_OutDec * * * * * * * *
link 	EQU 0
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
	
	SUB SP, #4
	STR LR, [SP, #link]
	;save LR so we know where to get back to
	; First, we will check if R0 > 9999.
	LDR R1,=9999
	CMP R0,R1
	; If we find out that R0 > 9999, we will output "*.***" ...
	BHI LCD_OutFix_overflow
	MOV R1,#1000
	UDIV R2,R0,R1 ; R2 <- pre-decimal part	
	MUL R3,R2,R1
	SUB R1,R0,R3 ; R1 <- post-decimal part
	MOV R0,R2 ; we will now print the whole part
	PUSH {R0,R1} ; save these since we have no guarantee they will be preserved.
	ADD R0,#48 ; add 48 to get the character
	BL ST7735_OutChar
	MOV R0,#46 ; 46 = '.'
	BL ST7735_OutChar
	POP {R0,R1}
	; Now we want to print out the decimal part. 
	; This is in R1, and we actually can just call OutDec for this.
	MOV R2,#100
	UDIV R0,R1,R2
	MUL R2,R2,R0
	SUB R1,R2 
	PUSH{R1,R2}
	ADD R0,#48
	BL ST7735_OutChar
	POP{R1,R2}
	
	MOV R2,#10
	UDIV R0,R1,R2
	MUL R2,R2,R0
	SUB R1,R2
	PUSH{R1,R2}
	ADD R0,#48
	BL ST7735_OutChar
	POP{R1,R2}
	
	MOV R0,R1
	ADD R0,#48
	BL ST7735_OutChar
	LDR LR, [SP, #link]
	ADD SP, #4
	BX LR

LCD_OutFix_overflow
	MOV R0,#42 ; 42 = '*'
	BL ST7735_OutChar
	MOV R0,#46 ; 46 = '.'
	BL ST7735_OutChar
	MOV R0,#42 ; 42 = '*'
	BL ST7735_OutChar
	MOV R0,#42 ; 42 = '*'
	BL ST7735_OutChar
	MOV R0,#42 ; 42 = '*'
	BL ST7735_OutChar
	LDR LR, [SP, #link]
	ADD SP, #4
	BX LR
	
	
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
