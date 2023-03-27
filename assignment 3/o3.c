#include "o3.h"
#include "gpio.h"
#include "systick.h"


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

// Variabler //

volatile gpio_map_t* gpio = (volatile gpio_map_t*) GPIO_BASE;
volatile systick_map* systick = (volatile systick_map*) SYSTICK_BASE;

// Enable, disable //

void led_on() {
  gpio->ports[GPIO_PORT_E].DOUTSET = 0b100;
}

void led_off() {
  gpio->ports[GPIO_PORT_E].DOUTCLR = 0b100;
}

void enable_systick() {
  systick->CTRL = systick->CTRL | 0b1;
}

void disable_systick() {
  systick->CTRL = systick->CTRL ^ 0b1;
}

void IFC_btn0() {
  gpio->IFC = (0b1 << 9);
}

void IFC_btn1() {
  gpio->IFC = (0b1 << 10);
}

void enable_btn0() {
  gpio->IEN = gpio->IEN | (0b1 << 9);
}

void enable_btn1() {
  gpio->IEN = gpio->IEN | (0b1 << 10);
}

void disable_btn0() {
  gpio->IEN = gpio->IEN & ~(0b1 << 9);
}

void disable_btn1() {
  gpio->IEN = gpio->IEN & ~(0b1 << 10);
}


// Konfigurering //

void set_led() {
  gpio->ports[GPIO_PORT_E].DOUTCLR = 0b100;
  word model = gpio->ports[GPIO_PORT_E].MODEL;
  model = model & ~(0b1111 << 8);
  model = model | (0b0100 << 8);
  gpio->ports[GPIO_PORT_E].MODEL = model;
}

void set_btn0() {
  gpio->ports[GPIO_PORT_B].DOUTCLR = (0b1 << 9);

  word modeh = gpio->ports[GPIO_PORT_B].MODEH;
  modeh = modeh & ~(0b1111 << 4);
  modeh = modeh | (0b0001 << 4);
  gpio->ports[GPIO_PORT_B].MODEH = modeh;

  word extipselh = gpio->EXTIPSELH;
  extipselh = extipselh & ~(0b1111 << 4);
  extipselh = extipselh | (0b0001 << 4);
  gpio->EXTIPSELH = extipselh;

  gpio->EXTIFALL = gpio->EXTIFALL | (0b1 << 9);

  IFC_btn0();

}

void set_btn1() {
  gpio->ports[GPIO_PORT_B].DOUTCLR = (0b1 << 10);

  word modeh = gpio->ports[GPIO_PORT_B].MODEH;
  modeh = modeh & ~(0b1111 << 8);
  modeh = modeh | (0b0001 << 8);
  gpio->ports[GPIO_PORT_B].MODEH = modeh;

  word extipselh = gpio->EXTIPSELH;
  extipselh = extipselh & ~(0b1111 << 8);
  extipselh = extipselh | (0b0001 << 8);
  gpio->EXTIPSELH = extipselh;

  gpio->EXTIFALL = gpio->EXTIFALL | (0b1 << 10);

  IFC_btn1();
}

void set_systick() {
  systick->CTRL = systick->CTRL | 0b110;
  systick->LOAD = FREQUENCY;
  systick->VAL = FREQUENCY;
}

void write() {
  time_to_string(timestamp, hours % 24, minutes % 60, seconds % 60);
  if (tick || tilstand > 3) {
      lcd_write(timestamp);
      return;
  }
  if (tilstand == 1) {
      timestamp[4] = timestamp[5] = ' ';
      lcd_write(timestamp);
      return;
  }
  if (tilstand == 2) {
      timestamp[2] = timestamp[3] = ' ';
      lcd_write(timestamp);
      return;
  }
  timestamp[0] = timestamp[1] = ' ';
  lcd_write(timestamp);
}

// Handlers //

void GPIO_ODD_IRQHandler(void) {
  IFC_btn0();
  if (tilstand == 1) {
      increment(s_ptr);
  }
  if (tilstand == 2) {
      increment(m_ptr);
  }
  if (tilstand == 3) {
      increment(h_ptr);
  }
  time_to_string(timestamp, hours % 24, minutes % 60, seconds % 60);
  lcd_write(timestamp);
}

void GPIO_EVEN_IRQHandler(void) {
  IFC_btn1();
  (*tilstand_ptr)++;
}

void SysTick_Handler(void) {
  if (tilstand < 4) {
      tick = !tick;
      write();
      return;
  }

  if (seconds != 0) {
      decrement(s_ptr);
      write();
      return;
  }
  if (minutes != 0) {
      decrement(m_ptr);
      *s_ptr = 59;
      write();
      return;
  }
  decrement(h_ptr);
  *m_ptr = 59;
  *s_ptr = 59;
  write();
}


// Tilstander //

void tilstand_1() {
  *tilstand_ptr = 1;
  enable_systick();
  enable_btn0();
  enable_btn1();
  led_off();
  while (tilstand != 2) {
    }
  tilstand_2();
}

void tilstand_2() {
  while (tilstand != 3) {
    }
  tilstand_3();
}

void tilstand_3() {
  while (tilstand != 4) {
    }
  tilstand_4();
}

void tilstand_4() {
  disable_btn0();
  disable_btn1();

  *s_ptr %= 60;
  *m_ptr %= 60;
  *h_ptr %= 24;

  while (hours != 0 || minutes != 0 || seconds != 0) {
    }
  tilstand_5();
}

void tilstand_5() {
  *tilstand_ptr = 5;

  led_on();
  disable_systick();
  enable_btn1();
  while (tilstand != 6) {
    }
  tilstand_1();
}


int main(void) {
    init();

    set_led();
    set_btn0();
    set_btn1();
    set_systick();

    tilstand_1();
    return 0;
}

