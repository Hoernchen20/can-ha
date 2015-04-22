#ifndef db_connect_h
#define db_connect_h
/*
 * mysql.h
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

//Ausgabe des MySQL-Moduls ein- oder ausschalten
#define MYSQL_AUSGABE   0

//Anmeldeinformationen für die Verbindung zur MySQL-Datenbank
#define MYSQL_HOST      NULL
#define MYSQL_BENUTZER  "***"
#define MYSQL_KENNWORT  "***"
#define MYSQL_DATENBANK "raspi_control"
#define MYSQL_HOST_PORT 0
#define MYSQL_SOCKET    NULL
#define MYSQL_FLAGS     0

//Variablen
extern MYSQL *my;
struct messwert_info {
    char            infopunkt[256];
    char            einheit[10];
    float           grenze_unten;
    float           warnung_unten;
    float           warnung_oben;
    float           grenze_oben;
    char            grenze_status;
    char            warnung_status;
    char            grenze_prioritaet;
    char            warnung_prioritaet;
    unsigned int    grenze_meldung_id;
    unsigned int    warnung_meldung_id;
};
struct einzelmeldung_info {
    char            infopunkt[256];
    char            status_00[40];
    char            status_01[40];
    char            status_10[40];
    char            status_11[40];
    unsigned char   status;
    unsigned char   prioritaet;
    unsigned int    meldung_id;
};

//Funktionen
int db_connect (void);
void db_connect_test (void);
void db_check_error(void);
void db_update_tbl_messwert (float *messwert, unsigned long *can_id, unsigned char can_laenge);
void db_insert_tbl_messwert_archiv (float *messwert, unsigned long *can_id);
void db_get_messwert_info (unsigned long *can_id, struct messwert_info *mw_info);
void db_get_einzelmeldung_info (unsigned long *can_id, struct einzelmeldung_info *em_info);
void db_update_meldung_id (unsigned int *meldung_id, char new_status);
void db_update_meldung_can_id (unsigned long *meldung_can_id, char new_status, char doppelmeldung);
void db_insert_meldung_archiv (unsigned int *meldung_id, char new_status, char *info, unsigned char info_lenght);
void db_update_tbl_sollwert (float *messwert, unsigned long *can_id);

#endif
