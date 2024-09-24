#ifndef HD44780_LCD_HD44780_PHY_LAYER_H_
#define HD44780_LCD_HD44780_PHY_LAYER_H_

#define SET_RS 	LCD_PORT(LCD_RSPORT) |= (1<<LCD_RS)			// stan wysoki na linii RS
#define CLR_RS 	LCD_PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		// stan niski na linii RS

#define SET_RW 	LCD_PORT(LCD_RWPORT) |= (1<<LCD_RW)			// stan wysoki na RW - odczyt z LCD
#define CLR_RW 	LCD_PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		// stan niski na RW - zapis do LCD

#define SET_E 	LCD_PORT(LCD_EPORT) |= (1<<LCD_E)			// stan wysoki na linii E
#define CLR_E 	LCD_PORT(LCD_EPORT) &= ~(1<<LCD_E)			// stan niski na linii E




// definicje adresów w DDRAM dla ró¿nych wyœwietlaczy
// inne s¹ w wyœwietlaczach 2wierszowych i w 4wierszowych
// definicje adresów w DDRAM dla ró¿nych wyœwietlaczy
// inne s¹ w wyœwietlaczach 2wierszowych i w 4wierszowych

#if ( (LCD_ROWS == 2) && (LCD_COLS == 16) ) || ( (LCD_ROWS == 2) && (LCD_COLS == 20) )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
	#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
#endif

#if ( (LCD_ROWS == 4) && (LCD_COLS == 20) )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
	#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
	#define LCD_LINE3 0x14  	// adres 1 znaku 3 wiersza
	#define LCD_LINE4 0x54  	// adres 1 znaku 4 wiersza
#endif

#if ( (LCD_ROWS == 4) && (LCD_COLS == 16) )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
	#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
	#define LCD_LINE3 0x10  	// adres 1 znaku 3 wiersza
	#define LCD_LINE4 0x50  	// adres 1 znaku 4 wiersza
#endif

#if ( (LCD_ROWS == 2) && (LCD_COLS == 40) )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
	#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
#endif

#if ( (LCD_ROWS == 4) && (LCD_COLS == 40) )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
	#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
	#define LCD_LINE3 0x00  	// adres 1 znaku 3 wiersza
	#define LCD_LINE4 0x40  	// adres 1 znaku 4 wiersza
#endif

#if ( (LCD_ROWS == 1) && (LCD_COLS == 16) && LCD_TYPE == 1 )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
	#define LCD_LINE2 0x40		// adres 9 znaku 1 wiersza
#endif

#if ( (LCD_ROWS == 1) && (LCD_COLS == 16) && LCD_TYPE == 2 )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#endif

#if ( (LCD_ROWS == 1) && (LCD_COLS == 8) )
	#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#endif




// Makra upraszczaj¹ce dostêp do portów
// *** PORT
#define LCD_PORT(x) LCD_SPORT(x)
#define LCD_SPORT(x) (PORT##x)
// *** PIN
#define LCD_PIN(x) LCD_SPIN(x)
#define LCD_SPIN(x) (PIN##x)
// *** DDR
#define LCD_DDR(x) LCD_SDDR(x)
#define LCD_SDDR(x) (DDR##x)



// Komendy steruj¹ce
#define LCDC_CLS					0x01
#define LCDC_HOME					0x02
#define LCDC_ENTRY					0x04
	#define LCDC_ENTRYR					0x02
	#define LCDC_ENTRYL					0
	#define LCDC_MOVE					0x01
#define LCDC_ONOFF					0x08
	#define LCDC_DISPLAYON				0x04
	#define LCDC_CURSORON				0x02
	#define LCDC_CURSOROFF				0
	#define LCDC_BLINKON				0x01
#define LCDC_SHIFT					0x10
	#define LCDC_SHIFTDISP				0x08
	#define LCDC_SHIFTR					0x04
	#define LCDC_SHIFTL					0
#define LCDC_FUNC					0x20
	#define LCDC_FUNC8B					0x10
	#define LCDC_FUNC4B					0
	#define LCDC_FUNC2L					0x08
	#define LCDC_FUNC1L					0
	#define LCDC_FUNC5x10				0x04
	#define LCDC_FUNC5x7				0
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80







#endif /* HD44780_LCD_HD44780_PHY_LAYER_H_ */
