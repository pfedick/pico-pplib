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

#include "picopplib-grafix.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

namespace picopplib
{

static inline int clamp(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

Color::Color()
{
    c = 0;
}

Color::Color(int red, int green, int blue)
{
    r = red & 0xff;
    g = green & 0xff;
    b = blue & 0xff;
    a = 0;
}

Color::Color(uint32_t rgb)
{
    setColor(rgb);
}

uint32_t Color::color() const
{
    return c;
}

uint32_t Color::rgb() const
{
    return c & 0xffffff;
}

int Color::red() const
{
    return r;
}

int Color::green() const
{
    return g;
}

int Color::blue() const
{
    return b;
}

void Color::setRed(int red)
{
    r = clamp(red);
}

void Color::setGreen(int green)
{
    g = clamp(green);
}

void Color::setBlue(int blue)
{
    b = clamp(blue);
}

void Color::setColor(int red, int green, int blue)
{
    r = red & 0xff;
    g = green & 0xff;
    b = blue & 0xff;
    a = 0;
}

void Color::setColor(uint32_t rgb)
{
    c = rgb;
}

void Color::setColor(const Color& other)
{
    c = other.c;
}

int Color::brightness() const
{
    return (r * 11 + g * 16 + b * 5) / 32;
}

Color Color::grey() const
{
    int v = brightness();
    return Color(v, v, v);
}

Color Color::negativ() const
{
    return Color(255 - r, 255 - g, 255 - b);
}

bool Color::match(const Color& other, int tolerance) const
{
    if (c == other.c) return true;
    if (abs(r - other.r) > tolerance) return false;
    if (abs(g - other.g) > tolerance) return false;
    if (abs(b - other.b) > tolerance) return false;
    return true;
}

Color& Color::blend(const Color& background, const Color& foreground, int intensity)
{
    int i2 = intensity & 255;
    int i1 = 255 - i2;
    r = ((background.r * i1) + (foreground.r * i2)) / 255;
    g = ((background.g * i1) + (foreground.g * i2)) / 255;
    b = ((background.b * i1) + (foreground.b * i2)) / 255;
    a = 255;
    return *this;
}

Color& Color::blendf(const Color& background, const Color& foreground, float intensity)
{
    float i2 = intensity;
    float i1 = 1.0f - i2;
    r = (int)((background.r * i1) + (foreground.r * i2));
    g = (int)((background.g * i1) + (foreground.g * i2));
    b = (int)((background.b * i1) + (foreground.b * i2));
    a = 255;
    return *this;
}

Color& Color::operator*=(float factor)
{
    r = clamp((int)(r * factor));
    g = clamp((int)(r * factor));
    b = clamp((int)(r * factor));
    return *this;
}

Color& Color::operator+=(const Color& other)
{
    r = clamp(r + other.r);
    g = clamp(g + other.g);
    b = clamp(b + other.b);
    a = clamp(a + other.a);
    return *this;
}

Color::operator uint32_t() const
{
    return c;
}

Color::operator int32_t() const
{
    return c;
}

bool Color::operator<(const Color& other) const
{
    if (c < other.c) return true;
    return false;
}

bool Color::operator<=(const Color& other) const
{
    if (c <= other.c) return true;
    return false;
}

bool Color::operator==(const Color& other) const
{
    if (c == other.c) return true;
    return false;
}

bool Color::operator!=(const Color& other) const
{
    if (c != other.c) return true;
    return false;
}

bool Color::operator>=(const Color& other) const
{
    if (c >= other.c) return true;
    return false;
}

bool Color::operator>(const Color& other) const
{
    if (c > other.c) return true;
    return false;
}

const Color operator*(const Color& color, float factor)
{
    if (factor < 0) factor = 0.0f;
    return Color(clamp((int)((float)color.r * factor)), clamp((int)((float)color.g * factor)),
                 clamp((int)((float)color.b * factor)));
}

const Color operator*(float factor, const Color& color)
{
    if (factor < 0) factor = 0.0;
    return Color(clamp((int)((float)color.r * factor)), clamp((int)((float)color.g * factor)),
                 clamp((int)((float)color.b * factor)));
}

Color Color::getBlended(const Color& background, const Color& foreground, int intensity)
{
    Color c;
    int i2 = intensity & 255;
    int i1 = 255 - i2;
    c.r = ((background.r * i1) + (foreground.r * i2)) / 255;
    c.g = ((background.g * i1) + (foreground.g * i2)) / 255;
    c.b = ((background.b * i1) + (foreground.b * i2)) / 255;
    return c;
}

Color Color::getBlendedf(const Color& background, const Color& foreground, float intensity)
{
    Color c;
    float i2 = intensity;
    float i1 = 1.0f - i2;
    c.r = (int)((background.r * i1) + (foreground.r * i2));
    c.g = (int)((background.g * i1) + (foreground.g * i2));
    c.b = (int)((background.b * i1) + (foreground.b * i2));
    return c;
}

const Color operator+(const Color& color1, const Color& color2)
{
    return Color(clamp(color1.r + color2.r), clamp(color1.g + color2.g), clamp(color1.b + color2.b));
}

Color::operator uint16_t() const
{
    uint16_t color;
    color = (r & 0xf8) << 8;
    color |= (g & 0xfc) << 3;
    color |= (b & 0xf8) >> 3;
    return (color >> 8) | (color << 8);
}

} // namespace picopplib