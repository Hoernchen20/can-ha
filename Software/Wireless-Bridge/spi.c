/*
 * spi.c
 *
 *  Created on: 25.01.2015
 *      Author: felix
 */

#include "spi.h"

void spi_switch_slow (void) {
    /*
     * Taktrate auf 1/16 reduzieren
     */

    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
    SPSR &= ~(1<<SPI2X);
}

void spi_switch_fast (void) {
    /*
     * Taktrate auf 1/2 reduzieren
     */

    SPCR = (1<<SPE)|(1<<MSTR);
    SPSR = (1<<SPI2X);
}

void spi_init (void) {
    /*
     * SPI auf maximale Geschwindigkeit initalisieren */
    SPI_DDR |= (1<<MOSI)|(1<<SCK)|(1<<RFM12_CS)|(1<<MCP2515_CS);
    SPI_PORT |= (1<<MCP2515_CS)|(1<<RFM12_CS);
    spi_switch_slow();
}

uint8_t spi_send (uint8_t data_in) {
    /*
     * SPI-Modul frei ? */
    while(SPSR&(1<<SPIF));

    /*
     * Sendet ein Byte */
    SPDR = data_in;

    /*
     *Wartet bis Byte gesendet wurde */
    while(!(SPSR&(1<<SPIF)));

    return SPDR;
}

void spi_transfer_bytes (uint8_t device, uint8_t n_bytes, uint8_t * data)
//Sendet eine variable Anzahl von Bytes via SPI
//"device" erwartet eine Eingabe in Form von: (1<<PB0)
//"n_bytes" erwartet die Anzahl der zu übertragenen Bytes
//"data" erwartet einen Pointer auf das Datenfeld, bei einer einfachen Variable: &Variable,
//bei einem Array nur den Namen des Array, da ein Array selber ein "Pointer" ist
{
    //Statusregister sichern und Interrupts deaktivieren
/*    uint8_t temp_sreg = SREG;
    cli();*/
    /*
     * SPI-Modul frei ? */
    while(SPSR&(1<<SPIF));

    //Chipselect-Leitung auf 0 setzen
    SPI_PORT &= ~device;

    //Anzahl n an Bytes übertragen
    for (uint8_t i=0; i<n_bytes; i++) {
        // Sendet ein Byte
        SPDR = *data;

        // Wartet bis Byte gesendet wurde
        while(!(SPSR&(1<<SPIF))) {
        }

        //Empfangene Daten auf die Pointeradresse speichern
        *data = SPDR;

        //Pointer erhöhen für nächsten Schleifendurchlauf
        data++;
    }

    //Chipselect-Leitung auf 1 setzen
    SPI_PORT |= device;

    //Statusregister wieder herstellen und so Interrupts aktivieren
//    SREG = temp_sreg;
}

