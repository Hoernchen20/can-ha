#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <avr/io.h>

//Funktionen
void adc_init (void);
void adc_ein (void);
unsigned int u_bat_mess (void);

#endif
