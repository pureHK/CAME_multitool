#ifndef HELPERS_H_HK_
#define HELPERS_H_HK_

#include <avr/io.h>



extern uint8_t num_len( uint32_t number );

extern void fa_on( void );
extern void fa_off( void );
extern void fa_tog( void );
extern void fc_on( void );
extern void fc_off( void );
extern void fc_tog( void );
extern void swap_fa_fc( void );

extern void show_option_name( char* option, uint8_t blank_fields );

extern uint32_t millis( void );
extern uint32_t micros( void );
extern void reset_time( void );





#endif /* HELPERS_H_HK_ */
