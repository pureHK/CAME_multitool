#include "encoder_counter.h"
#include "../macros.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../program_manager.h"
#include "../menu.h"
#include "../data.h"
#include "../helpers.h"
#include "../led.h"

#include <avr/pgmspace.h>




typedef struct {
	uint8_t fa_fc_change_course;
} T_CONTEXT;

T_CONTEXT *c;



static void show_gui( void ) {
	lcd_goto_XY(0, 0);
	lcd_print_str_P( PSTR("n-kod:") );
	lcd_goto_XY( 13 - num_len( gd->encoder_count ), 0);
	lcd_print_number( gd->encoder_count, 10 );
	lcd_goto_XY(14, 0);
	lcd_print_str_P( PSTR("\006x") );
}



static void show_gui_if_enco_changed( void ) {
	if( gd->encoder_increment ) {
		gd->encoder_increment = 0;
		show_gui();
	}
}



static void encoder_counter( uint8_t button_status ) {
	if( button_status & ESC ) {
		fc_off();
		fa_off();
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {
		if( gd->encoder_count ) {
			gd->calibration_data.imp_per_rotation = (uint8_t)gd->encoder_count;
			blink_at_on();
		}
	}

	if( button_status & LEFT ) {
		lcd_clear( 1 );
		gd->encoder_count = 0;
	}

	if( button_status & RIGHT ) {
		if( c->fa_fc_change_course < 2 ) fa_tog();
		else fc_tog();
		c->fa_fc_change_course = (c->fa_fc_change_course + 1) % 4;
	}

	if( continue_program( encoder_counter ) ) show_gui();
}


// called only once after executing the subprogram
void start_encoder_counter( uint8_t button_status ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;
	c->fa_fc_change_course = 0;

	lcd_clear( 0 );
	show_gui();
	lcd_goto_XY(0, 1);
	lcd_print_str_P( PSTR("# n-kod\x7eimp/obr") );

	fc_on();
	fa_on();

	set_continuous_procedure( show_gui_if_enco_changed );
	enter_program( encoder_counter, 1, 0 );
}
