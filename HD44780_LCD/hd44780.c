/*
 * hd44780.c
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>

#include "hd44780_config.h"				// abstract layer //
#include "hd44780_phy_layer.h"			// physical layer //



//----------------------------------------------------------------------------------------
//
//		 Wys³anie stringa do wyœwietlacza LCD z pamiêci RAM
//
//		 8 w³asnych znaków zdefiniowanych w CGRAM
//		 wysy³amy za pomoc¹ kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_print_str( char * str ) {
	register char znak;
	while( (znak = *(str++) ) ) lcd_put_char( (znak>=0x80 && znak<=0x87) ? (znak & 0x07) : znak );
}

void lcd_print_str_P( const char * str ) {
	register char znak;
	while( (znak = pgm_read_byte(str++) ) ) lcd_put_char( (znak>=0x80 && znak<=0x87) ? (znak & 0x07) : znak );
}


void lcd_init_printf( void ) {

	int put(char c, FILE * file){
		lcd_put_char(c);
		return 0;
	}

	fdevopen(&put, NULL);
}


void lcd_print_number( int32_t val, uint8_t base ) {
	char bufor[17];
	ltoa( val, bufor, base );
	lcd_print_str( bufor );
}


