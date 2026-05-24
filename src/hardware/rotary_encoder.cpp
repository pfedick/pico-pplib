#include "pico/stdlib.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pico/types.h"
#include "hardware/gpio.h"

#include "picopplib.h"
#include "picopplib-grafix.h"
#include "rotary_encoder.h"

static uint32_t instance_count = 0;
static RotaryEncoder* instances[32];

static void encoder_callback(uint gpio, uint32_t events)
{
    if (!instance_count) return;
    if (gpio < 32 && instances[gpio] != nullptr) {
        instances[gpio]->handle_interrupt(gpio, events);
    }
}

RotaryEncoder::RotaryEncoder()
{
    clk_pin = dt_pin = switch_pin = 0;
    is_initialized = false;
    if (instance_count == 0) {
        gpio_set_irq_callback(encoder_callback);
        irq_set_enabled(IO_IRQ_BANK0, true); // Schaltet die GPIO-Interrupts auf CPU-Ebene scharf!
        for (int i = 0; i < 32; i++)
            instances[i] = nullptr;
    }
    instance_count++;
}

RotaryEncoder::~RotaryEncoder()
{
    if (is_initialized) {
        gpio_set_irq_enabled(clk_pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(dt_pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
        gpio_set_irq_enabled(switch_pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
        instances[clk_pin] = nullptr;
        instances[dt_pin] = nullptr;
        instances[switch_pin] = nullptr;
    }
    if (instance_count == 1) {
        gpio_set_irq_callback(nullptr);
        irq_set_enabled(IO_IRQ_BANK0, false);
    }
    if (instance_count > 0) instance_count--;
}

void RotaryEncoder::init(uint8_t clk_pin, uint8_t dt_pin, uint8_t switch_pin)
{
    // GPIO-Pins initialisieren
    if (clk_pin > 31 || dt_pin > 31 || switch_pin > 31) throw picopplib::Exception("Invalid GPIO Pin");
    this->clk_pin = clk_pin;
    this->dt_pin = dt_pin;
    this->switch_pin = switch_pin;
    gpio_init(clk_pin);
    gpio_init(dt_pin);
    gpio_init(switch_pin);

    gpio_set_dir(clk_pin, GPIO_IN);
    gpio_set_dir(dt_pin, GPIO_IN);
    gpio_set_dir(switch_pin, GPIO_IN);

    // Pull-Ups aktivieren (KY-040 hat meist schon welche, schadet aber nicht)
    gpio_pull_up(clk_pin);
    gpio_pull_up(dt_pin);
    gpio_pull_up(switch_pin);

    gpio_set_irq_enabled(clk_pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(dt_pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(switch_pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    instances[clk_pin] = this;
    instances[dt_pin] = this;
    instances[switch_pin] = this;
    is_initialized = true;
}

void RotaryEncoder::handle_interrupt(uint gpio, uint32_t events)
{
    if (gpio == clk_pin || gpio == dt_pin) {

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_encoder_time < 1) {
            return;
        }
        last_encoder_time = now;

        bool clk = gpio_get(clk_pin);
        bool dt = gpio_get(dt_pin);
        uint8_t new_state = (clk << 1) | dt;

        // Nur bei Zustand 00 zählen (Raste)
        if (new_state == 0 && encoder_state != 0) {
            if (encoder_state == 1) {
                encoder_position++;
            } else if (encoder_state == 2) {
                encoder_position--;
            }
        }

        encoder_state = new_state;

    } else if (gpio == switch_pin) {
        if (events & GPIO_IRQ_EDGE_FALL) {
            button_pressed = true;
        } else if (events & GPIO_IRQ_EDGE_RISE) {
            button_pressed = false;
        }
    }
}

void RotaryEncoder::setPosition(int pos)
{
    encoder_position = pos;
}