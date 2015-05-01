/*
 * interrupt.c
 *
 *  Created on: 28.01.2015
 *      Author: Felix Horn
 */

#include "interrupt.h"


ISR(INT0_vect) {
    rfm12_receive_message();
}
