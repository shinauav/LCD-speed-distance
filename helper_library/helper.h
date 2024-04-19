#include <stdbool.h>
#include <stdint.h>


/* Description: Initialize the helper library */
void helper_library_init();


/* Description: Turn on the red LED on the Tiva board */
void turn_on_red_led();


/* Description: Turn on the blue LED on the Tiva board */
void turn_on_blue_led();


/* Description: Turn on the green LED on the Tiva board */
void turn_on_green_led();


/*
 * Description: Turn on the red LED on the Sensor Kit board
 * Assumption: Sensor Kit's D6 pin is connected to Tiva's PA2 pin
 */

void turn_on_sensor_kit_led();


/* Description: Turn off the red LED on the Tiva board */
void turn_off_red_led();


/* Description: Turn off the blue LED on the Tiva board */
void turn_off_blue_led();


/* Description: Turn off the green LED on the Tiva board */
void turn_off_green_led();


/*
 * Description: Turn off the red LED on the Sensor Kit board
 * Assumption: Sensor Kit's D6 pin is connected to Tiva's PA2 pin
 */
void turn_off_sensor_kit_led();


/* Description: Check whether Tiva's user switch 1 is pressed */
bool read_user_switch1();


/* Description: Check whether Tiva's user switch 2 is pressed */
bool read_user_switch2();


/*
 * Description: Check whether Sensor Kit's button is pressed
 * Assumption: Sensor Kit's D7 pin is connected to Tiva's PA3 pin
 */
bool read_sensor_kit_button();


/*
 * Description:
 *      Wait for a given number of milliseconds.
 *      The delay is realized via a busy-wait loop, and is not accurate (especially for small delays).
 */
void delay(uint32_t milliseconds);


/* Description: A printf replacement that uses UART0 over USB to print given string to a PC serial terminal. */
void print(char const * format, ...);


/* Description: A scanf replacement that uses UART0 over USB to get data from PC serial terminal. */
int scan(char const * format, ...);
