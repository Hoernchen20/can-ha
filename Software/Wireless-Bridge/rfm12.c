/*
 * rfm12.c
 *
 *  Created on: 25.01.2015
 *      Author: felix
 */

#include "rfm12.h"
#include "mcp2515.h"
volatile uint8_t rfm_data_counter = 0;
volatile uint8_t rfm_new_data;
volatile uint8_t rfm_receive_data[17];
uint8_t rfm_crc_error = 0;
uint8_t rfm_crc_error_last = 0;

void rfm12_receive_message (void) {
    /*
     * Daten Byte für Byte vom RFM12 abholen und in rfm_receive_data abspeichern.
     * Wenn alle Bytes empfangen worden sind, wird rfm_new_data gesetzt
     * und die Daten können ind die Struktur rfm_message sortiert werden.
     * Diese Funktion muss im Interrupt aufgerufen werden. */
    CONVERTW rx_data;
    uint16_t statusreg;
    statusreg = rfm12_write(0x0000);

    /*
     * prüfen ob neu Daten empfangen wurden */
    if (statusreg&(1<<FFIT)) {
        /*
         * SPI-Geschwindigkeit reduzieren, da der FIFO des FRM12 nicht schneller ist */
       // spi_switch_slow();

        /*
         * Daten abholen und zwischenspeichern */
        rx_data.w = rfm12_write(rx_fifo_read);
        rfm_receive_data[rfm_data_counter] = rx_data.b[0];
        rfm_data_counter = rfm_data_counter+1;
        
        if (rfm_data_counter >= 17) {
            rfm12_fifo_reset();
            rfm_data_counter = 0;
            rfm_new_data = 1;
        }

        /*
         * SPI-Geschwindigkeit wieder auf Maximum setzen */
        //spi_switch_fast();
    }
}

uint8_t rfm_copy_data (struct rfm_message *msg) {
    /*
     * CRC prüfen und dann Daten von rfm_receive_data in die angegebene Struktur sortieren */
    rfm_new_data = 0;
    uint8_t crc = 0;
    for (uint8_t i = 0; i < 17; i++) {
        crc = _crc_ibutton_update(crc, rfm_receive_data[i]);
    }
    
    if (crc == 0) {
        msg->typ = rfm_receive_data[0];
        msg->typ = (msg->typ<<8) | rfm_receive_data[1];
        msg->rtr = rfm_receive_data[2];
        msg->id = rfm_receive_data[3];
        msg->id = (msg->id<<8) | rfm_receive_data[4];
        msg->id = (msg->id<<8) | rfm_receive_data[5];
        msg->id = (msg->id<<8) | rfm_receive_data[6];
        msg->length = rfm_receive_data[7];
        for (uint8_t i = 0; i < msg->length; i++) {
            msg->data[i] = rfm_receive_data[8+i];
        }
        rfm_crc_error = 0;
        return 1;
    }
    rfm_crc_error = 1;
    return 0;
}

void rfm12_fifo_reset (void)
//FIFO nach Datenempfang resetten und wieder aktivieren
{
    //FIFO Reset
    rfm12_write(0xCA81);
    //FIFO ein
    rfm12_write(0xCA83);
}

void rfm12_rx_on (void)
//Empfänger einschalten
{
    //Power Management Command, Empfänger ein
    rfm12_write(0x82D8);
    
    //RX-FIFO ein
    rfm12_fifo_reset();
}

void rfm12_send_byte (unsigned char data) {
    /*
     * Byte über das RFM12 senden und warten bis fertig */
    CONVERTW val;

    /*
     * Status lesen, bis das nächste Byte gesendet werden kann
     */
    while ( !(rfm12_write(0x0000)&(1<<RGIT)) ) {
    }

    val.b[0] = data;
    val.b[1] = 0xB8;

    rfm12_write(val.w);
}

void rfm12_send (struct rfm_message * msg) {
    /*
     * Nachricht über das RFM12 senden */
    cli();

    /*
     * Temporäres Array erzeugen und die Daten sortieren */
    uint8_t tmp_msg[17] = {0};
        tmp_msg[0] = (uint8_t)(msg->typ>>8);
        tmp_msg[1] = msg->typ;
        tmp_msg[2] = msg->rtr;
        tmp_msg[3] = (uint8_t)(msg->id>>24);
        tmp_msg[4] = (uint8_t)(msg->id>>16);
        tmp_msg[5] = (uint8_t)(msg->id>>8);
        tmp_msg[6] = msg->id;
        tmp_msg[7] = msg->length;
        for (uint8_t i = 0; i < msg->length; i++) {
            tmp_msg[8+i] = (msg->data[i]);
        }

    /*
     * CRC erzeugen */
    uint8_t crc = 0;
    for (uint8_t i = 0; i < 16; i++) {
        crc = _crc_ibutton_update(crc, tmp_msg[i]);
    }
    tmp_msg[16] = crc;

    /*
     * Power Management Command, Sender ein */
    rfm12_write(0x8238);

    /*
     * Synchron-Bytes senden */
    rfm12_send_byte(0x2D);
    rfm12_send_byte(0xD4);

    /*
     * Daten-Bytes senden */
    for (uint8_t i = 0; i < 17; i++) {
        rfm12_send_byte(tmp_msg[i]);
    }

    /*
     * Dummy-Bytes senden */
    rfm12_send_byte(0xAA);
    rfm12_send_byte(0xAA);

    /*array
     * Power Management Command, Wake-Up ein, Status lesen */
    rfm12_write(0x8203);
    rfm12_write(0x0000);
    sei();
}

unsigned int rfm12_write (unsigned int data) {
    /*
     * Daten in die Register des Funkmoduls schreiben und gleichzeitig lesen.
     * Die Typumwandlung ist nötig, weil die Befehle des Funkmoduls immer ein Word lang sind,
     * die Funktion spi_transfer_bytes aber ein Array von Bytes erwartet.
     *
     * Die Funktion soll eigentlich mit spi_transfer_bytes ausgeführt werden,
     * dies funktioniert aber noch nicht */
    CONVERTW data_in;
    data_in.w = data;

    CONVERTW data_out;
    data_out.w = 0;

    SPI_PORT &= ~(1<<RFM12_CS);
    data_out.b[1] = spi_send(data_in.b[1]);
    data_out.b[0] = spi_send(data_in.b[0]);
    SPI_PORT |= (1<<RFM12_CS);

    return data_out.w;
}
   
void rfm12_init (void) {
    /*
     * RFM12 initalisieren */

    rfm12_write(0x0000);	//Status lesen
    rfm12_write(0x80D7);	//Configuration Setting, TX & RX FIFO an, 433MHz , 12pF
    rfm12_write(0xA620);	//Frequency Setting Command, 433,92MHz
    rfm12_write(0xC647);	//Data Rate Command, 4,789kbps
    rfm12_write(0x94A1);	//RX Control: VDI out, VDI Response Time= fast,
	    					//Bandbreite=134kHz, RSSI threshold = -97 dBm, 
	    					//LNA gain = 0 dB
    rfm12_write(0xC2AC);	//Data Filter:8bit
    rfm12_write(0x9850);	//TX Configuration Control: Delta f (fsk) = 90 kHz, 
	    					//Output power = 0 dB
    rfm12_write(0xEB93);	//Wake-up-Timer: 301056ms, eb93
    rfm12_write(0x8201);	//Power Management Command, Clock-Output aus
    rfm12_write(0x0000);	//Status lesen
}
 
  
