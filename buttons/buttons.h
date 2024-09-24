#ifndef BUTTONS_H_HK_
#define BUTTONS_H_HK_

#include <avr/io.h>


/* -------------------------------------- MACROS -------------------------------------- */
// As a user don't touch the #if, #ifndef and #ifdef statements; #define statements should get a proper value or get commented/uncommented

#define BUTTONS_PORT D				// choose port letter (ie. B indicates that PORTB, PINB, DDRB will be used)

#define BUTTONS_MASK 0b11110000		// decides which pins in a port are connected to buttons

#define BUTTONS_INVERSE_LOGIC 1 	// 0 - indicates that idle button state is low
									// 1 - indicates that idle button state is high

#if BUTTONS_INVERSE_LOGIC == 1
	#define BUTTONS_PULLUP 1		// 0 - doesn't enable buttons' pull-up resistors
									// 1 - enables buttons' pull-up resistors (recommended in inverse logic for simplicity)
#endif

// Notice that if you want to use long press or repeating, you must set below values to not be 0 and corresponding callbacks shoud be registered
#define LONG_PRESS	0				// tens of milliseconds (if 0 - long press is disabled)      MAX 255 ~ 2.55 sec
#define REPEAT 0					// tens of milliseconds (if 0 - repeated action is disabled) MAX 255 ~ 2.55 sec

// todo
//#define MATRIX_KEYBOARD 			// comment if you want to use single buttons as inputs
									// uncomment if you want to use matrix keyboard
#ifndef MATRIX_KEYBOARD
	#define BUTTON_VIBRATION_TIME 3 // tens of milliseconds
#endif


/* END -------------------------------------- MACROS -------------------------------------- */





/* -------------------------------------- TYPES -------------------------------------- */

typedef void (*T_BUTTON_CALLBACK)(uint8_t button_status); // In function, user has to check button (ie. (1 << 0)) against button_status
														  // no matter if it's registered in pressed, released or any other callback
														  // the way of using it is everywhere the same
typedef struct {
	volatile uint8_t timer;
	uint8_t pressed;
	uint8_t long_press;
} T_BUTTON;

/* END -------------------------------------- TYPES -------------------------------------- */





/* -------------------------------------- FUNCTION PROTOTYPES -------------------------------------- */

extern void buttons_init( void ); // should be called in the main function once

extern void register_on_button_pressed_callback( T_BUTTON_CALLBACK cb);  	// should be called in the main function once
extern void register_on_button_released_callback( T_BUTTON_CALLBACK cb); 	// as above
extern void register_on_long_press_callback( T_BUTTON_CALLBACK cb); 		// as above
extern void register_on_repeat_callback( T_BUTTON_CALLBACK cb); 			// as above

extern void buttons_time_base( void ); // should be called in a timer interrupt

extern void BUTTONS_EVENT( void ); // should be called in a loop

/* END -------------------------------------- FUNCTION PROTOTYPES -------------------------------------- */




#endif
