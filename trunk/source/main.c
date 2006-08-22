/*
 * main.c
 * Programm für Biotop µControl Project.
 * Copyright (C) 2006 Johann Fischer (terentij@gmx.net)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lcd_print.h"
#include "dsi.h"
#include "adc.h"

#define KEYS_PIN 	PINC
#define KEYS_PORT 	PORTC
#define KEYS_DDR 	DDRC
#define KEY_1	 	(1<<7)
#define KEY_2	 	(1<<6)
#define KEY_3	 	(1<<5)

#define KEY_ENTER 	(1<<0)
#define KEY_PLUS 	(1<<1)
#define KEY_MINUS 	(1<<2)

#define OUT_PIN_PWM_DAC1 	4
#define OUT_PIN_LCDBVDD 	5
volatile uint8_t user_keys;
void init_keys(void);
void keys_polling(void);
				     /* 0123456789ABCDEF0123456789ABCDEF */
const int8_t menu32_t[][33] PROGMEM = {"< Eintellungen ><     Exit    >>",
				       "< Eintellungen ><<    Zeit     >",
				       "< Eintellungen >< Lampen Relay >",
				       "< Eintellungen ><   Relay 1    >",
				       "< Eintellungen ><   Relay 2    >",
				       "< Eintellungen ><Dimm Prescaler>",
				       "< Eintellungen ><Dimm Max. Wert>",
				       "< Eintellungen ><Dimm Min. Wert>",
				       "< Eintellungen ><    Manuel    >",
				       "< Eintellungen ><  Speichern   >",
				       "< Eintellungen ><LCD Helligkeit>",
				       "< Eintellungen ><     Test     >"};

const int8_t m_info1_32_t[33] PROGMEM = {"xx:xx:xx BL:xxx%Licht:xxx Mode:x"};
const int8_t m_info2_32_t[33] PROGMEM = {"L:M0 R1:M0 R2:M0A:xxx M:xxx S:x "};
const int8_t m_manuel32_t[33] PROGMEM = {"Dimm Manuel +/- Wert: xxx = xxx%"};
const int8_t m_intens32_t[33] PROGMEM = {" LCD Helligkeit  +/-  xxx%      "};
const int8_t m_prescl32_t[33] PROGMEM = {" Prescaler  xxx  Zeit: xx:xx:xx "};
const int8_t m_maxval32_t[33] PROGMEM = {"Max.: xxx = xxx% Zeit: xx:xx:xx "};
const int8_t m_minval32_t[33] PROGMEM = {"Min.: xxx = xxx% Zeit: xx:xx:xx "};
const int8_t m_zeit32_t[33] PROGMEM = {"-   xx:xx:xx   +->            ->"};
const int8_t m_exit16_t[17] PROGMEM = {"+/-   Exit   +/-"};
const int8_t m_re1_16_t[17] PROGMEM = {"  Manuel  | Aus "};
const int8_t m_re2_16_t[17] PROGMEM = {"  Manuel  | Ein "};
const int8_t m_re3_16_t[17] PROGMEM = {" Programm | Aus "};
const int8_t m_re4_16_t[17] PROGMEM = {" Programm | Ein "};

const uint8_t dimm_table[256] PROGMEM = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
					2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 
					3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 
					4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
					5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
					6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 
					7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 
					8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 
					10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 
					12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 
					14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 
					17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 
					21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 
					25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 
					30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 
					36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 
					44, 45, 45, 46, 47, 48, 49, 50, 51, 52, 
					53, 53, 54, 55, 57, 58, 59, 60, 61, 62, 
					63, 64, 65, 67, 68, 69, 70, 72, 73, 74, 
					76, 77, 79, 80, 81, 83, 84, 86, 88, 89, 
					91, 93, 94, 96, 98, 100}; 

struct date_struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
};
 
#define R_STATUS_ON		1		//Relays On
#define R_STATUS_AUT		2		//Relays Automatic
#define DIMM_STATUS_UP		3
#define DIMM_STATUS_DOWN	4
struct relay_timer {
	struct date_struct begin;
	struct date_struct end;
	uint8_t status;
};

struct dimm_control {
	struct relay_timer relay;
	uint8_t dimm_value;
	uint8_t dimm_manuel;
	uint8_t prescl;
	uint8_t max;
	uint8_t min;
	uint16_t prescl_time;
};

#define MAGIC_BYTE 0xA0
volatile uint8_t main_time_ms;
volatile uint8_t ocr1al_value;			//LCD Intens.
volatile uint8_t ocr1bl_value;			//DAC
uint8_t menu_entry;
extern volatile uint8_t dsi_data;

