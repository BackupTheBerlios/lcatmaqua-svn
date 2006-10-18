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

//#define SIGNAL_ANALOG 1; 

uint8_t eeprom_image[512] EEMEM = {
//		0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
/* 0000 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0010 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0020 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0030 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0040 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0050 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0060 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0070 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0080 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0090 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 00A0 CH1 */	0x00,0x12,0x24,0x36,0x48,0x5a,0x6c,0x7e,0x90,0xa2,0xb4,0xc6,0xd8,0xea,0xff,0xff,
/* 00B0 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 00C0 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 00D0 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 00E0 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 00F0 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 0100 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 0110 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 0120 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
/* 0130 */	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xea,0xd8,0xc6,
/* 0140 CH2 */	0xb4,0xa2,0x90,0x7e,0x6c,0x5a,0x48,0x36,0x24,0x12,0x00,0xff,0xf1,0x00,0x00,0x00,
/* 0150 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0160 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0170 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0180 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 0190 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 01A0 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 01B0 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 01C0 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 01D0 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 01E0 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 01F0 */	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

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
				       "< Eintellungen ><   Relay 1    >",
				       "< Eintellungen ><   Relay 2    >",
				       "< Eintellungen ><   Relay 3    >",
				       "< Eintellungen ><Edit Prog. Kn1>",
				       "< Eintellungen >< Steuerung Kn1>",
				       "< Eintellungen ><Edit Prog. Kn2>",
				       "< Eintellungen >< Steuerung Kn2>",
				       "< Eintellungen ><  Speichern   >",
				       "< Eintellungen ><LCD Helligkeit>",
				       "< Eintellungen ><   ADC Test   >"};

const int8_t m_info1_32_t[33] PROGMEM = {"xx:xx:xx Mode:MMCH1:xxx%CH2:xxx%"};
const int8_t m_info2_32_t[33] PROGMEM = {"R:M0 R2:M0 R3:M01:xxx 2:xxx S:MM"};
const int8_t m_lctrlm32_t[33] PROGMEM = {" Mode:  Manuel  Wert: xxx = xxx%"};
const int8_t m_lctrlp32_t[33] PROGMEM = {" Mode: Programm   Start: xx:xx  "};
const int8_t m_lctrle32_t[33] PROGMEM = {" <    Exit    > Current Mode: M "};
const int8_t m_intens32_t[33] PROGMEM = {" LCD Helligkeit  +/-  xxx%      "};
const int8_t m_lictrl32_t[33] PROGMEM = {" Zeit  xx:xx:xx Wert: xxx = xxx%"};
const int8_t m_spabwa32_t[33] PROGMEM = {" [En]  [+]  [-]  Save Abort Exit"};
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
 
struct hm_struct {
	uint8_t minutes;
	uint8_t hours;
};

#define R_STATUS_ON		1		//Relays On
#define R_STATUS_AUT		2		//Relays Automatic
struct relay_timer {
	struct date_struct begin;
	struct date_struct end;
	uint8_t status;
};

/*
 * memory for 1 channel is enough for 12 hours, 2 minutes, 30 seconds
 * 170 (byte) * 255 = 43350 sekonds (a change per second)
 */
#define LI_CTRL_MAX	171
#define LI_CTRL_CH1	160
#define LI_CTRL_CH2	331
#define LI_CTRL_PSCL	25500
#define LI_CTRL_AUT	1
#define LI_CTRL_VALID	2
#define LI_CTRL_UP	3
#define LI_CTRL_DOWN	4

/*
 * struct for light control program.
 * begin_t is begin time of program
 * step is prescaler variable for interpolate between prev and next
 * prev is start value for any step, and next is end value
 * ptr is address pointer
 */
struct light_image_ctrl {
	struct hm_struct begin_t;
	uint16_t step;
	uint8_t prev;
	uint8_t next;
	uint16_t ptr;
	uint8_t status;
	uint8_t dimm_value;
};

#define BLINKING	0x04 
uint8_t blinking;
#define MAGIC_BYTE 0xA0
uint16_t eeprom_ptr = 0;
volatile uint16_t main_time_ms;
volatile uint8_t ocr1al_value;			//LCD Intens.
volatile uint8_t ocr1bl_value;			//DAC
uint8_t menu_entry;
extern volatile uint8_t dsi_data;
uint8_t cursor_pos;

