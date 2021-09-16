#pragma once
#include "driver/gpio.h"

#define LED_BIG_ICE GPIO_NUM_12
#define LED_SMALL_ICE GPIO_NUM_14
#define BTN_START GPIO_NUM_21
#define BTN_SEL GPIO_NUM_19

static unsigned int ice_maker_on = 0;

void init_ice_maker(void);

void start_making_ice(void);

void stop_making_ice(void);