#ifndef _NODE_H_
#define _NODE_H_


/**************************************************
 * MACRO definitions
 *************************************************/

/* green: LED1 */
#define node_turn_on_green_led()      BSP_TURN_ON_LED1()
#define node_turn_off_green_led()     BSP_TURN_OFF_LED1()
#define node_toggle_green_led()       BSP_TOGGLE_LED1()
#define node_is_green_led_on()        BSP_LED1_IS_ON()


/* red: LED2 */
#define node_turn_on_red_led()      BSP_TURN_ON_LED2()
#define node_turn_off_red_led()     BSP_TURN_OFF_LED2()
#define node_toggle_red_led()       BSP_TOGGLE_LED2()
#define node_is_red_led_on()        BSP_LED2_IS_ON()


/**************************************************
 * PUBLIC FUNCTIONS
 *************************************************/
bool node_init(void);



#endif // _NODE_H_
// eof...
