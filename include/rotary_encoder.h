#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pico/types.h"

class RotaryEncoder
{
private:
    uint8_t clk_pin, dt_pin, switch_pin;
    volatile int encoder_position = 0;
    volatile bool button_pressed = false;
    volatile uint32_t last_encoder_time = 0;
    volatile uint8_t encoder_state = 0;
    bool is_initialized;

public:
    RotaryEncoder();
    ~RotaryEncoder();
    void init(uint8_t clk_pin, uint8_t dt_pin, uint8_t switch_pin);
    void handle_interrupt(uint gpio, uint32_t events);

    int position() const { return encoder_position; }
    bool buttonPressed() const { return button_pressed; }
    void setPosition(int pos);
};