#include "o3.h"
#include "gpio.h"
#include "systick.h"

int seconds;
int minutes;
int hours;
states currentState;
volatile gpio_map_t* GPIORegister;
volatile gpio_sys_map* sysTickRegister;
port_pin_t led0;
port_pin_t button0;
port_pin_t button1;

/**************************************************************************//**
 * @brief Konverterer nummer til string 
 * Konverterer et nummer mellom 0 og 99 til string
 *****************************************************************************/
void int_to_string(char *timestamp, unsigned int offset, int i) {
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset+1] = '9';
        return;
    }

    while (i > 0) {
	    if (i >= 10) {
		    i -= 10;
		    timestamp[offset]++;
		
	    } else {
		    timestamp[offset+1] = '0' + i;
		    i=0;
	    }
    }
}

/**************************************************************************//**
 * @brief Konverterer 3 tall til en timestamp-string
 * timestamp-argumentet må være et array med plass til (minst) 7 elementer.
 * Det kan deklareres i funksjonen som kaller som "char timestamp[7];"
 * Kallet blir dermed:
 * char timestamp[7];
 * time_to_string(timestamp, h, m, s);
 *****************************************************************************/
void time_to_string(char *timestamp, int h, int m, int s) {
    timestamp[0] = '0';
    timestamp[1] = '0';
    timestamp[2] = '0';
    timestamp[3] = '0';
    timestamp[4] = '0';
    timestamp[5] = '0';
    timestamp[6] = '\0';

    int_to_string(timestamp, 0, h);
    int_to_string(timestamp, 2, m);
    int_to_string(timestamp, 4, s);
}



void setLed(bool state) {
	volatile word mask = 1;
	mask = mask << 2;
	if (state) GPIORegister->ports[led0.port].DOUT = mask;
	else  GPIORegister->ports[led0.port].DOUTCLR = mask;
}

void writeToLed() {
	char timeStamp[7];
	time_to_string(timeStamp, hours, minutes , seconds);
	lcd_write(timeStamp);
}

void GPIO_ODD_IRQHandler(void) { //pb0
	switch(currentState) {
	case(SECONDS_ST):
			seconds++;
			break;
	case(MINUTES_ST):
			minutes++;
			break;
	case(HOURS_ST):
			hours++;
			break;
	default:
		break;
	}

	writeToLed();

	volatile word interruptFlag = GPIORegister->IFC;
	volatile word mask = 1;
	mask = mask << button0.pin;
	interruptFlag = mask | interruptFlag;
	GPIORegister->IFC = interruptFlag;
}


void GPIO_EVEN_IRQHandler(void) { //pb1
	volatile word mask;

	switch(currentState) {
	case(SECONDS_ST):
			currentState = MINUTES_ST;
			break;
	case(MINUTES_ST):
			currentState = HOURS_ST;
			break;
	case(HOURS_ST):
			currentState = RUNNING_ST;
			sysTickRegister->CTRL = 7; //0b111
			break;
	case(FINISHED_ST):
			currentState = SECONDS_ST;
			setLed(false);
			break;
	default:
		break;
	}


	volatile word interruptFlag = GPIORegister->IFC;
	mask = 1;
	mask = mask << button1.pin;
	interruptFlag = mask | interruptFlag;
	GPIORegister->IFC = interruptFlag;
}


void  SysTick_Handler(void) {
	switch(currentState) {
		case(RUNNING_ST):
				seconds--;
				if (seconds <= 0 && minutes <= 0 && hours <= 0) {
					currentState = FINISHED_ST;
					setLed(true);
					break;
				}
				if (seconds <= 0 && minutes > 0 ) {
					seconds = 60;
					minutes--;
				}
				if (minutes <= 0 && hours > 0) {
					minutes = 60;
					hours--;
				}
				break;

		default:
			break;
		}

	writeToLed();
}


