/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* MCAL includes. */
#include "gpio.h"
#include "uart0.h"

#define GREEN_LED 0
#define RED_LED   1
#define Blue_LED  2

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND    369

void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}

/* The HW setup function */
static void prvSetupHardware( void );

/* FreeRTOS tasks */
void vRedLedTask(void *pvParameters);
void vGreenLedTask(void *pvParameters);
void vSyncTask(void *pvParameters);

/* Used to hold the handle of Task1 & Task2 */
TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;


int main()
{
    /* Setup the hardware for use with the Tiva C board. */
    prvSetupHardware();

    /* Create Tasks here */
    xTaskCreate(vSyncTask, "Task 2", 256, NULL, 2, NULL);
    xTaskCreate(vRedLedTask, "Task 1", 256, NULL, 1, &xTask1Handle);
    xTaskCreate(vGreenLedTask, "Task 2", 256, NULL, 1, &xTask2Handle);



    /* Now all the tasks have been started - start the scheduler.

    NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
    The processor MUST be in supervisor mode when vTaskStartScheduler is
    called.  The demo applications included in the FreeRTOS.org download switch
    to supervisor mode prior to main being called.  If you are not using one of
    these demo application projects then ensure Supervisor mode is used here. */
    vTaskStartScheduler();

    /* Should never reach here!  If you do then there was not enough heap
    available for the idle task to be created. */
    for (;;);

}

static void prvSetupHardware( void )
{
    /* Place here any needed HW initialization such as GPIO, UART, etc.  */
    GPIO_BuiltinButtonsLedsInit();
    UART0_Init();
}

void vSyncTask(void *pvParameters)
{
    vTaskSuspend(xTask1Handle);
    vTaskSuspend(xTask2Handle);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));

        vTaskResume(xTask1Handle);
        vTaskDelay(pdMS_TO_TICKS(4000));

        vTaskResume(xTask2Handle);
        vTaskDelay(pdMS_TO_TICKS(4000));

        UART0_SendString("Yellow is On\r\n");
        GPIO_RedLedOn();
        GPIO_GreenLedOn();
        vTaskDelay(pdMS_TO_TICKS(4000));

        GPIO_RedLedOff();
        GPIO_GreenLedOff();

    }
}

void vRedLedTask(void *pvParameters)
{

    for (;;)
    {
        UART0_SendString("Red is On\r\n");
        GPIO_GreenLedOff();
        GPIO_RedLedOn();
        vTaskSuspend(NULL);

    }
}

void vGreenLedTask(void *pvParameters)
{

    for (;;)
    {
        UART0_SendString("Green is On\r\n");
        GPIO_RedLedOff();
        GPIO_GreenLedOn();
        vTaskSuspend(NULL);

    }
}


/*-----------------------------------------------------------*/