#define TIMER0_VALUE		256 - 96 	// 1 / (9.8304MHz / 1024 / 96) = 10 ms
void init_timer0(void);
void init_timer1(void);

#define MENU_MAX 	11
#define MENU_MIN 	0
int menu_info( struct date_struct *date, struct dimm_control *dimm_control, 
		struct relay_timer *relay1, struct relay_timer *relay2);
int set_time_struct(struct date_struct *date);
int set_time_rcontrol( struct relay_timer *relay);
int menu_manuel(uint8_t *dimm_manuel);
int menu_lcd_intens(volatile uint8_t *intens_value);
int menu_prescl(struct dimm_control *dimm_control);
int menu_dimm_setmax(struct dimm_control *dimm_control);
int menu_dimm_setmin(struct dimm_control *dimm_control);

int menu_test_adc(void);
extern volatile uint16_t adc_value[ADC_NUMBER];

#define LCD_BUFFER_SIZE	32
char lcd_buffer[LCD_BUFFER_SIZE] = {"Load data from  EEPROM v.0.2 ..."};
void clear_lcd_buffer(void);
void int8t_tolcd_buffer( uint8_t position, uint8_t value );
void calc_value_to_dstruct(struct date_struct *date, uint16_t *value);
void calc_dstruct_to_value(struct date_struct *date, uint16_t *value);
#define T_STRUCT_HIGHER		2
#define T_STRUCT_LOWER		1
#define T_STRUCT_EQUAL		0
int compare_struct( struct date_struct *date1, struct date_struct *date2 );


/*
 * Display Infos: time, relays status, dimm. status...
 */
int menu_info( struct date_struct *date, struct dimm_control *dimm_control, 
		struct relay_timer *relay_1, struct relay_timer *relay_2){
	uint8_t i;
	switch ( user_keys ){
		case 1:
			return 1;
		case 2:
			menu_entry ++;
			if( menu_entry > 3 ) menu_entry = 1;
			break;
		case 4:
			break;
		default:
			break;
		}
	switch ( menu_entry ){
		case 1:
			for(i = 0; i < LCD_BUFFER_SIZE; i ++){
				lcd_buffer[i] = pgm_read_byte(&m_info1_32_t[i]);
			}
			sprintf(lcd_buffer, "%02d:%02d:%02d", 
				date->hours, date->minutes, date->seconds);
			lcd_buffer[8] = ' ';
			if( dimm_control->relay.status & (1<<R_STATUS_AUT) ) 
				int8t_tolcd_buffer( 12, 
					pgm_read_byte(&dimm_table[dimm_control->dimm_value]));
			else int8t_tolcd_buffer( 12, pgm_read_byte(&dimm_table[dimm_control->dimm_manuel]));
			if( dimm_control->relay.status & (1<<R_STATUS_AUT) ) 
				lcd_buffer[31] = 'A';
			else lcd_buffer[31] = 'M';
			if( dimm_control->relay.status & (1<<R_STATUS_ON) ){
				lcd_buffer[22] = 'E';
				lcd_buffer[23] = 'i';
				lcd_buffer[24] = 'n';
			}
			else {
				lcd_buffer[22] = 'A';
				lcd_buffer[23] = 'u';
				lcd_buffer[24] = 's';
			}
			break;
		case 2:
			for(i = 0; i < LCD_BUFFER_SIZE; i ++){
				lcd_buffer[i] = pgm_read_byte(&m_info2_32_t[i]);
			}
			if( dimm_control->relay.status & (1<<R_STATUS_AUT) ) lcd_buffer[2] = 'A';
			if( dimm_control->relay.status & (1<<R_STATUS_ON) ) lcd_buffer[3] = '1';
			if( relay_1->status & (1<<R_STATUS_AUT) ) lcd_buffer[8] = 'A';
			if( relay_1->status & (1<<R_STATUS_ON) ) lcd_buffer[9] = '1';
			if( relay_2->status & (1<<R_STATUS_AUT) ) lcd_buffer[14] = 'A';
			if( relay_2->status & (1<<R_STATUS_ON) ) lcd_buffer[15] = '1';
			int8t_tolcd_buffer( 18, dimm_control->dimm_value);
			int8t_tolcd_buffer( 24,	dimm_control->dimm_manuel);
			if( dimm_control->relay.status & (1<<DIMM_STATUS_UP) ) lcd_buffer[30] = '+';
			if( dimm_control->relay.status & (1<<DIMM_STATUS_DOWN) ) lcd_buffer[30] = '-';
			break;
		case 3:
			sprintf(lcd_buffer, "ch7=%02d, ch6=%02d, ch5=%02d", 
					adc_value[0], adc_value[1], adc_value[2]);
			break;
	}
	return 0;
}

