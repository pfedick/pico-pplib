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

Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

void Color::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    r = red;
    g = green;
    b = blue;
    a = alpha;
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

Color blendColor(const Color& background, const Color& foreground, int intensity)
{
    int i2 = intensity & 255;
    int i1 = 255 - i2;
    return Color(clamp(((background.r * i1) + (foreground.r * i2)) / 255), clamp(((background.g * i1) + (foreground.g * i2)) / 255),
                 clamp(((background.b * i1) + (foreground.b * i2)) / 255), 255);
}

Color blendColor(const Color& background, const Color& foreground, float intensity)
{
    float i2 = intensity;
    float i1 = 1.0f - i2;
    return Color(clamp((int)((background.r * i1) + (foreground.r * i2))), clamp((int)((background.g * i1) + (foreground.g * i2))),
                 clamp((int)((background.b * i1) + (foreground.b * i2))), 255);
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
    return Color(clamp((int)((float)color.r * factor)), clamp((int)((float)color.g * factor)), clamp((int)((float)color.b * factor)));
}

const Color operator*(float factor, const Color& color)
{
    if (factor < 0) factor = 0.0;
    return Color(clamp((int)((float)color.r * factor)), clamp((int)((float)color.g * factor)), clamp((int)((float)color.b * factor)));
}

const Color operator+(const Color& color1, const Color& color2)
{
    return Color(clamp(color1.r + color2.r), clamp(color1.g + color2.g), clamp(color1.b + color2.b));
}

} // namespace picopplib