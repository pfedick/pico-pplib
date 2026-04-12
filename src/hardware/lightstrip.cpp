
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
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, gpio, 1, true);
    hard_assert(success);
    ws2812_program_init(pio, sm, offset, gpio, frequency, is_rgbw);
    num = num_pixel;
    pixel.reserve(num_pixel);
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
    pixel.reserve(num_pixel);
    for (int i = 0; i < num; i++) {
        pixel[i] = 0;
    }
}

size_t LightStrip::size() const
{
    return num;
}

uint32_t LightStrip::getColor(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint32_t)(red) << 8) | ((uint32_t)(green) << 16) | (uint32_t)(blue);
}

void LightStrip::clear(picopplib::Color color)
{
    for (int i = 0; i < num; i++) {
        pixel[i] = color.rgb();
    }
}

void LightStrip::putPixel(int p, uint32_t color)
{
    if (p < num && p >= 0) pixel[p] = color;
}

void LightStrip::putPixel(int p, const picopplib::Color& color)
{
    if (p < num && p >= 0) pixel[p] = color.rgb();
}

picopplib::Color LightStrip::getPixel(int p) const
{
    if (p < num && p >= 0) return picopplib::Color(pixel[p]);
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
    old.reserve(num);
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
    clear(0);
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

void LightStripSection::clear(uint32_t color)
{
    for (size_t i = start; i <= end; i++) {
        ls->putPixel(i, color);
    }
}

void LightStripSection::putPixel(int p, uint32_t color)
{
    if (p < my_size && p >= 0) {
        if (dir == Direction::Forward)
            ls->putPixel(start + p, color);
        else
            ls->putPixel(end - p, color);
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

uint32_t LightStripSection::getPixel(int p) const
{
    if (p < my_size && p >= 0) {
        if (dir == Direction::Forward)
            return ls->getPixel(start + p);
        else
            return ls->getPixel(end - p);
    }
    return 0;
}

void LightStripSection::shift(Direction d, int count, bool rotate)
{
    // TODO: check this code
    std::vector<uint32_t> old;
    old.reserve(my_size);
    for (size_t i = 0; i < my_size; i++) {
        if (dir == Direction::Forward)
            old[i] = ls->getPixel(start + i);
        else
            old[i] = ls->getPixel(end - i);
    }
    if (count > my_size || count == 0) return;
    if (d == Direction::Forward) {
        for (int i = (my_size - 1); i >= (count - 1); i--) {
            if (dir == Direction::Forward)
                ls->putPixel(start + i, old[i - count]);
            else
                ls->putPixel(end - i, old[i - count]);
        }
        if (rotate) {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixel(start + i, old[my_size - count + i]);
                else
                    ls->putPixel(end - i, old[my_size - count + i]);
            }
        } else {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixel(start + i, 0);
                else
                    ls->putPixel(end - i, 0);
            }
        }
    } else {
        for (int i = 0; i < my_size - count; i++) {
            if (dir == Direction::Forward)
                ls->putPixel(start + i, old[i + count]);
            else
                ls->putPixel(end - i, old[i + count]);
        }
        if (rotate) {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixel(end - i, old[i]);
                else
                    ls->putPixel(start + i, old[i]);
            }
        } else {
            for (int i = 0; i < count; i++) {
                if (dir == Direction::Forward)
                    ls->putPixel(end - i, 0);
                else
                    ls->putPixel(start + i, 0);
            }
        }
    }
}