#define TIMER0_VALUE		256 - 96 	// 1 / (9.8304MHz / 1024 / 96) = 10 ms
void init_timer0(void);
void init_timer1(void);

#define MENU_MAX 	11
#define MENU_MIN 	0
int menu_info( struct date_struct *date, struct light_image_ctrl *li_ctrl, 
		struct relay_timer *relays );
int set_time_struct( struct date_struct *date );
int set_time_rcontrol( struct relay_timer *relay );
int menu_manuel( struct light_image_ctrl *li_ctrl );
int menu_lcd_intens( volatile uint8_t *intens_value );
int menu_edit_li( struct light_image_ctrl *li_ctrl , uint16_t offset);
void chk_lictrl_t( struct light_image_ctrl *li_ctrl, 
	struct date_struct *date, uint16_t tmp_prescl);

int menu_test_adc(void);
extern volatile uint16_t adc_value[ADC_NUMBER];

#define LCD_BUFFER_SIZE	32
char lcd_buffer[LCD_BUFFER_SIZE] = {"Load data from  EEPROM v.0.4 ..."};
void clear_lcd_buffer(void);
void lcd_putint8_t( uint8_t position, uint8_t value );
#define T_STRUCT_HIGHER		2
#define T_STRUCT_LOWER		1
#define T_STRUCT_EQUAL		0
int compare_struct( struct date_struct *date1, struct date_struct *date2 );


/*
 * Display Infos: time, relays status, dimm. status...
 */
