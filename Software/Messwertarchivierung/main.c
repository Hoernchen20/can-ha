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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "db_connect.c"

#define AUSGABE 0
#define OK 0
#define ERROR 1

/* Funktionsprototypen ************************************************/
void insert_archiv_1m(void);
void insert_archiv_15m(void);
void insert_archiv_1h(void);
void insert_archiv_1d(void);

/* Hauptprogramm ******************************************************/
int main(int argc, char **argv)
{
  /*
   * Uhrzeit lesen */
  time_t t;
  struct tm *ts;
  t = time(NULL);
  ts = localtime(&t);

  /*
   * Verbindung zur Datenbank aufbauen */
  if (db_connect() != OK) {
    return ERROR;
  }

  /*
   * Jede Minute Messwerte aus der Tabelle "messwert"
   * in die Tabelle "messwert_archiv" kopieren */
  insert_archiv_1m();

  /*
   * Alle 15 Minuten Min, Max und Avg der Messwerte aus der Tabelle
   * "messwert_archiv" in die Tabelle "messwert_archiv_15m" kopieren */
  if ( (ts->tm_min == 0) | (ts->tm_min == 15) | (ts->tm_min == 30) | (ts->tm_min == 45) ) {
    insert_archiv_15m();
  }

  /*
   * Jede Stunde Min, Max und Avg der Messwerte aus der Tabelle
   * "messwert_archiv" in die Tabelle "messwert_archiv_1h" kopieren */
  if (ts->tm_min == 1) {
    insert_archiv_1h();
  }

  /*
   * Jede Tag Min, Max und Avg der Messwerte aus der Tabelle
   * "messwert_archiv_1h" in die Tabelle "messwert_archiv_1d" kopieren */
  if ( (ts->tm_hour == 0) & (ts->tm_min == 1) ) {
    insert_archiv_1d();
  }

  /*
   * Verbindung zur Datenbank schließen */
  mysql_close(my);

  return OK;
}

/* Funktionen *********************************************************/
void insert_archiv_1m(void)
{
  MYSQL_ROW row;
  MYSQL_RES *result;
  char query[1024];
  char insert_query[4096];
  unsigned long i;

  /*
   * Alle Information der Messwerte abfragen */
   sprintf(query, "SELECT "
                    "messwert.id, "
                    "CONCAT_WS(' ', messwert.tw1, messwert.tw2, messwert.tw3) AS infopunkt, "
                    "messwert.akt_messwert,"
                    "messwert.einheit, "
                    "messwert.grenze_unten, "
                    "messwert.warnung_unten, "
                    "messwert.warnung_oben, "
                    "messwert.grenze_oben, "
                    "(SELECT meldung.akt_status "
                      "FROM meldung "
                      "WHERE meldung.id = messwert.grenze_meldung_id) AS akt_status_grenze, "
                    "(SELECT meldung.akt_status "
                      "FROM meldung "
                      "WHERE meldung.id = messwert.warnung_meldung_id) AS akt_status_warnung, "
                    "(SELECT meldung.prioritaet "
                      "FROM meldung "
                      "WHERE meldung.id = messwert.grenze_meldung_id) AS prioritaet_grenze, "
                    "(SELECT meldung.prioritaet "
                      "FROM meldung "
                      "WHERE meldung.id = messwert.warnung_meldung_id) AS prioritaet_warnung, "
                    "messwert.archiv_level "
                 "FROM messwert WHERE 1");

  mysql_real_query(my, query, strlen(query));
  db_check_error();

  result = mysql_store_result(my);
  db_check_error();

  /*
   * Anzahl der Zeilen der Abfrage feststelle und ein entsprechend
   * großes Array für die Daten bereitstellen */
  unsigned long num_rows = mysql_num_rows(result);

  unsigned int messwert_id[num_rows];
  char         infopunkt[num_rows][256];
  float        akt_messwert[num_rows];
  char         einheit[num_rows][10];
  float        grenze_unten[num_rows];
  float        warnung_unten[num_rows];
  float        warnung_oben[num_rows];
  float        grenze_oben[num_rows];
  char         grenze_status[num_rows];
  char         warnung_status[num_rows];
  char         grenze_prioritaet[num_rows];
  char         warnung_prioritaet[num_rows];
  char         archiv_level[num_rows];

  /*
   * Daten der Abfrage in das Array kopieren. Die Variable "num_rows"
   * wird dabei als Schleifenzähler genutzt und wieder neu hochgezählt */
  num_rows = 0;

  while ((row = mysql_fetch_row (result)) != NULL) {
    messwert_id[num_rows] = atoi(row[0]);
    strcpy(infopunkt[num_rows], row[1]);
    akt_messwert[num_rows] = atof(row[2]);
    strcpy(einheit[num_rows], row[3]);
    grenze_unten[num_rows] = atof(row[4]);
    warnung_unten[num_rows] = atof(row[5]);
    warnung_oben[num_rows] = atof(row[6]);
    grenze_oben[num_rows] = atof(row[7]);
    grenze_status[num_rows] = atoi(row[8]);
    warnung_status[num_rows] = atoi(row[9]);
    grenze_prioritaet[num_rows] = atoi(row[10]);
    warnung_prioritaet[num_rows] = atoi(row[11]);
    archiv_level[num_rows] = atoi(row[12]);

    num_rows++;
  }

  /*
   * Query für Eintrag Tabelle "messwert_archiv" erstellen */
  sprintf(insert_query, "INSERT INTO "
                          "messwert_archiv (messwert_id, wert, zeitstempel) "
                        "VALUES ");

  for (i = 0; i <= num_rows; i++) {
    /*
     *  Ist der Messwert innerhalb der Grenzwerte und das Archiv-Level größer null, dann Messwerte in das Archiv einfügen */
    if ( (akt_messwert[i] >= grenze_unten[i]) & (akt_messwert[i] <= grenze_oben[i]) & (archiv_level[i] > 0) ) {
      char tmp_str[100];
      sprintf (tmp_str, "(%u,%.3f, NOW()),", messwert_id[i], akt_messwert[i]);
      strcat(insert_query, tmp_str);
    }
  }

  /*
   * Letztes Zeichen (ein Komma) aus der voherigen for-Schleife
   * löschen */
  insert_query[strlen(insert_query)-1] = '\0';

  /*
   * Query bei Bedarf auf der Kommandozeile ausgeben */
#if AUSGABE
  printf("%s",insert_query);
#endif

  /*
   * Query an die Datenbank schicken und speicher wieder freigeben */
  mysql_real_query(my, insert_query, strlen(insert_query));
  db_check_error();
  mysql_free_result(result);
}

