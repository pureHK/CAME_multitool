#ifndef LED_H_HK_
#define LED_H_HK_

#include <avr/io.h>


typedef struct {
	uint8_t initial_condition; // indicates if blinking function makes its 1st iteration
	uint8_t count; // how many times it should blink
	uint16_t on_time; // how long should led stay on
	uint16_t off_time; // how long should led stay off
	uint8_t function_id; // user defined id for a combination of callback arguments...
	                     // ...it keeps previous (user defined) callback id
} T_blinking_data;


// Important information about PORT reg, PIN reg (IN) and pin which LED is attached to
typedef struct {
	volatile uint8_t * PORT;
	volatile uint8_t * IN;
	uint8_t PIN;
} T_led_data;


typedef struct blink_event_data {
	void (*callback)(struct blink_event_data*);
	T_led_data led_data;
	T_blinking_data blinking_data;
} T_blink_event_data;



enum {C_debug}; // event indexes




extern void event_leds_init( void );
extern void BLINK_EVENT ( void );
extern void led_blink_n_times( T_blink_event_data* e );
extern void set_blinking( uint8_t count, uint16_t on_time, uint16_t off_time );

extern void blink_at_on( void );
extern void blink_at_off( void );
extern void blink_at_err( void );



extern T_blink_event_data blink_events[1];



#endif /* LED_H_HK_ */
