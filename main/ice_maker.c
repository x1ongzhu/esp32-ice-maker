
#include "ice_maker.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#define GPIO_INPUT_PIN_SEL  ((1ULL<<LED_BIG_ICE) | (1ULL<<LED_SMALL_ICE))
#define ESP_INTR_FLAG_DEFAULT 0
static const char *TAG = "ICE_MAKER";

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

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
    // gpio_set_direction(LED_BIG_ICE, GPIO_MODE_INPUT);
    // gpio_set_direction(LED_SMALL_ICE, GPIO_MODE_INPUT);
    // gpio_set_direction(BTN_START, GPIO_MODE_OUTPUT);
    // gpio_set_direction(BTN_SEL, GPIO_MODE_OUTPUT);

    // gpio_pulldown_en(LED_BIG_ICE);
    // gpio_pulldown_en(LED_SMALL_ICE);

    // gpio_set_level(BTN_START, 1);
    // gpio_set_level(BTN_SEL, 1);

    // xTaskCreate(check_status_task, "check_status", 2048, NULL, 10, NULL);

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(LED_BIG_ICE, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(LED_SMALL_ICE, GPIO_INTR_POSEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(LED_BIG_ICE, gpio_isr_handler, (void *)LED_BIG_ICE);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(LED_SMALL_ICE, gpio_isr_handler, (void *)LED_SMALL_ICE);

}
void start_making_ice(void) {}
void stop_making_ice(void) {}