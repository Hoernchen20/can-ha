#ifndef can_typ_id
#define can_typ_id

//Nachrichten Typen
#define Messwert_16     2030    //0b0111 1110 1110, 0x7EE
#define MW_16           2030    //0b0111 1110 1110, 0x7EE
#define Messwert_32     2020    //0b0111 1110 0100, 0x7E4
#define MW_32           2020    //0b0111 1110 0100, 0x7E4
#define Sollwert_16     2010    //0b0111 1101 1010, 0x7DA
#define SW_16           2010    //0b0111 1101 1010, 0x7DA
#define Sollwert_32     2000    //0b0111 1101 0000, 0x7D0
#define SW_32           2000    //0b0111 1101 0000, 0x7D0
#define Einzelmeldung   1990    //0b0111 1100 0110, 0x7C6
#define EM              1990    //0b0111 1100 0110, 0x7C6
#define Doppelmeldung   1980    //0b0111 1011 1100, 0x7BC
#define DM              1980    //0b0111 1011 1100, 0x7BC
#define Einzelbefehl    1970    //0b0111 1011 0010, 0x7B2
#define EB              1970    //0b0111 1011 0010, 0x7B2
#define Doppelbefehl    1960    //0b0111 1010 1000, 0x7A8
#define DB              1960    //0b0111 1010 1000, 0x7A8
#define Zeit            1950    //0b0111 1001 1110, 0x79E

/*
 * Länge der Nachrichten */
#define Laenge_Messwert_16      2
#define Laenge_MW_16            2
#define Laenge_Sollwert_16      2
#define Laenge_SW_16            2
#define Laenge_Einzelmeldung    1
#define Laenge_EM               1
#define Laenge_Doppelmeldung    1
#define Laenge_DM               1
#define Laenge_Zeit             7

/*
 * Nachrichten ID Messwert_16
 * 
 * Messwert = Wert/100 (z.B 21,56°C = 2156/100)
 * Datenbyte[0] = High-Byte
 * Datenbate[1] = Low-Byte */
#define Testboard_Temperatur            262142  //0b0011 1111 1111 1111 1110, 0x3 FFFE
#define Funksensor_Aussen_Temperatur    262141  //0b0011 1111 1111 1111 1101, 0x3 FFFD
#define Funksensor_Aussen_Spannung      262140  //0b0011 1111 1111 1111 1100, 0x3 FFFC
#define Schlafzimmer_Solltemperatur_RM  262139  //0b0011 1111 1111 1111 1011, 0x3 FFFB

/*
 * Nachrichten ID Sollwert_16
 * 
 * Sollwert = Wert/100 (z.B 21,56°C = 2156/100)
 * Datenbyte[0] = High-Byte
 * Datenbate[1] = Low-Byte */
#define Schlafzimmer_Solltemperatur     262142  //0b0011 1111 1111 1111 1110, 0x3 FFFE

/*
 * Nachrichten ID Einzelmeldung
 * 
 * Datenbyte[0].bit0 = Einzelmeldung (0 oder 1) */
#define Testboard_Funkmodul_CRC_Error   262142  //0b0011 1111 1111 1111 1110, 0x3 FFFE
#define Testboard_Onewire_Bus_Status    262141  //0b0011 1111 1111 1111 1101, 0x3 FFFD
#define Funkkoppler_CRC_Error           262140  //0b0011 1111 1111 1111 1100, 0x3 FFFC
#define Schlafzimmer_Heizung            262139  //0b0011 1111 1111 1111 1011, 0x3 FFFB

/*
 * Nachrichten ID Doppelmeldung
 * 
 * Datenbyte[0].bit0-1 = Doppelmeldung (00 oder 01 oder 10 oder 11) */


/*
 * Nachrichten ID Zeit
 * 
 * Datenbyte[0] = Jahr (Kalenderjahr minus 2000)
 * Datenbyte[1] = Monat (1 bis 12)
 * Datenbyte[2] = Tag (1 bis 31)
 * Datenbyte[3] = Wochentag (0 bis 6, Sonntag = 0) 
 * Datenbyte[4] = Stunde (0 bis 23)
 * Datenbyte[5] = Minute (0 bis 59)
 * Datenbyte[6] = Sekunde (0 bis 59) */
#define Serverzeit                      262142  //0b0011 1111 1111 1111 1110, 0x3 FFFE

#endif
