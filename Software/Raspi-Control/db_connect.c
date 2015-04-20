/*
 * mysql.c
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
#include <stdlib.h>
#include <string.h>
#include "mysql/mysql.h"
#include "db_connect.h"

MYSQL *my;

int db_connect (void) {
    /* Handle initialisieren */
    my = mysql_init(NULL);
    
    if(my == NULL) {
        #if MYSQL_AUSGABE
            fprintf(stderr, " Initialisierung fehlgeschlagen\n");
        #endif
        return 1;
    }

    /* mit der Datenbank verbinden */
    if( mysql_real_connect (my,
                            MYSQL_HOST,
                            MYSQL_BENUTZER,
                            MYSQL_KENNWORT,
                            MYSQL_DATENBANK,
                            MYSQL_HOST_PORT,
                            MYSQL_SOCKET,
                            MYSQL_FLAGS)  == NULL) {

        #if MYSQL_AUSGABE
            fprintf (stderr, "Fehler mysql_real_connect():"
                "%u (%s)\n",mysql_errno (my), mysql_error (my));
        #endif
        return 1;
    }
    else {
        #if MYSQL_AUSGABE
            printf("Erfolgreich mit dem MySQL-Server verbunden\n");
        #endif
        return 0;
    }
}
   
void db_connect_test (void) {
    printf("Verbindung Datenbank: ");
    if (db_connect() == 0) {
        printf("OK\n");
    }
    else {
        printf("ERROR");
    }
    mysql_close (my);
    return;
}

void db_check_error(void) {
    if (mysql_errno(my) != 0) {
        fprintf(stderr, "Fehler: %s\n", mysql_error(my));
    }
}

void db_update_tbl_messwert (float *messwert, unsigned long *can_id, unsigned char can_laenge) {
    db_connect();

    /*Query für Update Tabelle "messwert" */
    char query[256];
    sprintf(query, "UPDATE "
                     "messwert "
                   "SET "
                     "akt_messwert = %.3f, akt_messwert_zeitstempel = NOW() "
                   "WHERE can_id = %lu AND can_laenge = ", *messwert, *can_id);
    
    if (can_laenge == 2) {
        strcat(query, "2");
    }
    
    mysql_real_query(my, query, strlen(query));
    db_check_error();
    mysql_close(my);
    return;
}

void db_insert_tbl_messwert_archiv (float *messwert, unsigned long *can_id) {
    db_connect();
    
    /* Query für Eintrag Tabelle "messwert_archiv" */
    char query[256];
    sprintf(query, "INSERT INTO "
                     "messwert_archiv (messwert_archiv.messwert_id, messwert_archiv.wert, messwert_archiv.zeitstempel) "
                   "VALUES "
                     "((SELECT "
                       "messwert.id "
                     "FROM "
                       "messwert "
                     "WHERE messwert.can_id = %lu)"
                   ", %.3f, NOW())", *can_id, *messwert);

    mysql_real_query(my, query, strlen(query));
    db_check_error();
    mysql_close(my);
    return;
}

char db_get_messwert_info (unsigned long *can_id, struct messwert_info *mw_info) {
    MYSQL_ROW row;
    MYSQL_RES *result;
    unsigned int i;
    
    db_connect();
    
    char query[512];
    sprintf(query, "SELECT "
                        "CONCAT_WS(' ', messwert.tw1, messwert.tw2, messwert.tw3) AS infopunkt, "
                        "messwert.einheit, "
                        "messwert.grenze_unten, "
                        "messwert.warnung_unten, "
                        "messwert.warnung_oben, "
                        "messwert.grenze_oben, "
                        "CAST(meldung.akt_status AS UNSIGNED) AS akt_status, "
                        "meldung.prioritaet, "
                        "meldung.id "
                    "FROM "
                        "messwert "
                    "INNER JOIN "
                        "infopunkt ON (infopunkt.id = messwert.infopunkt_id) "
                    "INNER JOIN "
                        "meldung ON (meldung.infopunkt_id = infopunkt.id) "
                    "WHERE messwert.can_id = %lu AND meldung.can_id = NULL ORDER BY messwert.id ASC LIMIT 2", *can_id);
    
    mysql_real_query(my, query, strlen(query));
    db_check_error();
    
    result = mysql_store_result(my);
    db_check_error();
    
    if (mysql_num_rows(result) != 0) {
        while ((row = mysql_fetch_row (result)) != NULL) {
            /* 
             * Erste Zeile auslesen */
            if ( i == 0) {    
                strcpy(mw_info->infopunkt, row[0]);
                strcpy(mw_info->einheit, row[1]);
                mw_info->grenze_unten = atof(row[2]);
                mw_info->warnung_unten = atof(row[3]);
                mw_info->warnung_oben = atof(row[4]);
                mw_info->grenze_oben = atof(row[5]);
                mw_info->grenze_status = atoi(row[6]);
                mw_info->grenze_prioritaet = atoi(row[7]);
                mw_info->grenze_meldung_id = atoi(row[8]);
            }
            /*
             * Zweite Zeile auslesen */
            else {
                mw_info->warnung_status = atoi(row[6]);
                mw_info->warnung_prioritaet = atoi(row[7]);
                mw_info->warnung_meldung_id = atoi(row[8]);
            }
            
            i++;        
        }
        /* Speicherplatz wieder freigeben */
        mysql_free_result(result);
        mysql_close(my);  
        return 0;
    } else {
        /* Speicherplatz wieder freigeben */
        mysql_free_result(result);
        mysql_close(my);  
        return 1;
    }
}

