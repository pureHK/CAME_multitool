#include "data.h"

#include <avr/eeprom.h>
#include <avr/pgmspace.h>


// ------------------ predefined profiles                                     // gear_module and gear_teeth_count not used yet
const T_CALIBRATION_DATA cal_data_BXV PROGMEM = {.name = "BXV", .max_impulses = 10610, .imp_per_rotation = 8, .worm_gear_teeth_count = 50, .gear_module = 4, .gear_teeth_count = 16, .gear_diameter = 7195};
const T_CALIBRATION_DATA cal_data_BKV PROGMEM = {.name = "BKV", .max_impulses = 11500, .imp_per_rotation = 8, .worm_gear_teeth_count = 40, .gear_module = 4, .gear_teeth_count = 18, .gear_diameter = 7995};

// table of predefined profiles
const T_CALIBRATION_DATA * const profile[2] PROGMEM = {
		&cal_data_BXV, &cal_data_BKV
};
// ------------ (END) predefined profiles


//data that is loaded when user use reset option                    // 4m gate length
const T_STORED_DATA default_data_PGM PROGMEM = {.calibration_data = { "BXV", 7073, 8, 50, 4, 16, 7195 }, .additional_data = { 0, 1, 1, 2 }};


// global data variable (with default options) and its pointer
struct data global_data = {.calibration_data = {.max_impulses = 7073, .imp_per_rotation = 8, .worm_gear_teeth_count = 50, .gear_module = 4, .gear_teeth_count = 16, .gear_diameter = 7195},
						   .additional_data  = {.naked = 1, .restrict_numerator = 1, .restrict_denominator = 2}};
struct data *gd = &global_data;


// ------------------ EEPROM data
T_STORED_DATA config_data_EEM EEMEM;
T_CALIBRATION_DATA user_profile_EEM EEMEM;  // not implemented yet
// ------------ (END) EEPROM data


// ------------------ Global timers
volatile uint16_t TimerLong;
volatile uint8_t TimerShort1;
volatile uint8_t TimerShort2;
volatile uint8_t TimerHidden = 250;
volatile uint16_t led_blink_timer;
// ------------ (END) Global timers



#define FIRST_EEPROM_BYTE_VALUE 0xAA

void save_data( void ) {
	eeprom_write_byte( &config_data_EEM.first_byte, FIRST_EEPROM_BYTE_VALUE );
	eeprom_write_block( &global_data.calibration_data, &config_data_EEM.calibration_data, sizeof(T_CALIBRATION_DATA) );
	eeprom_write_block( &global_data.additional_data, &config_data_EEM.additional_data, sizeof(T_ADDITIONAL_DATA));
}



void load_data_from_EEPROM( void ) {
	if( eeprom_read_byte( &config_data_EEM.first_byte ) != FIRST_EEPROM_BYTE_VALUE ) {
		global_data.data_corrupted = 1;
		return;
	}
	eeprom_read_block( &global_data.calibration_data, &config_data_EEM.calibration_data, sizeof(T_CALIBRATION_DATA) );
	eeprom_read_block( &global_data.additional_data, &config_data_EEM.additional_data, sizeof(T_ADDITIONAL_DATA));
}



void load_default_data( void ) {
	memcpy_P( &global_data.calibration_data, &default_data_PGM.calibration_data, sizeof(T_CALIBRATION_DATA) );
	memcpy_P( &global_data.additional_data, &default_data_PGM.additional_data, sizeof(T_ADDITIONAL_DATA) );
	save_data();
}



void load_motor_profile( T_CALIBRATION_DATA * profile ) {
	memcpy_P( &global_data.calibration_data, profile, sizeof(T_CALIBRATION_DATA));
}
