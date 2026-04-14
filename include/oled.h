#ifndef OLED_H
#define OLED_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "picopplib.h"
#include "picopplib-grafix.h"

class Oled
{
private:
    bool initialized = false;

public:
    Oled();
    void init(int i2c_sda_pin, int i2c_scl_pin);
    void clearDisplay();
    void draw(const picopplib::Drawable& img);

    bool isInitialized() const;
};

#endif // OLED_H