char db_get_einzelmeldung_info (unsigned long *can_id, struct einzelmeldung_info *em_info) {
    MYSQL_ROW row;
    MYSQL_RES *result;
    
    db_connect();
    
    char query[512];
    sprintf(query, "SELECT "
                        "CONCAT_WS(' ', meldung.tw1, meldung.tw2, meldung.tw3) AS infopunkt, "
                        "meldung.status_00, "
                        "meldung.status_01, "
                        "meldung.status_10, "
                        "meldung.status_11, "
                        "meldung.akt_status, "
                        "meldung.prioritaet, "
                        "meldung.id "
                    "FROM "
                        "meldung "
                    "WHERE meldung.can_id = %lu AND meldung.dm = 0", *can_id);
    
    mysql_real_query(my, query, strlen(query));
    db_check_error();
    
    result = mysql_store_result(my);
    db_check_error();
    
    if (mysql_num_rows(result) != 0) {
        while ((row = mysql_fetch_row (result)) != NULL) {
            strcpy(em_info->infopunkt, row[0]);
            strcpy(em_info->status_00, row[1]);
            strcpy(em_info->status_01, row[2]);
            strcpy(em_info->status_10, row[3]);
            strcpy(em_info->status_11, row[4]);
            em_info->status = atoi(row[5]);
            em_info->prioritaet = atoi(row[6]);
            em_info->meldung_id = atoi(row[7]);
        }
        /* Speicherplatz wieder freigeben */
        mysql_free_result(result);
        mysql_close(my);  
        return 0;
    } else {
        /* Speicherplatz wieder freigeben */
        mysql_free_result(result);
        mysql_close(my);  
        return 1;
    }
}

void db_update_meldung_id (unsigned int *meldung_id, char new_status) {
    db_connect();

    /*Query für Update Tabelle "meldung" */
    char query[256];
    sprintf(query, "UPDATE "
                     "meldung "
                   "SET "
                     "akt_status = %d, akt_status_zeitstempel = NOW() "
                   "WHERE id = %d", new_status, *meldung_id);

    mysql_real_query(my, query, strlen(query));
    db_check_error();
    mysql_close(my);
    return;
}

void db_update_meldung_can_id (unsigned long *meldung_can_id, char new_status, char doppelmeldung) {
    db_connect();

    /*Query für Update Tabelle "meldung" */
    char query[256];
    sprintf(query, "UPDATE "
                     "meldung "
                   "SET "
                     "akt_status = %d, akt_status_zeitstempel = NOW() "
                   "WHERE can_id = %ld AND dm = %d", new_status, *meldung_can_id, doppelmeldung);

    mysql_real_query(my, query, strlen(query));
    db_check_error();
    mysql_close(my);
    return;
}

void db_insert_meldung_archiv (unsigned int *meldung_id, char new_status, char *info, unsigned char info_length) {
    db_connect();

    /*Query für Insert Tabelle "meldung_archiv" */
    char query[256];
    sprintf(query, "INSERT INTO "
                     "meldung_archiv (meldung_id, status, info, zeitstempel) "
                   "VALUES "
                     "(%d, %d, ", *meldung_id, new_status);
    
    /*
     * Einen Info-Text einfügen. Gibt es keinen Text, wird stattdessen NULL eingefügt */
    if (info_length != 0) {
        strcat(query, "\"");
        strncat(query, info, info_length);
        strcat(query, "\"");
    }
    else {
        strcat(query, "NULL");
    }
    strcat(query, ", NOW())");

    mysql_real_query(my, query, strlen(query));
    db_check_error();
    mysql_close(my);
    return;
}

void db_update_tbl_sollwert (float *messwert, unsigned long *can_id) {
    db_connect();

    /*Query für Update Tabelle "messwert" */
    char query[256];
    sprintf(query, "UPDATE "
                     "sollwert "
                   "SET "
                     "akt_sollwert = %.3f, akt_sollwert_zeitstempel = NOW() "
                   "WHERE can_id = %lu", *messwert, *can_id);

    mysql_real_query(my, query, strlen(query));
    db_check_error();
    mysql_close(my);
    return;
}
