/*
 * interrupt.h
 *
 *  Created on: 28.01.2015
 *      Author: felix
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <avr/interrupt.h>
#include "rfm12.h"

/*
 * Allgemeine Einstellungen */
#define INTERRUPT0 0
#define INTERRUPT1 1
#define TIMER0_COMPA 0
#define TIMER2_OVF 0

/*
 * Funktionen */
void interrupt_init (void);
ISR(INT0_vect);
ISR(INT1_vect);
ISR(TIMER0_COMPA_vect);
ISR(TIMER2_OVF_vect);

#endif /* INTERRUPT_H_ */
