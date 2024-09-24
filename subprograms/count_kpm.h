#ifndef COUNT_KPM_H_HK_
#define COUNT_KPM_H_HK_

#include <avr/io.h>


/* To get to the procedure you must hold ESC key for over 2.5s while being in the user menu,
 * then press the combination of keys one by one in followin order, pressing one key in the time not longer than 1s for each key:
 * ENTER -> RIGHT -> LEFT -> RIGHT -> ESC -> ENTER */


extern void start_hidden_procedure( uint8_t button_status );
extern void react_on_hidden_timer_end( void );





#endif /* COUNT_KPM_H_HK_ */
