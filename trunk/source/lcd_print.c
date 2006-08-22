/*
 * LCD driver
 *
 * Copyright (C) 2006 Johann Fischer (terentij@gmx.net)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 */

#include <avr/io.h>
#include "lcd_print.h"


#ifndef F_CPU
#define F_CPU 		9830400
#endif

#include <util/delay.h>

void lcd_print(char *buffer, uint8_t buf_size){
	uint8_t loop;
	for( loop = 0; loop < 16; loop++ ){
		put_char_lcd(buffer[loop]);
	}
	for( loop = 0; loop < 24; loop++ ){
		put_char_lcd(0x00);
	}
	for( loop = 16; loop < 32; loop++ ){
		put_char_lcd(buffer[loop]);
	}
	return;
}

void put_char_lcd(int8_t daten){
	read_busy_lcd();
	/* set data direction out */
	LCD_DDR = 0xFF;
	/* set RS, clear WR, write mode - data */
	LCD_SPORT |= (1<<LCD_RS);
	LCD_SPORT &= ~(1<<LCD_WR);
	LCD_DPORT = daten;
	/* E pulse width time min. 200ns */
	LCD_SPORT |= (1<<LCD_E);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	LCD_SPORT &= ~(1<<LCD_E);
	return;
}

void command_lcd(uint8_t command){
	read_busy_lcd();
	/* set data direction out */
	LCD_DDR = 0xFF;
	/* clear RS, clear WR, write mode - command */
	LCD_SPORT &= ~(1<<LCD_RS);
	LCD_SPORT &= ~(1<<LCD_WR);
	LCD_DPORT = command;
	/* E pulse width time min. 200ns */
	LCD_SPORT |= (1<<LCD_E);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	LCD_SPORT &= ~(1<<LCD_E);
	return;
}

void read_busy_lcd(void){
	uint8_t temp;
	/* set data direction in */
	LCD_DDR = 0x00;
	/* clear RS, set WR, read mode - command */
	LCD_SPORT &= ~(1<<LCD_RS);
	LCD_SPORT |= (1<<LCD_WR);
	/* E pulse width time min. 200ns */
	do{
	LCD_SPORT |= (1<<LCD_E);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	temp = LCD_PIN;
	LCD_SPORT &= ~(1<<LCD_E);
	/* E cycle time min. 500ns */
	asm volatile ("nop"::);
	} while( temp & (1<<LCD_BUSY_FLAG) );
	return;
}

void init_lcd(void){
	LCD_SDDR |= (1<<LCD_RS);
	LCD_SDDR |= (1<<LCD_WR);
	LCD_SDDR |= (1<<LCD_E);
	_delay_ms(20);
	/* display init send 3x */
	command_lcd( LCD_INIT );
	_delay_ms(5);
	command_lcd( LCD_INIT );
	_delay_ms(5);
	command_lcd( LCD_INIT );
	/* set funktion */
	/* set 8 bit interface, set 2 line display, 5x7 dots */	
	command_lcd( (1<<LCD_SET_FUNC) | (1<<LCD_8_BIT) | (1<<LCD_2_LINE) );
	/* set display on, cursor on, blinking on */
//	command_lcd( LCD_FUNC_ON_OFF | LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINKING_ON );
	command_lcd( (1<<LCD_FUNC_ON_OFF) | (1<<LCD_DISPLAY_ON) );
	/* entry mode set */
	command_lcd( (1<<LCD_ENTRY_MODE) | (1<<LCD_INCREASE) );
	/* clear display */
	command_lcd( LCD_CLEAR );
	return;
}
