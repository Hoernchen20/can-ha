/*
 * mcp2515.c
 *
 *  Created on: 25.01.2015
 *      Author: felix
 */

#include "mcp2515.h"

uint8_t mcp2515_rx_status (void) {
    uint8_t temp_data[2] = {RX_Status, 0};
    spi_transfer_bytes((1<<MCP2515_CS), 2, temp_data);
	return temp_data[1];
}

uint8_t mcp2515_status (void) {
    uint8_t temp_data[2] = {Read_Status, 0};
    spi_transfer_bytes((1<<MCP2515_CS), 2, temp_data);
    return temp_data[1];
}        

uint8_t mcp2515_read_message (struct can_message * message) {
    /*
     * Prüfen, ob neue Nachricht da, wenn ja,
     * auszulesende Bufferadresse zuweisen */
    uint8_t temp = mcp2515_status();
    if (temp & (1<<CANINTF_RX0IF)) {
        temp = Read_RX0;
    }
    else if (temp & (1<<CANINTF_RX1IF)) {
        temp = Read_RX1;
    }
    else {
        return 0;
    }

    /*
     * Bufferadresse an den Anfang des temporären Arrays schreiben
     * und Daten vom MCP2515 abholen */
    uint8_t buf[14];
    buf[0] = temp;
    spi_transfer_bytes((1<<MCP2515_CS), 14, buf);

    uint8_t i;
    message->typ = buf[1];
    message->typ = message->typ<<3;
    message->typ = (message->typ|(buf[2]>>5));
    message->id = (0b00000011 & buf[2]);
    message->id = message->id<<8;
    message->id = (message->id|buf[3]);
    message->id = message->id<<8;
    message->id = (message->id|buf[4]);
    message->rtr = (buf[5]&(1<<RTR));
    message->length = (buf[5]&((1<<DLC3)|(1<<DLC2)|(1<<DLC1)|(1<<DLC0)));
    for(i=0; i<message->length; i++) {
        message->data[i] = buf[i+6];
    }

    /*
     * Nachrichtenlänge prüfen und bei Fehler Nachricht verwerfen */
    if(message->length > 8) {
        return 0;
    }

    return 1;
}

void can_send_message (struct can_message * msg) {
    //Freien Puffer auswählen
    uint8_t status = mcp2515_status();
    uint8_t buffer_adress = 0;
    uint8_t rts_txx = 0;
    
    if ((~status) & (1<<TXB0CNTRL_TXREQ)) {
        buffer_adress = Load_TX0;
        rts_txx = RTS_TX0;
    }
    else if ((~status) & (1<<TXB1CNTRL_TXREQ)) {
        buffer_adress = Load_TX1;
        rts_txx = RTS_TX1;
    }
    else if ((~status) & (1<<TXB2CNTRL_TXREQ)) {
        buffer_adress = Load_TX2;
        rts_txx = RTS_TX2;
    }

    //Temporäres Array erzeugen und die Daten sortieren
    uint8_t temp_msg[14];
    temp_msg[0] = buffer_adress;
    temp_msg[1] = msg->typ>>3;
    temp_msg[2] = (((uint8_t)msg->typ<<5)|(1<<EXIDE)|(uint8_t)(msg->id>>16));
    temp_msg[3] = (uint8_t)(msg->id>>8);
    temp_msg[4] = (msg->id);
    temp_msg[5] = ((uint8_t)(msg->rtr<<6)|(msg->length));
	for (uint8_t i=0; i<msg->length; i++)
	{
    	temp_msg[6+i] = (msg->data[i]);
    }
    
    //Daten an den Can-Controller übertragen
    spi_transfer_bytes((1<<MCP2515_CS), 14, temp_msg);

   	//Nachricht senden
    spi_transfer_bytes((1<<MCP2515_CS), 1, &rts_txx);
}

void mcp2515_init (void) {
    /*
	 * Reset durchführen */
    uint8_t temp_reset = RESET;
    spi_transfer_bytes((1<<MCP2515_CS), 1, &temp_reset);
    _delay_ms(10);

#if ((F_MCP2515 == 8000000) & (MCP2515_BAUDRATE == 10000))
    uint8_t temp_cnf3[5] = {WRITE, CNF3, ((1<<PHSEG22)|(1<<PHSEG21)|(1<<PHSEG20)),
            ((1<<BTLMODE)|(1<<PHSEG12)|(1<<PHSEG11)|(1<<PHSEG10)|(1<<PRSEG2)|(1<<PRSEG1)|(1<<PRSEG0)),
            ((1<<BRP3)|(1<<BRP2)|(1<<BRP1)|(1<<BRP0))};
#endif

#if ((F_MCP2515 == 16000000) & (MCP2515_BAUDRATE == 10000))
    uint8_t temp_cnf3[5] = {WRITE, CNF3, ((1<<PHSEG22)|(1<<PHSEG21)|(1<<PHSEG20)),
            ((1<<BTLMODE)|(1<<PHSEG12)|(1<<PHSEG11)|(1<<PHSEG10)|(1<<PRSEG2)|(1<<PRSEG1)|(1<<PRSEG0)),
            ((1<<BRP4)|(1<<BRP3)|(1<<BRP2)|(1<<BRP1)|(1<<BRP0))};
#endif

#if ((F_MCP2515 == 8000000) & (MCP2515_BAUDRATE == 100000))
    uint8_t temp_cnf3[5] = {WRITE, CNF3, ((1<<PHSEG22)|(1<<PHSEG21)|(1<<PHSEG20)),
            ((1<<BTLMODE)|(1<<PHSEG12)|(1<<PHSEG10)|(1<<PRSEG2)),
            (1<<BRP0)};
#endif

#if ((F_MCP2515 == 16000000) & (MCP2515_BAUDRATE == 100000))
    uint8_t temp_cnf3[5] = {WRITE, CNF3, ((1<<PHSEG22)|(1<<PHSEG21)|(1<<PHSEG20)),
            ((1<<BTLMODE)|(1<<PHSEG12)|(1<<PHSEG10)|(1<<PRSEG2)),
            ((1<<BRP1)|(1<<BRP0))};
#endif
    spi_transfer_bytes((1<<MCP2515_CS), 5, temp_cnf3);

    /*
     * Einstellungen Empfangsbuffer 0: Filter aus */
    uint8_t temp_rxb0ctrl[3] = {WRITE, RXB0CTRL, ((1<<RXM1)|(1<<RXM0))};
    spi_transfer_bytes((1<<MCP2515_CS), 3, temp_rxb0ctrl);

    /*
     * Einstellungen Empfangsbuffer 1: Filter aus */
    uint8_t temp_rxb1ctrl[3] = {WRITE, RXB1CTRL, ((1<<RXM1)|(1<<RXM0))};
    spi_transfer_bytes((1<<MCP2515_CS), 3, temp_rxb1ctrl);

    /*
     * Interrupts bei Nachrichtenempfang auslösen */
//      uint8_t temp_caninte[3] = {WRITE, CANINTE, ((1<<RX0IE)|(1<<RX0IE))};
//      spi_transfer_bytes((1<<MCP2515_CS), 3, temp_caninte);

    /*
     * In Normalen Modus umschalten */
    uint8_t temp_canctrl[3] = {WRITE, CANCTRL, 0};
    spi_transfer_bytes((1<<MCP2515_CS), 3, temp_canctrl);
}
