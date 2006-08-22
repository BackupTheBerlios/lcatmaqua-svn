/*
 * dsi.c
 *
 * generate DSI signal for dimmable ballast
 *
 * Copyright (C) 2006 Johann Fischer (terentij@gmx.net)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "dsi.h"

volatile uint8_t dsi_takt_cntr;
volatile uint8_t dsi_data;

void init_dsi(void){
	DSI_DDR |= (1<<DSI_PIN);
	DSI_PORT &= ~(1<<DSI_PIN);
	return;
}

void dsi_send(void){
	TCCR2 |= (1<<CS22 | 1<<CS21);			// prescaler = 256
	TCNT2 = T_VALUE;				// set timer value
	TIMSK |= (1<<TOIE2);				// set timer overflow interrupt
	dsi_takt_cntr = 0;				// clear dsi_takt_cntr
	return;
}

SIGNAL (SIG_OVERFLOW2){
	/* set timer value now */
	TCNT2 = T_VALUE;
	if( dsi_takt_cntr < 1){
		/* send start bit */
		DSI_PORT |= (1<<DSI_PIN);
		goto exit_cnt_overflow0;
	}
	if( dsi_takt_cntr < 17){
		/* send data bit manchester codet */
		if( (dsi_takt_cntr & 0x01) ){
			if( (dsi_data & (1<<7)) ) {
				DSI_PORT |= (1<<DSI_PIN);
			}
			else DSI_PORT &= ~(1<<DSI_PIN);
			goto exit_cnt_overflow0;
		}
		if( !(dsi_takt_cntr & 0x01) ){
			if( (dsi_data & (1<<7)) ){
				DSI_PORT &= ~(1<<DSI_PIN);
			}
			else DSI_PORT |= (1<<DSI_PIN);
			dsi_data <<= 1;
			goto exit_cnt_overflow0;
		}
	}
	if( dsi_takt_cntr < 21){
		/* send 4 stop bits */
		DSI_PORT &= ~(1<<DSI_PIN);
		goto exit_cnt_overflow0;
	}
	/* end trans. clear timer interrupt */
	TIMSK &= ~(1<<TOIE2);
	/* set dsi pin low */
	DSI_PORT &= ~(1<<DSI_PIN);
exit_cnt_overflow0:
	dsi_takt_cntr ++;
	return;
}


