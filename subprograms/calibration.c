#include "calibration.h"
#include "common_subprograms.h"
#include "../macros.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../program_manager.h"
#include "../menu.h"
#include "../data.h"
#include "../helpers.h"
#include "../HD44780_LCD/font.h"
#include "common_subprograms.h"

#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <avr/interrupt.h>




typedef enum { impulses, percent, rotations, wheel_rotations, meters } presentation_mode;

presentation_mode pm;


typedef struct {
	uint8_t let_first_line_to_change;
	uint8_t action_once;
	uint8_t clear_display;
	uint8_t calibration_stage;
	uint32_t last_calibration_count;
} T_CONTEXT;

T_CONTEXT *c;




static void show_gui_2nd_line( void ) { // BXV
	uint32_t lcc = c->last_calibration_count;
	uint32_t temp;

	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
		if( gd->encoder_count ) lcc = gd->encoder_count;
	}

	switch( pm ) {
	case impulses:
		lcd_goto_XY( 10 - num_len( lcc ), 1);
		lcd_print_number( lcc, 10 );
		break;
	case percent:
		temp = lcc * 100UL / gd->calibration_data.max_impulses;
		if( temp > 100 ) temp = 100;
		lcd_goto_XY( 9 - num_len( temp ), 1);
		lcd_print_number( temp, 10 );
		lcd_print_str_P( PSTR("%") );
		break;
	case rotations:
		temp = lcc / gd->calibration_data.imp_per_rotation;
		lcd_goto_XY( 10 - num_len( temp ), 1);
		lcd_print_number( temp, 10 );
		break;
	case wheel_rotations:
		temp =  10 * lcc / gd->calibration_data.imp_per_rotation / gd->calibration_data.worm_gear_teeth_count;
		lcd_goto_XY( 8 - num_len( temp/10 ), 1);
		lcd_print_number( temp/10, 10 );
		lcd_put_char('.');
		lcd_print_number( temp%10, 10 );
		break;
	case meters:
		temp = 3927UL * gd->calibration_data.gear_diameter / 1250UL * lcc / gd->calibration_data.imp_per_rotation / gd->calibration_data.worm_gear_teeth_count;
		lcd_goto_XY( 6 - num_len( temp/100000 ), 1);
		lcd_print_number( temp/100000, 10 );
		lcd_put_char('.');
		temp = temp / 1000 % 100;
		if( temp < 10) lcd_put_char('0');
		lcd_print_number( temp, 10 );
		lcd_put_char('m');
		break;
	}
}



static void show_gui( void (*show_gui_cb)(void) ) {
	if( !gd->encoder_increment ) return;

	gd->encoder_increment = 0;

	if( c->let_first_line_to_change ) {
		c->let_first_line_to_change = 0;

		lcd_clear( 0 );
		switch( pm ) {
		case impulses:
			lcd_print_str_P( PSTR("\006Impulsy n-kod \007") );
			break;
		case percent:
			lcd_print_str_P( PSTR("\006  % Manewru   \007") );
			break;
		case rotations:
			lcd_print_str_P( PSTR("\006 Rotacje sil. \007") );
			break;
		case wheel_rotations:
			lcd_print_str_P( PSTR("\006 Rotacje ko\004a \007") );
			break;
		case meters:
			lcd_print_str_P( PSTR("\006    Metry     \007") );
			break;
		}
	}

	show_gui_cb();
}



static void move_through_stages_odd( uint8_t next_stage ) {
	if( TimerShort1 ) return;

	c->last_calibration_count = gd->encoder_count;
	gd->encoder_count = 0;
	c->calibration_stage = next_stage;
}



static void prepare_for_gui( void (*show_gui_cb)(void) ) {
	c->let_first_line_to_change = 1;
	gd->encoder_increment = 1;
	show_gui( show_gui_cb );
}



static void move_through_stages_even( uint8_t timer, uint16_t enco, uint8_t limit_sw ) { // [limit_sw]: 0 == FA_ON, FC_OFF; 1 == FC_ON, FA_OFF
	TimerShort1 = timer;
	if( gd->encoder_count == 1) prepare_for_gui( show_gui_2nd_line );
	if( limit_sw ) {
		if( gd->encoder_count > 20) fc_on();
	} else if ( gd->encoder_count > 20) fa_on();
	if( gd->encoder_count == enco ) {
		if( limit_sw ) fa_off();
		else fc_off();
		c->calibration_stage = c->calibration_stage + 1;
	}
}



