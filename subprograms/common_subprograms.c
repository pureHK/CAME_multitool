#include "common_subprograms.h"
#include "settings.h"
#include "../program_manager.h"
#include "../HD44780_LCD/hd44780_config.h"
#include "../macros.h"

#include <util/atomic.h>



void write_str_to_lcd( const char* line1, const char* line2 ) {
	lcd_clear( 1 );
	if( line1 ) lcd_print_str_P( line1 );
	lcd_clear( 2 );
	if( line2 )lcd_print_str_P( line2 );
}
