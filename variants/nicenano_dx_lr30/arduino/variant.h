/* Derived from variants/promicro/variant.h.
 * Copyright (C) 2023 Seeed K.K. -- MIT License
 */
#pragma once
#include "WVariant.h"

#define VARIANT_MCK (64000000ul)
#define USE_LFRC
#define PINS_COUNT (23)
#define NUM_DIGITAL_PINS (23)
#define NUM_ANALOG_INPUTS (3)
#define NUM_ANALOG_OUTPUTS (0)
#define ADC_RESOLUTION 12

#define PIN_SERIAL1_TX (1)
#define PIN_SERIAL1_RX (0)
#define WIRE_INTERFACES_COUNT 1
#define PIN_WIRE_SDA (6)
#define PIN_WIRE_SCL (7)
#define SPI_INTERFACES_COUNT 1
#define PIN_SPI_SCK (12)
#define PIN_SPI_MISO (15)
#define PIN_SPI_MOSI (14)
#define PIN_SPI_NSS (13)

#define PIN_LED (22)
#define LED_PIN PIN_LED
#define LED_BLUE PIN_LED
#define LED_BUILTIN PIN_LED
#define LED_STATE_ON 1
