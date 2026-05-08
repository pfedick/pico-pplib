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
#include <string.h>

namespace picopplib
{

// ========== 1-Bit Monochrome (VERTIKAL) ==========
void Drawable::putPixelMonochrome1BitVertical(Drawable& self, int x, int y, uint32_t c)
{
    if (!self.buffer) return;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return;

    // Adresse: x + (y/8) * pitch
    // pitch = Bytes pro "Zeilen-Block" (8 Pixel hoch)
    uint8_t* ptr = self.buffer + x + (y >> 3) * self.pitch;
    int bit_offset = y & 7; // Bit innerhalb des Bytes (vertikal!)

    if (c)
        *ptr |= (1 << bit_offset);
    else
        *ptr &= ~(1 << bit_offset);
}

uint32_t Drawable::getPixelMonochrome1BitVertical(const Drawable& self, int x, int y)
{
    if (!self.buffer) return 0;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return 0;

    const uint8_t* ptr = self.buffer + x + (y >> 3) * self.pitch;
    int bit_offset = y & 7;

    return (*ptr & (1 << bit_offset)) ? 1 : 0;
}

void Drawable::blendPixelMonochrome1BitVertical(Drawable& self, int x, int y, uint32_t c, uint8_t intensity)
{
    if (intensity >= 100) {
        putPixelMonochrome1BitVertical(self, x, y, c);
    }
}

void Drawable::fillRectMonochrome1BitVertical(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c)
{
    if (x2 < x1 || y2 < y1 || !self.buffer) return;
    Rect s(0, 0, self.my_width, self.my_height);
    Rect r(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    Rect clipped = s.intersected(r);
    if (clipped.isNull()) return;
    for (int y = clipped.top(); y <= clipped.bottom(); y++) {
        for (int x = clipped.left(); x <= clipped.right(); x++) {
            uint8_t* ptr = self.buffer + x + (y >> 3) * self.pitch;
            int bit_offset = y & 7; // Bit innerhalb des Bytes (vertikal!)
            if (c)
                *ptr |= (1 << bit_offset);
            else
                *ptr &= ~(1 << bit_offset);
        }
    }
}

// ========== 16-Bit R5G6B5 ==========
void Drawable::putPixel16BitR5G6B5(Drawable& self, int x, int y, uint32_t c)
{
    if (!self.buffer) return;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return;
    uint16_t* row = (uint16_t*)(self.buffer + y * self.pitch);
    row[x] = c & 0xFFFF;
}

uint32_t Drawable::getPixel16BitR5G6B5(const Drawable& self, int x, int y)
{
    if (!self.buffer) return 0;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return 0;
    uint16_t* row = (uint16_t*)(self.buffer + y * self.pitch);
    return row[x];
}

void Drawable::blendPixel16BitR5G6B5(Drawable& self, int x, int y, uint32_t c, uint8_t intensity)
{
    if (!self.buffer) return;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return;

    uint16_t* row = (uint16_t*)(self.buffer + y * self.pitch);
    uint16_t dst = row[x];
    uint16_t src = c & 0xFFFF;

    // Einfache Alpha-Blending-Formel: result = (src * intensity + dst * (255 - intensity)) / 255
    uint8_t src_r = (src >> 11) & 0x1F;
    uint8_t src_g = (src >> 5) & 0x3F;
    uint8_t src_b = src & 0x1F;

    uint8_t dst_r = (dst >> 11) & 0x1F;
    uint8_t dst_g = (dst >> 5) & 0x3F;
    uint8_t dst_b = dst & 0x1F;

    uint8_t out_r = (src_r * intensity + dst_r * (255 - intensity)) / 255;
    uint8_t out_g = (src_g * intensity + dst_g * (255 - intensity)) / 255;
    uint8_t out_b = (src_b * intensity + dst_b * (255 - intensity)) / 255;

    row[x] = ((out_r & 0x1F) << 11) | ((out_g & 0x3F) << 5) | (out_b & 0x1F);
}

void Drawable::fillRect16BitR5G6B5(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c)
{
    if (x2 < x1 || y2 < y1 || !self.buffer) return;
    Rect s(0, 0, self.my_width, self.my_height);
    Rect r(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    Rect clipped = s.intersected(r);
    if (clipped.isNull()) return;
    uint16_t* row = (uint16_t*)(self.buffer + clipped.top() * self.pitch);
    for (int y = clipped.top(); y <= clipped.bottom(); y++) {
        for (int x = clipped.left(); x <= clipped.right(); x++) {
            row[x] = c & 0xFFFF;
        }
        row = (uint16_t*)((uint8_t*)row + self.pitch);
    }
}

// ========== 32-Bit A8R8G8B8 ==========
void Drawable::putPixel32BitA8R8G8B8(Drawable& self, int x, int y, uint32_t c)
{
    if (!self.buffer) return;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return;
    uint32_t* row = (uint32_t*)(self.buffer + y * self.pitch);
    row[x] = c;
}

uint32_t Drawable::getPixel32BitA8R8G8B8(const Drawable& self, int x, int y)
{
    if (!self.buffer) return 0;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return 0;
    uint32_t* row = (uint32_t*)(self.buffer + y * self.pitch);
    return row[x];
}

void Drawable::blendPixel32BitA8R8G8B8(Drawable& self, int x, int y, uint32_t c, uint8_t intensity)
{
    if (!self.buffer) return;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return;

    uint32_t* row = (uint32_t*)(self.buffer + y * self.pitch);
    uint32_t dst = row[x];
    uint32_t src = c;

    uint8_t src_a = (src >> 24) & 0xFF;
    uint8_t src_r = (src >> 16) & 0xFF;
    uint8_t src_g = (src >> 8) & 0xFF;
    uint8_t src_b = src & 0xFF;

    uint8_t dst_a = (dst >> 24) & 0xFF;
    uint8_t dst_r = (dst >> 16) & 0xFF;
    uint8_t dst_g = (dst >> 8) & 0xFF;
    uint8_t dst_b = dst & 0xFF;

    // Einfache Alpha-Blending-Formel: result = (src * intensity + dst * (255 - intensity)) / 255
    uint8_t out_a = (src_a * intensity + dst_a * (255 - intensity)) / 255;
    uint8_t out_r = (src_r * intensity + dst_r * (255 - intensity)) / 255;
    uint8_t out_g = (src_g * intensity + dst_g * (255 - intensity)) / 255;
    uint8_t out_b = (src_b * intensity + dst_b * (255 - intensity)) / 255;

    row[x] = ((out_a & 0xFF) << 24) | ((out_r & 0xFF) << 16) | ((out_g & 0xFF) << 8) | (out_b & 0xFF);
}

void Drawable::fillRect32BitA8R8G8B8(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c)
{
    if (x2 < x1 || y2 < y1 || !self.buffer) return;
    Rect s(0, 0, self.my_width, self.my_height);
    Rect r(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    Rect clipped = s.intersected(r);
    if (clipped.isNull()) return;
    uint32_t* row = (uint32_t*)(self.buffer + clipped.top() * self.pitch);
    for (int y = clipped.top(); y <= clipped.bottom(); y++) {
        for (int x = clipped.left(); x <= clipped.right(); x++) {
            row[x] = c;
        }
        row = (uint32_t*)((uint8_t*)row + self.pitch);
    }
}

// Class

Drawable::Drawable()
{
    buffer = NULL;
    my_width = my_height = 0;
    pitch = 0;
    putPixelImpl = nullptr;
    getPixelImpl = nullptr;
    blendPixelImpl = nullptr;
    fillRectImpl = nullptr;
}

Drawable::Drawable(const Drawable& other)
{
    buffer = other.buffer;
    pitch = other.pitch;
    my_width = other.my_width;
    my_height = other.my_height;
    rgb_format = other.rgb_format;
    putPixelImpl = other.putPixelImpl;
    getPixelImpl = other.getPixelImpl;
    blendPixelImpl = other.blendPixelImpl;
    fillRectImpl = other.fillRectImpl;
}

Drawable::Drawable(Drawable&& other) noexcept
{
    buffer = other.buffer;
    pitch = other.pitch;
    my_width = other.my_width;
    my_height = other.my_height;
    rgb_format = other.rgb_format;
    putPixelImpl = other.putPixelImpl;
    getPixelImpl = other.getPixelImpl;
    blendPixelImpl = other.blendPixelImpl;
    fillRectImpl = other.fillRectImpl;

    other.buffer = NULL;
    other.pitch = 0;
    other.my_width = 0;
    other.my_height = 0;
    other.putPixelImpl = nullptr;
    other.getPixelImpl = nullptr;
    other.blendPixelImpl = nullptr;
    other.fillRectImpl = nullptr;
}

Drawable::Drawable(void* buffer, uint32_t pitch, uint16_t width, uint16_t height, const RGBFormat& format)
{
    this->buffer = NULL;
    my_width = my_height = 0;
    this->pitch = 0;
    create(buffer, pitch, width, height, format);
}

Drawable& Drawable::operator=(const Drawable& other)
{
    if (this != &other) {
        buffer = other.buffer;
        pitch = other.pitch;
        my_width = other.my_width;
        my_height = other.my_height;
        rgb_format = other.rgb_format;
        putPixelImpl = other.putPixelImpl;
        getPixelImpl = other.getPixelImpl;
    }
    return *this;
}

Drawable& Drawable::operator=(Drawable&& other) noexcept
{
    if (this != &other) {
        buffer = other.buffer;
        pitch = other.pitch;
        my_width = other.my_width;
        my_height = other.my_height;
        rgb_format = other.rgb_format;
        putPixelImpl = other.putPixelImpl;
        getPixelImpl = other.getPixelImpl;

        other.buffer = NULL;
        other.pitch = 0;
        other.my_width = 0;
        other.my_height = 0;
        other.putPixelImpl = nullptr;
        other.getPixelImpl = nullptr;
    }
    return *this;
}

void Drawable::create(void* buffer, uint32_t pitch, uint16_t width, uint16_t height, const RGBFormat& format)
{
    switch (format.format()) {
    case RGBFormat::Monochrome1BitVertical:
        putPixelImpl = putPixelMonochrome1BitVertical;
        getPixelImpl = getPixelMonochrome1BitVertical;
        blendPixelImpl = blendPixelMonochrome1BitVertical;
        fillRectImpl = fillRectMonochrome1BitVertical;
        break;
    case RGBFormat::R5G6B5:
        putPixelImpl = putPixel16BitR5G6B5;
        getPixelImpl = getPixel16BitR5G6B5;
        blendPixelImpl = blendPixel16BitR5G6B5;
        fillRectImpl = fillRect16BitR5G6B5;
        break;
    case RGBFormat::A8R8G8B8:
        putPixelImpl = putPixel32BitA8R8G8B8;
        getPixelImpl = getPixel32BitA8R8G8B8;
        blendPixelImpl = blendPixel32BitA8R8G8B8;
        fillRectImpl = fillRect32BitA8R8G8B8;
        break;
    default:
        throw Exception("Unsupported RGB format");
    }
    this->buffer = (uint8_t*)buffer;
    this->pitch = pitch;
    this->my_width = width;
    this->my_height = height;
    this->rgb_format = format;
}

Drawable Drawable::getDrawable(const Rect& rect) const
{
    return getDrawable(rect.x1, rect.y1, rect.x2, rect.y2);
}

Drawable Drawable::getDrawable(const Point& p, const Size& s) const
{
    return getDrawable(p.x, p.y, p.x + s.width - 1, p.y + s.height - 1);
}

Drawable Drawable::getDrawable(int x1, int y1, int x2, int y2) const
{
    // TODO: Das funktioniert nicht für das Monochrome OLED!
    if (rgb_format.format() == RGBFormat::Monochrome1BitVertical) {
        throw Exception("getDrawable for Monochrome1BitVertical format not supported");
    }

    Rect r(x1, y1, x2, y2);
    Rect self_rect(0, 0, my_width - 1, my_height - 1);
    Rect intersect = r.intersected(self_rect);
    if (intersect.isNull()) {
        // Leeres Drawable zurückgeben
        return Drawable();
    }
    return Drawable(buffer + intersect.y1 * pitch + (intersect.x1 * rgb_format.bitdepth()) / 8, pitch, intersect.width(),
                    intersect.height(), rgb_format);
}

bool Drawable::isEmpty() const
{
    if (!buffer) return true;
    return false;
}

uint32_t Drawable::toNativeColor(const Color& c) const
{
    switch (rgb_format.format()) {
    case RGBFormat::Monochrome1BitVertical:
        return c.brightness() > 127 ? 1 : 0;

    case RGBFormat::R5G6B5: {
        // SSD1351 needs byte-swapped R5G6B5 (from original Color::operator uint16_t)
        uint16_t color = ((c.red() & 0xf8) << 8) | ((c.green() & 0xfc) << 3) | ((c.blue() & 0xf8) >> 3);
        return (color >> 8) | (color << 8); // Byte swap for SPI transmission
    }

    case RGBFormat::A8R8G8B8:
        return (c.alpha() << 24) | (c.red() << 16) | (c.green() << 8) | c.blue();

    default:
        return 0;
    }
}

Color Drawable::fromNativeColor(uint32_t native) const
{
    switch (rgb_format.format()) {
    case RGBFormat::Monochrome1BitVertical:
        return Color(native ? 255 : 0, native ? 255 : 0, native ? 255 : 0);

    case RGBFormat::R5G6B5: {
        // Un-swap bytes first (reverse of toNativeColor)
        uint16_t color = ((native >> 8) | (native << 8)) & 0xFFFF;
        uint8_t r = ((color >> 8) & 0xF8); // Top 5 bits of high byte
        uint8_t g = ((color >> 3) & 0xFC); // 6 bits spanning both bytes
        uint8_t b = ((color << 3) & 0xF8); // Bottom 5 bits of low byte
        return Color(r, g, b);
    }

    case RGBFormat::A8R8G8B8: {
        uint8_t a = (native >> 24) & 0xFF;
        uint8_t r = (native >> 16) & 0xFF;
        uint8_t g = (native >> 8) & 0xFF;
        uint8_t b = native & 0xFF;
        return Color(r, g, b, a);
    }

    default:
        return Color(0, 0, 0);
    }
}

void Drawable::clear(const Color& color)
{
    fillRect(0, 0, my_width - 1, my_height - 1, color);
}

uint8_t* Drawable::ptr() const
{
    return buffer;
}

Size Drawable::size() const
{
    return Size(my_width, my_height);
}

void Drawable::drawRect(int x1, int y1, int x2, int y2, const Color& color)
{
    uint32_t native_color = toNativeColor(color);
    for (int x = x1; x <= x2; x++) {
        putPixelImpl(*this, x, y1, native_color);
        putPixelImpl(*this, x, y2, native_color);
    }
    for (int y = y1 + 1; y <= y2; y++) {
        putPixelImpl(*this, x1, y, native_color);
        putPixelImpl(*this, x2, y, native_color);
    }
}

void Drawable::invertRect(int x1, int y1, int x2, int y2)
// Das ist nur für 1-Bit Monochrom
{
    int c;
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            int c = getPixelImpl(*this, x, y);
            if (c)
                putPixelImpl(*this, x, y, 0);
            else
                putPixelImpl(*this, x, y, 1);
        }
    }
}

/* signum function */
static int sgn(int x)
{
    return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

void Drawable::line(int x1, int y1, int x2, int y2, const Color& color)
{
    int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;
    uint32_t native_color = toNativeColor(color);
    /* Entfernung in beiden Dimensionen berechnen */
    dx = x2 - x1;
    dy = y2 - y1;

    /* Vorzeichen des Inkrements bestimmen */
    incx = sgn(dx);
    incy = sgn(dy);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    /* feststellen, welche Entfernung größer ist */
    if (dx > dy) {
        /* x ist schnelle Richtung */
        pdx = incx;
        pdy = 0; /* pd. ist Parallelschritt */
        ddx = incx;
        ddy = incy; /* dd. ist Diagonalschritt */
        es = dy;
        el = dx; /* Fehlerschritte schnell, langsam */
    } else {
        /* y ist schnelle Richtung */
        pdx = 0;
        pdy = incy; /* pd. ist Parallelschritt */
        ddx = incx;
        ddy = incy; /* dd. ist Diagonalschritt */
        es = dx;
        el = dy; /* Fehlerschritte schnell, langsam */
    }

    /* Initialisierungen vor Schleifenbeginn */
    x = x1;
    y = y1;
    err = el / 2;
    putPixelImpl(*this, x, y, native_color);

    /* Pixel berechnen */
    for (t = 0; t < el; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
    {
        /* Aktualisierung Fehlerterm */
        err -= es;
        if (err < 0) {
            /* Fehlerterm wieder positiv (>=0) machen */
            err += el;
            /* Schritt in langsame Richtung, Diagonalschritt */
            x += ddx;
            y += ddy;
        } else {
            /* Schritt in schnelle Richtung, Parallelschritt */
            x += pdx;
            y += pdy;
        }
        putPixelImpl(*this, x, y, native_color);
    }
}

// void Drawable::blendPixel(int x, int y, int c, int brightness) {}

} // namespace picopplib
