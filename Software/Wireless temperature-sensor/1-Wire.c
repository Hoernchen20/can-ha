#include "1-Wire.h"

uint8_t onewire_presence_pulse;

void onewire_reset(void) {
    //1Wire-Bus auf low ziehen, 1Wire_Pin auf Ausgang stellen und den Port low treiben lassen
    ONEWIRE_DDR |= (1<<ONEWIRE_BIT);
    ONEWIRE_PORT &= ~(1<<ONEWIRE_BIT);

    _delay_us(496);

    //1Wire-Pin auf Eingang stellen, �ber ext. Pullup stellt sich ein high Pegel ein
    ONEWIRE_DDR &= ~(1<<ONEWIRE_BIT);

    _delay_us(65);

    //1Wire_Bus Pegel einlesen, 0V = mind. ein Slave vorhanden
    if (!(ONEWIRE_PIN & (1<<ONEWIRE_BIT))) {
        onewire_presence_pulse = 1;
    }
    else {
        onewire_presence_pulse = 0;
    }

    _delay_us(424);
}

void onewire_send_bit(unsigned char bitwert) {
//Ausgabe eines Bits
    ONEWIRE_DDR |= (1<<ONEWIRE_BIT);		//1Wire-Bus auf low ziehen, 1Wire_Pin auf Ausgang
    ONEWIRE_PORT &= ~(1<<ONEWIRE_BIT);		//stellen und den Port low treiben lassen

    _delay_us(1);

    if (bitwert) {
    	ONEWIRE_DDR &= ~(1<<ONEWIRE_BIT);
    }

    _delay_us(66);
    ONEWIRE_DDR &= ~(1<<ONEWIRE_BIT);		//1Wire-Pin auf Eingang stellen, �ber ext. Pullup
    										//stellt sich ein high Pegel ein
}

void onewire_send_byte(unsigned char bytewert) {
//Ausgabe eines Bytes
    unsigned char i;
    unsigned char maske = 1;

    for (i=0; i<8; i++) {
        if (bytewert & maske) {
            onewire_send_bit(1);
        }

        else {
            onewire_send_bit(0);
        }

        maske = maske * 2;
    }
}

unsigned char onewire_read_bit(void) {
//Einlesen eines Bits
    unsigned char bitwert;

    ONEWIRE_DDR |= (1<<ONEWIRE_BIT);		//1Wire-Bus auf low ziehen, 1Wire_Pin auf Ausgang
    ONEWIRE_PORT &= ~(1<<ONEWIRE_BIT);		//stellen und den Port low treiben lassen

    _delay_us(1);

    ONEWIRE_DDR &= ~(1<<ONEWIRE_BIT);		//1Wire-Pin auf Eingang stellen, �ber ext. Pullup
										    //stellt sich ein high Pegel ein

    _delay_us(15);
    bitwert = (ONEWIRE_PIN & (1<<ONEWIRE_BIT));
    _delay_us(66);

    return(bitwert);
}

unsigned char onewire_read_byte(void) {
//Einlesen eines Bytes
    unsigned char i;
    unsigned char bytewert = 0;

    for (i=0; i<8; i++) {
        if (onewire_read_bit()) {
            bytewert |= 1 << i;
        }
    }

    return(bytewert);
}

void onewire_read_rom(uint8_t *rom) {
//Lesen der 64-bit Adresse
    onewire_reset();
    onewire_send_byte(read_rom);

    /*Schleifenzähler läuft rückwärts, weil der Slave mit dem letzten Byte
    (Family-Code) anfängt und so der ROM-Code richtig herum in "adresse" steht*/
    for (uint8_t i=8; i>0; i--) {
        *rom = onewire_read_byte();
        rom++;
    }
}

void onewire_send_command(uint8_t *rom, uint8_t command) {
//1Wire-Slave über die 64-bit Adresse ansprechen
    unsigned char i;

    onewire_reset();

    for (i=0; i<8; i++) {
        onewire_send_byte(*rom);
        rom++;
    }

    onewire_send_byte(command);
}

void onewire_start_temp(void) {
// Temperaturmessung an allen DS18x20 starten
    onewire_reset();
    onewire_send_byte(skip_rom);
    onewire_send_byte(convert_t);
}

uint8_t onewire_read_temp (struct onewire_temp_sensor *sensor)
//Temperatur eines DS18x20 auslesen und umrechnen
{
    uint8_t spad[9];
    uint8_t i;
    uint8_t crc = 0;
    float temp_f;

    //einzelnen Sensor ansprechen
    onewire_reset();
    if (onewire_presence_pulse == 0) {
        return 0;
        }
    onewire_send_byte(match_rom);

    for (i=0; i<8; i++) {
        onewire_send_byte(sensor->rom[i]);
    }

    //Scratchpad auslesen
    onewire_send_byte(read_spad);
    for (i=0; i<9; i++) {
        spad[i] = onewire_read_byte();
    }

    //CRC Check
    for (i=0; i<9; i++) {
        crc = _crc_ibutton_update(crc, spad[i]);
    }

    //Temperatur berechnen
    if (crc == 0) {
        //0,5°C Bit löschen
        spad[temperature_lsb] &= ~(1<<0);

        //Temperatur umrechnen
        if (spad[temperature_msb] == 0) {
            temp_f = spad[temperature_lsb]/2.0;
        }
        else {
            i = ~(spad[temperature_lsb]);
            temp_f = i+1;
            temp_f = (temp_f*(-1))/2.0;
        }

        //Nachkommaberechnung
        temp_f = temp_f-0.25+(16.0-spad[count_remain])/16.0;

        //Umrechnung, um Kommazahl als Int zu speichern und abspeichern
        sensor->temperature = temp_f*100;
        return 1;
    }
    return 0;
}
