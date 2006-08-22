/*
 * adc.c
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
#include "adc.h"


volatile uint16_t adc_value[ADC_NUMBER];
volatile uint16_t adc_value_tmp;
volatile uint8_t adc_tmp_n;

void set_adc (void){
	adc_value_tmp = 0;
	adc_tmp_n = 0;
	ADC_DDR &= ~(1<<ADC_K1 | 1<<ADC_K2 | 1<<ADC_K3);
	ADC_DDR |= (1<<3 | 1<<4);
	ADC_PORT |= (1<<3 | 1<<4);
/* enable adc and set prescaler */
	ADCSRA = (1<<ADEN | 1<<ADPS2 | 1<<ADPS1);	// prescaler = 64
/* Select ADC Auto Trigger Source */
//	SFIOR &= ~(1<<ADTS2 | 1<<ADTS1 | 1<<ADTS0);
/* Enable Free Running mode and/or_only Interrupt*/
//	ADCSRA |= (1<<ADATE | 1<<ADIE);
	ADCSRA |= (1<<ADIE);
/* select channel and set Internal 2.56V Reference */
	ADMUX = 5;
	ADMUX |= (1<<REFS1 | 1<<REFS0);
/* start conversion */
	ADCSRA |= (1<<ADSC);
	return;
}

SIGNAL (SIG_ADC){
	adc_value_tmp += ADC;
	adc_tmp_n ++;
	if( adc_tmp_n == 4 ){
		adc_value_tmp /= 4;
		adc_tmp_n = ADMUX;	
		adc_tmp_n &= (1<<MUX4 | 1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
		if( adc_tmp_n == 7 ) { 
			adc_value[0] = adc_value_tmp;
			ADMUX = 6;
			goto _exit_sig_adc;
		}
		if( adc_tmp_n == 6 ) { 
			adc_value[1] = adc_value_tmp;
			ADMUX = 5;
			goto _exit_sig_adc;
		}
		if( adc_tmp_n == 5 ) { 
			adc_value[2] = adc_value_tmp;
			ADMUX = 7;
			goto _exit_sig_adc;
		}
_exit_sig_adc:
		ADMUX |= (1<<REFS1 | 1<<REFS0);
		adc_tmp_n = 0;
		adc_value_tmp = 0;

	}
	ADCSRA |= (1<<ADSC);
	return;
}
