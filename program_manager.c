#include "program_manager.h"
#include "data.h"
#include "menu.h"
#include "macros.h"
#include "led.h"
#include "HD44780_LCD/hd44780_config.h"
#include <avr/pgmspace.h>
#include "subprograms/calibration.h"
#include "subprograms/count_kpm.h"



static T_PROGRAM_CONTEXT program_context;

// pm == program_manager

void pm_init( void ) {
	program_context.input_procedure = user_menu; // sets the User Menu as a first input procedure
	gd->current_program_context = &program_context;
}



void get_user_input( uint8_t button_status ) {
	gd->current_program_context->input_procedure( button_status );
}



void get_hidden_input( uint8_t button_status ) {
	if( gd->in_hidden_procedure ) return;
	if( gd->current_program_context->input_procedure != user_menu ) return;

	if( button_status == ESC ) {
		if( !TimerHidden ) {
			gd->in_hidden_procedure = 1;
			set_blinking( 1, 5, 0 );
			enter_program( start_hidden_procedure, 1, 0 );
			set_continuous_procedure( react_on_hidden_timer_end );
			TimerHidden = 100;
		} else {
			gd->in_hidden_procedure = 0;
			gd->count_hidden_timer = 0;
			exit_to_menu( 1, 0 );
		}
	}
}



void enter_program( T_INPUT_PROC proc, uint8_t enter_instantly, uint8_t button_status ) {
	gd->current_program_context->input_procedure = proc; 	 // redirect the user input to another program (aka. `proc')
	if( enter_instantly )
		gd->current_program_context->input_procedure( button_status );   // run the program instantly
													   	     	 	 	 // if we won't run it instantly, the gui part of the `proc' would be hidden until any button press
}


// Run it within an if statement before displaying the GUI to ensure
// that the GUI of the previous program does not overlap with the GUI of the subsequently executed program. (ChatGPT comment)
uint8_t continue_program( T_INPUT_PROC proc ) {
	return proc == gd->current_program_context->input_procedure;
}


// Run it in main loop.
void run_continuous_procedure( void ) {
	if( gd->current_program_context->continuous_procedure ) // there is an if statement because gd->repeated_program may be empty
		gd->current_program_context->continuous_procedure();
}



void set_continuous_procedure( T_CONT_PROC proc ) {
	gd->current_program_context->continuous_procedure = proc;
}



void exit_continuous_procedure( void ) {
	gd->current_program_context->continuous_procedure = 0;
}



void exit_to_menu( uint8_t menu_on_foreground, uint8_t button_status ) { // immediately exits to User Menu | 1 - brings menu to foreground, 0 - menu will appear after a button press
	exit_continuous_procedure();
	enter_program( user_menu, menu_on_foreground, button_status );
}


// If you don't exit_to_menu(1) but instead enter_program(go_to_menu, 1) it will bring a delay of one button press.
// This is useful while displaying a self closing message before going to the menu.
// Pressing a button while the message is being diaplayed will perform the action of going to menu instead of performing an action on unvisible menu.
void go_to_menu( uint8_t button_status ) {
	if( (button_status & ENTER) ||
		(button_status & ESC)   ||
		(button_status & LEFT)  ||
		(button_status & RIGHT)) {

		exit_to_menu( 1, 0 );
	}
}



void ERROR_HANDLER_EVENT( void ) {
	if( gd->data_corrupted ) {
		gd->data_corrupted = 0;

		enter_program( go_to_menu, 1, 0 );

		lcd_clear( 1 );
		lcd_print_str_P( PSTR("Dane w EEPROM"));
		lcd_clear( 2 );
		lcd_print_str_P( PSTR("s\004 uszkodzone!"));
	}

	// other errors
}


// Call it once in TIMER ISR
void program_time_base( void ) {
	uint8_t n = TimerShort1;
	if(n) TimerShort1 = --n;
	n = TimerShort2;
	if(n) TimerShort2 = --n;

	if( gd->count_hidden_timer ) {
		n = TimerHidden;
		if(n) TimerHidden = --n;
	}

	uint16_t m = TimerLong;
	if(m) TimerLong = --m;
	m = led_blink_timer;
	if(m) led_blink_timer = --m;
}
