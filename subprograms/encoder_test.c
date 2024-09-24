#include "encoder_test.h"
#include "common_subprograms.h"
#include "../macros.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../program_manager.h"
#include "../menu.h"
#include "../data.h"
#include "../helpers.h"
#include "../led.h"
#include "../HD44780_LCD/font.h"
#include "../HD44780_LCD/hd44780_phy_layer.h"

#include <util/atomic.h>
#include <avr/pgmspace.h>



typedef struct {
	uint8_t first_impulse;
	uint8_t measurements;
	uint8_t idx;
	uint8_t period_too_short;
	uint8_t count_time;
	uint32_t short_period_cnt;
	uint32_t short_period;
	uint16_t RPM;
	uint8_t p;
} T_CONTEXT;

static T_CONTEXT *c;



static void cleanup( void ) {
	fa_off();
	fc_off();
	font_init();
	exit_continuous_procedure();
}

static void encoder_process( void ) {
	static uint32_t prev_time, prev_period, period;
	static uint32_t measurement[8];


	if( gd->encoder_increment ) {
		gd->encoder_increment = 0;

		uint32_t enctime = gd->enctime;
		TimerShort2 = 20;
		DEBUG_LED_TOG;


		if( c->first_impulse ) {
			c->first_impulse = 0;

			prev_time = enctime;
			lcd_clear( 0 );
			lcd_goto_XY(0, 1);
			lcd_print_str_P( PSTR("# Ograniczenia "));
			if( gd->additional_data.naked ) lcd_print_str_P( PSTR("x") );
			else lcd_print_str_P( PSTR("\x80"));

		} else {

			if( c->p ) c->p--;

			period = enctime - prev_time;
			prev_time = enctime;

			if( !gd->additional_data.naked ) {
				if( ( gd->additional_data.restrict_numerator * period > gd->additional_data.restrict_denominator * prev_period) && !c->p ) {
					lcd_clear( 1 );
					lcd_print_str_P( PSTR("Okres(n-1):") );
					lcd_print_number(period, 10);
					lcd_clear( 2 );
					lcd_print_str_P( PSTR("Okres(n)  :") );
					lcd_print_number(prev_period, 10);

					cleanup();
					return;
				}
			}
			prev_period = period;

			if( ++c->measurements > 8 ) c->measurements = 8;

			measurement[c->idx] = period;

			period = 0;
			for( uint8_t i = 0; i < c->measurements; i++ ) {
				period += measurement[i];
			}
			period /= c->measurements;

			c->RPM = 60 * 1000000UL / period / gd->calibration_data.imp_per_rotation;

			if( ++c->idx == 8) c->idx = 0;
		}
	}

	if( !TimerShort2 && !gd->additional_data.naked ) { // if period between impulses is greater than 200ms stop the procedure
		write_str_to_lcd( PSTR("     -stop-"), 0 );
		cleanup();
		return;
	}

	if( !TimerLong && c->first_impulse ) { // wait 10 seconds for the first impulse
		TimerLong = 100;
		TimerShort2 = 150;
		lcd_goto_XY( 10, 1 );
		--c->count_time;
		if( c->count_time < 10 ) lcd_print_str_P( PSTR(" ") );
		lcd_print_number( c->count_time, 10 );
		if( !c->count_time ) {
			write_str_to_lcd( PSTR("Nie wykryto"),
					      PSTR("impuls\003w") );
			blink_at_err();
			cleanup();
			return;
		}
	}

	if( !TimerShort1 && !c->first_impulse ) { // print data every 80ms
			TimerShort1 = 10;

			lcd_clear( 1 );
//			lcd_print_number( period, 10 );
//			lcd_print_str_P( PSTR("us") );
			lcd_print_number( c->RPM, 10 );
			lcd_print_str_P( PSTR(" RPM") );
		}
}


static void encoder_test( uint8_t button_status ) {
	if( button_status & ESC ) {
		DEBUG_LED_OFF;
		cleanup();
		exit_to_menu( 1, 0 );
	}

	if( button_status & ENTER ) {
		gd->additional_data.naked = !gd->additional_data.naked;
		if( gd->additional_data.naked ) blink_at_off();
		else blink_at_on();
		lcd_goto_XY(15, 1);
		if( !c->first_impulse && TimerShort2 ) {
			if( gd->additional_data.naked ) lcd_print_str_P( PSTR("x"));
			else lcd_print_str_P( PSTR("\x80"));
		}
	}

	if( button_status & LEFT ) {
		fa_tog();
	}

	if( button_status & RIGHT ) {
		fc_tog();
	}
}


// called only once after executing the subprogram
void start_encoder_test( uint8_t button_status ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;

	lcd_defchar_P( 0, charTick );

	reset_time();

	gd->encoder_increment = 0;
	gd->encoder_aux_increment = 0;
	c->first_impulse = 1;
	c->measurements = 0;
	c->idx = 0;
	c->count_time = 11;
	c->period_too_short = 0;
	c->short_period_cnt = 0;
	c->p = 2;

	TimerShort2 = 10;
	TimerLong = 0;

	write_str_to_lcd( PSTR("Uruchom"),
				      PSTR("enkoder...") );

	fa_on();
	fc_on();

	set_continuous_procedure( encoder_process );
	enter_program( encoder_test, 1, 0 );
}
