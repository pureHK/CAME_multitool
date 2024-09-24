#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>

#include "hd44780_config.h"
#include "hd44780_phy_layer.h"



#if USE_RW == 1
uint8_t check_busy_flag( void );
#endif


//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WYj�cia
//
//----------------------------------------------------------------------------------------
inline void data_dir_out( void ) {
	LCD_DDR(LCD_D7PORT)	|= (1<<LCD_D7);
	LCD_DDR(LCD_D6PORT)	|= (1<<LCD_D6);
	LCD_DDR(LCD_D5PORT)	|= (1<<LCD_D5);
	LCD_DDR(LCD_D4PORT)	|= (1<<LCD_D4);
}

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WEj�cia
//
//----------------------------------------------------------------------------------------
inline void data_dir_in( void ) {			// todo: do optymalizacji - static
	LCD_DDR(LCD_D7PORT)	&= ~(1<<LCD_D7);
	LCD_DDR(LCD_D6PORT)	&= ~(1<<LCD_D6);
	LCD_DDR(LCD_D5PORT)	&= ~(1<<LCD_D5);
	LCD_DDR(LCD_D4PORT)	&= ~(1<<LCD_D4);
}


//----------------------------------------------------------------------------------------
//
//		 Wys�anie po��wki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
inline void lcd_sendHalf( uint8_t data ) {		// todo: do optymalizacji - static

	if( data&(1<<0) ) LCD_PORT(LCD_D4PORT) |= (1<<LCD_D4); else LCD_PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	if( data&(1<<1) ) LCD_PORT(LCD_D5PORT) |= (1<<LCD_D5); else LCD_PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if( data&(1<<2) ) LCD_PORT(LCD_D6PORT) |= (1<<LCD_D6); else LCD_PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if( data&(1<<3) ) LCD_PORT(LCD_D7PORT) |= (1<<LCD_D7); else LCD_PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}


#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt po��wki bajtu z LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
inline uint8_t lcd_readHalf( void ) {			// todo: do optymalizacji - static

	uint8_t result=0;

	if( LCD_PIN(LCD_D4PORT)&(1<<LCD_D4) ) result |= (1<<0);
	if( LCD_PIN(LCD_D5PORT)&(1<<LCD_D5) ) result |= (1<<1);
	if( LCD_PIN(LCD_D6PORT)&(1<<LCD_D6) ) result |= (1<<2);
	if( LCD_PIN(LCD_D7PORT)&(1<<LCD_D7) ) result |= (1<<3);

	return result;
}
#endif



//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte( uint8_t _data ) {

	// Ustawienie pin�w portu LCD D4..D7 jako wyj�cia
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif

	SET_E;
	lcd_sendHalf(_data >> 4);			// wys�anie starszej cz�ci bajtu danych D7..D4
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);				// wys�anie m�odszej cz�ci bajtu danych D3..D0
	CLR_E;

#if USE_RW == 1
	while( (check_busy_flag() & (1<<7)) );
#else
	_delay_us(120);
#endif

}


#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt bajtu z wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
uint8_t _lcd_read_byte( void ) {

	uint8_t result=0;
	data_dir_in();

	SET_RW;

	SET_E;
	result = (lcd_readHalf() << 4);	// odczyt starszej cz�ci bajtu z LCD D7..D4
	CLR_E;

	SET_E;
	result |= lcd_readHalf();			// odczyt m�odszej cz�ci bajtu z LCD D3..D0
	CLR_E;

	return result;
}
#endif




#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Sprawdzenie stanu Busy Flag (Zaj�to�ci wy�wietlacza)
//
//----------------------------------------------------------------------------------------
uint8_t check_busy_flag( void ) {
	CLR_RS;
	return _lcd_read_byte();
}
#endif



//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd( uint8_t cmd ) {				// todo: do optymalizacji - static

	CLR_RS;
	_lcd_write_byte(cmd);
}



//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_put_char( uint8_t data ) {			// todo: do optymalizacji - static

	SET_RS;
	_lcd_write_byte(data);
}




