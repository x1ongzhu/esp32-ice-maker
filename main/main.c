/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "blufi_setup.h"
#include "mqtt.h"
#include "ice_maker.h"
// #include <u8g2.h>
// #include "u8g2_esp32_hal.h"

// SDA - GPIO21
#define PIN_SDA 16

// SCL - GPIO22
#define PIN_SCL 17

void app_main(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    setup_blufi();

    init_ice_maker();

    // u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    // u8g2_esp32_hal.sda = PIN_SDA;
    // u8g2_esp32_hal.scl = PIN_SCL;
    // u8g2_esp32_hal_init(u8g2_esp32_hal);

    // u8g2_t u8g2; // a structure which will contain all the data for one display
    // u8g2_Setup_ssd1306_i2c_128x80_noname_f(
    //     &u8g2,
    //     U8G2_R1,
    //     //u8x8_byte_sw_i2c,
    //     u8g2_esp32_i2c_byte_cb,
    //     u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
    // u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

    // u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

    // u8g2_SetPowerSave(&u8g2, 0); // wake up display
    // u8g2_ClearBuffer(&u8g2);
    // // u8g2_DrawBox(&u8g2, 0, 26, 80, 6);
    // // u8g2_DrawFrame(&u8g2, 0, 26, 100, 6);

    // u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese1);
    // u8g2_DrawStr(&u8g2, 2, 17, "你好Hello!");
    // u8g2_SendBuffer(&u8g2);

}
