#pragma once
#include "driver/gpio.h"

#define LED_BIG_ICE 22
#define LED_SMALL_ICE 19
#define BTN_START 21
#define BTN_SEL 25

void init_ice_maker(void);

void start_making_ice(void);

void stop_making_ice(void);