/*
 * spi.h
 *
 *  Created on: 25.01.2015
 *      Author: felix
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * Portzuweisung */
#define RFM12_CS    PB1
#define MCP2515_CS  PB2

#ifdef __AVR_ATmega16__
    #define SPI_DDR     DDRB
    #define SPI_PORT    PORTB
    #define MOSI        PB5
    #define MISO        PB6
    #define SCK         PB7
#endif

#ifdef __AVR_ATmega48__
    #define SPI_DDR     DDRB
    #define SPI_PORT    PORTB
    #define MOSI        PB3
    #define MISO        PB4
    #define SCK         PB5
#endif

/*
 * Funktionen */
void spi_init (void);
void spi_switch_slow (void);
void spi_switch_fast (void);
uint8_t spi_send (uint8_t data_in);
void spi_transfer_bytes (uint8_t device, uint8_t n_bytes, uint8_t* data);

#endif /* SPI_H_ */
