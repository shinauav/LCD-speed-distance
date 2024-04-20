#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/flash.h"
#include "driverlib/eeprom.h"
#include "helper_library/helper.h"
#include "driverlib/i2c.h"
#include "FreeRTOSConfig.h"
#include "ssd1315.h"
#include "FreeRTOS.h"
#include "task.h"

#define SEQ_NUM 0
#define SEQ_PRIO 0
#define STEP_NUM 0
#define STACK_SIZE 512
#define MAX_STR_LEN 20
#define SPEED_LABEL_LEN 6
#define DISTANCE_LABEL_LEN 10
#define EEPROM_ADDRESS 0x00000000

volatile uint32_t sensor_value;
volatile uint32_t distance_covered;
volatile uint32_t time;

void init_eeprom()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
    {
    }
    uint32_t ui32EEPROMInit = EEPROMInit();

    if (ui32EEPROMInit != EEPROM_INIT_OK)
    {
        while (1)
        {
        }
    }

}

void saveState(uint32_t sensor_valueEEPROM)
{
    uint32_t state_word = sensor_valueEEPROM;
    EEPROMProgramNonBlocking(state_word, EEPROM_ADDRESS);

}

int loadState()
{
    uint32_t state_word;
    EEPROMRead(&state_word, EEPROM_ADDRESS, sizeof(state_word));
    return state_word;
}

void write_static_text()
{
    const char speed_local[] = "Speed:  m/s";
    const char distance_local[] = "Covered:     m";

    SSD1315_SetCursorPosition(1, 0);
    SSD1315_WriteString((uint8_t*) speed_local, sizeof(speed_local) - 1);
    SSD1315_SetCursorPosition(2, 0);
    SSD1315_WriteString((uint8_t*) distance_local, sizeof(distance_local) - 1);

}

void Timer1IntHandler()
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    if (time >= 60)
    {
        saveState(distance_covered);
        time = 0;
        //save to eeprom
    }
}

void init_timer()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {

    }
    TimerClockSourceSet(TIMER1_BASE, TIMER_CLOCK_PIOSC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerEnable(TIMER1_BASE, TIMER_A);
    TimerLoadSet(TIMER1_BASE, TIMER_A, 16000000);

    TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1IntHandler);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}

void init_ADC()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, SEQ_NUM, ADC_TRIGGER_PROCESSOR,
    SEQ_PRIO);
    ADCSequenceStepConfigure(ADC0_BASE, SEQ_NUM, STEP_NUM,
    ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
    ADCSequenceEnable(ADC0_BASE, SEQ_NUM);
}

void init_i2c()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    I2CMasterEnable(I2C1_BASE);
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);

}
void init_oled()
{
    SSD1315_Init(I2C1_BASE);
    SSD1315_Clear();
}

void ADC_task(void *params)
{
    uint32_t potentiometer_raw_value;
    uint32_t potentiometer_sensor_value;

    uint8_t speed_local;
    uint8_t distance_local;

    char distanceCovered_string[MAX_STR_LEN];
    char speed_string[MAX_STR_LEN];

    uint8_t distance_string_len;
    uint8_t speed_string_len;

    write_static_text();

    while (1)
    {


        ADCProcessorTrigger(ADC0_BASE, SEQ_NUM);
        while (!ADCIntStatus(ADC0_BASE, 0, false))
        {
            ;
        }
        ADCSequenceDataGet(ADC0_BASE, SEQ_NUM, &potentiometer_raw_value);
        potentiometer_sensor_value = potentiometer_raw_value * 25 / 4095;

        sensor_value = potentiometer_sensor_value;
        distance_covered = distance_covered + sensor_value;

        speed_local = sensor_value;
        distance_local = distance_covered;

        speed_string_len = sprintf(speed_string, "%3u", speed_local);
        SSD1315_SetCursorPosition(1, SPEED_LABEL_LEN);
        SSD1315_WriteString((uint8_t*) speed_string, speed_string_len);

        distance_string_len = sprintf(distanceCovered_string, "%2u", distance_local);
        SSD1315_SetCursorPosition(2, DISTANCE_LABEL_LEN);
        SSD1315_WriteString((uint8_t*) distanceCovered_string, distance_string_len);

        vTaskDelay(1000);
    }
}

void init()
{
    helper_library_init();
    init_i2c();
    init_ADC();
    init_oled();
    init_timer();
    init_eeprom();

}

int main(void)
{

    init();

    distance_covered = loadState();
    xTaskCreate(ADC_task, "SVE", STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

}

