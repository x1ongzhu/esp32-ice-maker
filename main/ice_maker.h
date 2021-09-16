#pragma once
#include "driver/gpio.h"

#define LED_BIG_ICE 12
#define LED_SMALL_ICE 14
#define BTN_START 21
#define BTN_SEL 19

static unsigned int ice_maker_on = 0;

void init_ice_maker(void);

void start_making_ice(void);

void stop_making_ice(void);