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

#ifndef PICOPPLIB_GRAFIX_COLOR_H_
#define PICOPPLIB_GRAFIX_COLOR_H_

#include <stdint.h>
#include <stddef.h>

namespace picopplib
{

class Color
{
    friend const Color operator*(const Color& size, float factor);
    friend const Color operator*(float factor, const Color& size);
    friend const Color operator+(const Color& color1, const Color& color2);

private:
    union {
        struct
        {
            uint8_t b, r, g, a;
        };
        uint32_t c;
    };

public:
    Color();
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
    Color(uint32_t rgba);
    inline uint8_t red() const { return r; }
    inline uint8_t green() const { return g; }
    inline uint8_t blue() const { return b; }
    inline uint8_t alpha() const { return a; }
    inline uint32_t rgba() const { return c; }
    inline void setRed(uint8_t red) { r = red; }
    inline void setGreen(uint8_t green) { g = green; }
    inline void setBlue(uint8_t blue) { b = blue; }
    inline void setAlpha(uint8_t alpha) { a = alpha; }
    inline void setColor(uint32_t rgba) { c = rgba; }
    inline void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
    int brightness() const;
    Color grey() const;
    Color negativ() const;
    bool match(const Color& other, int tolerance = 0) const;
    void setColor(const Color& other);

    Color& operator*=(float factor);
    Color& operator+=(const Color& other);

    bool operator<(const Color& other) const;
    bool operator<=(const Color& other) const;
    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;
    bool operator>=(const Color& other) const;
    bool operator>(const Color& other) const;

    static Color lerp(const Color& c1, const Color& c2, float t);
};
const Color operator*(const Color& color, float factor);
const Color operator*(float factor, const Color& color);
const Color operator+(const Color& color1, const Color& color2);

Color blendColor(const Color& background, const Color& foreground, int intensity);
Color blendColor(const Color& background, const Color& foreground, float intensity);

} // namespace picopplib

#endif