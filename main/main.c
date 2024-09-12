#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t MyFirstTaskHandle = NULL;
TaskHandle_t MySecondTaskHandle = NULL; // แก้ไขชื่อจาก MySeconeTaskHandle เป็น MySecondTaskHandle

void My_First_Task(void * arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My First Task %d\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i++;

        if(i == 5)
        {
            if(MySecondTaskHandle != NULL) // ตรวจสอบว่า task ถูกสร้างหรือไม่
            {
                vTaskSuspend(MySecondTaskHandle);
                printf("Second Task suspended\n");
            }
        }

        if(i == 10)
        {
            if(MySecondTaskHandle != NULL) // ตรวจสอบว่า task ถูกสร้างหรือไม่
            {
                vTaskResume(MySecondTaskHandle);
                printf("Second Task resumed\n");
            }
        }

        if(i == 15)
        {
            if(MySecondTaskHandle != NULL) // ตรวจสอบว่า task ถูกสร้างหรือไม่
            {
                vTaskDelete(MySecondTaskHandle);
                MySecondTaskHandle = NULL; // ตั้งค่าเป็น NULL หลังจากลบ task
                printf("Second Task deleted\n");
            }
        }

        if(i == 20)
        {
            printf("MyFirstTaskHandle will suspend itself\n");
            vTaskSuspend(NULL); // ระงับตัวเอง
        }
    }
}

void My_Second_Task(void * arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My Second Task %d\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i++;
    }
}

void app_main(void)
{
    // สร้าง My_First_Task
    if(xTaskCreate(My_First_Task, "First_Task", 4096, NULL, 10, &MyFirstTaskHandle) != pdPASS)
    {
        printf("Failed to create My_First_Task\n");
    }
    
    // สร้าง My_Second_Task และกำหนดให้รันบน Core 1
    if(xTaskCreatePinnedToCore(My_Second_Task, "Second_Task", 4096, NULL, 10, &MySecondTaskHandle, 1) != pdPASS)
    {
        printf("Failed to create My_Second_Task\n");
    }
}
