#include "./vibrator.h"

void device_vibrate(uint8_t count)
{
    xTaskCreate(device_vibrate_task, "Vibrator Task", configMINIMAL_STACK_SIZE, (void*)(intptr_t)count, TASK_PRIORITY_IDLE, NULL);
}

void device_vibrate_task(void* pvParameters)
{
    uint8_t count = (int)(intptr_t)pvParameters;

    for (uint8_t i = 0; i < count; i++)
    {
        if (i != 0) vTaskDelay(pdMS_TO_TICKS(300));

        digitalWrite(88, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        digitalWrite(88, 0);
    }
}