/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "mqtt.h"
#include "utils.h"
#include "driver/gpio.h"
#define LED_NO_WATER GPIO_NUM_2
#define LED_FULL GPIO_NUM_15
#define LED_BIG_ICE GPIO_NUM_22
#define LED_SMALL_ICE GPIO_NUM_23
#define BTN_START GPIO_NUM_12
#define BTN_SEL GPIO_NUM_34
static const char *TAG = "MQTT_EXAMPLE";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void check_status()
{
    int a = 0, b = 0, c = 0, d = 0;
    for (int i = 0; i < 1000; i++)
    {
        a += gpio_get_level(LED_NO_WATER);
        b += gpio_get_level(LED_FULL);
        c += gpio_get_level(LED_BIG_ICE);
        d += gpio_get_level(LED_SMALL_ICE);
        vTaskDelay(1 / portTICK_RATE_MS);
    }
    ESP_LOGI("ICE_MAKER", "LED_NO_WATER: %d LED_FULL:%d LED_BIG_ICE:%d LED_SMALL_ICE:%d", a, b, c, d);
}

void check_status_task()
{
    while (true)
    {
        check_status();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "home/ice_maker/set", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (cmd_equal("home/ice_maker/set", event->topic, event->topic_len))
        {
            if (cmd_equal("ON", event->data, event->data_len))
            {
                gpio_set_level(12, 0);
                vTaskDelay(200 / portTICK_RATE_MS);
                gpio_set_level(12, 1);
                esp_mqtt_client_publish(event->client, "home/ice_maker/get", "ON", 0, 1, 0);
            }
            else if (cmd_equal("OFF", event->data, event->data_len))
            {
                esp_mqtt_client_publish(event->client, "home/ice_maker/get", "OFF", 0, 1, 0);
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void setup_mqtt(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://192.168.50.242",
        .username = "drew",
        .password = "x1ongDrew"};

    esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    xTaskCreate(check_status_task, "check_status", 2048, NULL, 10, NULL);
}
