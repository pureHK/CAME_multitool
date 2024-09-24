#include "limit_sw_test.h"
#include "common_subprograms.h"
#include "../macros.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../HD44780_LCD/font.h"
#include "../program_manager.h"
#include "../menu.h"
#include "../helpers.h"
#include "../data.h"
#include "../led.h"

#include <avr/pgmspace.h>



static void show_gui( void ) {
	write_str_to_lcd( PSTR("\006 FA        FC \007"),
				  PSTR("# FA""\x80""FC"));
	if( gd->additional_data.fa_fc_swap ) lcd_print_str_P( PSTR(" \x81") );
	else lcd_print_str_P( PSTR(" x") );
}



static void limit_sw_test( uint8_t button_status ) {
	if( button_status & ESC ) {
		font_init();
		fa_off();
		fc_off();
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {
		swap_fa_fc();
		if( gd->additional_data.fa_fc_swap ) blink_at_on();
		else blink_at_off();
	}

	if( button_status & LEFT ) {
		fa_tog();
	}

	if( button_status & RIGHT ) {
		fc_tog();
	}

	if( continue_program( limit_sw_test ) ) show_gui();
}



void start_limit_sw_test( uint8_t button_status ) {
	lcd_defchar_P( 0, charSwap );
	lcd_defchar_P( 1, charTick );

	enter_program( limit_sw_test, 1, 0 );
}
