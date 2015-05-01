/*
 * interrupt.c
 *
 *  Created on: 28.01.2015
 *      Author: Felix Horn
 */

#include "interrupt.h"
#include "mcp2515.h"

extern volatile uint16_t taktmerker;
extern volatile uint16_t taktmerker_last;

void interrupt_init (void) {
    /*
     * INT0 */
#if INTERRUPT0
    #if defined(__AVR_ATmega16__)
        GICR |= (1<<INT0);
    #endif

    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__)
        EIMSK |= (1<<INT0);
    #endif

    #if defined(__AVR_ATtiny24__) || (__AVR_ATtiny44__) || (__AVR_ATtiny84__)
        GIMSK |= (1<<INT0);
    #endif
#endif

    /*
     * INT1 */
#if INTERRUPT1
    #if defined(__AVR_ATmega16__)
        GICR |= (1<<INT1);
    #endif

    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__)
        EIMSK |= (1<<INT1);
    #endif

    #if defined(__AVR_ATtiny24__) || (__AVR_ATtiny44__) || (__AVR_ATtiny84__)
        GIMSK |= (1<<INT1);
    #endif
#endif
    sei();
}

#if INTERRUPT0
ISR(INT0_vect) {
    /*
     * MCP2515 */
}
#endif

#if INTERRUPT1
ISR(INT1_vect) {
    /*
     * RFM12 */
    rfm12_receive_message();
}
#endif

#if TIMER0_COMPA
ISR(TIMER0_COMPA_vect) {
    taktmerker_last = taktmerker;
    taktmerker++;
}
#endif

#if TIMER2_OVF
ISR(TIMER2_OVF_vect) {
}
#endif
