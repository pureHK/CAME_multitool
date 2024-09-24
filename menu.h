#ifndef MENU_H_HK_
#define MENU_H_HK_

#include <avr/io.h>

#include "program_manager.h"



typedef struct menu {
	uint8_t level;

	struct menu *prev;
	struct menu *next;

	struct menu *parent;
	struct menu *submenu;

	T_INPUT_PROC subprogram;
	char name[15];

} T_MENU;





extern void menu_init( void );
extern void show_user_menu( void );

extern void user_menu( uint8_t button_status );








#endif /* MENU_H_HK_ */
