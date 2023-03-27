.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO
.include "sys-tick_constants.s" // Register-adresser og konstanter for SysTick

.text
	.global Start
	
Start:

    // Skriv din kode her...

#Konfigurerer SYSTICK_CTRL
LDR R0, =SYSTICK_BASE
LDR R1, [R0]
ORR R2, R1, 0b110 // Klokka slås ikke på før bryteren trykkes, derfor 0 på siste bit
STR R2, [R0]

#Konfigurerer SYSTICK_LOAD og _VAL
LDR R1, =FREQUENCY
MOV R2, #10
UDIV R3, R1, R2
STR R3, [R0, #4]
STR R3, [R0, #8]

#Konfigurerer knappen for interrupts
LDR R0, =GPIO_BASE
LDR R1, =GPIO_EXTIPSELH
ADD R2, R0, R1
MOV R3, 0b11110000
MVN R3, R3
LDR R4, [R2]
AND R5, R3, R4
ORR R6, R5, 0b10000
STR R6, [R2] // EXTIPSEL

LDR R1, =GPIO_EXTIFALL
ADD R2, R0, R1
MOV R3, 0b1000000000
LDR R4, [R2]
ORR R5, R3, R4
STR R5, [R2] // EXTIFALL

LDR R1, =GPIO_IFC
ADD R2, R0, R1
STR R3, [R2] // IFC

LDR R1, =GPIO_IEN
ADD R2, R0, R1
LDR R4, [R2]
ORR R5, R3, R4
STR R5, [R2]



#Lagrer minne-adressen til LED DOUTTGL
LDR R0, =GPIO_BASE
LDR R1, =LED_PORT
LDR R2, =PORT_SIZE
LDR R3, =GPIO_PORT_DOUTTGL
MUL R4, R1, R2
ADD R5, R0, R4
ADD R10, R3, R5 // R10 er fredet

#Uendelig løkke for å holde programmet levende
LOOP:
B LOOP


.global GPIO_ODD_IRQHandler
.thumb_func
GPIO_ODD_IRQHandler:
	LDR R0, =GPIO_BASE
	LDR R1, =GPIO_IFC
	ADD R2, R0, R1
	MOV R3, 0b1000000000
	STR R3, [R2] // Reset IF-register

	LDR R0, =SYSTICK_BASE
	LDR R1, [R0]
	EOR R2, R1, 0b1
	STR R2, [R0] // Flipper siste bit for å slå klokke-funksjon av/på
BX LR


.global SysTick_Handler
.thumb_func
SysTick_Handler:
	LDR R0, =tenths
	LDR R1, [R0]
	CMP R1, #9
	BNE END_TENTHS

	MOV R2, #0
	STR R2, [R0]
	MOV R0, 0b100
	STR R0, [R10] // LED-toggle
	LDR R0, =seconds
	LDR R1, [R0]
	CMP R1, #59
	BNE END_SECONDS

	STR R2, [R0]
	LDR R0, =minutes
	LDR R1, [R0]
	CMP R1, #59
	BNE END_MINUTES
	MOV R2, #0
	STR R2, [R0]
	B END

END_TENTHS:
	ADD R1, R1, #1
	STR R1, [R0]
	B END
END_SECONDS:
	ADD R1, R1, #1
	STR R1, [R0]
	B END
END_MINUTES:
	ADD R1, R1, #1
	STR R1, [R0]
END:
BX LR

NOP // Behold denne på bunnen av fila