void calc_dstruct_to_value(struct date_struct *date, uint16_t *value){
	(*value) = (uint16_t)(date->hours * (uint32_t)3600);
	(*value) += (uint16_t)(date->minutes * 60);
	(*value) += (uint16_t)(date->seconds);
	return;
}

void calc_value_to_dstruct(struct date_struct *date, uint16_t *value){
	date->seconds = (*value) % 60;
	date->minutes = ((*value) / 60) % 60;
	date->hours = ((*value) / (uint16_t)3600) % 24;
	return;
}

int compare_struct( struct date_struct *date1, struct date_struct *date2 ){
	if( date1->hours > date2->hours ) return 2;
	if( date1->hours < date2->hours ) return 1;
	if( date1->hours == date2->hours ){
		if( date1->minutes > date2->minutes) return 2;
		if( date1->minutes < date2->minutes) return 1;
		if( date1->minutes == date2->minutes) {
			if( date1->seconds > date2->seconds) return 2;
			if( date1->seconds < date2->seconds) return 1;
			if( date1->seconds == date2->seconds) return 0;
		}
	}
	return 4;
}

/*
 * set time
 */
#define CURSOR_POS	4
int set_time_struct( struct date_struct *date ){
	uint8_t i;
	static uint8_t cursor_pos;

	for(i = 0; i < LCD_BUFFER_SIZE; i ++){
		lcd_buffer[i] = pgm_read_byte(&m_zeit32_t[i]);
	}
	sprintf(lcd_buffer + CURSOR_POS, "%02d:%02d:%02d", date->hours, date->minutes, date->seconds);
	lcd_buffer[CURSOR_POS + 8] = ' ';

	if( cursor_pos == 0 ) lcd_buffer[CURSOR_POS + 16] = '^';
	if( cursor_pos == 1 ) lcd_buffer[CURSOR_POS + 19] = '^';
	if( cursor_pos == 2 ) lcd_buffer[CURSOR_POS + 22] = '^';
	if( cursor_pos == 3 ) {
		for(i = 0; i < LCD_BUFFER_SIZE / 2; i ++){
			lcd_buffer[i + 16] = pgm_read_byte(&m_exit16_t[i]);
		}
	}
	if( user_keys == KEY_ENTER ) {
		cursor_pos ++;
		if(cursor_pos > 3) cursor_pos = 0;
	}
	if( ((user_keys == KEY_PLUS) || (user_keys == KEY_MINUS)) && 
				(cursor_pos == 3)) {
			cursor_pos = 0;
			return 1;
	}
	if( user_keys == KEY_PLUS ) {
		if( cursor_pos == 0)	{
			date->hours ++;
			if(date->hours > 23) date->hours = 0;
		}
		if( cursor_pos == 1)	{
			date->minutes ++;
			if(date->minutes > 59) date->minutes = 0;
		}
		if( cursor_pos == 2)	{
			date->seconds ++;
			if(date->seconds > 59) date->seconds = 0;
		}
	}
	if( user_keys == KEY_MINUS ) {
		if( cursor_pos == 0)	{
			date->hours --;
			if(date->hours > 23) date->hours = 23;
		}
		if( cursor_pos == 1)	{
			date->minutes --;
			if(date->minutes > 59) date->minutes = 59;
		}
		if( cursor_pos == 2)	{
			date->seconds --;
			if(date->seconds > 59) date->seconds = 59;
		}
	}
	return 0;
}

/*
 * setup relays start and end times, on/off, automatik/manual
 */
