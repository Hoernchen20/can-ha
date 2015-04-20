/*
 * mail.c
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
#include <string.h>

void mail_messwert (char art, float *messwert, struct messwert_info *mw_info) {
    char mail[1024];
    char mail2[1024];
    sprintf(mail, "echo \"%s ", mw_info->infopunkt);
    switch(art) {
        case 0: {
            sprintf(mail2, "Grenzwert OK\" | mail -s\"%s Grenzwert OK\" pi", mw_info->infopunkt);
            strcat(mail, mail2);
            break;
        }
        case 1: {
            sprintf(mail2, "Oberer Grenzwert ueberschritten %.3f%s > %.3f%s\" | mail -s\"%s Grenzwert ueberschritten\" pi",
                *messwert, mw_info->einheit, mw_info->grenze_oben, mw_info->einheit, mw_info->infopunkt);
            strcat(mail, mail2);
            break;
        }
        case 2: {
            sprintf(mail2, "Unterer Grenzwert unterschritten %.3f%s < %.3f%s\" | mail -s\"%s Grenzwert unterschritten\" pi",
                *messwert, mw_info->einheit, mw_info->grenze_unten, mw_info->einheit, mw_info->infopunkt);
            strcat(mail, mail2);
            break;
        }
        case 3: {
            break;
        }
        case 4: {
            sprintf(mail2, "Warngrenze OK\" | mail -s\"%s Warngrenze OK\" pi", mw_info->infopunkt);
            strcat(mail, mail2);
            break;
        }
        case 5: {
            sprintf(mail2, "Obere Warngrenze ueberschritten %.3f%s > %.3f%s\" | mail -s\"%s Warngrenze ueberschritten\" pi",
                *messwert, mw_info->einheit, mw_info->warnung_oben, mw_info->einheit, mw_info->infopunkt);
            strcat(mail, mail2);
            break;
        }
        case 6: {
            sprintf(mail2, "Untere Warngrenze unterschritten %.3f%s < %.3f%s\" | mail -s\"%s Warngrenze unterschritten\" pi",
                *messwert, mw_info->einheit, mw_info->warnung_unten, mw_info->einheit, mw_info->infopunkt);
            strcat(mail, mail2);
            break;
        }
        case 7: {
            break;
        }
    }
    system(mail);
}

void mail_meldung (struct einzelmeldung_info *em_info, unsigned char akt_status) {
    char mail[1024];
    char mail2[1024];
    sprintf(mail, "echo \"%s ", em_info->infopunkt);
    switch(akt_status) {
        case 0: {
            sprintf(mail2, "%s\" | mail -s\"%s %s\" pi", em_info->status_00, em_info->infopunkt, em_info->status_00);
            strcat(mail, mail2);
            break;
        }
        case 1: {
            sprintf(mail2, "%s\" | mail -s\"%s %s\" pi", em_info->status_01, em_info->infopunkt, em_info->status_01);
            strcat(mail, mail2);
            break;
        }
        case 2: {
            sprintf(mail2, "%s\" | mail -s\"%s %s\" pi", em_info->status_10, em_info->infopunkt, em_info->status_10);
            strcat(mail, mail2);
            break;
        }
        case 3: {
            sprintf(mail2, "%s\" | mail -s\"%s %s\" pi", em_info->status_11, em_info->infopunkt, em_info->status_11);
            strcat(mail, mail2);
            break;
        }
    }
    system(mail);
}
