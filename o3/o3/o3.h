/* En rask måte å unngå header recursion på er å sjekke om verdi, f.eks. 'O3_H',
   er definert. Hvis ikke, definer 'O3_H' og deretter innholdet av headeren 
   (merk endif på bunnen). Nå kan headeren inkluderes så mange ganger vi vil 
   uten at det blir noen problemer. */
#ifndef O3_H
#define O3_H

// Type-definisjoner fra std-bibliotekene
#include <stdint.h>
#include <stdbool.h>

// Type-aliaser
typedef uint32_t word;
typedef uint8_t  byte;

// Prototyper for bibliotekfunksjoner
void init(void);
void lcd_write(char* string);
void int_to_string(char *timestamp, unsigned int offset, int i);
void time_to_string(char *timestamp, int h, int m, int s);

// Variabler

volatile int tilstand = 1;
volatile int* tilstand_ptr = &tilstand;

volatile int seconds = 0;
volatile int minutes = 0;
volatile int hours = 0;

volatile int* s_ptr = &seconds;
volatile int* m_ptr = &minutes;
volatile int* h_ptr = &hours;

char timestamp[7];

int tick = 0;

// Prototyper
// legg prototyper for dine funksjoner her

void led_on();
void led_off();

void enable_btn0();
void enable_btn1();
void disable_btn0();
void disable_btn1();

void IFC_btn0();
void IFC_btn1();

void enable_systick();
void disable_systick();

void set_led();
void set_btn0();
void set_btn1();
void set_systick();

void write();

void increment(volatile int* number) {(*number)++;}
void decrement(volatile int* number) {(*number)--;}
void reset(volatile int* number) {(*number) = 0;}

void GPIO_ODD_IRQHandler(void);
void GPIO_EVEN_IRQHandler(void);
void SysTick_Handler(void);

void tilstand_1();
void tilstand_2();
void tilstand_3();
void tilstand_4();
void tilstand_5();


// Structs

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

typedef struct {
    volatile word CTRL;
    volatile word LOAD;
    volatile word VAL;
    volatile word CALIB;
  } systick_map;
#endif
