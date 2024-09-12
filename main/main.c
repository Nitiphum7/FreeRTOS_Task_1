#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t MyFirstTaskHandle = NULL;

void My_First_Task(void* arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My Frist Task %d\n", i);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        i++;
    }
}

void app_main(void)
{
    xTaskCreate(My_First_Task, "Frist_Task", 4096 , NULL ,10 ,&MyFirstTaskHandle);
}