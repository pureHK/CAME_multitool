#ifndef PROGRAM_MANAGER_H_HK_
#define PROGRAM_MANAGER_H_HK_

#include <avr/io.h>


typedef void (*T_INPUT_PROC)( uint8_t button_status );
typedef void (*T_CONT_PROC)( void );

typedef struct {
	T_INPUT_PROC input_procedure;
	T_CONT_PROC continuous_procedure;
	uint8_t buffer[32];

} T_PROGRAM_CONTEXT;


extern void pm_init( void );
extern void get_user_input( uint8_t button_status );
extern void get_hidden_input( uint8_t button_status );
extern void enter_program( T_INPUT_PROC proc, uint8_t immediatly_enter, uint8_t button_status );
extern uint8_t continue_program( T_INPUT_PROC proc );
extern void run_continuous_procedure( void );
extern void set_continuous_procedure( T_CONT_PROC proc );
extern void exit_continuous_procedure( void );
extern void exit_to_menu( uint8_t menu_on_foreground, uint8_t button_status );
extern void go_to_menu( uint8_t button_status );
extern void program_time_base( void );

extern void ERROR_HANDLER_EVENT( void ); // run it in main loop



#endif