int menu_info( struct date_struct *date, struct light_image_ctrl *li_ctrl, 
		struct relay_timer *relays){
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
			lcd_putint8_t( 20, pgm_read_byte(&dimm_table[li_ctrl[0].dimm_value]));
			lcd_putint8_t( 28, pgm_read_byte(&dimm_table[li_ctrl[1].dimm_value]));
			if( li_ctrl[0].status & (1<<LI_CTRL_AUT) ) lcd_buffer[15] = 'A';
			if( li_ctrl[1].status & (1<<LI_CTRL_AUT) ) lcd_buffer[14] = 'A';
			break;
		case 2:
			for(i = 0; i < LCD_BUFFER_SIZE; i ++){
				lcd_buffer[i] = pgm_read_byte(&m_info2_32_t[i]);
			}
			if( relays[0].status & (1<<R_STATUS_AUT) ) lcd_buffer[2] = 'A';
			if( relays[0].status & (1<<R_STATUS_ON) ) lcd_buffer[3] = '1';
			if( relays[1].status & (1<<R_STATUS_AUT) ) lcd_buffer[8] = 'A';
			if( relays[1].status & (1<<R_STATUS_ON) ) lcd_buffer[9] = '1';
			if( relays[2].status & (1<<R_STATUS_AUT) ) lcd_buffer[14] = 'A';
			if( relays[2].status & (1<<R_STATUS_ON) ) lcd_buffer[15] = '1';
			lcd_putint8_t( 18, li_ctrl[0].dimm_value);
			lcd_putint8_t( 24, li_ctrl[1].dimm_value);
			if( li_ctrl[0].status & (1<<LI_CTRL_AUT) ) {
				lcd_buffer[30] = 'A';
				if( li_ctrl[0].status & (1<<LI_CTRL_VALID) ) lcd_buffer[30] = 'V';
				if( li_ctrl[0].status & (1<<LI_CTRL_UP) ) lcd_buffer[30] = '+';
				if( li_ctrl[0].status & (1<<LI_CTRL_DOWN) ) lcd_buffer[30] = '-';
			}
			if( li_ctrl[1].status & (1<<LI_CTRL_AUT) ) {
				lcd_buffer[31] = 'A';
				if( li_ctrl[0].status & (1<<LI_CTRL_VALID) ) lcd_buffer[31] = 'V';
				if( li_ctrl[1].status & (1<<LI_CTRL_UP) ) lcd_buffer[31] = '+';
				if( li_ctrl[1].status & (1<<LI_CTRL_DOWN) ) lcd_buffer[31] = '-';
			}
			break;
		case 3:
			sprintf(lcd_buffer, "ch7=%02d, ch6=%02d, ch5=%02d", 
					adc_value[0], adc_value[1], adc_value[2]);
			break;
	}
	return 0;
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
int set_time_rcontrol( struct relay_timer *relay){
	uint8_t i;

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
 * set light control manual or porgram, adjust intens. manual,
 * set li_ctrl begin time.
 * memory for 1 channel is enough for 12 hours, 2 minutes, 30 seconds
 * 170 (byte) * 255 = 43350 sekonds (a change per second)
 */
int menu_manuel( struct light_image_ctrl *li_ctrl ){
	uint8_t tmp1;
	if( cursor_pos < 3 ){
		if( user_keys == KEY_PLUS ) {
			cursor_pos ++;
			if(cursor_pos > 2) cursor_pos = 0;
		}
		if( user_keys == KEY_MINUS ) {
			cursor_pos --;
			if(cursor_pos > 2) cursor_pos = 2;
		}
		/* set light control manual 
		 * reset LI_CTRL_AUT and LI_CTRL_VALID */
		if( cursor_pos == 0 ) { 
			for(tmp1 = 0; tmp1 < LCD_BUFFER_SIZE; tmp1 ++){
				lcd_buffer[tmp1] = pgm_read_byte(&m_lctrlm32_t[tmp1]);
			}
			lcd_putint8_t( 22, li_ctrl->dimm_value );
			tmp1 = pgm_read_byte(&dimm_table[li_ctrl->dimm_value]);
			lcd_putint8_t( 28, tmp1 );
			if( user_keys == KEY_ENTER ) {
				cursor_pos = 3;
				li_ctrl->status &= ~(1<<LI_CTRL_AUT  | 1<<LI_CTRL_VALID);
			}
		}
		/* set light control program */
		if( cursor_pos == 1 ) { 
			for(tmp1 = 0; tmp1 < LCD_BUFFER_SIZE; tmp1 ++){
				lcd_buffer[tmp1] = pgm_read_byte(&m_lctrlp32_t[tmp1]);
			}
			sprintf(lcd_buffer + 25, "%02d:%02d", li_ctrl->begin_t.hours, 
					li_ctrl->begin_t.minutes );
			lcd_buffer[30] = ' ';
			if( user_keys == KEY_ENTER ) cursor_pos = 4;
		}
		/* display current light control mode and exit */
		if( cursor_pos == 2 ) { 
			for(tmp1 = 0; tmp1 < LCD_BUFFER_SIZE; tmp1 ++){
				lcd_buffer[tmp1] = pgm_read_byte(&m_lctrle32_t[tmp1]);
			}
			if( li_ctrl->status & ( 1<<LI_CTRL_AUT) ) lcd_buffer[30] = 'A';
			if( user_keys == KEY_ENTER ) {
				cursor_pos = 0;
				return 1;
			}
		}
		lcd_buffer[0] = '<'; lcd_buffer[15] = '>'; 
		return 0;
	}
	/* light control manual: adjust value manual */
	if( cursor_pos == 3 ){
		if( user_keys == KEY_ENTER ) {
			cursor_pos = 0;
		}
		if( user_keys == KEY_PLUS ) (li_ctrl->dimm_value) ++;
		if( user_keys == KEY_MINUS ) (li_ctrl->dimm_value) --;
		for(tmp1 = 0; tmp1 < LCD_BUFFER_SIZE; tmp1 ++){
			lcd_buffer[tmp1] = pgm_read_byte(&m_lctrlm32_t[tmp1]);
		}
		lcd_buffer[21] = '>'; lcd_buffer[25] = '<'; 
		lcd_putint8_t( 22, li_ctrl->dimm_value );
		tmp1 = pgm_read_byte(&dimm_table[li_ctrl->dimm_value]);
		lcd_putint8_t( 28, tmp1 );
		return 0;
	}
	/* light control program: adjust begin time 
	 * set LI_CTRL_AUT */
	if( cursor_pos > 3 ){
		if( user_keys == KEY_ENTER ) {
			cursor_pos ++;
			if( cursor_pos > 5 ) {
				cursor_pos = 1;
				li_ctrl->status |= ( 1<<LI_CTRL_AUT );
			}
		}
		for(tmp1 = 0; tmp1 < LCD_BUFFER_SIZE; tmp1 ++){
			lcd_buffer[tmp1] = pgm_read_byte(&m_lctrlp32_t[tmp1]);
		}
		sprintf(lcd_buffer + 25, "%02d:%02d", li_ctrl->begin_t.hours, 
				li_ctrl->begin_t.minutes );
		lcd_buffer[30] = ' ';
		if( cursor_pos == 4 ) { 
			if( blinking & (BLINKING) ){
				lcd_buffer[25] = ' '; lcd_buffer[26] = ' ';
			}
			if( user_keys == KEY_PLUS ) {
				li_ctrl->begin_t.hours ++;
				if( li_ctrl->begin_t.hours > 23) li_ctrl->begin_t.hours = 0;
			}
			if( user_keys == KEY_MINUS ) {
				li_ctrl->begin_t.hours --;
				if( li_ctrl->begin_t.hours > 23) li_ctrl->begin_t.hours = 23;
			}
		}
		if( cursor_pos == 5 ) { 
			if( blinking & (BLINKING) ){
				lcd_buffer[28] = ' '; lcd_buffer[29] = ' ';
			}
			if( user_keys == KEY_PLUS ) {
				li_ctrl->begin_t.minutes ++;
				if( li_ctrl->begin_t.minutes > 59) li_ctrl->begin_t.minutes = 0;
			}
			if( user_keys == KEY_MINUS ) {
				li_ctrl->begin_t.minutes --;
				if( li_ctrl->begin_t.minutes > 59) li_ctrl->begin_t.minutes = 59;
			}
		}
		blinking ++;
		return 0;
	}
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
	lcd_putint8_t( 22, (*intens_value) / 2 );
	return 0;
}
 
/*
 * edit light control program manual
 */
int menu_edit_li( struct light_image_ctrl *li_ctrl , uint16_t offset){
	uint32_t tmp1;
	uint8_t tmp2;
	uint8_t tmp3;
	static uint8_t tmp4; //address pointer
	static uint8_t tmp5; //dimm value
	if( cursor_pos < 2 ){
		if( user_keys == KEY_ENTER ) {
			cursor_pos ++;
			if(cursor_pos > 2) cursor_pos = 0;
		}
		for(tmp2 = 0; tmp2 < LCD_BUFFER_SIZE; tmp2 ++){
			lcd_buffer[tmp2] = pgm_read_byte(&m_lictrl32_t[tmp2]);
		}
		/* calculate and display current edit time (tmp1)
		 * step = 255 seconds(LI_CTRL_PSCL/100)
		 * tmp1 is time in seconds 
		 * tmp5 is address pointer for li_ctrl program memory
		 * tmp1 = step * tmp5 + curren time(in seconds) */
		tmp1 = ((uint32_t)(LI_CTRL_PSCL / 100) * tmp5);
		tmp1 += li_ctrl->begin_t.minutes * (uint32_t)60;
		tmp1 += li_ctrl->begin_t.hours * (uint32_t)3600;
		tmp2 = ( tmp1 / 60 ) % 60;
		tmp3 = ( tmp1 / 3600 ) % 24;
		tmp1 %= 60;
		sprintf(lcd_buffer + 7, "%02d:%02d:%02d", tmp3, tmp2, (uint8_t)tmp1);
		lcd_buffer[15] = ' ';
		/* adjust address pointer */
		if( cursor_pos == 0 ) { 
			lcd_buffer[6] = '>'; lcd_buffer[6 + 9] = '<'; 
			if( user_keys == KEY_PLUS ) {
				tmp5 ++;
				if( tmp5 >= LI_CTRL_MAX ) tmp5 = 0;
			}
			if( user_keys == KEY_MINUS ) {
				tmp5 --;
				if( tmp5 >= LI_CTRL_MAX ) tmp5 = LI_CTRL_MAX - 1;
			}
			/* read previously dimm value from eeprom */
			eeprom_ptr = offset + tmp5;
			tmp4 = eeprom_read_byte((void *)eeprom_ptr);
		}
		/* adjust dimm value */
		if( cursor_pos == 1 ) { 
			lcd_buffer[21] = '>'; lcd_buffer[21 + 4] = '<'; 
			if( user_keys == KEY_PLUS ) {
				tmp4 ++;
			}
			if( user_keys == KEY_MINUS ) {
				tmp4 --;
			}
		}
		lcd_putint8_t( 22, tmp4 );
		tmp2 = pgm_read_byte(&dimm_table[tmp4]);
		lcd_putint8_t( 28, tmp2 );
		return 0;
	}
	/* save, abort or exit */
	if( cursor_pos == 2 ) { 
		for(tmp2 = 0; tmp2 < LCD_BUFFER_SIZE; tmp2 ++){
			lcd_buffer[tmp2] = pgm_read_byte(&m_spabwa32_t[tmp2]);
		}
		if( user_keys == KEY_ENTER ) {
			cursor_pos = 0;
			/* store value by address pointed with tmp5 + offset */
			eeprom_ptr = offset + tmp5;
			eeprom_write_byte((void *) eeprom_ptr, tmp4);
		}
		if( user_keys == KEY_PLUS ) {
			cursor_pos = 0;
		}
		if( user_keys == KEY_MINUS ) {
			cursor_pos = 0;
			return 1;
		}
	}
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
void lcd_putint8_t( uint8_t position, uint8_t value ){
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

void chk_lictrl_t( struct light_image_ctrl *li_ctrl, 
	struct date_struct *date, uint16_t tmp_prescl){
	uint8_t tmp1;
	uint8_t tmp2;
	uint16_t tmp3;
	li_ctrl->status &= ~( 1<<LI_CTRL_VALID );
	/* check valid time for li_ctrl,
	 * tmp3 time in seconds */
	for ( tmp1 = 0; tmp1 <= 12 ; tmp1 ++){
		tmp2 = li_ctrl->begin_t.hours + tmp1;
		if( tmp2 > 23 ) tmp2 %= 24;
		/* if (begin time + posible program time
		 * == current time) then calculate eeprom pointer
		 * li_ctrl.ptr and step value li_ctrl.step */
		if( tmp2 == date->hours ) {
			tmp3 = tmp1 * (uint16_t)3600;
			/* if (current time minutes < begin program 
			 * time minutes) then break */
			if( date->minutes < li_ctrl->begin_t.minutes ) break;
			tmp1 = date->minutes - li_ctrl->begin_t.minutes;
			tmp3 += tmp1 * (uint8_t)60;
			tmp3 += date->seconds;
			/* caluculate li_ctrl.ptr 
			 * tmp3 time in seconds, 
			 * LI_CTRL_PSCL time in 0,01 seconds */
			li_ctrl->ptr = (uint8_t)(tmp3 / ((uint16_t)LI_CTRL_PSCL 
				/ 100));
			li_ctrl->ptr ++;
			if( (li_ctrl->ptr + 1) >= LI_CTRL_MAX) break;
			/* caluculate li_ctrl.step 
			 * li_ctrl->step = remainder time(seconds) 
			 * up to the next step */
			tmp3 %= ((uint16_t)LI_CTRL_PSCL / 100); 
			li_ctrl->step = (uint16_t)LI_CTRL_PSCL - (tmp3 * 100);
			tmp_prescl = main_time_ms;
			li_ctrl->prev = li_ctrl->next;
			li_ctrl->status |= ( 1<<LI_CTRL_VALID );
			break;
		}
	}
	return;	
}

int main(void){
	int8_t i;
	uint16_t tmp_prescl_ch1 = 0;
	uint16_t tmp_prescl_ch2 = 0;
	uint8_t tmp_prescl1 = 0;
	uint8_t tmp_prescl2 = 0;
	uint8_t tmp_prescl3 = 0;
	uint16_t tmp = 0;
	uint8_t menu_position = 0;
	menu_entry = 1;
	eeprom_ptr = 0;
	static struct date_struct date;
	static struct relay_timer relays[3];
	struct light_image_ctrl li_ctrl[2];
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
		eeprom_read_block((void *)&li_ctrl[0], (void *) eeprom_ptr, sizeof(li_ctrl[0]));
		eeprom_ptr += sizeof(li_ctrl[0]);
		eeprom_read_block((void *)&relays[0], (void *) eeprom_ptr, sizeof(relays[0]));
		eeprom_ptr += sizeof(relays[0]);
		eeprom_read_block((void *)&relays[1], (void *) eeprom_ptr, sizeof(relays[1]));
		eeprom_ptr = 0;
		relays[0].status &= ~(1<<R_STATUS_ON);
		relays[1].status &= ~(1<<R_STATUS_ON);
//		relays[2].status &= ~(1<<R_STATUS_ON);
//		li_ctrl[0].status &= ~(1<<LI_CTRL_VALID | 1<<LI_CTRL_AUT);
	}
	li_ctrl[0].ptr = 0;
	li_ctrl[0].prev = 0;
	li_ctrl[0].next = 0;
	li_ctrl[0].step = 0;
	li_ctrl[0].begin_t.hours = 0;
	li_ctrl[0].begin_t.minutes = 0;
	li_ctrl[0].dimm_value = 0;

	li_ctrl[1].ptr = 0;
	li_ctrl[1].prev = 0;
	li_ctrl[1].next = 0;
	li_ctrl[1].step = 0;
	li_ctrl[1].begin_t.hours = 0;
	li_ctrl[1].begin_t.minutes = 0;
	li_ctrl[1].dimm_value = 0;
/* test */ li_ctrl[0].status |= (1<<LI_CTRL_VALID | 1<<LI_CTRL_AUT);
/* test */ li_ctrl[1].status |= (1<<LI_CTRL_VALID | 1<<LI_CTRL_AUT);
	sei();
	while(1){
		/* if possible: compute current values for digital or analog signals and send they */
		if( (uint16_t)((main_time_ms - tmp_prescl_ch1)) == li_ctrl[0].step){
			tmp_prescl_ch1 = main_time_ms;
			/* if LI_CTRL_AUT set &&  current time and begin time are equal
			 * then set LI_CTRL_VALID */
			if( (li_ctrl[0].status & (1<<LI_CTRL_AUT)) &&
				!(li_ctrl[0].status & (1<<LI_CTRL_VALID)) ){
				if( li_ctrl[0].begin_t.hours == date.hours )
					if( li_ctrl[0].begin_t.minutes == date.minutes ){
						li_ctrl[0].status |= (1<<LI_CTRL_VALID); 
						li_ctrl[0].ptr = 0;
					}
			}
			/* if LI_CTRL_AUT && LI_CTRL_VALID set, compute current value
			 * and send it. */
			if( li_ctrl[0].status & (1<<LI_CTRL_VALID | 1<<LI_CTRL_AUT) ){
				/* if prev == next, set li_ctrl[0].prev 
				 * and li_ctrl[0].next new, calculate step prescaler variable
				 * li_ctrl[0].step */
				if( li_ctrl[0].prev == li_ctrl[0].next ){
					if( li_ctrl[0].ptr < LI_CTRL_MAX ){ 
						li_ctrl[0].prev = eeprom_read_byte((void *)li_ctrl[0].ptr + LI_CTRL_CH1);
						li_ctrl[0].ptr ++;
						li_ctrl[0].next = eeprom_read_byte((void *)li_ctrl[0].ptr + LI_CTRL_CH1);
						if( li_ctrl[0].prev < li_ctrl[0].next) {
							/* dimm up */
							li_ctrl[0].step = li_ctrl[0].next - li_ctrl[0].prev;
							li_ctrl[0].status |= (1<<LI_CTRL_UP);
							li_ctrl[0].status &= ~(1<<LI_CTRL_DOWN);
						}
							/* dimm down */
						else {
							li_ctrl[0].step = li_ctrl[0].prev - li_ctrl[0].next;
							li_ctrl[0].status |= (1<<LI_CTRL_DOWN);
							li_ctrl[0].status &= ~(1<<LI_CTRL_UP);
						}
						if( li_ctrl[0].step ) {
							/* step prescaler = LI_CTRL_PSCL /
							 * (start_value - end_value),
							 * example: start = 0,
							 * end = 255,
							 * LI_CTRL_PSCL = 25500,
							 * 25500 / (255 - 0) = 100 ->
							 * increment variable by 1 each second */
							li_ctrl[0].step = LI_CTRL_PSCL / li_ctrl[0].step;
						}
						else {
							/* if new values equal, 
							 * no action, set step prescaler max.*/
							li_ctrl[0].step = LI_CTRL_PSCL;
							li_ctrl[0].dimm_value = li_ctrl[0].prev;
							li_ctrl[0].status &= ~(1<<LI_CTRL_UP);
							li_ctrl[0].status &= ~(1<<LI_CTRL_DOWN);
						}
					}
					else {
						/* light control program ende, reset
						 * LI_CTRL_VALID, set li_ctrl[0].step 1s */
						li_ctrl[0].status &= ~(1<<LI_CTRL_VALID);
						li_ctrl[0].step = 100;
					}
				}
				if( li_ctrl[0].prev < li_ctrl[0].next ){
					/* dimm up */
					li_ctrl[0].prev ++;
					li_ctrl[0].dimm_value = li_ctrl[0].prev;
				}
				if( li_ctrl[0].prev > li_ctrl[0].next ){
					/* dimm down */
					li_ctrl[0].prev --;
					li_ctrl[0].dimm_value = li_ctrl[0].prev;
				}
			}
		}
		if( (uint16_t)((main_time_ms - tmp_prescl_ch2)) == li_ctrl[1].step){
			tmp_prescl_ch2 = main_time_ms;
			if( (li_ctrl[1].status & (1<<LI_CTRL_AUT)) &&
				!(li_ctrl[1].status & (1<<LI_CTRL_VALID)) ){
				if( li_ctrl[1].begin_t.hours == date.hours )
					if( li_ctrl[1].begin_t.minutes == date.minutes ){
						li_ctrl[1].status |= (1<<LI_CTRL_VALID); 
						li_ctrl[1].ptr = 0;
					}
			}
			if( li_ctrl[1].status & (1<<LI_CTRL_VALID | 1<<LI_CTRL_AUT) ){
				if( li_ctrl[1].prev == li_ctrl[1].next ){
					if( li_ctrl[1].ptr < LI_CTRL_MAX ){ 
						li_ctrl[1].prev = eeprom_read_byte((void *)li_ctrl[1].ptr + LI_CTRL_CH2);
						li_ctrl[1].ptr ++;
						li_ctrl[1].next = eeprom_read_byte((void *)li_ctrl[1].ptr + LI_CTRL_CH2);
						if( li_ctrl[1].prev < li_ctrl[1].next) {
							li_ctrl[1].step = li_ctrl[1].next - li_ctrl[1].prev;
							li_ctrl[1].status |= (1<<LI_CTRL_UP);
							li_ctrl[1].status &= ~(1<<LI_CTRL_DOWN);
						}
						else {
							li_ctrl[1].step = li_ctrl[1].prev - li_ctrl[1].next;
							li_ctrl[1].status |= (1<<LI_CTRL_DOWN);
							li_ctrl[1].status &= ~(1<<LI_CTRL_UP);
						}
						if( li_ctrl[1].step ) {
							li_ctrl[1].step = LI_CTRL_PSCL / li_ctrl[1].step;
						}
						else {
							li_ctrl[1].step = LI_CTRL_PSCL;
							li_ctrl[1].dimm_value = li_ctrl[1].prev;
							li_ctrl[1].status &= ~(1<<LI_CTRL_UP);
							li_ctrl[1].status &= ~(1<<LI_CTRL_DOWN);
						}
					}
					else {
						li_ctrl[1].status &= ~(1<<LI_CTRL_VALID);
						li_ctrl[1].step = 100;
					}
				}
				if( li_ctrl[1].prev < li_ctrl[1].next ){
					li_ctrl[1].prev ++;
					li_ctrl[1].dimm_value = li_ctrl[1].prev;
				}
				if( li_ctrl[1].prev > li_ctrl[1].next ){
					li_ctrl[1].prev --;
					li_ctrl[1].dimm_value = li_ctrl[1].prev;
				}
			}
		}

			/* keys polling */
		if( (uint8_t)(main_time_ms -  tmp_prescl3) == 6 ){
			tmp_prescl3 = main_time_ms;
			keys_polling();
		}
		/* 
		 * big clock machine, and relays controll
		 */
		if( (uint8_t)(main_time_ms - tmp_prescl1) == 100 ){
			tmp_prescl1 = main_time_ms;
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
			if( relays[0].status & (1<<R_STATUS_AUT) ){
				tmp = compare_struct( &date, &relays[0].begin );
				if( tmp == T_STRUCT_EQUAL ) {
					relays[0].status |= (1<<R_STATUS_ON);
				}
				tmp = compare_struct( &date, &relays[0].end );
				if( tmp == T_STRUCT_EQUAL ) {
					relays[0].status &= ~(1<<R_STATUS_ON);
				}
			}
			if( relays[1].status & (1<<R_STATUS_AUT) ){
				tmp = compare_struct( &date, &relays[1].begin );
				if( tmp == T_STRUCT_EQUAL ) {
					relays[1].status |= (1<<R_STATUS_ON);
				}
				tmp = compare_struct( &date, &relays[1].end );
				if( tmp == T_STRUCT_EQUAL ) {
					relays[1].status &= ~(1<<R_STATUS_ON);
				}
			}
			if( relays[2].status & (1<<R_STATUS_AUT) ){
				tmp = compare_struct( &date, &relays[2].begin );
				if( tmp == T_STRUCT_EQUAL ) {
					relays[2].status |= (1<<R_STATUS_ON);
				}
				tmp = compare_struct( &date, &relays[2].end );
				if( tmp == T_STRUCT_EQUAL ) {
					relays[2].status &= ~(1<<R_STATUS_ON);
				}
			}
		}
		/*
		 * display menu
		 */
		if( (uint8_t)(main_time_ms - tmp_prescl2) == 10 ){
			tmp_prescl2 = main_time_ms;
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
					if( menu_info( &date, li_ctrl, relays) ){
						menu_entry = 0;
					}
					break;
				case 1:
					if( set_time_struct( &date ) ){
						chk_lictrl_t(&li_ctrl[0], &date, tmp_prescl_ch1);
						chk_lictrl_t(&li_ctrl[1], &date, tmp_prescl_ch2);
						menu_entry = 0;
					}
					break;
				case 2:
					if( set_time_rcontrol( &relays[0] ) ){
						menu_entry = 0;
					}
					break;
				case 3:
					if( set_time_rcontrol( &relays[1] ) ){
						menu_entry = 0;
					}
					break;
				case 4:
					if( set_time_rcontrol( &relays[2] ) ){
						menu_entry = 0;
					}
					break;
				case 5:
					if( menu_edit_li(&li_ctrl[0], LI_CTRL_CH1) ){
						menu_entry = 0;
					}
					break;
				case 6:
					if( menu_manuel(&li_ctrl[0]) ){
						chk_lictrl_t(&li_ctrl[0], &date, tmp_prescl_ch1);
						menu_entry = 0;
					}
					break;
				case 7:
					if( menu_edit_li(&li_ctrl[1], LI_CTRL_CH2) ){
						menu_entry = 0;
					}
					break;
				case 8:
					if( menu_manuel(&li_ctrl[1]) ){
						chk_lictrl_t(&li_ctrl[1], &date, tmp_prescl_ch2);
						menu_entry = 0;
					}
					break;
				case 9:
					eeprom_write_byte((uint8_t *) eeprom_ptr, MAGIC_BYTE);
					eeprom_ptr ++;
					eeprom_write_block(&li_ctrl[0], (void *) eeprom_ptr, sizeof(li_ctrl[0]));
					eeprom_ptr += sizeof(li_ctrl[0]);
					eeprom_write_block(&relays[0], (void *) eeprom_ptr, sizeof(relays[0]));
					eeprom_ptr += sizeof(relays[0]);
					eeprom_write_block(&relays[1], (void *) eeprom_ptr, sizeof(relays[1]));
					eeprom_ptr += sizeof(relays[1]);
					eeprom_write_block(&relays[2], (void *) eeprom_ptr, sizeof(relays[2]));
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
			dsi_data = li_ctrl[0].dimm_value;
			ocr1bl_value = li_ctrl[1].dimm_value;
			dsi_send();
			user_keys = 0;
			command_lcd( 0x01 );
			lcd_print( lcd_buffer, 32);
		}
	}
}

