#ifndef SETTINGS_H_HK_
#define SETTINGS_H_HK_

#include <avr/io.h>


extern void start_swap_fa_fc( uint8_t button_status );
extern void start_save_settings( uint8_t button_status );
extern void start_reset_settings( uint8_t button_status );
extern void start_load_profile( uint8_t button_status );
extern void start_set_restriction( uint8_t button_status );


#endif /* SETTINGS_H_HK_ */
