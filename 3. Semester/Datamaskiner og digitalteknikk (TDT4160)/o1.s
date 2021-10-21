.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO

.text
	.global Start

//R5 - PORT B
//R6 - GPIO_PORT_DIN
//R7 - PORT E
//R8 - GPIO_PORT_DOUTCLR
//R9 - GPIO_PORT_DOUSET

Start:

	LDR R6, =GPIO_PORT_DIN
	LDR R8, =GPIO_PORT_DOUTCLR
	LDR R9, =GPIO_PORT_DOUTSET

	LDR R0, =LED_PORT
	LDR R1, =PORT_SIZE
	MUL R0, R0, R1
	LDR R1, =GPIO_BASE
	ADD R7, R0, R1

	LDR R0, =BUTTON_PORT
	LDR R1, =PORT_SIZE
	MUL R0, R0, R1
	LDR R1, =GPIO_BASE
	ADD R5, R0, R1


	Loop:
		LDR R2, [R5, R6] //Laster inn data fra DIN-adressen inn i R2

		CMP R2, 512
		BEQ TurnOff
		B TurnOn


	TurnOn:
		MOV R2, #1
		LSL R2, R2, #LED_PIN
		STR R2, [R7, R9]

		B Loop

	TurnOff:
		MOV R2, #1
		LSL R2, R2, #LED_PIN
		STR R2, [R7, R8]

		B Loop



NOP // Behold denne på bunnen av fila