/*
 * rfm12.h
 *
 *  Created on: 25.01.2015
 *      Author: felix
 */

#ifndef RFM12_H_
#define RFM12_H_

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/crc16.h>
#include "../CAN_TYP_ID.h"

/*
 * Allgemeine Einstellungen */
#define RFM12_CS    PA3
#define RFM12_MOSI  PA6
#define RFM12_MISO  PA5
#define RFM12_SCK   PA4
#define SPI_PORT    PORTA
#define SPI_DDR     DDRA
#define SPI_PIN     PINA

/*
 * Variablen */
extern volatile uint8_t rfm_new_data;
extern volatile uint8_t rfm_wake_up;

//Typdefinitionen
struct rfm_message
{
    uint16_t    typ;
    uint8_t     rtr;
    uint32_t    id;
    uint8_t     length;
    uint8_t     data[8];
};

//Funktionen
void rfm12_receive_message (void);
uint8_t rfm_copy_data (struct rfm_message *msg);
void rfm12_fifo_reset (void);
void rfm12_rx_on (void);
void rfm12_send_byte (unsigned char data);
void rfm12_send_message (struct rfm_message * msg);
unsigned int rfm12_write (unsigned int data);
void rfm12_init (void);

/*
 * Unions */
typedef union conver2_ {
    int16_t i;
    uint16_t w;
    uint8_t b[2];
} CONVERTW;

//Befehle
#define rx_fifo_read    0xB000

//Statusregisterbits
#define FFIT            15  //wenn er=1, Anzahl der Datenbits im FIFO-Puffer hat das eingestellte Limit erreicht
#define RGIT            15  //wenn er=0, Senderegister ist bereit für neue Sendedaten
#define WKUP            12  //Aufweck-Timer ist abgelaufen
#endif
