
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "picopplib/grafix.h"
#include "picopplib/hardware/lightstrip.h"

#include "pico/types.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include <math.h>
#include <stdlib.h>

static PIO ls_pio = nullptr;
uint ls_offset = 0;

LightStrip::LightStrip(int gpio, size_t num_pixel, float frequency, bool is_rgbw)
{
    sm = 0;
    offset = 0;
    this->is_rgbw = is_rgbw;
    if (ls_pio == nullptr) {
        bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, gpio, 1, true);
        hard_assert(success);
        ls_pio = pio;
        ls_offset = offset;
    } else {
        pio = ls_pio;
        sm = 0;
        offset = ls_offset;
        sm = pio_claim_unused_sm(pio, true);
    }

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
    for (int i = 0; i < num; i++) {
        pixel[i] = color;
    }
}

void LightStrip::write()
{
    if (is_rgbw) {
        for (int i = 0; i < num; i++) {
            pio_sm_put_blocking(pio, sm, toSK6812(pixel[i]));
        }
    } else {
        for (int i = 0; i < num; i++) {
            // Nach links shiften für die 24-Bit PIO-State-Machine
            pio_sm_put_blocking(pio, sm, toWS2812(pixel[i]) << 8u);
        }
    }
}

void LightStrip::shift(Direction d, int count, bool rotate)
{
    std::vector<picopplib::Color> old;
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

LightStripSection::LightStripSection(const LightStripSection& other)
{
    start = other.start;
    end = other.end;
    my_size = other.my_size;
    dir = other.dir;
    ls = other.ls;
}

LightStripSection::LightStripSection(LightStripSection&& other)
{
    start = other.start;
    end = other.end;
    my_size = other.my_size;
    dir = other.dir;
    ls = other.ls;

    other.start = 0;
    other.end = 0;
    other.my_size = 0;
    other.dir = Direction::Forward;
    other.ls = nullptr;
}

LightStripSection& LightStripSection::operator=(const LightStripSection& other)
{
    if (this != &other) {
        start = other.start;
        end = other.end;
        my_size = other.my_size;
        dir = other.dir;
        ls = other.ls;
    }
    return *this;
}

LightStripSection& LightStripSection::operator=(LightStripSection&& other)
{
    if (this != &other) {
        start = other.start;
        end = other.end;
        my_size = other.my_size;
        dir = other.dir;
        ls = other.ls;

        other.start = 0;
        other.end = 0;
        other.my_size = 0;
        other.dir = Direction::Forward;
        other.ls = nullptr;
    }
    return *this;
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

void LightStripSection::write()
{
    ls->write();
}

void LightStripSection::shift(Direction d, int count, bool rotate)
{
    // TODO: check this code
    std::vector<picopplib::Color> old;
    old.resize(my_size);
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
