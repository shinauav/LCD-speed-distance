#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"


#define SENSOR_KIT_LED_PIN      GPIO_PIN_2
#define SENSOR_KIT_BUTTON_PIN   GPIO_PIN_3
#define TIVA_RED_LED_PIN        GPIO_PIN_1
#define TIVA_BLUE_LED_PIN       GPIO_PIN_2
#define TIVA_GREEN_LED_PIN      GPIO_PIN_3
#define TIVA_USER_SWITCH_1_PIN  GPIO_PIN_4
#define TIVA_USER_SWITCH_2_PIN  GPIO_PIN_0
#define MAX_STR_LEN 100


/* Initializing UART Stdio Console, necessary to be called before print/scan functions */
static void initUart()
{
    // Enable the UART0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Wait until the UART0 peripheral becomes ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
    {
        ;
    }

    // Configure the PA0 & PA1 pins for UART function
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);

    // Configure communication parameters
    UARTConfigSetExpClk(
        UART0_BASE,
        SysCtlClockGet(),
        38400,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)
    );

    // Config Stdio over UART
    UARTStdioConfig(0, 38400, SysCtlClockGet());
}


/* Description: Initialize the helper library */
void helper_library_init()
{
    // Enable the entire GPIOF peripheral (used for Tiva LEDs & switches)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure pins for red, green & blue LEDs as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, TIVA_RED_LED_PIN | TIVA_BLUE_LED_PIN | TIVA_GREEN_LED_PIN);

    // Unlock PF0; it must be unlocked, since it is a 'special' pin (it allows non-maskable interrupts)
    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_0);

    // Configure pins for user switch 1 & user switch 2 for GPIO input functionality
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, TIVA_USER_SWITCH_1_PIN | TIVA_USER_SWITCH_2_PIN);

    // Configure pins for user switch 1 & user switch 2 to allow input signals
    GPIODirModeSet(GPIO_PORTF_BASE, TIVA_USER_SWITCH_1_PIN | TIVA_USER_SWITCH_2_PIN, GPIO_DIR_MODE_IN);

    // Configure pins for user switch 1 & user switch 2 with a weak pull-up resistor
    GPIOPadConfigSet(GPIO_PORTF_BASE, TIVA_USER_SWITCH_1_PIN | TIVA_USER_SWITCH_2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Enable the entire GPIOA peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure pin for the Sensor Kit LED at D6 as output
    // Assumption: Sensor Kit's D6 is connected to Tiva's PA2
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, SENSOR_KIT_LED_PIN);

    // Configure pin for the Sensor Kit button at D4 as input
    // Assumption: Sensor Kit's D4 is connected to Tiva's PA3
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, SENSOR_KIT_BUTTON_PIN);

    // Init UART for terminal communication
    initUart();
}


/* Description: Turn on the red LED on the Tiva board */
void turn_on_red_led()
{
    GPIOPinWrite(GPIO_PORTF_BASE, TIVA_RED_LED_PIN, 0xFF);
}


/* Description: Turn on the blue LED on the Tiva board */
void turn_on_blue_led()
{
    GPIOPinWrite(GPIO_PORTF_BASE, TIVA_BLUE_LED_PIN, 0xFF);
}


/* Description: Turn on the green LED on the Tiva board */
void turn_on_green_led()
{
    GPIOPinWrite(GPIO_PORTF_BASE, TIVA_GREEN_LED_PIN, 0xFF);
}


/*
 * Description: Turn on the red LED on the Sensor Kit board
 * Assumption: Sensor Kit's D6 pin is connected to Tiva's PA2 pin
 */

void turn_on_sensor_kit_led()
{
    GPIOPinWrite(GPIO_PORTA_BASE, SENSOR_KIT_LED_PIN, 0xFF);
}


/* Description: Turn off the red LED on the Tiva board */
void turn_off_red_led()
{
    GPIOPinWrite(GPIO_PORTF_BASE, TIVA_RED_LED_PIN, 0x00);
}


/* Description: Turn off the blue LED on the Tiva board */
void turn_off_blue_led()
{
    GPIOPinWrite(GPIO_PORTF_BASE, TIVA_BLUE_LED_PIN, 0x00);
}


/* Description: Turn off the green LED on the Tiva board */
void turn_off_green_led()
{
    GPIOPinWrite(GPIO_PORTF_BASE, TIVA_GREEN_LED_PIN, 0x00);
}


/*
 * Description: Turn off the red LED on the Sensor Kit board
 * Assumption: Sensor Kit's D6 pin is connected to Tiva's PA2 pin
 */
void turn_off_sensor_kit_led()
{
    GPIOPinWrite(GPIO_PORTA_BASE, SENSOR_KIT_LED_PIN, 0x00);
}


/* Description: Check whether Tiva's user switch 1 is pressed */
bool read_user_switch1()
{
    int32_t pin_state = GPIOPinRead(GPIO_PORTF_BASE, TIVA_USER_SWITCH_1_PIN);
    if(pin_state == 0x00)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/* Description: Check whether Tiva's user switch 2 is pressed */
bool read_user_switch2()
{
    int32_t pin_state = GPIOPinRead(GPIO_PORTF_BASE, TIVA_USER_SWITCH_2_PIN);
    if(pin_state == 0x00)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/*
 * Description: Check whether Sensor Kit's button is pressed
 * Assumption: Sensor Kit's D7 pin is connected to Tiva's PA3 pin
 */
bool read_sensor_kit_button()
{
    int32_t pin_state = GPIOPinRead(GPIO_PORTA_BASE, SENSOR_KIT_BUTTON_PIN);
    if(pin_state == 0x00)
    {
        return false;
    }
    else
    {
        return true;
    }
}


/*
 * Description:
 *      Wait for a given number of milliseconds.
 *      The delay is realized via a busy-wait loop, and is not accurate (especially for small delays).
 */
void delay(uint32_t milliseconds)
{
    // Fetch system clock frequency (should be 16 MHz by default)
    uint32_t sys_clock = SysCtlClockGet();

    // Calculate the number of iterations that should be executed
    const uint64_t instructions = ((uint64_t)sys_clock * milliseconds) / 1000;
    const uint32_t iterations = (uint32_t) (instructions / 3);

    // Perform busy-wait
    SysCtlDelay(iterations);
}


/* Description: Print given string to UART0 */
void UART_printf(char* s)
{
    while(*s)
    {
        UARTCharPut(UART0_BASE, *s);
        s++;
    }

}


/* Used for writing formated data on UART */
void print(char const * format, ...){
    va_list args;
    va_start(args, format);

    UARTvprintf(format, args);

    va_end(args);
}


/* Used for reading formatted data from UART */
int scan(char const * format, ...){
   int rc;
   char buffer[MAX_STR_LEN];

   UARTgets(buffer, MAX_STR_LEN);

   va_list args;
   va_start(args, format);

   rc = vsscanf(buffer, format, args);

   va_end(args);

   return rc;
}
