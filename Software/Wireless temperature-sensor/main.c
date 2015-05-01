#include "main.h"
#include "1-Wire.h"
#include "rfm12.h"
#include "adc.h"
#include "interrupt.h"
#include "avr/sleep.h"

struct onewire_temp_sensor temp_sensor_1;

int main(void) {
    /*
     * Warten bis alle externen Module hochgefahren sind */
    _delay_ms(10);

    /*
     * AD-Wandler initialisieren */
    adc_init();

    /*
     * ROM-Code des 1Wire-Sensors auslesen */
    onewire_read_rom(temp_sensor_1.rom);

    /*
     * Funkmodul initialisieren */
    rfm12_init();

    /*
     * rfm_wake_up setzen, um erste Messung direkt beim Starten anzustossen */
    rfm_wake_up = 1;
    sei();

    /*
     * Hauptprogramm */
    while (1) {
        if (rfm_wake_up) {
            rfm_wake_up = 0;

            /*
             * Temperaturmessung starten */
	        onewire_start_temp();

	        /*
	         * Bateriespannung messen und senden */
	        adc_ein();
	        uint16_t u_batt;
	        u_batt = u_bat_mess();

	        struct rfm_message tmp_rfm_message;
	        tmp_rfm_message.typ = Messwert_16;
	        tmp_rfm_message.id = Funksensor_Aussen_Spannung;
	        tmp_rfm_message.rtr = 0;
	        tmp_rfm_message.length = Laenge_Messwert_16;
	        tmp_rfm_message.data[0] = (uint8_t)(u_batt>>8);
	        tmp_rfm_message.data[1] = u_batt;
	        rfm12_send_message(&tmp_rfm_message);
            
	        /*
	         * 750ms warten bis die Temperaturmessung abgeschlossen ist */
            _delay_ms(750);
	        
            /*
             * Temperatur auslesen und bei korrekter Messung senden */
            if (onewire_read_temp(&temp_sensor_1)) {
                struct rfm_message tmp_rfm_message;

                tmp_rfm_message.typ = Messwert_16;
                tmp_rfm_message.id = Funksensor_Aussen_Temperatur;
                tmp_rfm_message.rtr = 0;
                tmp_rfm_message.length = Laenge_Messwert_16;
                tmp_rfm_message.data[0] = (uint8_t)(temp_sensor_1.temperature>>8);
                tmp_rfm_message.data[1] = temp_sensor_1.temperature;
                rfm12_send_message(&tmp_rfm_message);
            }
        }

        /*
         * Anschlissend wieder schlafen gehen */
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
    }
}
