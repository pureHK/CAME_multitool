#include "count_kpm.h"
#include "common_subprograms.h"
#include "../macros.h"
#include "../program_manager.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../data.h"
#include "../helpers.h"

#include <avr/pgmspace.h>
#include <util/atomic.h>


#define COUNT 4

typedef struct {
	uint8_t first_hit;
	uint32_t prev_time;
	uint32_t time;
	uint16_t delta;
	uint16_t kpm_current;
	uint16_t kpm[COUNT];
	uint8_t idx;
	uint8_t count;
} T_CONTEXT;

T_CONTEXT *c;




static void show_gui( void ) {
	lcd_clear( 1 );
	lcd_print_str_P( PSTR("KPM: ") );
	lcd_goto_XY( 8 - num_len( c->kpm_current ), 0 );
	lcd_print_number( c->kpm_current, 10 );
}



static void reset_measure( void ) {
	c->first_hit = 1;
	c->idx = 0;
	c->count = 0;
	c->kpm_current = 0;
	reset_time();
}



static void count_kpm( uint8_t button_status ) {
	if( ( button_status & ESC) ) {
		exit_to_menu( 1, 0 );
	}

	if( ( (button_status & LEFT) ||
		  (button_status & RIGHT)) ) {
			reset_measure();
		}

	if( ( button_status & ENTER ) ) {
		ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
		c->time = millis();
		}

		if( c->first_hit ) {
			c->first_hit = 0;

			c->prev_time = c->time;

		} else {

			c->delta = c->time - c->prev_time;
			c->prev_time = c->time;

			if( ++c->count == COUNT + 1 ) c->count = COUNT;

			c->kpm_current = 60000UL / c->delta;

			c->kpm[c->idx] = c->kpm_current;

			c->kpm_current = 0;
			for( uint8_t i = 0; i < c->count; i++ ) {
				c->kpm_current += c->kpm[i];
			}
			c->kpm_current /= c->count;

			c->idx = (c->idx + 1) % COUNT;
		}
	}

	if( continue_program( count_kpm ) ) show_gui();
}



void react_on_hidden_timer_end( void ) {
	if( !TimerHidden ) {
		exit_to_menu( 1, 0 );
	}
}



static void start_count_kpm( uint8_t button_status ) {
	c = (T_CONTEXT*)gd->current_program_context->buffer;

	reset_measure();
	write_str_to_lcd( PSTR("KPM: ---"), PSTR("reset \006\007") );

	enter_program( count_kpm, 1, 0 );
}



static void reset_and_go_to_menu( uint8_t button_status ) {
	gd->in_hidden_procedure = 0;
	gd->count_hidden_timer = 0;
	exit_to_menu( 1, button_status );
}



static void hidden_proc5( uint8_t button_status ) {
	if( (button_status & RIGHT) ||
		(button_status & ESC)   ||
		(button_status & LEFT) ) {

		reset_and_go_to_menu( button_status );
	}

	if( (button_status & ENTER) && TimerHidden ) {
		gd->in_hidden_procedure = 0;
		gd->count_hidden_timer = 0;
		exit_continuous_procedure();
		enter_program( start_count_kpm, 1, 0 );
	}
}



static void hidden_proc4( uint8_t button_status ) {
	if( (button_status & ENTER) ||
		(button_status & RIGHT)   ||
		(button_status & LEFT) ) {

		reset_and_go_to_menu( button_status );
	}

	if( (button_status & ESC) && TimerHidden ) {
		TimerHidden = 100;
		enter_program( hidden_proc5, 1, 0 );
	}
}



static void hidden_proc3( uint8_t button_status ) {
	if( (button_status & ENTER) ||
		(button_status & ESC)   ||
		(button_status & LEFT) ) {

		reset_and_go_to_menu( button_status );
	}

	if( (button_status & RIGHT) && TimerHidden ) {
		TimerHidden = 100;
		enter_program( hidden_proc4, 1, 0 );
	}
}

static void hidden_proc2( uint8_t button_status ) {
	if( (button_status & ENTER) ||
		(button_status & ESC)   ||
		(button_status & RIGHT) ) {

		reset_and_go_to_menu( button_status );
	}

	if( (button_status & LEFT) && TimerHidden ) {
		TimerHidden = 100;
		enter_program( hidden_proc3, 1, 0 );
	}
}

static void hidden_proc1( uint8_t button_status ) {
	if( (button_status & ENTER) ||
		(button_status & ESC)   ||
		(button_status & LEFT) ) {

		reset_and_go_to_menu( button_status );
	}

	if( (button_status & RIGHT) && TimerHidden ) {
		TimerHidden = 100;
		enter_program( hidden_proc2, 1, 0 );
	}
}

void start_hidden_procedure( uint8_t button_status ) {
	if( (button_status & RIGHT) ||
		(button_status & ESC)   ||
		(button_status & LEFT) ) {

		reset_and_go_to_menu( button_status );
	}

	if( (button_status & ENTER) && TimerHidden ) {
		TimerHidden = 100;
		enter_program( hidden_proc1, 1, 0 );
	}
}
