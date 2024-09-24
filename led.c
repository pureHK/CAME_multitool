#include "led.h"
#include "macros.h"
#include "data.h"

#include <util/atomic.h>



T_blink_event_data blink_events[1]; // Tablica struktur zbiorczych dla danych s³u¿¹cych do migania diodami.
									// Pojedynczy element zawiera informacje o tym która dioda bêdzie miga³a,
									// jaki bêdzie charakter migania (liczba powtórzeñ, czas stanu wysokiego i niskiego, itd.),
									// oraz zawiera funkcjê odpowiedzialn¹ za wykonywanie migania zgodnie z podanym charakterem.


void event_leds_init( void ) {
	blink_events[C_debug].led_data.PORT = &DEBUG_LED_PORT;
	blink_events[C_debug].led_data.IN   = &DEBUG_LED_IN;
	blink_events[C_debug].led_data.PIN  =  DEBUG_LED_PIN;

	DEBUG_LED_INIT;
}



static void set_blinking_s ( // This function loads approppriate data about blinking character, function responsible for blinking, and other things to the substruct of T_blink_event_data struct
		uint8_t function_id, // user defined (any, different than 0) id that makes unique the current instance of blinking settings
		uint8_t let_override_itself, // allow to restart whole blinking sequence if following callback settings have the same id as current one
		T_blink_event_data * event, // pointer to whole struct containing data about LED, blinking data and pointer to callback function
	    void (*cb)(T_blink_event_data*), // callback function that is assigned to callback pointer
		uint8_t count, // part of blinking data used with current callback (@see T_blinking_data)
		uint16_t on_time, // same as above
		uint16_t off_time ) // same as above
{
	event->blinking_data.initial_condition = 1;
	event->blinking_data.count = count;
	event->blinking_data.on_time = on_time;
	event->blinking_data.off_time = off_time;
	event->callback = cb;

	// Note: The function called as the last one has the highest priority

	if(event->blinking_data.function_id == function_id) {
		event->blinking_data.initial_condition = let_override_itself;
	} else {
		event->blinking_data.function_id = function_id;
	}

}
// Usage:
//set_blinking(1, 0, &blink_events[C_debug], led_blink_n_times, 3, 10, 40);



void set_blinking( uint8_t count, uint16_t on_time, uint16_t off_time ) {
	set_blinking_s(1, 1, &blink_events[C_debug], led_blink_n_times, count, on_time, off_time);
}

inline void blink_at_on( void ) {
	set_blinking( 1, 30, 0 );
}

inline void blink_at_off( void ) {
	set_blinking( 2, 5, 25 );
}

inline void blink_at_err( void ) {
	set_blinking( 3, 10, 40 );
}



inline void led_on(T_led_data* data) {
	*data->PORT |= data->PIN;
}

inline void led_off(T_led_data* data) {
	*data->PORT &= ~data->PIN;
}

inline uint8_t is_led_on(T_led_data* data) {
	return *data->IN & data->PIN;
}



// Assumption: this function works only if was set to global callback 'led_blink_cb'
void led_blink_n_times( T_blink_event_data* e ) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if(e->blinking_data.initial_condition) {
			e->blinking_data.initial_condition = 0;
			led_blink_timer = 0;
			if(is_led_on(&e->led_data)) // make the interrupt of new command visible (when the old one is currently on led_on state)...
				led_blink_timer = 3;    // ...it brings small delay to the indicator (only for asthetic purpose)
			led_off(&e->led_data);
		}

		// Actual blinking
		if(led_blink_timer) return;

		if(is_led_on(&e->led_data)) { // turn led off
			led_off(&e->led_data);
			led_blink_timer = e->blinking_data.off_time;
		} else { // turn led on
			if(!e->blinking_data.count) {
				e->callback = 0;
				e->blinking_data.function_id = 0;
				return;
			}
			led_on(&e->led_data);
			led_blink_timer = e->blinking_data.on_time;
			(e->blinking_data.count)--;
		}

	}
}



void BLINK_EVENT ( void ) {
	for(uint8_t i = 0; i < TAB_SIZE(blink_events); i++) {
		if(blink_events[i].callback)
			blink_events[i].callback(&blink_events[i]);
	}
}