#define BLINKING	0x04 
int set_time_rcontrol( struct relay_timer *relay){
	uint8_t i;
	static uint8_t cursor_pos;
	static uint8_t blinking;

	sprintf(lcd_buffer, "B-%02d:%02d E-%02d:%02d", relay->begin.hours, relay->begin.minutes,
			relay->end.hours, relay->end.minutes);
	
	lcd_buffer[15] = ' ';
	if( (relay->status & (1<<R_STATUS_ON | 1<<R_STATUS_AUT)) == 0){
		for(i = 0; i < LCD_BUFFER_SIZE / 2; i ++){
			lcd_buffer[i + 16] = pgm_read_byte(&m_re1_16_t[i]);
		}
	}
	if( (relay->status & (1<<R_STATUS_ON | 1<<R_STATUS_AUT )) == (1<<R_STATUS_ON)){
		for(i = 0; i < LCD_BUFFER_SIZE / 2; i ++){
			lcd_buffer[i + 16] = pgm_read_byte(&m_re2_16_t[i]);
		}
	}
	if( (relay->status & (1<<R_STATUS_ON | 1<<R_STATUS_AUT )) == (1<<R_STATUS_AUT)){
		for(i = 0; i < LCD_BUFFER_SIZE / 2; i ++){
			lcd_buffer[i + 16] = pgm_read_byte(&m_re3_16_t[i]);
		}
	}
	if( (relay->status & (1<<R_STATUS_ON | 1<<R_STATUS_AUT )) == (1<<R_STATUS_ON | 1<<R_STATUS_AUT)){
		for(i = 0; i < LCD_BUFFER_SIZE / 2; i ++){
			lcd_buffer[i + 16] = pgm_read_byte(&m_re4_16_t[i]);
		}
	}

	if( user_keys == KEY_ENTER ) {
		cursor_pos ++;
		if(cursor_pos > 6) cursor_pos = 0;
	}
	if( cursor_pos == 4 ) { 
		lcd_buffer[16] = '>'; lcd_buffer[16 + 9] = '<'; 
		if( (user_keys == KEY_PLUS) || (user_keys == KEY_MINUS) ) {
			if( relay->status & (1<<R_STATUS_AUT))
				relay->status &= ~(1<<R_STATUS_AUT);
			else relay->status |= (1<<R_STATUS_AUT);
		}
	}
	if( cursor_pos == 5 ) { 
		lcd_buffer[16 + 11] = '>'; lcd_buffer[16 + 15] = '<'; 
		if( (user_keys == KEY_PLUS) || (user_keys == KEY_MINUS) ) {
			if( relay->status & (1<<R_STATUS_ON))
				relay->status &= ~(1<<R_STATUS_ON);
			else relay->status |= (1<<R_STATUS_ON);
		}
	}
	if( cursor_pos == 0)	{
		if( blinking & (BLINKING) ){
			lcd_buffer[2] = ' '; lcd_buffer[3] = ' ';
		}
		if( user_keys == KEY_PLUS ) {
			relay->begin.hours ++;
			if(relay->begin.hours > 23) relay->begin.hours = 0;
		}
		if( user_keys == KEY_MINUS ) {
			relay->begin.hours --;
			if(relay->begin.hours > 23) relay->begin.hours = 23;
		}
	}
	if( cursor_pos == 1)	{
		if( blinking & (BLINKING) ){
			lcd_buffer[5] = ' '; lcd_buffer[6] = ' ';
		}
		if( user_keys == KEY_PLUS ) {
			relay->begin.minutes ++;
			if(relay->begin.minutes > 59) relay->begin.minutes = 0;
		}
		if( user_keys == KEY_MINUS ) {
			relay->begin.minutes --;
			if(relay->begin.minutes > 59) relay->begin.minutes = 59;
		}
	}
	if( cursor_pos == 2)	{
		if( blinking & (BLINKING) ){
			lcd_buffer[10] = ' '; lcd_buffer[11] = ' ';
		}
		if( user_keys == KEY_PLUS ) {
			relay->end.hours ++;
			if(relay->end.hours > 23) relay->end.hours = 0;
		}
		if( user_keys == KEY_MINUS ) {
			relay->end.hours --;
			if(relay->end.hours > 23) relay->end.hours = 23;
		}
	}
	if( cursor_pos == 3)	{
		if( blinking & (BLINKING) ){
			lcd_buffer[13] = ' '; lcd_buffer[14] = ' ';
		}
		if( user_keys == KEY_PLUS ) {
			relay->end.minutes ++;
			if(relay->end.minutes > 59) relay->end.minutes = 0;
		}
		if( user_keys == KEY_MINUS ) {
			relay->end.minutes --;
			if(relay->end.minutes > 59) relay->end.minutes = 59;
		}
	}
	if( cursor_pos == 6 ) {
		for(i = 0; i < LCD_BUFFER_SIZE / 2; i ++){
			lcd_buffer[i + 16] = pgm_read_byte(&m_exit16_t[i]);
		}
		if( (user_keys == KEY_PLUS) || (user_keys == KEY_MINUS) ) {
			cursor_pos = 0;
			return  1;
		}
	}
	blinking ++;
	return 0;
}
 
/*
 * adjust intens. manual
 */