void lcd_defchar( uint8_t nr, uint8_t *def_znak ) {

	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++) {
		c = *(def_znak++);
		lcd_put_char(c);
	}
	lcd_goto_XY(0, 0);
}


void lcd_defchar_P( uint8_t nr, const uint8_t *def_znak ) {

	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++) {
		c = pgm_read_byte(def_znak++);
		lcd_put_char(c);
	}
	lcd_goto_XY(0, 0);
}




//----------------------------------------------------------------------------------------
//
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zale�no�ci od rodzaju wy�wietlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
void lcd_goto_XY( uint8_t x, uint8_t y ) {

	switch(y)
	{
		case 0: y = LCD_LINE1; break;

#if (LCD_ROWS>1)
	    case 1: y = LCD_LINE2; break; // adres 1 znaku 2 wiersza
#endif
#if (LCD_ROWS>2)
    	case 2: y = LCD_LINE3; break; // adres 1 znaku 3 wiersza
#endif
#if (LCD_ROWS>3)
    	case 3: y = LCD_LINE4; break; // adres 1 znaku 4 wiersza
#endif
	}

	lcd_write_cmd( (0x80 + y + x) );
}



//----------------------------------------------------------------------------------------
//
//		W��czenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_on(void) {

	lcd_write_cmd( LCDC_ONOFF | LCDC_DISPLAYON | LCDC_CURSORON );
}

//----------------------------------------------------------------------------------------
//
//		Wy��czenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_off(void) {

	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}


//----------------------------------------------------------------------------------------
//
//		W��cza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_on(void) {

	lcd_write_cmd( LCDC_ONOFF | LCDC_DISPLAYON | LCDC_BLINKON );
}

//----------------------------------------------------------------------------------------
//
//		WY��cza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_off(void) {

	lcd_write_cmd( LCDC_ONOFF | LCDC_DISPLAYON );
}







//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wy�wietlacza
//
//----------------------------------------------------------------------------------------
void lcd_clear( uint8_t line ) {

	void lcd_clear_line( uint8_t ln ) {		// nested function -------------
		lcd_goto_XY( 0, ln );
		for( uint8_t i = 0; i < LCD_COLS; i++ ) lcd_put_char( ' ' );
	}

	if( !line ) {
		lcd_write_cmd( LCDC_CLS );
		#if USE_RW == 0
			_delay_ms(4.9);
		#endif
		lcd_goto_XY( 0, 0 );
	} else {
		lcd_clear_line( line-1 );
		lcd_goto_XY( 0, line-1 );
	}
}




//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WY�WIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init( void ) {
	// inicjowanie pin�w port�w ustalonych do pod��czenia z wy�wietlaczem LCD
	// ustawienie wszystkich jako wyj�cia
	data_dir_out();
	LCD_DDR(LCD_RSPORT) |= (1<<LCD_RS);
	LCD_DDR(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
	LCD_DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	LCD_PORT(LCD_RSPORT) |= (1<<LCD_RS);
	LCD_PORT(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		LCD_PORT(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	_delay_ms(15);
	LCD_PORT(LCD_EPORT) &= ~(1<<LCD_E);
	LCD_PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	#if USE_RW == 1
		LCD_PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
	#endif

	// jeszcze nie mo�na u�ywa� Busy Flag
	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy
	CLR_E;
	_delay_ms(4.1);

	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy
	CLR_E;

	// ju� mo�na sprawdza� Busy Flag

	SET_E;
	lcd_sendHalf(0x02);// tryb 4-bitowy
	CLR_E;

#if USE_RW == 1
	while( (check_busy_flag() & (1<<7)) );
#else
	_delay_us(120);
#endif

	// ju� mo�na u�ywa� Busy Flag
	// tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	// wy��czenie kursora
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	// w��czenie wy�wietlacza
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	// przesuwanie kursora w prawo bez przesuwania zawarto�ci ekranu
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );

	// kasowanie ekranu
	lcd_clear(0);
}




