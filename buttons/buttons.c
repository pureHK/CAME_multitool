#include "buttons.h"
#include <util/atomic.h>




#define BTNS_DIR(port_letter) BTNS_SDIR(port_letter)
#define BTNS_SDIR(port_letter) DDR##port_letter

#define BTNS_PORT(port_letter) BTNS_SPORT(port_letter)
#define BTNS_SPORT(port_letter) PORT##port_letter

#define BTNS_IN(port_letter) BTNS_SIN(port_letter)
#define BTNS_SIN(port_letter) PIN##port_letter




static volatile uint8_t time_flag = 1; 	// time flag is decremented in an interrupt; the higher the set value is the longer are sampling intervals
										// ...it's useful when dealing with button wibrations but brings a delay between button press and read operation
										// ...initialized with 1 here to prevent fail reading at the beginning of the program
static volatile uint8_t button_status; 	// stores freshly received button status
static uint8_t bs_curr, bs_prev; 		// stores current and previous button status
										// they are active only once after the action (of pressing or releasing) happens

#if LONG_PRESS || REPEAT
static T_BUTTON single_buttons[8];		// array of button data structures containing info necessary for handle long press and repeating
static T_BUTTON_CALLBACK on_long_press;
static T_BUTTON_CALLBACK on_repeat;
#endif

static T_BUTTON_CALLBACK on_button_pressed;
static T_BUTTON_CALLBACK on_button_released;




void buttons_init( void ) {
	BTNS_DIR(BUTTONS_PORT) &= ~(BUTTONS_MASK); // assure button pins are inputs
#ifdef BUTTONS_PULLUP
	if( BUTTONS_PULLUP ) BTNS_PORT(BUTTONS_PORT) |= BUTTONS_MASK; // pull up all declared button pins
#endif
}



void register_on_button_pressed_callback( T_BUTTON_CALLBACK cb) {
	on_button_pressed = cb;
}

void register_on_button_released_callback( T_BUTTON_CALLBACK cb) {
	on_button_released = cb;
}
#if LONG_PRESS
void register_on_long_press_callback( T_BUTTON_CALLBACK cb) {
	on_long_press = cb;
}
#endif
#if REPEAT
void register_on_repeat_callback( T_BUTTON_CALLBACK cb) {
	on_repeat = cb;
}
#endif


void buttons_time_base( void ) {
	button_status = BTNS_IN(BUTTONS_PORT);
	if(time_flag) time_flag--;

#if LONG_PRESS  || REPEAT
	uint8_t n;
	for(uint8_t i = 0; i < 8; i++) {
		if( !(BUTTONS_MASK & (1 << i)) ) continue; // (may speed up the operation but takes more memory - can be commented if needed)
		n = single_buttons[i].timer;
		if(n) single_buttons[i].timer = --n;
	}
#endif
}



// Note: Notice that the call of the callbacks is done differently here than in BUTTONS_EVENT().
//       Here we call the same callback 8 times in such a way that only one button is handeled each time.
//       It's some kind of a workaround because here we are dealing with each button structure separately.
//       This is because a button has it's own timer and a set of flags, and they have to be maintained in a unique time for each button.
//       In BUTTONS_EVENT() we call callbacks only once per function iteration passing to them status of all buttons at once,
//       because pressed and released states can be handled and processed for all buttons altogether.
#if LONG_PRESS || REPEAT
static void long_press_and_repeat( uint8_t bs_pressed, uint8_t bs_released ) {
	for(uint8_t i = 0; i < 8; i++) { // for all buttons (structures of T_BUTTON)
		if( !(BUTTONS_MASK & (1 << i)) ) continue; // if a button is masked out then skip it (may speed up the operation but takes more memory - can be commented if needed)
		// comment of a block: preparing for action
		// comment of a block: happens only once after pressing button
		if( bs_pressed & (1 << i) ) { // check if button has just been pressed, and if it has...
			single_buttons[i].pressed = 1; // save that information in the button structure
			if( LONG_PRESS ) { // if long press is activated
				single_buttons[i].timer = LONG_PRESS; // load the button timer with some value that will make the delay after which function of long press can be called
				single_buttons[i].long_press = 1; // save information that long press has been prepared
			}
			else if( REPEAT ) { // but if long press isn't active, check if at least repeating is set
				single_buttons[i].timer = REPEAT; // if it is, load some value to the timer
			}
		} // END comment of a block: happens only once after pressing button
		// comment of a block: happens only once after releasing button
		else if( bs_released & (1 << i) ) { // ...but if it hasn't, check if button has just been released
			single_buttons[i].pressed = 0; // if it has, then save that information in the button structure
		} // END comment of a block: happens only once after releasing button
		// END comment of a block: preparing for action

		// comment of a block: doing the action
		if(!single_buttons[i].timer && single_buttons[i].pressed) { // when timer of a button reaches 0 and button is still pressed
			if(single_buttons[i].long_press) {	// check if it was the first iteration after the press by reading long_press value
				single_buttons[i].long_press = 0; // ...and if it was, make sure the next reading on still pressed button won't be for long press action
				if( on_long_press ) on_long_press( 1 << i ); // ...and call the function of a long press
				if( REPEAT ) single_buttons[i].timer = REPEAT; // ...and if repeating is on, prepare timer for it...
			}																	 // (not doing it would cause the repeating action to happen immediately after the long press action)
			else if( REPEAT ) { // ... but if it wasn't first iteration, check if repeating is activated
				if( on_repeat ) on_repeat(1 << i); // ...if it is, treat the read as a repeated action and call proper function
				single_buttons[i].timer = REPEAT; // and load the button timer with value no matter if on_repeat is NULL or not
			}
		}
		// END comment of a block: doing the action
	}
}
#endif



void BUTTONS_EVENT( void ) {

	if( time_flag ) return;


#ifdef MATRIX_KEYBOARD
	time_flag = 1;

#else

	time_flag = BUTTON_VIBRATION_TIME;

	bs_curr = button_status;
	if( BUTTONS_INVERSE_LOGIC ) bs_curr = ~bs_curr;
	bs_curr &= BUTTONS_MASK;

	uint8_t bs_pressed  = (bs_prev ^ bs_curr) & bs_curr; // indicates which buttons have been pressed
	uint8_t bs_released = (bs_prev ^ bs_curr) & bs_prev; // ...and released

	bs_prev = bs_curr;

	if( bs_pressed ) {
		if(on_button_pressed) on_button_pressed( bs_pressed );
	}

	if( bs_released ) {
		if(on_button_released) on_button_released( bs_released );
	}

	#if LONG_PRESS || REPEAT
	long_press_and_repeat(bs_pressed, bs_released);
	#endif


#endif
}





















