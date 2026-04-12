#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include <vector>

class LightStrip;

class LightStripSection
{
    friend class LightStrip;

public:
    enum class Direction
    {
        Forward = 0,
        Backward = 1
    };

private:
    size_t start;
    size_t end;
    size_t my_size;
    Direction dir;
    LightStrip* ls;

public:
    LightStripSection();
    LightStripSection(LightStrip* ls, size_t start, size_t end, Direction dir = Direction::Forward);

    size_t size() const;
    void clear(uint32_t color = 0);
    void putPixel(int p, uint32_t color);
    void putPixel(int p, const ppl7light::Color& color);
    uint32_t getPixel(int p) const;
    void shift(Direction d, int count = 1, bool rotate = false);
    static uint32_t getColor(uint8_t red, uint8_t green, uint8_t blue);
};

class LightStrip
{
private:
    std::vector<uint32_t> pixel;
    size_t num;
    PIO pio;
    uint sm = 0;
    uint offset = 0;

public:
    enum class Direction
    {
        Forward = 0,
        Backward = 1
    };
    LightStrip(int gpio, size_t num_pixel, float frequency = 800000.0f, bool is_rgbw = false);
    ~LightStrip();
    size_t size() const;
    void setSize(int num_pixel);

    void clear(ppl7light::Color color = ppl7light::Color(0, 0, 0));
    void write();
    void putPixel(int p, uint32_t color);
    void putPixel(int p, const ppl7light::Color& color);
    ppl7light::Color getPixel(int p) const;
    void shift(Direction d, int count = 1, bool rotate = false);

    static uint32_t getColor(uint8_t red, uint8_t green, uint8_t blue);

    LightStripSection getSection(size_t start, size_t end, LightStripSection::Direction dir = LightStripSection::Direction::Forward);

    void playIntro();
};

#endif