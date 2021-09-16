#pragma once
#include "driver/gpio.h"

#define LED_BIG_ICE 21
#define LED_SMALL_ICE 19
#define BTN_START 12
#define BTN_SEL 14

static unsigned int ice_maker_on = 0;

void init_ice_maker(void);

void start_making_ice(void);

void stop_making_ice(void);