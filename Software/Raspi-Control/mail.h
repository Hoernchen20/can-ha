#ifndef mail_h
#define mail_h
/*
 * mail.h
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


//Funktionen
void mail_messwert (char art, float *messwert, struct messwert_info *mw_info);
/*
 * Verschicken von Mails bei Messwertabweichungen an den User "pi"
 * Über die Variable "art" wird der Text ausgewählt:
 * 0 = Grenzwert OK
 * 1 = Oberer Grenzwert überschritten
 * 2 = Unterer Grenzwert unterschritten
 * 3 = 
 * 4 = Warngrenze OK
 * 5 = Obere Warngrenze überschritten
 * 6 = Untere Warngrenze unterschritten
 * 7 = 
 */
void mail_meldung (struct einzelmeldung_info *em_info, unsigned char akt_status);
#endif
