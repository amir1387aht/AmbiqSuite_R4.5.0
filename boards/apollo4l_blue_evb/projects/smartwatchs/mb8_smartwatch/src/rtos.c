#include "rtos.h"

TaskHandle_t xSetupTask;
TaskHandle_t xWDTTask;
TaskHandle_t xLvglTask;
TaskHandle_t xLightSensTask;
TaskHandle_t xAcclSensTask;

void run_tasks(void)
{
    xTaskCreate(mcu_init, "Setup Task", configMINIMAL_STACK_SIZE, 0, TASK_PRIORITY_SUPER_HIGH, &xSetupTask);
    xTaskCreate(wdt_init, "WDT Task", configMINIMAL_STACK_SIZE, 0, TASK_PRIORITY_SUPER_HIGH, &xWDTTask);
    // TODO: Create ble task with TASK_PRIORITY_HIGH priority
    xTaskCreate(lvgl_display_init, "Lvgl Task", 4096, 0, TASK_PRIORITY_MEDIUM, &xLvglTask);
    xTaskCreate(lightSens_task, "LightSensor Task", configMINIMAL_STACK_SIZE, 0, TASK_PRIORITY_IDLE, xLightSensTask);
    xTaskCreate(acclSens_task, "AcclSensor Task", configMINIMAL_STACK_SIZE, 0, TASK_PRIORITY_IDLE, xAcclSensTask);
    vTaskStartScheduler();
}

uint32_t am_freertos_sleep(uint32_t idleTime)
{
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    return 0;
}

//*****************************************************************************
//
// Recovery function called from FreeRTOS IDLE task, after waking up from Sleep
// Do necessary 'wakeup' operations here, e.g. to power up/enable peripherals etc.
//
//*****************************************************************************
void am_freertos_wakeup(uint32_t idleTime)
{
    return;
}


//*****************************************************************************
//
// FreeRTOS debugging functions.
//
//*****************************************************************************
void vApplicationMallocFailedHook(void)
{
    const char* pTaskName;

    //
    // Called if a call to pvPortMalloc() fails because there is insufficient
    // free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    // internally by FreeRTOS API functions that create tasks, queues, software
    // timers, and semaphores.  The size of the FreeRTOS heap is set by the
    // configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
    //
    am_util_stdio_printf("Heap malloc failed!\r\n");

    pTaskName = pcTaskGetName(NULL);

    am_util_stdio_printf("Suspend %s task...\r\n", pTaskName);

    vTaskSuspend(NULL);
}

void
vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    //
    // Run time stack overflow checking is performed if
    // configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    // function is called if a stack overflow is detected.
    //
    while (1) __asm("BKPT #0\n"); // Break into the debugger
}