.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO

.text
	.global Start
	
Start:

LDR R0, =GPIO_BASE
LDR R1, =PORT_SIZE

#MEMORY LOATION OF BUTTON DIN
LDR R2, =PORT_B
LDR R3, =GPIO_PORT_DIN
MUL R4, R1, R2
ADD R5, R3, R4
ADD R6, R0, R5

#MEMORY LOCATION OF LED DOUTSET
LDR R2, =PORT_E
LDR R3, =GPIO_PORT_DOUTSET
MUL R4, R1, R2
ADD R5, R3, R4

#MOVING MEMORY LOCATIONS TO R0, R1 AND R2 FOR SIMPLICITY
ADD R0, R0, R5 // LED DOUTSET
ADD R1, R0, #4 // LED DOUTCLR
MOV R2, R6 // BUTTON DIN

#PIN LOCATION OF LED AND BUTTON
MOV R3, 0b100
MOV R4, 0b1000000000

LOOP:
	LDR R5, [R2]
	AND R6, R4, R5
	CMP R6, #0
	BEQ LED_ON
	B LED_OFF

LED_ON:
	STR R3, [R0]
	B LOOP

LED_OFF:
	STR R3, [R1]
	B LOOP

NOP

