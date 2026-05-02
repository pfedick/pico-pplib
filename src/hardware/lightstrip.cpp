
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "picopplib-grafix.h"
#include "lightstrip.h"

#include "pico/types.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include <math.h>
#include <stdlib.h>

LightStrip::LightStrip(int gpio, size_t num_pixel, float frequency, bool is_rgbw)
{
    sm = 0;
    offset = 0;
    this->is_rgbw = is_rgbw;
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, gpio, 1, true);
    hard_assert(success);
    ws2812_program_init(pio, sm, offset, gpio, frequency, is_rgbw);
    num = num_pixel;
    pixel.resize(num_pixel);
    for (int i = 0; i < num; i++) {
        pixel[i] = 0;
    }
}

LightStrip::~LightStrip()
{
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}

void LightStrip::setSize(int num_pixel)
{
    num = num_pixel;
    pixel.resize(num_pixel);
    for (int i = 0; i < num; i++) {
        pixel[i] = 0;
    }
}

size_t LightStrip::size() const
{
    return num;
}

void LightStrip::clear(const picopplib::Color& color)
{
    uint32_t native_color = toWS2812(color);
    for (int i = 0; i < num; i++) {
        pixel[i] = native_color;
    }
}

void LightStrip::putPixel(int p, const picopplib::Color& color)
{
    if (p < num && p >= 0) pixel[p] = toWS2812(color);
}

picopplib::Color LightStrip::getPixel(int p) const
{
    if (p < num && p >= 0) {
        uint32_t ws = pixel[p];
        uint8_t g = (ws >> 16) & 0xFF;
        uint8_t r = (ws >> 8) & 0xFF;
        uint8_t b = ws & 0xFF;
        return picopplib::Color(r, g, b);
    }
    return picopplib::Color(0, 0, 0);
}

void LightStrip::write()
{
    for (int i = 0; i < num; i++) {
        pio_sm_put_blocking(pio, sm, (pixel[i]) << 8u);
        // pio_sm_put_blocking(pio, sm, 0);
    }
}

void LightStrip::shift(Direction d, int count, bool rotate)
{
    std::vector<uint32_t> old;
    old.resize(num);
    for (int i = 0; i < num; i++)
        old[i] = pixel[i];
    if (count > num || count == 0) return;
    if (d == Direction::Forward) {
        for (int i = (num - 1); i >= (count - 1); i--) {
            pixel[i] = pixel[i - count];
        }
        if (rotate) {
            for (int i = 0; i < count; i++) {
                pixel[i] = old[num - count + i];
            }
        } else {
            for (int i = 0; i < count; i++) {
                pixel[i] = 0;
            }
        }
    } else {
    }
}

void LightStrip::playIntro()
{
    clear(picopplib::Color(0, 0, 0));
    write();
    sleep_ms(10);
    for (int i = 0; i < num + 8; i++) {
        putPixel(i, picopplib::Color(0, 0, 255));
        putPixel(i - 1, picopplib::Color(0, 0, 255 / 8 * 7));
        putPixel(i - 2, picopplib::Color(0, 0, 255 / 8 * 6));
        putPixel(i - 3, picopplib::Color(0, 0, 255 / 8 * 5));
        putPixel(i - 4, picopplib::Color(0, 0, 255 / 8 * 4));
        putPixel(i - 5, picopplib::Color(0, 0, 255 / 8 * 3));
        putPixel(i - 6, picopplib::Color(0, 0, 255 / 8 * 2));
        putPixel(i - 7, picopplib::Color(0, 0, 255 / 8 * 1));
        putPixel(i - 7, picopplib::Color(0, 0, 0));
        write();
        sleep_ms(30);
    }
}

LightStripSection LightStrip::getSection(size_t start, size_t end, LightStripSection::Direction dir)
{
    return LightStripSection(this, start, end, dir);
}

LightStripSection::LightStripSection()
{
    start = 0;
    end = 0;
    my_size = 0;
    dir = Direction::Forward;
    ls = nullptr;
}

LightStripSection::LightStripSection(LightStrip* ls, size_t start, size_t end, Direction dir)
{
    this->start = start;
    this->end = end;
    this->dir = dir;
    this->ls = ls;
    my_size = end - start + 1;
}

size_t LightStripSection::size() const
{
    return my_size;
}

void LightStripSection::clear(const picopplib::Color& color)
{
    for (size_t i = start; i <= end; i++) {
        ls->putPixel(i, color);
    }
}

void LightStripSection::putPixel(int p, const picopplib::Color& color)
{
    if (p < my_size && p >= 0) {
        if (dir == Direction::Forward)
            ls->putPixel(start + p, color);
        else
            ls->putPixel(end - p, color);
    }
}

picopplib::Color LightStripSection::getPixel(int p) const
{
    if (p < my_size && p >= 0) {
        if (dir == Direction::Forward)
            return ls->getPixel(start + p);
        else
            return ls->getPixel(end - p);
    }
    return picopplib::Color();
}

void LightStripSection::shift(Direction d, int count, bool rotate)
{
    // TODO: check this code
    std::vector<uint32_t> old;
    old.resize(my_size);
    for (size_t i = 0; i < my_size; i++) {
        if (dir == Direction::Forward)
            old[i] = ls->getPixelDirect(start + i);
        else
            old[i] = ls->getPixelDirect(end - i);
    }
    if (count > my_size || count == 0) return;
    if (d == Direction::Forward) {
        for (int i = (my_size - 1); i >= (count - 1); i--) {
            if (dir == Direction::Forward)
                ls->putPixelDirect(start + i, old[i - count]);
            else
                ls->putPixelDirect(end - i, old[i - count]);
        }
        if (rotate) {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixelDirect(start + i, old[my_size - count + i]);
                else
                    ls->putPixelDirect(end - i, old[my_size - count + i]);
            }
        } else {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixelDirect(start + i, 0);
                else
                    ls->putPixelDirect(end - i, 0);
            }
        }
    } else {
        for (int i = 0; i < my_size - count; i++) {
            if (dir == Direction::Forward)
                ls->putPixelDirect(start + i, old[i + count]);
            else
                ls->putPixelDirect(end - i, old[i + count]);
        }
        if (rotate) {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixelDirect(end - i, old[i]);
                else
                    ls->putPixelDirect(start + i, old[i]);
            }
        } else {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixelDirect(end - i, 0);
                else
                    ls->putPixelDirect(start + i, 0);
            }
        }
    }
}
