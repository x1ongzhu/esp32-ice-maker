
#include "ice_maker.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "ICE_MAKER";

void check_status_task()
{
    while (true)
    {
        int a = 0, b = 0, c = 0, d = 0;
        for (int i = 0; i < 1000; i++)
        {
            c += gpio_get_level(LED_BIG_ICE);
            d += gpio_get_level(LED_SMALL_ICE);
            vTaskDelay(1 / portTICK_RATE_MS);
        }
        ESP_LOGI(TAG, "LED_NO_WATER: %d LED_FULL:%d LED_BIG_ICE:%d LED_SMALL_ICE:%d", a, b, c, d);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void init_ice_maker(void)
{
    gpio_set_direction(LED_BIG_ICE, GPIO_MODE_INPUT);
    gpio_set_direction(LED_SMALL_ICE, GPIO_MODE_INPUT);
    gpio_set_direction(BTN_START, GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN_SEL, GPIO_MODE_OUTPUT);

    gpio_pulldown_en(LED_BIG_ICE);
    gpio_pulldown_en(LED_SMALL_ICE);

    gpio_set_level(BTN_START, 1);
    gpio_set_level(BTN_SEL, 1);

    xTaskCreate(check_status_task, "check_status", 2048, NULL, 10, NULL);
}
void start_making_ice(void) {}
void stop_making_ice(void) {}