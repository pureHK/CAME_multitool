#include "info.h"
#include "common_subprograms.h"
#include "../macros.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../program_manager.h"
#include "../menu.h"
#include "../data.h"
#include "../helpers.h"

#include <util/atomic.h>
#include <avr/pgmspace.h>




static void show_version( uint8_t button_status ) {
	if( (button_status & ESC)  ||
		(button_status & LEFT) ||
		(button_status & RIGHT) ) {
		exit_to_menu( 1, 0 );
	}

	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ) {
	if( button_status & ENTER ) {
		TimerLong = 500; // keep showing version for another 5 seconds since button press
	}
	}
}



static void auto_close_info( void ) {
	if( !TimerLong ) {
		exit_to_menu( 1, 0 );
	}
}



void start_show_version( uint8_t button_status ) {
	TimerLong = 500; // for showing version for 5 seconds

	write_str_to_lcd( PSTR("Wersja programu:"),
				  PSTR(" V0.98 21.04.24 ") );

	set_continuous_procedure( auto_close_info );
	enter_program( show_version, 0, 0 );
}
