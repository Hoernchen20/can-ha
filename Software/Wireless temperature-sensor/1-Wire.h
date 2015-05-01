#ifndef _1wire_h
#define _1wire_h

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/crc16.h>

/*
 * Allgemeine Einstellungen */
#define ONEWIRE_BIT     PA2
#define ONEWIRE_PORT    PORTA
#define ONEWIRE_DDR     DDRA
#define ONEWIRE_PIN     PINA

//Register
extern uint8_t onewire_presence_pulse;         //1 = mind. 1 Slave antwortet

//Typdefinitionen
struct onewire_temp_sensor
{
    uint8_t     rom[8];
    int16_t     temperature;
};

//Funktionen
void onewire_reset(void);
void onewire_send_bit(unsigned char bitwert);
void onewire_send_byte(unsigned char bytewert);
unsigned char onewire_read_bit(void);
unsigned char onewire_read_byte(void);
void onewire_read_rom(uint8_t *rom);
void onewire_send_command(uint8_t *rom, uint8_t command);
void onewire_start_temp(void);
uint8_t onewire_read_temp(struct onewire_temp_sensor *sensor);

//Rom Commends
#define read_rom        0x33    //Adresse eines Slaves am Bus auslesen
#define match_rom       0x55    //einen Slave gezielt ansprechen
#define skip_rom        0xCC    //Alles Slaves ansprechen
#define search_rom      0xF0    //Adressen aller Slaves am Bus auslesen, es gibt keine
                                //Zuordnung, welcher Slave welcher ist (App-Note 187)

//DS18s20 Commends
#define convert_t       0x44    //Temperaturmessung starten
#define read_spad       0xBE    //Scratchpad auslesen
#define write_spad      0x4E    //Th und Tl beschreiben
#define copy_spad       0X48    //Th und Tl in das EEPROM speichern
#define recall_eepr     0xB8    //Th und Tl aus dem EEPROM laden
#define read_pow_sup    0xB4    //Power Supply Mode auslesen

//Scratchpad
#define temperature_lsb 0
#define temperature_msb 1
#define th_register     2
#define tl_register     3
#define count_remain    6
#define count_per_c     7      //ist im DS18S20 fest geschrieben
#define CRC             8

#endif
