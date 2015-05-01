/*
 * main.c
 *
 *  Created on: 25.01.2015
 *      Author: felix
 */

#include "main.h"
#include "mcp2515.h"
#include "rfm12.h"
#include "interrupt.h"
#include "avr/sleep.h"

int main (void) {
    /*
     * SPI-Modul initialisieren */
    spi_init();

    /*
     * Funkmodul initialisieren */
    rfm12_init();
    rfm12_rx_on();

    /*
     * CAN-Bus initialisieren */
    mcp2515_init();

    /* Interrupts initialisieren */
    interrupt_init();

    /*
     * Hauptprogramm */
    while (1) {
        if (rfm_new_data) {
            rfm_new_data = 0;
            cli();
            struct rfm_message tmp_rfm_message;
            if (rfm_copy_data(&tmp_rfm_message)) {
                struct can_message tmp_can_message;
                tmp_can_message.typ = tmp_rfm_message.typ;
                tmp_can_message.rtr = tmp_rfm_message.rtr;
                tmp_can_message.id = tmp_rfm_message.id;
                tmp_can_message.length = tmp_rfm_message.length;
                for (uint8_t i=0; i<8; i++) {
                    tmp_can_message.data[i] = tmp_rfm_message.data[i];
                }
                can_send_message(&tmp_can_message);
            }
            sei();
        }

        if (rfm_crc_error != rfm_crc_error_last) {
            struct can_message tmp_can_message;
            tmp_can_message.typ = Einzelmeldung;
            tmp_can_message.id = Funkkoppler_CRC_Error;
            tmp_can_message.length = Laenge_Einzelmeldung;
            tmp_can_message.data[0] = rfm_crc_error;
            can_send_message(&tmp_can_message);
            rfm_crc_error_last = rfm_crc_error;
        }

        set_sleep_mode(SLEEP_MODE_STANDBY);
        sleep_mode();
    }
}