int main(void) {
    init();

    led0.port = GPIO_PORT_E;
    led0.pin = 2;

    button0.port = GPIO_PORT_B;
    button0.pin = 9;

    button1.port = GPIO_PORT_B;
    button1.pin = 10;


    GPIORegister = (gpio_map_t*) GPIO_BASE;
    sysTickRegister = (gpio_sys_map*) SYSTICK_BASE;

    seconds = 0;
    minutes = 0;
    hours = 0;

    currentState = SECONDS_ST;


    //Setter DOUT til 0 for led0
    volatile word mask = 1;
    mask = mask << led0.pin;
    GPIORegister->ports[led0.port].DOUTCLR = mask;


    //Setter MODEL til output for for led0
    volatile word model = GPIORegister->ports[led0.port].MODEL; // Hneter ut bitsene fra MODEL
    mask = 15; // 15 = 0b1111
    mask = mask << 8; // left shifter med 8 plasser = 1111 0000 0000
    mask = ~mask; // not mask = 0000 1111 1111
    model = model & mask; // AND med mask og det som var i MODEL = 0000 XXXX XXXX
    mask = GPIO_MODE_OUTPUT; // 0b0100 = koden for output
    mask = mask << 8; // left shifter med 8 plasser = 0100 0000 0000
    model = model | mask; // MODEL: 0100 XXXX XXXX
    GPIORegister->ports[led0.port].MODEL = model;


    //Setter DOUT til 0 for button0
    mask = 1;
    mask = mask << button0.pin;
    GPIORegister->ports[button0.port].DOUTCLR = mask;


    //Setter DOUT til 0 for button1
    mask = 1;
    mask = mask << button1.pin;
    GPIORegister->ports[button1.port].DOUTCLR = mask;


    //Setter MODEH til input for button0 og button1
    volatile word modeh = GPIORegister->ports[button0.port].MODEH;
    mask = 255; // ob1111 1111
    mask = mask << 4; // left shifter med 4 plasser = 1111 1111 0000
    mask = ~mask; // not mask = 0000 0000 1111
    modeh = modeh & mask; // AND med mask og det som var i MODEH = 0000 0000 XXXX

    mask = 17; // 0b0001 0001
    mask = mask << 4; // 0b0001 0001 0000
    modeh = modeh | mask;
    GPIORegister->ports[button0.port].MODEH = modeh;


    //Setter opp EXTIPSELH, for button0
    volatile word extipselh = GPIORegister->EXTIPSELH;
    mask = 15; // 0b1111
    mask = mask << 4; // 0b1111 0000
    mask = ~mask; // 0b0000 1111
    extipselh = extipselh & mask; // 0b0000 XXXX

    mask = 1; //ob0001
    mask = mask << 4; //0b0001 0000
    extipselh = extipselh | mask; //0b0001 XXXX
    GPIORegister->EXTIPSELH = extipselh;

    // Setter opp EXTIFALL for button0
    volatile word extifall = GPIORegister->EXTIFALL;
    mask = 1; //0b1
    mask = mask << button0.pin; //0b1000000000
    extifall = extifall | mask; //0b1XXXXXXXXX
    GPIORegister->EXTIFALL = extifall;

    //Clearer IF for button0
    volatile word ifc = GPIORegister->IFC;
    mask = 1; //0b1
    mask = mask << button0.pin; //0b1000000000
    ifc = mask | ifc; //0b1XXXXXXXXX
    GPIORegister->IFC = ifc;

    // Setter opp Interrupt Enable for button0
    volatile word en = GPIORegister->IEN;
    mask = 1; //0b1
    mask = mask << button0.pin; //0b1000000000
    en = mask | en; //0b1XXXXXXXXX
    GPIORegister->IEN = en;


    //Setter opp EXTIPSELH, for button1
    extipselh = GPIORegister->EXTIPSELH;
    mask = 15; // 0b1111
    mask = mask << 8; // 0b1111 0000 0000
    mask = ~mask; // 0b0000 1111 1111
    extipselh = extipselh & mask; // 0b0000 XXXX XXXX

    mask = 1; //ob0001
    mask = mask << 8; //0b0001 0000 0000
    extipselh = extipselh | mask; //0b0001 XXXX XXXX
    GPIORegister->EXTIPSELH = extipselh;


    // Setter opp EXTIFALL for button1
    extifall = GPIORegister->EXTIFALL;
    mask = 1; //0b1
    mask = mask << button1.pin; //0b10000000000
    extifall = extifall | mask; //0b1XXXXXXXXXX
    GPIORegister->EXTIFALL = extifall;

    //Clearer IF for button1
    ifc = GPIORegister->IFC;
    mask = 1; //0b1
    mask = mask << button1.pin; //0b10000000000
    ifc = mask | ifc; //0b1XXXXXXXXXX
    GPIORegister->IFC = ifc;

    // Setter opp Interrupt Enable for button1
    en = GPIORegister->IEN;
    mask = 1; //0b1
    mask = mask << button1.pin; //0b10000000000
    en = mask | en; //0b1XXXXXXXXXX
    GPIORegister->IEN = en;


    //Setter opp sysTick registeret
    sysTickRegister->LOAD = FREQUENCY;
    sysTickRegister->VAL = FREQUENCY;


    writeToLed();

    while(1) {
    }

    return 0;
}
