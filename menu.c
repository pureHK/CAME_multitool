#include "menu.h"
#include "data.h"
#include "macros.h"
#include "helpers.h"
#include "program_manager.h"
#include "HD44780_LCD/hd44780_config.h"
#include "subprograms/limit_sw_test.h"
#include "subprograms/encoder_test.h"
#include "subprograms/calibration.h"
#include "subprograms/info.h"
#include "subprograms/encoder_counter.h"
#include "subprograms/settings.h"

#include <avr/pgmspace.h>
#include "subprograms/calibration.h"


static T_MENU m_calibration = {.name = "Kalibracja", .level = 1};
static T_MENU m_bxv_calibration = {.name = "BXV", .level = 2};
static T_MENU m_bkv_calibration = {.name = "BKV", .level = 2};
static T_MENU m_custom_calibration = {.name = "U\001ytkownika", .level = 2};
static T_MENU m_testing = {.name = "Test", .level = 1};
static T_MENU m_tools = {.name = "Narz\005dzia", .level = 1};
static T_MENU m_lim_sw_test = {.name = "Kra\002c\003wki", .level = 2};
static T_MENU m_encoder_test = {.name = "Enkoder", .level = 2};
static T_MENU m_encoder_counter = {.name = "Licznik n-kod", .level = 2};
static T_MENU m_info = {.name = "Informacje", .level = 1};
static T_MENU m_settings = {.name = "Ustawienia", .level = 1};
static T_MENU m_swap_fa_fc = {.name = "Zamie\002 FA z FC", .level = 2};
static T_MENU m_save_settings = {.name = "Zapis ust.", .level = 2};
static T_MENU m_load_settings = {.name = "Reset ust.", .level = 2};
static T_MENU m_load_profile = {.name = "Wybierz profil", .level = 2};
static T_MENU m_set_restrictions = {.name = "Ograniczenia", .level = 2};
static T_MENU m_user_settings = {.name = "U\001ytkownika", .level = 2};


/* ----------------------------------------- USER MENU ----------------------------------------- */
void menu_init( void ) {

	m_calibration.prev = &m_settings;
	m_calibration.next = &m_tools;
	m_calibration.submenu = &m_bxv_calibration;

		m_bxv_calibration.prev = &m_custom_calibration;
		m_bxv_calibration.next = &m_bkv_calibration;
		m_bxv_calibration.parent = &m_calibration;
		m_bxv_calibration.subprogram = start_bxv_calibration;

		m_bkv_calibration.prev = &m_bxv_calibration;
		m_bkv_calibration.next = &m_custom_calibration;
		m_bkv_calibration.parent = &m_calibration;
		m_bkv_calibration.subprogram = start_bkv_calibration;

		m_custom_calibration.prev = &m_bkv_calibration;
		m_custom_calibration.next = &m_bxv_calibration;
		m_custom_calibration.parent = &m_calibration;
		m_custom_calibration.subprogram = start_custom_calibration;

// ---------------------

	m_tools.prev = &m_calibration;
	m_tools.next = &m_testing;
	m_tools.submenu = &m_encoder_counter;

		m_encoder_counter.parent = &m_tools;
		m_encoder_counter.subprogram = start_encoder_counter;

// ---------------------

	m_testing.prev = &m_tools;
	m_testing.next = &m_info;
	m_testing.submenu = &m_encoder_test;

		m_lim_sw_test.parent = &m_testing;
		m_lim_sw_test.prev = &m_encoder_test;
		m_lim_sw_test.next = &m_encoder_test;
		m_lim_sw_test.subprogram = start_limit_sw_test;

		m_encoder_test.parent = &m_testing;
		m_encoder_test.prev = &m_lim_sw_test;
		m_encoder_test.next = &m_lim_sw_test;
		m_encoder_test.subprogram = start_encoder_test;

// ---------------------

	m_info.prev = &m_testing;
	m_info.next = &m_settings;
	m_info.subprogram = start_show_version;

// ---------------------

	m_settings.prev = &m_info;
	m_settings.next = &m_calibration;
	m_settings.submenu = &m_load_profile;

		m_save_settings.parent = &m_settings;
		m_save_settings.prev = &m_load_settings;
		m_save_settings.next = &m_load_profile;
		m_save_settings.subprogram = start_save_settings;

		m_load_profile.parent = &m_settings;
		m_load_profile.prev = &m_save_settings;
		m_load_profile.next = &m_swap_fa_fc;
		m_load_profile.subprogram = start_load_profile;

		m_swap_fa_fc.parent = &m_settings;
		m_swap_fa_fc.prev = &m_load_profile;
		m_swap_fa_fc.next = &m_set_restrictions;
		m_swap_fa_fc.subprogram = start_swap_fa_fc;

		m_set_restrictions.parent = &m_settings;
		m_set_restrictions.prev = &m_swap_fa_fc;
		m_set_restrictions.next = &m_user_settings;
		m_set_restrictions.subprogram = start_set_restriction;

		m_user_settings.parent = &m_settings;
		m_user_settings.prev = &m_set_restrictions;
		m_user_settings.next = &m_load_settings;
		m_user_settings.subprogram = 0;

		m_load_settings.parent = &m_settings;
		m_load_settings.prev = &m_user_settings;
		m_load_settings.next = &m_save_settings;
		m_load_settings.subprogram = start_reset_settings;

// ----------------------------------------------------------
	gd->current_menu = &m_calibration;
	show_user_menu();
}



