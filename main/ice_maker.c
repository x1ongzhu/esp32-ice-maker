
#include "ice_maker.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pcnt.h"
#include "mqtt.h"

typedef enum
{
    ICE_MAKER_STATUS_IDEL,
    ICE_MAKER_STATUS_WORKING
} ice_maker_status_t;

static const char *TAG = "ICE_MAKER";
static ice_maker_status_t current_status;

static void
pcnt_init(int unit, int gpio)
{
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = gpio,
        .ctrl_gpio_num = -1,
        .channel = PCNT_CHANNEL_0,
        .unit = unit,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 10000,
        .counter_l_lim = -10000,
    };
    pcnt_unit_config(&pcnt_config);

    pcnt_set_filter_value(unit, 100);
    pcnt_filter_enable(unit);

    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);

    pcnt_counter_resume(unit);
}

static void status_check_task(void *arg)
{
    while (true)
    {
        int16_t count_0 = 0, count_1 = 0;
        pcnt_get_counter_value(PCNT_UNIT_0, &count_0);
        pcnt_get_counter_value(PCNT_UNIT_1, &count_1);

        ESP_LOGI(TAG, "counter0 value:%d, counter1 value: %d", count_0, count_1);
        char str[80];
        sprintf(str, "counter0 value:%d, counter1 value: %d", count_0, count_1);
        send_msg("/test", (char *)str);

        pcnt_counter_pause(PCNT_UNIT_0);
        pcnt_counter_pause(PCNT_UNIT_1);
        pcnt_counter_clear(PCNT_UNIT_0);
        pcnt_counter_clear(PCNT_UNIT_1);
        pcnt_counter_resume(PCNT_UNIT_0);
        pcnt_counter_resume(PCNT_UNIT_1);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void init_ice_maker(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL << BTN_START) | (1ULL << BTN_SEL));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_set_level(BTN_START, 1);
    gpio_set_level(BTN_SEL, 1);

    pcnt_init(PCNT_UNIT_0, LED_BIG_ICE);
    pcnt_init(PCNT_UNIT_1, LED_SMALL_ICE);

    xTaskCreate(status_check_task, "status_check_task", 2048, NULL, 10, NULL);
}

void start_making_ice(void)
{
    if (current_status == ICE_MAKER_STATUS_IDEL)
    {
        gpio_set_level(BTN_START, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(BTN_START, 0);
        send_msg("/home/ice_maker/get", "ON");
    }
}

void stop_making_ice(void)
{
    if (current_status == ICE_MAKER_STATUS_WORKING)
    {
        gpio_set_level(BTN_START, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(BTN_START, 0);
        send_msg("/home/ice_maker/get", "OFF");
    }
}