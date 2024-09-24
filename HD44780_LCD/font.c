#include <avr/io.h>
#include <avr/pgmspace.h>

#include "font.h"
#include "hd44780_config.h"


void font_init( void ) {
	lcd_defchar_P( 0, charS );
	lcd_defchar_P( 1, charZ );
	lcd_defchar_P( 2, charN );
	lcd_defchar_P( 3, charO );
	lcd_defchar_P( 4, charA );
	lcd_defchar_P( 5, charE );

	lcd_defchar_P( 6, charLeft );
	lcd_defchar_P( 7, charRight );
}



const uint8_t charS[8] PROGMEM = // œ
	{
			2,4,14,16,14,1,30,0
	};

const uint8_t charC[8] PROGMEM = // æ
	{
			2,4,14,16,16,17,14,0
	};
const uint8_t charN[8] PROGMEM = // ñ
	{
			2,4,22,25,17,17,17,0
	};

const uint8_t charO[8] PROGMEM = // ó
	{
			2,4,14,17,17,17,14,0
	};

const uint8_t charA[8] PROGMEM = // ¹
	{
			0,0,14,1,15,17,15,1
	};

const uint8_t charE[8] PROGMEM = // ê
	{
			0,0,14,17,31,16,14,3
	};

const uint8_t charL[8] PROGMEM = // ³
	{
			12,4,6,12,4,4,14,0
	};

const uint8_t charZ[8] PROGMEM = // ¿
	{
			4,0,31,2,4,8,31,0
	};

const uint8_t charZ2[8] PROGMEM = // Ÿ
	{
			2,4,31,2,4,8,31,0
	};

const uint8_t charMi[8] PROGMEM = // mi
	{
			17,17,17,19,13,16,16,16
	};

const uint8_t charSS[8] PROGMEM = // s
	{
			0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00, 0x00, 0x00
	};

const uint8_t charLeft[8] PROGMEM = // Left
	{
			2,6,14,30,14,6,2,32
	};

const uint8_t charRight[8] PROGMEM = // Right
	{
			8,12,14,15,14,12,8,32
	};

const uint8_t charTick[8] PROGMEM = // Tick
	{
			32,32,1,2,20,8,32,32
	};

const uint8_t charSwap[8] PROGMEM = // Swap
	{
			4,8,31,8,2,31,2,4
	};