int menu_manuel(uint8_t *dimm_manuel){
	uint8_t i;
	uint8_t tmp;
	if( user_keys == KEY_ENTER ) return 1;
	if( user_keys == KEY_PLUS ) (*dimm_manuel) ++;
	if( user_keys == KEY_MINUS ) (*dimm_manuel) --;

	for(i = 0; i < LCD_BUFFER_SIZE; i ++){
		lcd_buffer[i] = pgm_read_byte(&m_manuel32_t[i]);
	}
	int8t_tolcd_buffer( 22, *dimm_manuel );
	tmp = pgm_read_byte(&dimm_table[*dimm_manuel]);
	int8t_tolcd_buffer( 28, tmp );
	return 0;
}

/*
 * adjust lcd intens.
 */
int menu_lcd_intens(volatile uint8_t *intens_value){
	uint8_t i;
	if( user_keys == KEY_ENTER ) return 1;
	if( user_keys == KEY_PLUS ){
		(*intens_value) += 10; 
		if( *intens_value > 200 ) *intens_value = 0;
	}
	if( user_keys == KEY_MINUS ){
		(*intens_value) -= 10;
		if( *intens_value > 200 ) *intens_value = 200;
	}
	for(i = 0; i < LCD_BUFFER_SIZE; i ++){
		lcd_buffer[i] = pgm_read_byte(&m_intens32_t[i]);
	}
	int8t_tolcd_buffer( 22, (*intens_value) / 2 );
	return 0;
}
 
/*
 * setup prescaler
 */
int menu_prescl(struct dimm_control *dimm_control){
	uint8_t i;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	if( user_keys == KEY_ENTER ) return 1;
	if( user_keys == KEY_PLUS ) (dimm_control->prescl) ++;
	if( user_keys == KEY_MINUS ) (dimm_control->prescl) --;
	for(i = 0; i < LCD_BUFFER_SIZE; i ++){
		lcd_buffer[i] = pgm_read_byte(&m_prescl32_t[i]);
	}
	int8t_tolcd_buffer( 12, dimm_control->prescl );
	dimm_control->prescl_time = (uint16_t)((dimm_control->prescl) *
			(uint8_t)(dimm_control->max - dimm_control->min));
	dimm_control->prescl_time /= 10;
	seconds = dimm_control->prescl_time % 60;
	minutes = ( dimm_control->prescl_time / 60 ) % 60;
	hours = ( dimm_control->prescl_time / 3600 ) % 24;
	sprintf(lcd_buffer + 23, "%02d:%02d:%02d", hours, minutes, seconds);
	lcd_buffer[31] = ' ';
	return 0;
}

/*
 * setup maximal value
 */
int menu_dimm_setmax(struct dimm_control *dimm_control){
	uint8_t i;
	uint8_t tmp;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	if( user_keys == KEY_ENTER ) return 1;
	if( user_keys == KEY_PLUS ) {
		(dimm_control->max) ++;
		if(dimm_control->max <= dimm_control->min)
			dimm_control->max = dimm_control->min + 1;
	}
	if( user_keys == KEY_MINUS ) {
		(dimm_control->max) --;
		if(dimm_control->max <= dimm_control->min)
			dimm_control->max = 255;
	}
	for(i = 0; i < LCD_BUFFER_SIZE; i ++){
		lcd_buffer[i] = pgm_read_byte(&m_maxval32_t[i]);
	}
	int8t_tolcd_buffer( 6, dimm_control->max );
	tmp = pgm_read_byte(&dimm_table[dimm_control->max]);
	int8t_tolcd_buffer( 12, tmp );
	dimm_control->prescl_time = (uint16_t)((dimm_control->prescl) *
			(uint8_t)(dimm_control->max - dimm_control->min));
	dimm_control->prescl_time /= 10;
	seconds = dimm_control->prescl_time % 60;
	minutes = ( dimm_control->prescl_time / 60 ) % 60;
	hours = ( dimm_control->prescl_time / 3600 ) % 24;
	sprintf(lcd_buffer + 23, "%02d:%02d:%02d", hours, minutes, seconds);
	lcd_buffer[31] = ' ';
	return 0;
}

/*
 * setup minimal value
 */