static void show_menu_name( T_MENU * menu, uint8_t blank_fields ) {

	show_option_name( menu->name, blank_fields);
}



void show_user_menu( void ) {

	if( gd->current_menu->level == 0 || gd->current_menu->level == 1 ) {
		lcd_clear( 0 );
		lcd_print_str_P( PSTR("\006") );

		show_menu_name( gd->current_menu, 14 );

		lcd_goto_XY( 15, 0 );
		lcd_print_str_P( PSTR("\007") );
	}

	if( gd->current_menu->level == 1 ) {
		if( gd->current_menu->submenu ) {
			lcd_clear( 2 );
			show_menu_name( gd->current_menu->submenu, 16 );
		}
	}

	if( gd->current_menu->level == 2 ) {
		lcd_clear( 1 );

		if( gd->current_menu->parent ) {
			gd->current_menu->parent->submenu = gd->current_menu;
			show_menu_name( gd->current_menu->parent, 16 );
		}

		lcd_clear( 2 );
		if( gd->current_menu->prev )
			lcd_print_str_P( PSTR("\006") );
		else lcd_print_str_P( PSTR(">") );

		show_menu_name( gd->current_menu, 14 );

		lcd_goto_XY( 15, 1 );
		if( gd->current_menu->next )
			lcd_print_str_P( PSTR("\007") );
		else lcd_print_str_P( PSTR("<") );
	}

}




void user_menu( uint8_t button_status ) {

	if( button_status & ENTER ) {
		if( gd->current_menu->submenu )
			gd->current_menu = gd->current_menu->submenu;
		else if ( gd->current_menu->subprogram )
			enter_program( gd->current_menu->subprogram, 1, 0 );
	}

	if( button_status & ESC ) {
		TimerHidden = 250;
		gd->count_hidden_timer = 1;
		gd->in_hidden_procedure = 0;

		if( gd->current_menu->parent )
			gd->current_menu = gd->current_menu->parent;
	}

	if( button_status & LEFT ) {
		if( gd->current_menu->prev )
			gd->current_menu = gd->current_menu->prev;
	}

	if( button_status & RIGHT ) {
		if( gd->current_menu->next )
			gd->current_menu = gd->current_menu->next;
	}

	if( continue_program( user_menu ) ) show_user_menu();
}

/* ----------------------------------- (END) USER MENU ----------------------------------------- */

