.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO
.include "sys-tick_constants.s" // Register-adresser og konstanter for SysTick


.text
	.global Start
	

Start:

	MOV R11, #0//Clock running flag

    // Setter opp systick interupten
    LDR R0, =SYSTICK_BASE
	LDR R1, =SYSTICK_CTRL

	MOV R2, 0b110
	STR R2, [R0, R1] //Setter CTRL bitsene for systick

	LDR R1, =SYSTICK_LOAD
	LDR R2, =2500000
	STR R2, [R0, R1] //Setter LOAD til frekvensen/10

	LDR R1, =SYSTICK_VAL
	LDR R2, =2500000
	STR R2, [R0, R1] //Setter VAL til frekvensen/10


	LDR R10, =GPIO_BASE // R10 - GPIO base

	//Setter opp pin 9 i port B i exitpselh
	LDR R0, =GPIO_EXTIPSELH
	ADD R0, R0, R10

	MOV R1, 0b1111
  	LSL R2, R1, 4
  	MVN R3, R2
  	LDR R4, [R0] // Anta at R0 = adressen til EXTIPSELH
  	AND R5, R3, R4
   	LDR R6, =PORT_B // 0b0001
  	LSL R7, R6, 4
  	ORR R8, R5, R7
  	STR R8, [R0] // Anta at R0 = adressen til EXTIPSELH

	//GPIO_EXTIFALL
	LDR R0, =GPIO_EXTIFALL
	ADD R0, R0, R10

	MOV R1, 0b1
	LSL R1, 9
	LDR R2, [R0]
	ORR R2, R2, R1
	STR R2, [R0]


	//Interupt flag
	LDR R0, =GPIO_IFC
	ADD R0, R0, R10

	MOV R1, 0b1
	LSL R1, 9
	LDR R2, [R0]
	ORR R2, R2, R1
	STR R2, [R0]


	//GPIO_IEN
	LDR R0, =GPIO_IEN
	ADD R0, R0, R10

	MOV R1, 0b1
	LSL R1, 9
	LDR R2, [R0]
	ORR R2, R2, R1
	STR R2, [R0]



	//Setter opp konstanter for LED
	LDR R0, =LED_PORT
	LDR R1, =PORT_SIZE
	MUL R0, R0, R1
	LDR R1, =GPIO_BASE
	ADD R7, R0, R1 //R7 - PORT E
	LDR R8, =GPIO_PORT_DOUTCLR //R8 -GPIO Clear
	LDR R9, =GPIO_PORT_DOUTSET //R9 - GPIO Douset


    Loop:
    	B Loop



IncTenths:
	LDR R1, =tenths //Laster adressen til tenths inn i R1
    LDR R2, [R1] //Laster verdien fra tenths inn i R2
	ADD R2, R2, #1 //Legger til 1

	CMP R2, 10
	BEQ IncSeconds

	STR R2, [R1] //Lagrer nye verdien i minnelokasjonen til tenths

	BX LR


IncSeconds:
	MOV R2, #0
	STR R2, [R1]

	PUSH {LR}
	BL LightOn
	POP {LR}
	LDR R1, =seconds //Laster adressen til seconds inn i R1
    LDR R2, [R1] //Laster verdien fra seconds inn i R2
	ADD R2, R2, #1 //Legger til 1
	STR R2, [R1] //Lagrer nye verdien i minnelokasjonen til seconds

	CMP R2, 60
	BEQ IncMinutes

	STR R2, [R1] //Lagrer nye verdien i minnelokasjonen til seconds
	BX LR


IncMinutes:
	MOV R2, #0
	STR R2, [R1]

	LDR R1, =minutes //Laster adressen til minutes inn i R1
    LDR R2, [R1] //Laster verdien fra minutes inn i R2
	ADD R2, R2, #1 //Legger til 1
	STR R2, [R1] //Lagrer nye verdien i minnelokasjonen til minutes


	BX LR

LightOn:
	MOV R2, #1
	LSL R2, R2, #LED_PIN
	STR R2, [R7, R9]
	BX LR

LightOff:
	MOV R2, #1
	LSL R2, R2, #LED_PIN
	STR R2, [R7, R8]
	BX LR


EnableClock:
  	LDR R0, =SYSTICK_BASE
	LDR R1, =SYSTICK_CTRL

	MOV R2, 0b111
	STR R2, [R0, R1] //Setter CTRL bitsene for systick
	MOV R11, #1
	BX LR


DisableClock:
	LDR R0, =SYSTICK_BASE
	LDR R1, =SYSTICK_CTRL

	MOV R2, 0b110
	STR R2, [R0, R1] //Setter CTRL bitsene for systick
	MOV R11, #0
	BX LR



.global SysTick_Handler
.thumb_func
SysTick_Handler:

	PUSH {LR}
	BL LightOff
	POP {LR}

	PUSH {LR}
	BL IncTenths
	POP {LR}

	BX LR //Retunerer fra interupt


.global GPIO_ODD_IRQHandler
.thumb_func
GPIO_ODD_IRQHandler:

	LDR R0, =GPIO_IFC
	ADD R0, R0, R10

	MOV R1, 0b1
	LSL R1, 9
	LDR R2, [R0]
	ORR R2, R2, R1
	STR R2, [R0]

	CMP R11, 0
	BEQ EnableClock
	B DisableClock

	BX LR




NOP // Behold denne på bunnen av fila

