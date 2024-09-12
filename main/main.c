#include <stdio.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t MyFirstTaskHandle = NULL;
TaskHandle_t MySecondTaskHandle = NULL;
TaskHandle_t ISR = NULL;

#define LED_PIN 27         
#define PUSH_BUTTON_PIN 33

void interrupt_task(void *arg)
{
    bool led_status = false;
    while(1)
    {
        vTaskSuspend(NULL);
        led_status = !led_status;
        gpio_set_level(LED_PIN, led_status);
        printf("Button pressed!\n");
    }
}

void IRAM_ATTR button_isr_handler(void *arg)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xTaskResumeFromISR(ISR);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

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
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    esp_rom_gpio_pad_select_gpio(PUSH_BUTTON_PIN);

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(PUSH_BUTTON_PIN, GPIO_MODE_INPUT);

    gpio_set_intr_type(PUSH_BUTTON_PIN, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(PUSH_BUTTON_PIN, button_isr_handler, NULL);

    // สร้าง tasks
    xTaskCreate(interrupt_task, "interrupt_task", 4096, NULL, 10, &ISR);
    
    if(xTaskCreate(My_First_Task, "First_Task", 4096, NULL, 10, &MyFirstTaskHandle) != pdPASS)
    {
        printf("Failed to create My_First_Task\n");
    }
    
    if(xTaskCreatePinnedToCore(My_Second_Task, "Second_Task", 4096, NULL, 10, &MySecondTaskHandle, 1) != pdPASS)
    {
        printf("Failed to create My_Second_Task\n");
    }
}
