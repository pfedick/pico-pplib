/*******************************************************************************
 * This file is part of "Patrick's Programming Library" for Raspberry Pico,
 * based on PPLib Version 7.
 * Web: https://github.com/pfedick/pico-pplib
 *******************************************************************************
 * Copyright (c) 2026, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "picopplib-grafix.h"
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
    void putPixel(int p, const picopplib::Color& color);
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

    void clear(picopplib::Color color = picopplib::Color(0, 0, 0));
    void write();
    void putPixel(int p, uint32_t color);
    void putPixel(int p, const picopplib::Color& color);
    picopplib::Color getPixel(int p) const;
    void shift(Direction d, int count = 1, bool rotate = false);

    static uint32_t getColor(uint8_t red, uint8_t green, uint8_t blue);

    LightStripSection getSection(size_t start, size_t end,
                                 LightStripSection::Direction dir = LightStripSection::Direction::Forward);

    void playIntro();
};

#endif