int menu_dimm_setmin(struct dimm_control *dimm_control){
	uint8_t i;
	uint8_t tmp;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	if( user_keys == KEY_ENTER ) return 1;
	if( user_keys == KEY_PLUS ) {
		(dimm_control->min) ++;
		if(dimm_control->min >= dimm_control->max)
			dimm_control->min = 0;
	}
	if( user_keys == KEY_MINUS ) {
		(dimm_control->min) --;
		if(dimm_control->min >= dimm_control->max)
			dimm_control->min = dimm_control->max - 1;
	}
	for(i = 0; i < LCD_BUFFER_SIZE; i ++){
		lcd_buffer[i] = pgm_read_byte(&m_minval32_t[i]);
	}
	int8t_tolcd_buffer( 6, dimm_control->min );
	tmp = pgm_read_byte(&dimm_table[dimm_control->min]);
	int8t_tolcd_buffer( 12, tmp );
	dimm_control->prescl_time = (uint16_t)((dimm_control->prescl) *
			(uint8_t)(dimm_control->max - dimm_control->min));
	dimm_control->prescl_time /= 10;
	seconds = dimm_control->prescl_time % 60;
	minutes = ( dimm_control->prescl_time / 60 ) % 60;
	hours = ( dimm_control->prescl_time / 3600 ) % 24;
	sprintf(lcd_buffer + 23, "%02d:%02d:%02d", hours, minutes, seconds);
	lcd_buffer[31] = ' ';
	return 0;
}

int menu_test_adc(void){
	if( user_keys ) return 1;
	sprintf(lcd_buffer, "ch7=%02d, ch6=%02d, ch5=%02d", 
			adc_value[0], adc_value[1], adc_value[2]);
	return 0;
}

/* 
 * put a int8_t value to display buffer
 */
void int8t_tolcd_buffer( uint8_t position, uint8_t value ){
	uint8_t tmp1;
	uint8_t tmp2;
	uint8_t tmp3;
	tmp1 = value % 10;
	value = value / 10;
	tmp2 = value % 10;
	value = value / 10;
	tmp3 = value % 10;
	if( tmp3 ) lcd_buffer[position] = tmp3 + 0x30;
	else lcd_buffer[position] = ' ';
	if( tmp2  || tmp3) lcd_buffer[++position] = tmp2 + 0x30;
	else lcd_buffer[++position] = ' ';
	lcd_buffer[++position] = tmp1 + 0x30;
	return;
}

/*
void clear_lcd_buffer(void){
	uint8_t i = 0;
	while( i < LCD_BUFFER_SIZE ){
		lcd_buffer[i] = ' ';
		i ++;
	}
	return;
}
*/

/* set timer0, time clock generator */
void init_timer0(void){
	TCCR0 |= (1<<CS02 | 1<<CS00);			// prescaler = 1024
	TCNT0 = TIMER0_VALUE;				// set timer value
	TIMSK |= (1<<TOIE0);				// set timer overflow interrupt
	return;
}

SIGNAL (SIG_OVERFLOW0){
	TCNT0 = TIMER0_VALUE;				// set timer value now
	main_time_ms ++;
}

/* set timer1, LCD PWM(oc1a), DAC PWM(oc1b) */
void init_timer1(void){
	DDRD |= (1<<OUT_PIN_PWM_DAC1 
			| 1<<OUT_PIN_LCDBVDD);
	TCCR1A = (1<<COM1A1 | 1<<COM1B1 | 1<<WGM10);	// fast PWM 8 bit top 255
	TCCR1B = (1<<CS11 | 1<<CS10 | 1<<WGM12);	// prescaler = 256/150HZ, fast PWM 8 bit top 255
	OCR1AH = 0;
	OCR1AL = ocr1al_value;
	OCR1BH = 0;
	OCR1BL = ocr1bl_value;
	TIMSK |= (1<<TOIE1);
	return;
}

SIGNAL ( SIG_OVERFLOW1 ){
	OCR1AL = ocr1al_value;
	OCR1BL = ocr1bl_value;
	return;
}

void init_keys(void){
	/* set PINS direction */
	KEYS_DDR &= ~KEY_1;
	KEYS_DDR &= ~KEY_2;
	KEYS_DDR &= ~KEY_3;
	/* set pullup resistors */
	KEYS_PORT |= KEY_1;
	KEYS_PORT |= KEY_2;
	KEYS_PORT |= KEY_3;
	user_keys = 0;
	return;
}

void keys_polling(void){
	uint8_t n, i;
	static uint8_t kp_tmp1, kp_tmp2, kp_tmp3;
	kp_tmp1 = KEYS_PIN;
	kp_tmp1 &= (KEY_1 | KEY_2 | KEY_3);
	n = (kp_tmp1 & kp_tmp2 & kp_tmp3);
	i = (kp_tmp1 | kp_tmp2 | kp_tmp3);
	if ( n == i ) {
		if( !(n & KEY_1) ) user_keys |= KEY_ENTER;
		if( !(n & KEY_2) ) user_keys |= KEY_PLUS;
		if( !(n & KEY_3) ) user_keys |= KEY_MINUS;
		kp_tmp2 = 0;	// kake //
		kp_tmp1 = 0;
	}
	kp_tmp3 = kp_tmp2;
	kp_tmp2 = kp_tmp1;
	return;
}

