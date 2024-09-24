#include "settings.h"
#include "common_subprograms.h"
#include "../macros.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../program_manager.h"
#include "../menu.h"
#include "../data.h"
#include "../helpers.h"
#include "../HD44780_LCD/font.h"
#include "../led.h"

#include <util/atomic.h>
#include <avr/pgmspace.h>


enum EditPhase { C_edit_numerator, C_edit_denominator, C_edit_end };

typedef struct {
	uint8_t action_once;
	uint8_t answer_yes;
	uint8_t number_visible;
	uint8_t temp_numerator;
	uint8_t temp_denominator;
	enum EditPhase edit_phase;
	T_CALIBRATION_DATA * current_calibration_data;
} T_CONTEXT;

T_CONTEXT *c;



/* ------------------------------------------------------------------------------------------------ */

static void show_prompt( const char* line1, const char* line2 ) {
	if( c->action_once ) {
		c->action_once = 0;
		write_str_to_lcd( line1, line2 );
	}
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
	if( !TimerLong ) {
		exit_to_menu( 1, 0 );
	}
	}
}

static void common_start( void ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;

	c->answer_yes = 0;
	c->action_once = 1;

	write_str_to_lcd( PSTR("  Akceptujesz?"),
				  PSTR("\006              \007") );
}

static void common_button_handle( void (*function)(void), T_CONT_PROC proc, uint8_t button_status ) {
	if( button_status & ESC ) {
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {
		if( c->answer_yes ) {
			function();
			set_continuous_procedure( proc );
			TimerLong = 250;
			enter_program( go_to_menu, 1, 0 ); // don't go to the menu directly, to prevent doing an action on unseen menu
		} else {
			exit_to_menu( 1, 0 );
		}
	}

	if( (button_status & LEFT) || (button_status & RIGHT) ) {
		c->answer_yes = !c->answer_yes;
	}
}

static void show_gui( void ) {
	lcd_goto_XY(6, 1);
	if( c->answer_yes ) lcd_print_str_P( PSTR("TAK"));
	else lcd_print_str_P( PSTR("NIE"));
}


/* ------------------------------------------------------------------------------------------------ */


static void show_prompt_change( void ) {
	show_prompt( PSTR("   Zmieniono"),
				 PSTR("   ustawienia") );
}

static void swap( uint8_t button_status ) {
	common_button_handle( swap_fa_fc, show_prompt_change, button_status );
	if( continue_program( swap ) ) show_gui();
}

void start_swap_fa_fc( uint8_t button_status ) {
	common_start();
	enter_program( swap, 1, 0 );
}


/* ------------------------------------------------------------------------------------------------ */


static void show_prompt_save( void ) {
	show_prompt( PSTR("    Zapisano"),
				 PSTR("   ustawienia") );
}

static void save_settings( uint8_t button_status ) {
	common_button_handle( save_data, show_prompt_save, button_status);
	if( continue_program( save_settings ) ) show_gui();
}

void start_save_settings( uint8_t button_status ) {
	common_start();
	enter_program( save_settings, 1, 0 );
}


/* ------------------------------------------------------------------------------------------------ */


static void show_prompt_reset( void ) {
	show_prompt( PSTR("   Zresetowano"),
			     PSTR("   ustawienia") );
}

static void reset_settings( uint8_t button_status ) {
	common_button_handle( load_default_data, show_prompt_reset, button_status);
	if( continue_program( reset_settings ) ) show_gui();
}

void start_reset_settings( uint8_t button_status ) {
	common_start();
	enter_program( reset_settings, 1, 0 );
}


/* ------------------------------------------------------------------------------------------------ */


static void show_prompt_load_profile( void ) {
	show_prompt( PSTR("    Wczytano"),
			     PSTR("     profil") );
}

static void copy_name_from_PGM_profile( char * str, char * buffer ) {
	strcpy_P( buffer, (PGM_P)pgm_read_word( str ) );
}

static void show_profile_gui( void ) {
	write_str_to_lcd( PSTR(" Wybierz profil"), 0);
	lcd_clear( 2 );
	lcd_put_char('\006');
	char buffer[5];
//	copy_name_from_PGM_profile( (char*)&profile[gd->profile_idx], buffer ); // alternative code line below does the same thing
	copy_name_from_PGM_profile( (char*)(&profile[gd->profile_idx] + (size_t)(&((T_CALIBRATION_DATA*)0)->name)), buffer );
	show_option_name( buffer, 14 );
	lcd_goto_XY(15, 1);
	lcd_put_char('\007');
}

static void load_profile( uint8_t button_status ) {
	if( button_status & ESC ) {
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {
		load_motor_profile( c->current_calibration_data ); // load profile to RAM
		set_continuous_procedure( show_prompt_load_profile );
		TimerLong = 250;
		enter_program( go_to_menu, 1, 0 );
	}

	if( button_status & LEFT ) {
		if( --gd->profile_idx < 0) gd->profile_idx = PROFILE_SIZE - 1;
		c->current_calibration_data = (T_CALIBRATION_DATA*)pgm_read_ptr(&profile[gd->profile_idx]);
	}

	if( button_status & RIGHT ) {
		if( ++gd->profile_idx > PROFILE_SIZE - 1) gd->profile_idx = 0;
		c->current_calibration_data = (T_CALIBRATION_DATA*)pgm_read_ptr(&profile[gd->profile_idx]);
	}

	if( continue_program( load_profile ) ) show_profile_gui();
}

void start_load_profile( uint8_t button_status ) {
	common_start();
	lcd_clear( 0 );

	c->current_calibration_data = (T_CALIBRATION_DATA*)pgm_read_ptr(&profile[gd->profile_idx]);

	enter_program( load_profile, 1, 0 );
}


/* ------------------------------------------------------------------------------------------------ */



static void blink_number( uint8_t number, uint8_t visible ) {
	if( visible ) {
		lcd_print_number( number, 10);
	} else {
		uint8_t len = num_len( number );
		while( len-- ) lcd_put_char('\xff');
	}
}

static void wait( void ) {
	if( !TimerLong ) {
		exit_to_menu( 1, 0 );
	}
}

static void show_restriction_edit( void ) {
	if( !TimerShort1 ) {
		TimerShort1 = 50;
		c->number_visible = !c->number_visible;
		c->action_once = 1;
	}

	if( !c->action_once ) return;

	c->action_once = 0;

	switch( c->edit_phase ) {
	case C_edit_numerator:
		lcd_clear( 2 );
		lcd_goto_XY( 7 - num_len(c->temp_numerator), 1);
		blink_number( c->temp_numerator, c->number_visible );
		lcd_put_char('/');
		lcd_print_number( c->temp_denominator, 10);
		break;
	case C_edit_denominator:
		lcd_clear( 2 );
		lcd_goto_XY( 7 - num_len(c->temp_numerator), 1);
		lcd_print_number( c->temp_numerator , 10 );
		lcd_put_char('/');
		blink_number( c->temp_denominator, c->number_visible );
		break;
	case C_edit_end:
		lcd_clear( 1 );
		lcd_print_str_P( PSTR(" Zmieniono ust.") );
		lcd_clear( 2 );
		lcd_goto_XY( 7 - num_len(c->temp_numerator), 1);
		lcd_print_number( c->temp_numerator , 10 );
		lcd_put_char('/');
		lcd_print_number( c->temp_denominator, 10);
		ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
		TimerLong = 250;
		}
		set_continuous_procedure( wait );
		break;
	}
}

static void set_restriction_edit_num( uint8_t button_status );

static void reset_blink( void ) {
	TimerShort1 = 0;
	c->action_once = 1;
	c->number_visible = 1;
}

static void set_restriction_edit_denom( uint8_t button_status ) {
	if( button_status & ESC ) {
		c->edit_phase = C_edit_numerator;
		enter_program( set_restriction_edit_num, 1, 0 );
		reset_blink();
	}

	if( button_status & ENTER ) {
		if( c->temp_numerator <=  c->temp_denominator ) {
			gd->additional_data.restrict_numerator = c->temp_numerator;
			gd->additional_data.restrict_denominator = c->temp_denominator;
			blink_at_on();
			c->edit_phase = C_edit_end;
			enter_program( go_to_menu, 1, 0 );
		} else {
			c->edit_phase = C_edit_numerator;
			enter_program( set_restriction_edit_num, 1, 0 );
			reset_blink();
		}
	}

	if( button_status & LEFT ) {
		if( c->temp_denominator > 2 ) c->temp_denominator--;
	}

	if( button_status & RIGHT ) {
		if( c->temp_denominator < 50 ) c->temp_denominator++;
	}

	c->action_once = 1;
}

static void set_restriction_edit_num( uint8_t button_status ) {
	if( button_status & ESC ) {
		blink_at_off();
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {
		enter_program( set_restriction_edit_denom, 1, 0 );
		c->edit_phase = C_edit_denominator;
		reset_blink();
	}

	if( button_status & LEFT ) {
		if( c->temp_numerator > 1 ) c->temp_numerator--;
	}

	if( button_status & RIGHT ) {
		if( c->temp_numerator < 20 ) c->temp_numerator++;
	}

	c->action_once = 1;
}

void start_set_restriction( uint8_t button_status ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;

	c->action_once = 1;
	c->edit_phase = C_edit_numerator;
	c->temp_numerator = gd->additional_data.restrict_numerator;
	c->temp_denominator = gd->additional_data.restrict_denominator;

	lcd_clear( 1 );
	lcd_print_str_P( PSTR("   Edycja ogr.") );

	TimerShort1 = 0;

	enter_program( set_restriction_edit_num, 1, 0 );
	set_continuous_procedure( show_restriction_edit );
}





/* ------------------------------------------------------------------------------------------------ */



static void show_imp_per_rotation( void ) {

}



static void set_imp_per_rotation( uint8_t button_status ) {
	if( button_status & ESC ) {
		blink_at_off();
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {

	}

	if( button_status & LEFT ) {
		if( c->temp_numerator > 2 ) c->temp_numerator--;
	}

	if( button_status & RIGHT ) {
		if( c->temp_numerator < 20 ) c->temp_numerator++;
	}

	c->action_once = 1;
}



void start_set_imp_per_rotation( uint8_t button_status ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;

	c->action_once = 1;

	lcd_clear( 1 );
	lcd_print_str_P( PSTR(" Ustaw imp/obr:") );

	TimerShort1 = 0;

	enter_program( set_imp_per_rotation, 1, 0 );
	set_continuous_procedure( show_imp_per_rotation );
}



/* ------------------------------------------------------------------------------------------------ */