void insert_archiv_15m(void) {
  char query[1024];

  sprintf(query, "INSERT INTO "
                    "messwert_archiv_15m (messwert_id, min, max, avg, zeitstempel) "
                  "SELECT "
                    "messwert_archiv.messwert_id, "
                    "MIN(messwert_archiv.wert), "
                    "MAX(messwert_archiv.wert), "
                    "ROUND(AVG(messwert_archiv.wert), 3), "
                    "DATE_FORMAT(messwert_archiv.zeitstempel, '%%Y-%%m-%%d %%k:%%i:00') "
                  "FROM "
                    "messwert_archiv "
                  "JOIN "
                    "messwert ON messwert_archiv.messwert_id = messwert.id "
                    "WHERE EXTRACT(DAY_MINUTE FROM messwert_archiv.zeitstempel) >= EXTRACT(DAY_MINUTE FROM (NOW() - INTERVAL 15 MINUTE)) "
                    "AND messwert.archiv_level >= 2 "
                    "GROUP BY messwert_archiv.messwert_id");

  /*
   * Query bei Bedarf auf der Kommandozeile ausgeben */
#if AUSGABE
  printf("%s",query);
#endif

  mysql_real_query(my, query, strlen(query));
  db_check_error();
}

void insert_archiv_1h(void) {
  char query[1024];

  sprintf(query, "INSERT INTO "
                    "messwert_archiv_1h (messwert_id, min, max, avg, zeitstempel) "
                  "SELECT "
                    "messwert_archiv.messwert_id, "
                    "MIN(messwert_archiv.wert), "
                    "MAX(messwert_archiv.wert), "
                    "ROUND(AVG(messwert_archiv.wert), 3), "
                    "DATE_FORMAT(messwert_archiv.zeitstempel, '%%Y-%%m-%%d %%k:00:00') "
                  "FROM "
                    "messwert_archiv "
                  "JOIN "
                    "messwert ON messwert_archiv.messwert_id = messwert.id "
                    "WHERE EXTRACT(DAY_HOUR FROM messwert_archiv.zeitstempel) = EXTRACT(DAY_HOUR FROM (NOW() - INTERVAL 1 HOUR)) "
                    "AND messwert.archiv_level >= 3 "
                    "GROUP BY messwert_archiv.messwert_id");

  /*
   * Query bei Bedarf auf der Kommandozeile ausgeben */
#if AUSGABE
  printf("%s",query);
#endif

  mysql_real_query(my, query, strlen(query));
  db_check_error();
}

void insert_archiv_1d(void) {
  char query[1024];

  sprintf(query, "INSERT INTO "
                   "messwert_archiv_1d (messwert_id, min, max, avg, datum) "
                "SELECT "
                   "messwert_archiv_1h.messwert_id, "
                   "MIN(messwert_archiv_1h.min), "
                   "MAX(messwert_archiv_1h.max), "
                   "ROUND(AVG( messwert_archiv_1h.avg ), 3), "
                   "DATE(messwert_archiv_1h.zeitstempel) "
                "FROM "
                   "messwert_archiv_1h "
                   "WHERE DATE(messwert_archiv_1h.zeitstempel) = DATE_SUB(CURDATE(), INTERVAL 1 DAY) "
                   "GROUP BY messwert_archiv_1h.messwert_id");

  /*
   * Query bei Bedarf auf der Kommandozeile ausgeben */
#if AUSGABE
  printf("%s",query);
#endif

  mysql_real_query(my, query, strlen(query));
  db_check_error();
}