static void run_bkv_calibration( void ) {

	switch( c->calibration_stage ) {
	case 0:
		move_through_stages_even( 40, 200, 0 );
		break;
	case 1:
		move_through_stages_odd(2);
		break;
	case 2:
		move_through_stages_even( 40, gd->calibration_data.max_impulses, 1 );
		break;
	case 3:
		move_through_stages_odd(4);
		break;
	case 4:
		move_through_stages_even( 40, gd->calibration_data.max_impulses, 0 );
		break;
	case 5:
		move_through_stages_odd(6);
		break;
	case 6:
		move_through_stages_even( 40, gd->calibration_data.max_impulses, 1 );
		c->action_once = 1;
		break;
	case 7:
		move_through_stages_odd(8);

		TimerLong = 300;
		lcd_goto_XY( 0, 0);
		lcd_print_str_P( PSTR("   Kalibracja   ") );
		lcd_goto_XY( 0, 1);
		lcd_print_str_P( PSTR("   zako\001czona   ") );
		break;

	case 8:
		if( !TimerLong ) {
			if( c->action_once ) {
				c->action_once = 0;
				prepare_for_gui( show_gui_2nd_line );
			}
		}

		move_through_stages_even( 40, gd->calibration_data.max_impulses, 0 );
		break;
	case 9:
		move_through_stages_odd(10);
		break;
	case 10:
		move_through_stages_even( 40, gd->calibration_data.max_impulses, 1 );
		break;
	case 11:
		move_through_stages_odd(8);
		break;
	}

	show_gui( show_gui_2nd_line );
}



static void calibration( uint8_t button_status ) {
	if( button_status & ENTER ) {

	}

	if( button_status & ESC ) {
		font_init();

		fc_off();
		fa_off();
		exit_continuous_procedure();
		enter_program( user_menu, 1, 0 );
	}

	if( button_status & LEFT ) {
		if( !pm ) pm = 4;
		else pm -= 1;
		prepare_for_gui( show_gui_2nd_line );
	}

	if( button_status & RIGHT ) {
		pm = (pm + 1) % 5;
		prepare_for_gui( show_gui_2nd_line );
	}
}



static void run_bxv_calibration( void ) {

	switch( c->calibration_stage ) {
	case 0:
		move_through_stages_even( 40, 200, 0 );
		break;
	case 1:
		move_through_stages_odd(2);
		break;
	case 2:
		move_through_stages_even( 40, gd->calibration_data.max_impulses, 1 );
		c->action_once = 1;
		break;
	case 3:
		move_through_stages_odd(4);

		TimerLong = 300;
		write_str_to_lcd( PSTR("   Kalibracja   "),
				      PSTR("   zako\001czona   ") );
		break;

	case 4:
		if( !TimerLong ) {
			if( c->action_once ) {
				c->action_once = 0;
				prepare_for_gui( show_gui_2nd_line );
			}
		}

		move_through_stages_even( 40, gd->calibration_data.max_impulses, 0 );
		break;
	case 5:
		move_through_stages_odd(6);
		break;
	case 6:
		move_through_stages_even( 40, gd->calibration_data.max_impulses, 1 );
		break;
	case 7:
		move_through_stages_odd(4);
		break;
	}

	show_gui( show_gui_2nd_line );
}



static void prepare_calibration_context( void ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;

	write_str_to_lcd( PSTR("Uruchom"),
			      PSTR("kalibracj\005...") );

	fc_on();
	fa_on();

	gd->encoder_count = 0;
	gd->encoder_increment = 0;
	c->let_first_line_to_change = 1;
	c->calibration_stage = 0;
	c->last_calibration_count = 0;

	lcd_defchar_P(1, charN);
	lcd_defchar_P(3, charA);
	lcd_defchar_P(4, charL);
}



void start_custom_calibration( uint8_t button_status ) {
	prepare_calibration_context();
	set_continuous_procedure( run_bxv_calibration );
	enter_program( calibration, 0, 0 );
}



void start_bxv_calibration( uint8_t button_status ) {
	load_motor_profile( (T_CALIBRATION_DATA*)&cal_data_BXV );
	prepare_calibration_context();
	set_continuous_procedure( run_bxv_calibration );
	enter_program( calibration, 0, 0 );
}



void start_bkv_calibration( uint8_t button_status ) {
	load_motor_profile( (T_CALIBRATION_DATA*)&cal_data_BKV );
	prepare_calibration_context();
	set_continuous_procedure( run_bkv_calibration );
	enter_program( calibration, 0, 0 );
}



ISR( INT1_vect ) { // motor encoder
	gd->enctime = micros();
	gd->encoder_count++;
	gd->encoder_increment = 1;
}

ISR( INT0_vect ) { // auxiliary encoder
	gd->enctime_aux = micros();
	gd->encoder_aux_count++;
	gd->encoder_aux_increment = 1;
}




