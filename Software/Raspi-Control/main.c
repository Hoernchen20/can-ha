/*
 * main.c
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
#define DEBUG 1 

#if DEBUG 
#define ANFANG(Funktion) \
        printf("Start %s [%s:%d]\n", Funktion, __FILE__, __LINE__); 
#define ENDE(Funktion) \ 
        printf("Ende  %s [%s:%d]\n", Funktion, __FILE__, __LINE__);
#else 
#define ANFANG(Funktion) 
#define ENDE(Funktion) 
#endif

#include <stdio.h>
#include "db_connect.c"
#include "mcp2515.c"
#include "can.c"
#include "mail.c"

#define AUSGABE 1

int main(int argc, char **argv)
{
	db_connect_test();
    
    mcp2515_init();
    
    struct can_message message;
    
    while(1)
    {
        can_send_time();
        
        if (can_read_message(&message)) {
            switch(message.typ) {
                //16bit-Messwert in MySQL-Datenbank speichern
                case Messwert_16: {
                    //Messwert umrechnen
                    int16_t messwert_int;
                    uint16_t messwert_uint;
                    float messwert_float;
                    struct messwert_info mw_info;

                    messwert_uint = message.data[0];
                    messwert_uint = messwert_uint<<8;
                    messwert_uint = messwert_uint | message.data[1];
                    messwert_int = (int)messwert_uint;
                    messwert_float = messwert_int;
                    messwert_float = messwert_float / 100;
                               
                    #if AUSGABE
                        printf(" Messwert: %.3f", messwert_float);
                        printf("\n");
                    #endif

                    /*
                     * Messwert in Datenbank eintragen */
                    db_update_tbl_messwert (&messwert_float, &message.id, 2);
                    //db_get_messwert_info(&message.id, &mw_info);
                    
                    /*
                     * Ist der Messwert innerhalb der Grenzwerte, dann Messwerte in das Archiv einfügen */
                   // if ( (messwert_float >= mw_info.grenze_unten) & (messwert_float <= mw_info.grenze_oben) ) {
                    //    db_insert_tbl_messwert_archiv (&messwert_float, &message.id);
                        
                        /*
                         * Ist der Messwert wieder innerhalb der Grenzwerte, Meldung in das Archiv schreiben */
                   /*     if ( (mw_info.grenze_status == 1) | (mw_info.grenze_status == 2) ) {
                            db_update_meldung_id(&mw_info.grenze_meldung_id, 0);
                            db_insert_meldung_archiv(&mw_info.grenze_meldung_id, 0, NULL, 0);

                            if (mw_info.grenze_prioritaet <= 2) {
                                mail_messwert(0, NULL, &mw_info);
                            }
                        }
                        
                        /*
                         * Ist der Messwert wieder innerhalb der Warngrenze, Meldung in das Archiv schreiben */
                     /*   if ( (messwert_float >= mw_info.warnung_unten) & (messwert_float <= mw_info.warnung_oben) 
                        & ( (mw_info.warnung_status == 1) | (mw_info.warnung_status == 2) ) ) {
                            db_update_meldung_id(&mw_info.warnung_meldung_id, 0);
                            db_insert_meldung_archiv(&mw_info.warnung_meldung_id, 0, NULL, 0);

                            if (mw_info.warnung_prioritaet <= 2) {
                                mail_messwert(4, NULL, &mw_info);
                            }
                        }
                    }
                    
                    /*
                     * Ist der Messwert unterhalb des Grenzwertes, dann Meldung erzeugen */
                  /*  if (messwert_float < mw_info.grenze_unten) {
                        db_update_meldung_id(&mw_info.grenze_meldung_id, 2);
                        
                        /*
                         * Tritt die Unterschreitung zum ersten mal auf, dann Meldung in das Archiv schreiben 
                         * und prüfen, ob eine Mail verschickt werden soll */
                      /*  if (mw_info.grenze_status != 2) {
                            char info[50];
                            sprintf(info, "%.3f%s < %.3f%s", messwert_float, mw_info.einheit, mw_info.grenze_unten, mw_info.einheit);
                            db_insert_meldung_archiv(&mw_info.grenze_meldung_id, 2, info, strlen(info));
                            
                            if (mw_info.grenze_prioritaet <= 2) {
                                mail_messwert(2, &messwert_float, &mw_info);
                            }
                        }
                    }
                    
                    /*
                     * Ist der Messwert oberhalb des Grenzwertes, dann Meldung erzeugen */
                  /*  if (messwert_float > mw_info.grenze_oben) {
                        db_update_meldung_id(&mw_info.grenze_meldung_id, 1);
                        
                        /*
                         * Tritt die Überschreitung zum ersten mal auf, dann Meldung in das Archiv schreiben
                         * und prüfen, ob eine Mail verschickt werden soll */
                 /*       if (mw_info.grenze_status != 1) {
                            char info[50];
                            sprintf(info, "%.3f%s > %.3f%s", messwert_float, mw_info.einheit, mw_info.grenze_unten, mw_info.einheit);
                            db_insert_meldung_archiv(&mw_info.grenze_meldung_id, 1, info, strlen(info));
                            
                            if (mw_info.grenze_prioritaet <= 2) {
                                mail_messwert(1, &messwert_float, &mw_info);
                            }
                        }
                    }
                    
                    /*
                     * Ist der Messwert unterhalb der Warngrenze, dann Meldung erzeugen */
                 /*   if (messwert_float < mw_info.warnung_unten) {
                        db_update_meldung_id(&mw_info.warnung_meldung_id, 2);
                        
                        /*
                         * Tritt die Unterschreitung zum ersten mal auf, dann Meldung in das Archiv schreiben
                         * und prüfen, ob eine Mail verschickt werden soll */
                     /*   if (mw_info.warnung_status != 2) {
                            char info[50];
                            sprintf(info, "%.3f%s < %.3f%s", messwert_float, mw_info.einheit, mw_info.warnung_unten, mw_info.einheit);
                            db_insert_meldung_archiv(&mw_info.warnung_meldung_id, 2, info, strlen(info));
                            
                            if (mw_info.warnung_prioritaet <= 2) {
                                mail_messwert(6, &messwert_float, &mw_info);
                            }
                        }
                    }
                    
                    /*
                     * Ist der Messwert oberhalb der Warngrenze, dann Meldung erzeugen */
                 /*   if (messwert_float > mw_info.warnung_oben) {
                        db_update_meldung_id(&mw_info.warnung_meldung_id, 1);
                        
                        /*
                         * Tritt die Überschreitung zum ersten mal auf, dann Meldung in das Archiv schreiben
                         * und prüfen, ob eine Mail verschickt werden soll */
                     /*   if (mw_info.warnung_status != 1) {
                            char info[50];
                            sprintf(info, "%.3f%s > %.3f%s", messwert_float, mw_info.einheit, mw_info.warnung_oben, mw_info.einheit);
                            db_insert_meldung_archiv(&mw_info.warnung_meldung_id, 1, info, strlen(info));
                            
                            if (mw_info.warnung_prioritaet <= 2) {
                                mail_messwert(5, &messwert_float, &mw_info);
                            }
                        }
                    }*/
                    break;
                }
                
                case Sollwert_16: {
                    //Messwert umrechnen
                    int16_t sollwert_int;
                    uint16_t sollwert_uint;
                    float sollwert_float;
                    
                    sollwert_uint = message.data[0];
                    sollwert_uint = sollwert_uint<<8;
                    sollwert_uint = sollwert_uint | message.data[1];
                    sollwert_int = (int)sollwert_uint;
                    sollwert_float = sollwert_int;
                    sollwert_float = sollwert_float / 100;
                               
                    #if AUSGABE
                        printf(" Sollwert: %.3f", sollwert_float);
                        printf("\n");
                    #endif

                    /*
                     * Sollwert in Datenbank eintragen */
                    db_update_tbl_sollwert (&sollwert_float, &message.id);
                    break;
                }
                
                case Einzelmeldung: {
                    /*
                     * Prüfen ob die Daten stimmig sind */
                    if ( (message.data[0] < 0) | (message.data[0] > 2) ) {
                        #if AUSGABE
                        printf("Fehler\n");
                        #endif
                        break;
                    }
                    
                    struct einzelmeldung_info em_info;
                    if (db_get_einzelmeldung_info(&message.id, &em_info)) {
                        #if AUSGABE
                        printf("Unbekannte CAN-ID\n");
                        #endif
                        break;
                    }
                    db_update_meldung_can_id(&message.id, message.data[0], 0);
                                        
                    /*
                     * Hat sich der Status geändert, dann Eintrag im Archiv */
                    if (message.data[0] != em_info.status) {
                        db_insert_meldung_archiv(&em_info.meldung_id, message.data[0], NULL, 0);
                        
                        if (em_info.prioritaet <= 2) {
                            mail_meldung(&em_info, message.data[0]);
                        }
                    }
                        
                    #if AUSGABE
                        printf("\n");
                    #endif
                    break;
                }
                
                default: {
                    #if AUSGABE
                        printf("\n");
                    #endif
                }
            }
        }
        delay(10);
    }
	return 0;
}
