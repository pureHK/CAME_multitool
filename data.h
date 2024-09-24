#ifndef DATA_H_HK_
#define DATA_H_HK_

#include <avr/io.h>

#include "menu.h"
#include "program_manager.h"




typedef struct {
	char name[15];
	uint32_t max_impulses;
	uint8_t imp_per_rotation;
	uint8_t worm_gear_teeth_count;
	uint8_t gear_module;
	uint8_t gear_teeth_count;
	uint16_t gear_diameter; // hundreds of mm

} T_CALIBRATION_DATA;


typedef struct {
	uint8_t fa_fc_swap;
	uint8_t naked;
	uint8_t restrict_numerator;
	uint8_t restrict_denominator;
} T_ADDITIONAL_DATA;


typedef struct {
	uint8_t first_byte;
	T_CALIBRATION_DATA calibration_data;
	T_ADDITIONAL_DATA additional_data;
} T_STORED_DATA;


struct data {
	// ISR specific
	volatile int32_t encoder_count;
	volatile int32_t enctime;
	volatile uint8_t encoder_increment;
	volatile int32_t encoder_aux_count;
	volatile int32_t enctime_aux;
	volatile uint8_t encoder_aux_increment;

	// program specific
	T_MENU *current_menu;
	T_PROGRAM_CONTEXT *current_program_context;

	T_CALIBRATION_DATA calibration_data;
	T_ADDITIONAL_DATA additional_data;

	int8_t profile_idx;
	uint8_t data_corrupted;
	uint8_t in_hidden_procedure;
	uint8_t volatile count_hidden_timer;
};



extern void save_data( void );
extern void load_data_from_EEPROM( void );
extern void load_default_data( void );
extern void load_motor_profile( T_CALIBRATION_DATA * profile );



extern volatile uint16_t TimerLong;
extern volatile uint8_t TimerShort1;
extern volatile uint8_t TimerShort2;
extern volatile uint8_t TimerHidden;
extern volatile uint16_t led_blink_timer;

extern struct data *gd;
//extern const T_CALIBRATION_DATA cal_data_BXV;
//extern const T_CALIBRATION_DATA cal_data_BKV;
extern const T_CALIBRATION_DATA * const profile[];



extern const T_CALIBRATION_DATA cal_data_BXV;
extern const T_CALIBRATION_DATA cal_data_BKV;



#endif /* DATA_H_HK_ */
