#include "FreeRTOS.h"
#include "task.h"


/* This function gets called in case of a stack overflow.
 * Hopefully, this function will never be called.
 * In case that the function gets called, try increasing the stack size.
 */
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    while(1)
    {
        ;
    }
}
