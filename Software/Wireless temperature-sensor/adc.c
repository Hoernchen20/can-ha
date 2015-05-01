#include "adc.h"

void adc_init (void) {
    /*
     * Analog-Digital-Converter einstellen */
    ADMUX  |= (1<<MUX0)|(1<<MUX1)|(1<<MUX2);	//Kanal: 7 (PA7), Referenz: VCC
    ADCSRA |= (1<<ADPS1)|(1<<ADPS2);			//Vorteiler: 64
    DIDR0  |= (1<<ADC7D);						//Digital Input disable: PA7
    ACSR   |= (1<<ACD);							//Analog Comperator aus
}

void adc_ein (void) {
    /*
     * Analog-Digital-Converter einschalten + Dummy-Readout */
    ADCSRA |= (1<<ADEN)|(1<<ADSC);

    while (ADCSRA & (1<<ADSC)) {
    }
    uint16_t result = ADCW;
}

unsigned int u_bat_mess (void) {
    /*
     * Batterie Spannung messen und ADC ausschalten */

    uint32_t result = 0;
    uint8_t tmp;

    /*
     * Wert vom ADC lesen und den Durchschnitt bilden */
    for (tmp=0; tmp<254; tmp++) {
        ADCSRA |= (1<<ADSC);

        while (ADCSRA & (1<<ADSC)) {
        }

        result = result + ADCW;
    }
    result = result/254;

    ADCSRA &= ~(1<<ADEN);						//ADC ausschalten

    result = result * 49 / 100 * 3;

    return (result);
}
