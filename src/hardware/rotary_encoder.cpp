#include "pico/stdlib.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pico/types.h"
#include "hardware/gpio.h"

#include "picopplib.h"
#include "picopplib/hardware/rotary_encoder.h"

// Richtungs-Bitmasken
#define DIR_NONE 0x0
#define DIR_CW 0x10
#define DIR_CCW 0x20

// Zustände der State-Machine
#define R_START 0x0
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

// Ben Buxtons State-Table für Full-Step Encoder (Ruhezustand 11)
static const uint8_t ttable[7][4] = {
    // Pin-Zustände: 00           01           10           11
    /* R_START */ {R_START, R_CW_BEGIN, R_CCW_BEGIN, R_START},
    /* R_CW_FINAL */ {R_CW_NEXT, R_START, R_CW_FINAL, R_START | DIR_CW},
    /* R_CW_BEGIN */ {R_CW_NEXT, R_CW_BEGIN, R_START, R_START},
    /* R_CW_NEXT */ {R_CW_NEXT, R_CW_BEGIN, R_CW_FINAL, R_START},
    /* R_CCW_BEGIN */ {R_CCW_NEXT, R_START, R_CCW_BEGIN, R_START},
    /* R_CCW_FINAL */ {R_CCW_NEXT, R_CCW_FINAL, R_START, R_START | DIR_CCW},
    /* R_CCW_NEXT */ {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START}};

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
    r_state = R_START;
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
    r_state = R_START;
}

void RotaryEncoder::handle_interrupt(uint gpio, uint32_t events)
{
    if (gpio == clk_pin || gpio == dt_pin) {
        // Aktuellen Hardware-Zustand beider Pins bestimmen
        bool clk = gpio_get(clk_pin);
        bool dt = gpio_get(dt_pin);
        uint8_t pin_state = (clk << 1) | dt;

        // Zustand über die State-Table aktualisieren
        r_state = ttable[r_state & 0x07][pin_state];

        // Richtung prüfen
        uint8_t direction = r_state & 0x30;
        if (direction == DIR_CW) {
            encoder_position++;
            push_event(EVENT_CW);
        } else if (direction == DIR_CCW) {
            encoder_position--;
            push_event(EVENT_CCW);
        }
    } else if (gpio == switch_pin) {
        // Taster entprellen (Debounce): Nur jede Statusänderung zulassen, die > 20ms auseinander liegt
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_button_time > 20) {
            bool state = gpio_get(switch_pin);
            // Durch Pull-up gilt: Low (GND) = gedrückt, High (VCC) = offen
            button_pressed = !state;
            push_event(button_pressed ? EVENT_BUTTON_DOWN : EVENT_BUTTON_UP);
            last_button_time = now;
        }
    }
}

void RotaryEncoder::setPosition(int pos)
{
    encoder_position = pos;
}

void RotaryEncoder::push_event(Event event)
{
    uint8_t next_head = (queue_head + 1) % QUEUE_SIZE;
    if (next_head != queue_tail) { // Nur einreihen, wenn nicht voll (Überlaufschutz)
        event_queue[queue_head] = event;
        queue_head = next_head;
    }
}

RotaryEncoder::Event RotaryEncoder::popEvent()
{
    if (queue_head == queue_tail) {
        return EVENT_NONE;
    }
    Event event = event_queue[queue_tail];
    queue_tail = (queue_tail + 1) % QUEUE_SIZE;
    return event;
}