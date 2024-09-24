#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "HD44780_LCD/hd44780_config.h"
#include "HD44780_LCD/font.h"
#include "buttons/buttons.h"
#include "program_manager.h"
#include "data.h"
#include "menu.h"
#include "macros.h"
#include "led.h"

// -Wl,--gc-sections

/* ----------------------------------------- PROTOTYPES ----------------------------------------- */
void system_init( void );
void user_input( uint8_t button_status );
void hidden_input( uint8_t button_status );
/* ----------------------------------- (END) PROTOTYPES ----------------------------------------- */




/* ----------------------------------------- MAIN ----------------------------------------- */
int main( void ) {

	system_init();
	register_on_button_pressed_callback( user_input );
	register_on_button_released_callback( hidden_input );
	load_data_from_EEPROM();

	sei();


	while( 1 ) {

		BUTTONS_EVENT(); // manages user input
		BLINK_EVENT ();
		run_continuous_procedure(); // handles the 'continuous' part of the program

		ERROR_HANDLER_EVENT();

	}
}
/* ----------------------------------- (END) MAIN ----------------------------------------- */



/* ----------------------------------------- INIT FUNCTIONS ----------------------------------------- */
void timers_init( void ) {
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TIMSK0 |= (1 << TOIE0);

	TCCR2B |= (1 << CS22);
	TIMSK2 |= (1 << TOIE2);
}

void external_int_init( void ) {
	EICRA |= (1 << ISC10) | (1 << ISC00);
	EIMSK |= (1 << INT1)  | (1 << INT0);
}

void system_init( void ) {
	DEBUG_LED_INIT;
	FC_INIT;
	FA_INIT;

	lcd_init();
	font_init();
	pm_init();
	menu_init();
	buttons_init();
	timers_init();
	external_int_init();
	event_leds_init();
}
/* ----------------------------------- (END) INIT FUNCTIONS ----------------------------------------- */



/* ----------------------------------------- DEFINITIONS ----------------------------------------- */
void user_input( uint8_t button_status ) {
	get_user_input( button_status );
}

void hidden_input( uint8_t button_status ) {
	get_hidden_input( button_status );
}
/* ----------------------------------- (END) DEFINITIONS ----------------------------------------- */



/* ----------------------------------------- ISRs ----------------------------------------- */
ISR( TIMER0_OVF_vect ) {
	TCNT0 = 256 - 156;

	buttons_time_base();
	program_time_base();
}
/* ----------------------------------- (END) ISRs ----------------------------------------- */









