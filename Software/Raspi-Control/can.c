/*
 * can.c
 * 
 * Copyright 2015  Felix Horn
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <time.h>
#include "can.h"
#include "mcp2515.h"

char neue_minute;

char can_read_message (struct can_message *message) {
    /*
     * Prüfen ob es eine neue CAN-Nachricht gibt,
     * wenn nicht, dann Funktion beenden und "0" zurückgeben */
    char raw_message[13];
    if (mcp2515_read_message(raw_message) != 1) {
        return 0;
    }

    /*
     * Neue CAN-Nachricht vorhanden ->
     * Daten in Struktur "can_message" sortieren */
    unsigned char i;
    message->typ = raw_message[0];
    message->typ = message->typ<<3;
    message->typ = (message->typ|(raw_message[1]>>5));
    message->id = (0b00000011 & raw_message[1]);
    message->id = message->id<<8;
    message->id = (message->id|raw_message[2]);
    message->id = message->id<<8;
    message->id = (message->id|raw_message[3]);
    message->rtr = (raw_message[4]&(1<<RTR));
    message->length = (raw_message[4]&((1<<DLC3)|(1<<DLC2)|(1<<DLC1)|(1<<DLC0)));
    for(i=0; i<message->length; i++) {
        message->data[i] = raw_message[i+5];
        }
    
    /*
     * Nachrichtenlänge prüfen und bei Fehler Nachricht verwerfen */
    if(message->length > 8) {
        message->typ = 0;
        return 0;
        }
    
    #if CAN_AUSGABE
        //Uhrzeit
        time_t t;
        struct tm *ts;

        t = time(NULL);
        ts = localtime(&t);
        printf("%s     Typ: %d  ID: %lu  Länge: %d Daten: ", asctime(ts), message->typ, message->id, message->length);
        for(i=0; i<message->length; i++) {
            printf("%02X ", message->data[i]);
        }
    #endif
    return 1;
}

void can_send_message (struct can_message *message) {
    mcp2515_send_message(message);
}

void can_send_time (void) {
    time_t t;
    struct tm *ts;
    
    t = time(NULL);
    ts = localtime(&t);
    
    if ((ts->tm_sec == 58) & (neue_minute == 0)) {
        struct can_message time_message;
        
        /*
         * Zeit anpassen */
        ts->tm_year = ts->tm_year-100;
        ts->tm_mon = ts->tm_mon+1;
        
        /*
         * Nachricht vorbereiten */
        time_message.typ = Zeit;
        time_message.id = Serverzeit;
        time_message.length = Laenge_Zeit;
        time_message.data[0] = ts->tm_year;
        time_message.data[1] = ts->tm_mon;
        time_message.data[2] = ts->tm_mday;
        time_message.data[3] = ts->tm_wday;
        time_message.data[4] = ts->tm_hour;
        time_message.data[5] = ts->tm_min;
        time_message.data[6] = ts->tm_sec;
        
        /*
         * Nachricht senden */
        can_send_message(&time_message);
        
        neue_minute = 1;
    }
    
    if ((ts->tm_sec == 59) & (neue_minute == 1)) {
        neue_minute = 0;
    }
}
    
