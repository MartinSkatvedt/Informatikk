/* En rask måte å unngå header recursion på er å sjekke om verdi, f.eks. 'O3_H',
   er definert. Hvis ikke, definer 'O3_H' og deretter innholdet av headeren 
   (merk endif på bunnen). Nå kan headeren inkluderes så mange ganger vi vil 
   uten at det blir noen problemer. */
#ifndef O3_H
#define O3_H

// Type-definisjoner fra std-bibliotekene
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

// Type-aliaser
typedef uint32_t word;
typedef uint8_t  byte;

// Prototyper for bibliotekfunksjoner
void init(void);
void lcd_write(char* string);
void int_to_string(char *timestamp, unsigned int offset, int i);
void time_to_string(char *timestamp, int h, int m, int s);

// Prototyper
// legg prototyper for dine funksjoner her

void setled(bool state);
void writeToLed();

typedef enum {
	SECONDS_ST,
	MINUTES_ST,
	HOURS_ST,
	RUNNING_ST,
	FINISHED_ST,
} states;

typedef struct {
	volatile word CTRL;
	volatile word LOAD;
	volatile word VAL;
	volatile word CALIB;
} gpio_sys_map;


typedef struct {
   volatile word CTRL;
   volatile word MODEL;
   volatile word MODEH;
   volatile word DOUT;
   volatile word DOUTSET;
   volatile word DOUTCLR;
   volatile word DOUTTGL;
   volatile word DIN;
   volatile word PINLOCKN;
} gpio_port_map_t;

typedef struct {
	volatile gpio_port_map_t ports[6];
    volatile word unused_space[10];
    volatile word EXTIPSELL;
    volatile word EXTIPSELH;
    volatile word EXTIRISE;
    volatile word EXTIFALL;
    volatile word IEN;
    volatile word IF;
    volatile word IFS;
    volatile word IFC;
    volatile word ROUTE;
    volatile word INSENSE;
    volatile word LOCK;
    volatile word CTRL;
    volatile word CMD;
    volatile word EM4WUEN;
    volatile word EM4WUPOL;
    volatile word EM4WUCAUSE;
} gpio_map_t;

extern int seconds;
extern int mminutes;
extern int hours;
extern states currentState;
extern volatile gpio_map_t* GPIORegister;
extern volatile gpio_sys_map* sysTickRegister;
extern port_pin_t led0;
extern port_pin_t button0;
extern port_pin_t button1;

#endif