int main(void){
	int8_t i;
	uint8_t tmp_t_control = 0;
	uint8_t tmp_t_date = 0;
	uint8_t time_10ms = 0;
	uint8_t tmp_prescl = 0;
	uint8_t tmp_prescl2 = 0;
	uint8_t tmp = 0;
	uint8_t menu_position = 0;
	menu_entry = 1;
	uint16_t tmp_16t = 0;
	uint16_t tmp2_16t = 0;
	uint16_t eeprom_ptr = 0;
	static struct date_struct date;
	static struct relay_timer relay_1;
	static struct relay_timer relay_2;
	static struct dimm_control dimm_control;
	dimm_control.max = 0xFF;
	ocr1al_value = 100;			// lcd intens.
	ocr1bl_value = 127;			// relay3/dac

	init_dsi();
	init_lcd();
	init_keys();
	init_timer0();
	init_timer1();
	set_adc();
	/*
	 * read configuration from EEPROM
	 */
	if( eeprom_read_byte((uint8_t *)eeprom_ptr) == MAGIC_BYTE ) {
		lcd_print( lcd_buffer, 32);
		eeprom_ptr ++;
		eeprom_read_block((void *)&dimm_control, (void *) eeprom_ptr, sizeof(dimm_control));
		eeprom_ptr += sizeof(dimm_control);
		eeprom_read_block((void *)&relay_1, (void *) eeprom_ptr, sizeof(relay_1));
		eeprom_ptr += sizeof(relay_1);
		eeprom_read_block((void *)&relay_2, (void *) eeprom_ptr, sizeof(relay_2));
		eeprom_ptr = 0;
		dimm_control.relay.status &= ~(1<<DIMM_STATUS_UP | 1<<DIMM_STATUS_DOWN | 1<<R_STATUS_ON);
		relay_1.status &= ~(1<<R_STATUS_ON);
		relay_2.status &= ~(1<<R_STATUS_ON);
	}
	sei();
	while(1){
		/* 
		 * send DSI signal or update timer value
		 */
		if( (uint8_t)((main_time_ms - tmp_prescl)) == 10 ){
			tmp_prescl2 ++;
			tmp_prescl = main_time_ms;
			if(tmp_prescl2 == dimm_control.prescl){
				tmp_prescl2 = 0;
				if( dimm_control.relay.status & (1<<R_STATUS_AUT) ){
					if( dimm_control.relay.status & (1<<DIMM_STATUS_UP) ){
						dimm_control.dimm_value ++;
						if(dimm_control.dimm_value == dimm_control.max)
							dimm_control.relay.status &= ~(1<<DIMM_STATUS_UP);
					}
					if( dimm_control.relay.status & (1<<DIMM_STATUS_DOWN) ){
						dimm_control.dimm_value --;
						if(dimm_control.dimm_value == dimm_control.min)
							dimm_control.relay.status &= ~(1<<DIMM_STATUS_DOWN);
					}
				}
			}
			if( dimm_control.relay.status & (1<<R_STATUS_AUT) ){
				dsi_data = dimm_control.dimm_value;
				ocr1bl_value = dimm_control.dimm_value;
			}
			else {
				dsi_data = dimm_control.dimm_manuel;
				ocr1bl_value = dimm_control.dimm_manuel;
			}
			dsi_send();
		}
		/* keys polling */
		if( (uint8_t)(main_time_ms -  time_10ms) == 6 ){
			time_10ms = main_time_ms;
			keys_polling();
		}
		/* 
		 * big clock machine, and relays controll
		 */
		if( (uint8_t)(main_time_ms - tmp_t_date) == 100 ){
			tmp_t_date = main_time_ms;
			date.seconds ++;
			if(date.seconds > 59) { 
				date.seconds = 0; 
				date.minutes ++;
			}
			if(date.minutes > 59) {
				date.minutes = 0;
				date.hours ++;
			}
			if(date.hours > 23) {
				date.hours = 0;
			}
			if( relay_1.status & (1<<R_STATUS_AUT) ){
				tmp = compare_struct( &date, &relay_1.begin );
				if( tmp == T_STRUCT_EQUAL ) {
					relay_1.status |= (1<<R_STATUS_ON);
				}
				tmp = compare_struct( &date, &relay_1.end );
				if( tmp == T_STRUCT_EQUAL ) {
					relay_1.status &= ~(1<<R_STATUS_ON);
				}
			}
			if( relay_2.status & (1<<R_STATUS_AUT) ){
				tmp = compare_struct( &date, &relay_2.begin );
				if( tmp == T_STRUCT_EQUAL ) {
					relay_2.status |= (1<<R_STATUS_ON);
				}
				tmp = compare_struct( &date, &relay_2.end );
				if( tmp == T_STRUCT_EQUAL ) {
					relay_2.status &= ~(1<<R_STATUS_ON);
				}
			}
			if( dimm_control.relay.status & (1<<R_STATUS_AUT) ){
				tmp = compare_struct( &date, &dimm_control.relay.begin );
				if( tmp == T_STRUCT_EQUAL ) {
					dimm_control.relay.status |= (1<<R_STATUS_ON);
					dimm_control.relay.status |= (1<<DIMM_STATUS_UP);
					dimm_control.relay.status &= ~(1<<DIMM_STATUS_DOWN);
				}
				tmp = compare_struct( &date, &dimm_control.relay.end );
				if( tmp == T_STRUCT_EQUAL ) {
					dimm_control.relay.status &= ~(1<<R_STATUS_ON);
				}
				calc_dstruct_to_value(&dimm_control.relay.end, &tmp_16t);
				tmp_16t = tmp_16t - dimm_control.prescl_time;
				calc_dstruct_to_value(&date, &tmp2_16t);
				if( tmp2_16t == tmp_16t ) {
					dimm_control.relay.status |= (1<<DIMM_STATUS_DOWN);
					dimm_control.relay.status &= ~(1<<DIMM_STATUS_UP);
				}
			}
			
		}
		/*
		 * display menu
		 */
		if( (uint8_t)(main_time_ms - tmp_t_control) == 10 ){
			tmp_t_control = main_time_ms;
			if( !menu_entry ) {
			switch ( user_keys ){
				case 1:
					menu_entry = 1;
					break;
				case 2:
					menu_position ++;
					if( menu_position > MENU_MAX ) menu_position = MENU_MIN;
					break;
				case 4:
					menu_position --;
					if( menu_position > MENU_MAX ) menu_position = MENU_MAX;
					break;
				default:
					break;
				}
				for(i = 0; i < LCD_BUFFER_SIZE; i ++){
					lcd_buffer[i] = pgm_read_byte(&menu32_t[menu_position][i]);
				}
			}
			else {
			switch ( menu_position ){
				case 0:
					if( menu_info( &date, &dimm_control, &relay_1, &relay_2) ){
						menu_entry = 0;
					}
					break;
				case 1:
					if( set_time_struct( &date ) ){
						menu_entry = 0;
					}
					break;
				case 2:
					if( set_time_rcontrol( &dimm_control.relay ) ){
						menu_entry = 0;
					}
					break;
				case 3:
					if( set_time_rcontrol( &relay_1 ) ){
						menu_entry = 0;
					}
					break;
				case 4:
					if( set_time_rcontrol( &relay_2 ) ){
						menu_entry = 0;
					}
					break;
				case 5:
					if( menu_prescl(&dimm_control) ){
						menu_entry = 0;
					}
					break;
				case 6:
					if( menu_dimm_setmax(&dimm_control) ){
						menu_entry = 0;
					}
					break;
				case 7:
					if( menu_dimm_setmin(&dimm_control) ){
						menu_entry = 0;
					}
					break;
				case 8:
					if( menu_manuel(&dimm_control.dimm_manuel) ){
						menu_entry = 0;
					}
					break;
				case 9:
					eeprom_write_byte((uint8_t *) eeprom_ptr, MAGIC_BYTE);
					eeprom_ptr ++;
					eeprom_write_block(&dimm_control, (void *) eeprom_ptr, sizeof(dimm_control));
					eeprom_ptr += sizeof(dimm_control);
					eeprom_write_block(&relay_1, (void *) eeprom_ptr, sizeof(relay_1));
					eeprom_ptr += sizeof(relay_1);
					eeprom_write_block(&relay_2, (void *) eeprom_ptr, sizeof(relay_2));
					eeprom_ptr = 0;
					menu_entry = 0;
					menu_position = 0;
					break;
				case 10:
					if( menu_lcd_intens(&ocr1al_value) ){
						menu_entry = 0;
					}
					break;
				case 11:
					if( menu_test_adc() ){
						menu_entry = 0;
					}
					break;
				default:
					menu_entry = 1;
					menu_position = 0;
					break;
			}
			}
			user_keys = 0;
			command_lcd( 0x01 );
			lcd_print( lcd_buffer, 32);
		}
	}
}

