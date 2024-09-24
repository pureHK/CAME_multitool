#ifndef HD44780_LCD_HD44780_CONFIG_H_
#define HD44780_LCD_HD44780_CONFIG_H_

//----------------------------------------------------------------------------------------
//
//		Parametry pracy sterownika
//
//----------------------------------------------------------------------------------------
// rozdzielczoœæ wyœwietlacza LCD (wiersze/kolumny)
#define LCD_ROWS 	2		// iloœæ wierszy wyœwietlacza LCD
#define LCD_COLS 	16		// iloœæ kolumn wyœwietlacza LCD
#define LCD_TYPE	1		// typ 1 lub typ 2 (tylko w przypadku wyœwietlaczy 16x1 wystêpuj¹ 2 ró¿ne typy adresów pamiêci DDRAM)

// tu ustalamy za pomoc¹ zera lub jedynki czy sterujemy pinem RW
//	0 - pin RW pod³¹czony na sta³e do GND
//	1 - pin RW pod³¹czony do mikrokontrolera
#define USE_RW 		1

//----------------------------------------------------------------------------------------
//
//		Ustawienia sprzêtowe po³¹czeñ sterownika z mikrokontrolerem
//
//----------------------------------------------------------------------------------------
// tu konfigurujemy port i piny do jakich pod³¹czymy linie D7..D4 LCD
#define LCD_D7PORT  C
#define LCD_D7 3
#define LCD_D6PORT  C
#define LCD_D6 2
#define LCD_D5PORT  C
#define LCD_D5 1
#define LCD_D4PORT  C
#define LCD_D4 0


// tu definiujemy piny procesora do których pod³¹czamy sygna³y RS, RW, E
#define LCD_RSPORT B
#define LCD_RS 1

#define LCD_RWPORT B
#define LCD_RW 2

#define LCD_EPORT B
#define LCD_E 3
//------------------------------------------------  koniec ustawieñ sprzêtowych ---------------







/* function headers */


//**** physical LAYER - mk_hd44780_phy_layer.c ****/
extern void lcd_init( void );
extern void lcd_clear( uint8_t line );
extern void lcd_put_char( uint8_t data );
extern void lcd_goto_XY( uint8_t x, uint8_t y );
extern void lcd_defchar( uint8_t nr, uint8_t *def_znak );
extern void lcd_defchar_P( uint8_t nr, const uint8_t *def_znak );

extern void lcd_cursor_on(void);
extern void lcd_cursor_off(void);
extern void lcd_blink_on(void);
extern void lcd_blink_off(void);


//**** abstract LAYER - mk_hd44780.c ****/
extern void lcd_print_str( char * str );
extern void lcd_print_str_P( const char * str );

extern void lcd_print_number( int32_t val, uint8_t base );
extern void lcd_init_printf( void );







#endif /* HD44780_LCD_HD44780_CONFIG_H_ */
