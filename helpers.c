#include "helpers.h"
#include "data.h"
#include "macros.h"
#include "HD44780_LCD/hd44780_config.h"

#include <avr/interrupt.h>
#include <string.h>


// Returns the length of 'number' passed as an argument
uint8_t num_len( uint32_t number ) {
	uint8_t result = 0;

	while( number > 0 ) {
		number /= 10;
		result++;
	}
	if( !result ) result = 1;

	return result;
}



void fa_on( void ) {
	if( gd->additional_data.fa_fc_swap ) FC_ON;
	else FA_ON;
}

void fa_off( void ) {
	if( gd->additional_data.fa_fc_swap ) FC_OFF;
	else FA_OFF;
}

void fa_tog( void ) {
	if( gd->additional_data.fa_fc_swap ) FC_TOG;
	else FA_TOG;
}

void fc_on( void ) {
	if( gd->additional_data.fa_fc_swap ) FA_ON;
	else FC_ON;
}

void fc_off( void ) {
	if( gd->additional_data.fa_fc_swap ) FA_OFF;
	else FC_OFF;
}

void fc_tog( void ) {
	if( gd->additional_data.fa_fc_swap ) FA_TOG;
	else FC_TOG;
}

void swap_fa_fc( void ) {
	gd->additional_data.fa_fc_swap = !gd->additional_data.fa_fc_swap;
}



void show_option_name( char* option, uint8_t blank_fields ) {

	uint8_t distance = (blank_fields - strlen(option)) / 2;
	for(uint8_t i = 0; i < distance; i++ ) lcd_put_char(' ');
	lcd_print_str( option );
}




/* ========================================= TIME FUNCTIONS ========================================= */
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)



volatile uint32_t timer2_overflow_count = 0;
volatile uint32_t timer2_millis = 0;
volatile uint8_t  timer2_fract = 0;

SIGNAL( TIMER2_OVF_vect )
{
    // copy these to local variables so they can be stored in registers
    // (volatile variables must be read from memory on every access)
	uint32_t m = timer2_millis;
	uint8_t f = timer2_fract;

    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    }

    timer2_fract = f;
    timer2_millis = m;
    timer2_overflow_count++;
}

unsigned long millis( void )
{
	uint32_t m;
    uint8_t oldSREG = SREG;

    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = timer2_millis;
    SREG = oldSREG;

    return m;
}

uint32_t micros( void ) { // can be used only in interrupts
	uint32_t m;
	uint8_t t;

	m = timer2_overflow_count;

	t = TCNT2;

	if ((TIFR2 & _BV(TOV2)) && (t < 255))
		m++;

	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void reset_time( void ) {
	uint8_t sreg = SREG;
	cli();
	timer2_overflow_count = 0;
	timer2_millis = 0;
	timer2_fract = 0;
	TCNT2 = 0;
	if(TIFR2 & (1 << TOV2))
		TIFR2 |= (1 << TOV2);
	SREG = sreg;
}
