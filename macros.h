#ifndef MACROS_H_HK_
#define MACROS_H_HK_


#define ENTER (1 << PD6)
#define ESC   (1 << PD5)
#define LEFT  (1 << PD7)
#define RIGHT (1 << PD4)


#define DEBUG_LED_DDR  DDRB
#define DEBUG_LED_PORT PORTB
#define DEBUG_LED_IN   PINB
#define DEBUG_LED_PIN  (1 << PB0)

#define DEBUG_LED_INIT ( DEBUG_LED_DDR  |=  DEBUG_LED_PIN )
#define DEBUG_LED_ON   ( DEBUG_LED_PORT |=  DEBUG_LED_PIN )
#define DEBUG_LED_OFF  ( DEBUG_LED_PORT &= ~DEBUG_LED_PIN )
#define DEBUG_LED_TOG  ( DEBUG_LED_PORT ^=  DEBUG_LED_PIN )


#define FC_DDR  DDRB
#define FC_PORT PORTB
#define FC_PIN  (1 << PB4)

#define FC_INIT ( FC_DDR  |=  FC_PIN )
#define FC_ON   ( FC_PORT |=  FC_PIN )
#define FC_OFF  ( FC_PORT &= ~FC_PIN )
#define FC_TOG  ( FC_PORT ^=  FC_PIN )


#define FA_DDR  DDRB
#define FA_PORT PORTB
#define FA_PIN  (1 << PB5)

#define FA_INIT ( FA_DDR  |=  FA_PIN )
#define FA_ON   ( FA_PORT |=  FA_PIN )
#define FA_OFF  ( FA_PORT &= ~FA_PIN )
#define FA_TOG  ( FA_PORT ^=  FA_PIN )



#define PROFILE_SIZE 2


#define TAB_SIZE(x) (sizeof(x)/sizeof(*x))



#endif
