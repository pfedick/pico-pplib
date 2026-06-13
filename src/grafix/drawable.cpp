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
#include <math.h>

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
    uint16_t dst_swapped = row[x];
    uint16_t dst = (dst_swapped >> 8) | (dst_swapped << 8);
    uint16_t src_swapped = c & 0xFFFF;
    uint16_t src = (src_swapped >> 8) | (src_swapped << 8);

    // Kanäle extrahieren
    uint8_t src_r = (src >> 11) & 0x1F;
    uint8_t src_g = (src >> 5) & 0x3F;
    uint8_t src_b = src & 0x1F;

    uint8_t dst_r = (dst >> 11) & 0x1F;
    uint8_t dst_g = (dst >> 5) & 0x3F;
    uint8_t dst_b = dst & 0x1F;

    // Blending
    uint8_t out_r = (src_r * intensity + dst_r * (255 - intensity)) / 255;
    uint8_t out_g = (src_g * intensity + dst_g * (255 - intensity)) / 255;
    uint8_t out_b = (src_b * intensity + dst_b * (255 - intensity)) / 255;

    // Ergebnis zusammensetzen und wieder swappen
    uint16_t res = ((out_r & 0x1F) << 11) | ((out_g & 0x3F) << 5) | (out_b & 0x1F);
    row[x] = (res >> 8) | (res << 8);
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

// ========== 8-Bit GREYSCALE ==========
void Drawable::putPixel8BitGREY(Drawable& self, int x, int y, uint32_t c)
{
    if (!self.buffer) return;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return;
    uint8_t* row = (uint8_t*)(self.buffer + y * self.pitch);
    row[x] = (c & 0xff);
}
uint32_t Drawable::getPixel8BitGREY(const Drawable& self, int x, int y)
{
    if (!self.buffer) return 0;
    if (x < 0 || x >= (int)self.my_width || y < 0 || y >= (int)self.my_height) return 0;
    uint8_t* row = (uint8_t*)(self.buffer + y * self.pitch);
    return row[x];
}

void Drawable::blendPixel8BitGREY(Drawable& self, int x, int y, uint32_t c, uint8_t intensity)
{
    // Not implemented
}

void Drawable::fillRect8BitGREY(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c)
{
    if (x2 < x1 || y2 < y1 || !self.buffer) return;
    Rect s(0, 0, self.my_width, self.my_height);
    Rect r(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    Rect clipped = s.intersected(r);
    if (clipped.isNull()) return;
    uint8_t* row = (uint8_t*)(self.buffer + clipped.top() * self.pitch);
    for (int y = clipped.top(); y <= clipped.bottom(); y++) {
        for (int x = clipped.left(); x <= clipped.right(); x++) {
            row[x] = c;
        }
        row = (uint8_t*)((uint8_t*)row + self.pitch);
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
        blendPixelImpl = other.blendPixelImpl;
        fillRectImpl = other.fillRectImpl;
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
    case RGBFormat::GREY8:
        putPixelImpl = putPixel8BitGREY;
        getPixelImpl = getPixel8BitGREY;
        blendPixelImpl = blendPixel8BitGREY;
        fillRectImpl = fillRect8BitGREY;
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

void Drawable::use(const Drawable& other)
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

Drawable Drawable::getDrawable(const Rect& rect) const
{
    return getDrawable(rect.x1, rect.y1, rect.x2, rect.y2);
}

Drawable Drawable::getDrawable(const Rect16& rect) const
{
    return getDrawable(rect.x1, rect.y1, rect.x2, rect.y2);
}

Drawable Drawable::getDrawable(const Point& p, const Size& s) const
{
    return getDrawable(p.x, p.y, p.x + s.width - 1, p.y + s.height - 1);
}

Drawable Drawable::getDrawable(const Point16& p, const Size16& s) const
{
    return getDrawable(p.x, p.y, p.x + s.width, p.y + s.height);
}

Drawable Drawable::getDrawable(int x1, int y1, int x2, int y2) const
{
    // TODO: Das funktioniert nicht für das Monochrome OLED!
    if (rgb_format.format() == RGBFormat::Monochrome1BitVertical) {
        throw Exception("getDrawable for Monochrome1BitVertical format not supported");
    }

    Rect r(x1, y1, x2 - x1, y2 - y1);
    Rect self_rect(0, 0, my_width, my_height);
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
        return (uint16_t)((color >> 8) | (color << 8)); // Byte swap for SPI transmission und auf 16 Bit stutzen
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

Size16 Drawable::size16() const
{
    return Size16(my_width, my_height);
}

Rect Drawable::rect() const
{
    return Rect(0, 0, my_width, my_height);
}

Rect16 Drawable::rect16() const
{
    return Rect16(0, 0, my_width, my_height);
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

void Drawable::line(const Point& p1, const Point& p2, const Color& color)
{
    line(p1.x, p1.y, p2.x, p2.y, color);
}

static inline void SwapFloat(float* w1, float* w2)
{
    float t = *w1;
    *w1 = *w2;
    *w2 = t;
}

static inline float WuTrunc(float value) // Ganzzahligen Wert von Value zur�ckgeben
{
    return (float)((int32_t)value);
}

static inline float WuFrac(float value) // Kommastellen zurueckgeben
{
    return value - WuTrunc(value);
}

static inline float WuInvFrac(float value)
{
    return 1 - WuFrac(value);
}

static void WuLine(Drawable& draw, float x1, float y1, float x2, float y2, const Color& color)
{
    float grad, xd, yd; //,length,xm,ym;
    float brightness1, brightness2;

    xd = (x2 - x1); // Breite und Hoehe der Linie
    yd = (y2 - y1);

    if (abs((int32_t)xd) > abs((int32_t)yd)) { // check line gradient							==> Horizontale Linie
        if (x1 > x2) {                         // Wenn Linie von rechts nach links gezeichnet wird, tauschen
            SwapFloat(&x1, &x2);               // wir einfach die Koordinaten
            SwapFloat(&y1, &y2);
            xd = (x2 - x1); // Breite und Hoehe der Linie neu berechnen
            yd = (y2 - y1);
        }
        grad = yd / xd; // Gradient der Linie
        float xgap, xend, yend, yf;

        // End Point 1
        xend = WuTrunc(x1 + 0.5f);
        yend = y1 + grad * (xend - x1);

        xgap = WuInvFrac(x1 + 0.5f);

        int32_t ix1 = (int32_t)xend;
        int32_t iy1 = (int32_t)yend;

        brightness1 = WuInvFrac(yend) * xgap;
        brightness2 = WuFrac(yend) * xgap;
        draw.blendPixel(ix1, iy1, color, (int)(brightness1 * 255));
        draw.blendPixel(ix1, iy1 + 1, color, (int)(brightness2 * 255));

        yf = yend + grad;

        // End Point 2
        xend = WuTrunc(x2 + 0.5f);
        yend = y2 + grad * (xend - x2);

        xgap = WuInvFrac(x2 - 0.5f);

        int32_t ix2 = (int32_t)xend;
        int32_t iy2 = (int32_t)yend;

        brightness1 = WuInvFrac(yend) * xgap;
        brightness2 = WuFrac(yend) * xgap;
        draw.blendPixel(ix2, iy2, color, (int)(brightness1 * 255));
        draw.blendPixel(ix2, iy2 + 1, color, (int)(brightness2 * 255));

        // Main Loop
        for (int32_t x = ix1 + 1; x < ix2; x++) {
            brightness1 = WuInvFrac(yf);
            brightness2 = WuFrac(yf);
            draw.blendPixel(x, (int)yf, color, (int)(brightness1 * 255));
            draw.blendPixel(x, (int)yf + 1, color, (int)(brightness2 * 255));
            yf = yf + grad;
        }

    } else {                     // check line gradient							==> Vertikale Linie
        if (y1 > y2) {           // Wenn Linie von rechts nach links gezeichnet wird, tauschen
            SwapFloat(&x1, &x2); // wir einfach die Koordinaten
            SwapFloat(&y1, &y2);
            xd = (x2 - x1); // Breite und Hoehe der Linie neu berechnen
            yd = (y2 - y1);
        }
        grad = xd / yd; // Gradient der Linie

        float xend, yend, xf, ygap;
        // End Point 1
        yend = WuTrunc(y1 + 0.5f);
        xend = x1 + grad * (yend - y1);

        ygap = WuInvFrac(y1 + 0.5f);

        int32_t ix1 = (int32_t)xend;
        int32_t iy1 = (int32_t)yend;

        brightness1 = WuInvFrac(xend) * ygap;
        brightness2 = WuFrac(xend) * ygap;
        draw.blendPixel(ix1, iy1, color, (int)(brightness1 * 255));
        draw.blendPixel(ix1 + 1, iy1, color, (int)(brightness2 * 255));

        xf = xend + grad;

        // End Point 2
        yend = WuTrunc(y2 + 0.5f);
        xend = x2 + grad * (yend - y2);

        ygap = WuInvFrac(y2 - 0.5f);

        int32_t ix2 = (int32_t)xend;
        int32_t iy2 = (int32_t)yend;

        brightness1 = WuInvFrac(xend) * ygap;
        brightness2 = WuFrac(xend) * ygap;
        draw.blendPixel(ix2, iy2, color, (int)(brightness1 * 255));
        draw.blendPixel(ix2 + 1, iy2, color, (int)(brightness2 * 255));

        // Main Loop
        for (int32_t y = iy1 + 1; y < iy2; y++) {
            brightness1 = WuInvFrac(xf);
            brightness2 = WuFrac(xf);
            draw.blendPixel((int)xf, y, color, (int)(brightness1 * 255));
            draw.blendPixel((int)xf + 1, y, color, (int)(brightness2 * 255));
            xf = xf + grad;
        }
    }
}

static void WuLineThick(Drawable& draw, float x1, float y1, float x2, float y2, Color color, int strength)
{
    float grad, xd, yd; //,length,xm,ym;
    float brightness1, brightness2;

    xd = (x2 - x1); // Breite und Hoehe der Linie
    yd = (y2 - y1);

    if (abs((int32_t)xd) > abs((int32_t)yd)) { // check line gradient							==> Horizontale Linie
        // Zuerst korrigieren wir die Start- und Zielkoordinaten, damit die Linie mittig ist
        y1 -= strength / 2;
        y2 -= strength / 2;

        if (x1 > x2) {           // Wenn Linie von rechts nach links gezeichnet wird, tauschen
            SwapFloat(&x1, &x2); // wir einfach die Koordinaten
            SwapFloat(&y1, &y2);
            xd = (x2 - x1); // Breite und Hoehe der Linie neu berechnen
            yd = (y2 - y1);
        }
        grad = yd / xd; // Gradient der Linie
        float xgap, xend, yend, yf;
        // End Point 1
        xend = WuTrunc(x1 + 0.5f);
        yend = y1 + grad * (xend - x1);

        xgap = WuInvFrac(x1 + 0.5f);

        int32_t ix1 = (int32_t)xend;
        int32_t iy1 = (int32_t)yend;

        brightness1 = WuInvFrac(yend) * xgap;
        brightness2 = WuFrac(yend) * xgap;
        draw.blendPixel(ix1, iy1, color, (int)(brightness1 * 255));
        for (int i = 1; i < strength; i++)
            draw.blendPixel(ix1, iy1 + i, color, 255);
        draw.blendPixel(ix1, iy1 + strength, color, (int)(brightness2 * 255));

        yf = yend + grad;

        // End Point 2
        xend = WuTrunc(x2 + 0.5f);
        yend = y2 + grad * (xend - x2);

        xgap = WuInvFrac(x2 - 0.5f);

        int32_t ix2 = (int32_t)xend;
        int32_t iy2 = (int32_t)yend;

        brightness1 = WuInvFrac(yend) * xgap;
        brightness2 = WuFrac(yend) * xgap;
        draw.blendPixel(ix2, iy2, color, (int)(brightness1 * 255));
        for (int i = 1; i < strength; i++)
            draw.blendPixel(ix2, iy2 + i, color, 255);
        draw.blendPixel(ix2, iy2 + strength, color, (int)(brightness2 * 255));

        // Main Loop
        for (int32_t x = ix1 + 1; x < ix2; x++) {
            brightness1 = WuInvFrac(yf);
            brightness2 = WuFrac(yf);
            draw.blendPixel(x, (int32_t)yf, color, (int)(brightness1 * 255));
            for (int i = 1; i < strength; i++)
                draw.blendPixel(x, (int32_t)yf + i, color, 255);
            draw.blendPixel(x, (int32_t)yf + strength, color, (int)(brightness2 * 255));
            yf = yf + grad;
        }

    } else { // check line gradient							==> Vertikale Linie
        // Zuerst korrigieren wir die Start- und Zielkoordinaten, damit die Linie mittig ist
        x1 -= strength / 2;
        x2 -= strength / 2;

        if (y1 > y2) {           // Wenn Linie von rechts nach links gezeichnet wird, tauschen
            SwapFloat(&x1, &x2); // wir einfach die Koordinaten
            SwapFloat(&y1, &y2);
            xd = (x2 - x1); // Breite und Hoehe der Linie neu berechnen
            yd = (y2 - y1);
        }
        grad = xd / yd; // Gradient der Linie
        float xend, yend, xf, ygap;
        // End Point 1
        yend = WuTrunc(y1 + 0.5f);
        xend = x1 + grad * (yend - y1);

        ygap = WuInvFrac(y1 + 0.5f);

        int32_t ix1 = (int32_t)xend;
        int32_t iy1 = (int32_t)yend;

        brightness1 = WuInvFrac(xend) * ygap;
        brightness2 = WuFrac(xend) * ygap;
        draw.blendPixel(ix1, iy1, color, (int)(brightness1 * 255));
        for (int i = 1; i < strength; i++)
            draw.blendPixel(ix1 + i, iy1, color, 255);
        draw.blendPixel(ix1 + strength, iy1, color, (int)(brightness2 * 255));

        xf = xend + grad;

        // End Point 2
        yend = WuTrunc(y2 + 0.5f);
        xend = x2 + grad * (yend - y2);

        ygap = WuInvFrac(y2 - 0.5f);

        int32_t ix2 = (int32_t)xend;
        int32_t iy2 = (int32_t)yend;

        brightness1 = WuInvFrac(xend) * ygap;
        brightness2 = WuFrac(xend) * ygap;
        draw.blendPixel(ix2, iy2, color, (int)(brightness1 * 255));
        for (int i = 1; i < strength; i++)
            draw.blendPixel(ix2 + i, iy2, color, 255);
        draw.blendPixel(ix2 + strength, iy2, color, (int)(brightness2 * 255));

        // Main Loop
        for (int32_t y = iy1 + 1; y < iy2; y++) {
            brightness1 = WuInvFrac(xf);
            brightness2 = WuFrac(xf);
            draw.blendPixel((int32_t)xf, y, color, (int)(brightness1 * 255));
            for (int i = 1; i < strength; i++)
                draw.blendPixel((int32_t)xf + i, y, color, 255);
            draw.blendPixel((int32_t)xf + strength, y, color, (int)(brightness2 * 255));
            xf = xf + grad;
        }
    }
}

void Drawable::lineAA(int x1, int y1, int x2, int y2, const Color& c, int strength)
{
    if (strength == 1) {
        WuLine(*this, (float)x1, (float)y1, (float)x2, (float)y2, c);
    } else {
        WuLineThick(*this, (float)x1, (float)y1, (float)x2, (float)y2, c, strength);
    }
}

void Drawable::lineAA(const Point& start, const Point& end, const Color& c, int strength)
{
    lineAA(start.x, start.y, end.x, end.y, c, strength);
}

void Drawable::colorGradient(const Rect16& rect, const Color& c1, const Color& c2, int direction)
{
    colorGradient(rect.x1, rect.y1, rect.x2 - 1, rect.y2 - 1, c1, c2, direction);
}

void Drawable::colorGradient(int x1, int y1, int x2, int y2, const Color& c1, const Color& c2, int direction)
{
    Color c;
    uint32_t w1, w2;
    int range;
    c.setAlpha(255);
    if (direction == 0) {
        range = x2 - x1 + 1;
        for (int32_t x = 0; x < range; x++) {
            w1 = range - x;
            w2 = x;
            c.setRed((c1.red() * w1 / range) + (c2.red() * w2 / range));
            c.setGreen((c1.green() * w1 / range) + (c2.green() * w2 / range));
            c.setBlue((c1.blue() * w1 / range) + (c2.blue() * w2 / range));
            line(x1 + x, y1, x1 + x, y2, c);
        }
    } else {
        range = y2 - y1 + 1;
        for (int32_t y = 0; y < range; y++) {
            w1 = range - y;
            w2 = y;
            c.setRed((c1.red() * w1 / range) + (c2.red() * w2 / range));
            c.setGreen((c1.green() * w1 / range) + (c2.green() * w2 / range));
            c.setBlue((c1.blue() * w1 / range) + (c2.blue() * w2 / range));
            line(x1, y1 + y, x2, y1 + y, c);
        }
    }
}

/*!\brief Überprüft, ob eine Blit-Aktion in den Zeichenbereich passt.
 *
 * \desc
 * Diese Funktion prüft, ob das zu zeichnende Rechteck überhaupt in die aktuelle
 * Zeichenfläche. Dabei wird das Quellrechteck bei Bedarf angepasst.
 *
 * \param[in,out] x X-Koordinate der Zielposition
 * \param[in,out] y Y-Koordinate der Zielposition
 * \param[in,out] r Quell-Rechteck
 *
 * \return
 * Die Funktion liefert 0 zurück, wenn das Rechteck komplett ausserhalb der
 * Zeichenfläche liegt, oder 1, wenn es ganz oder zumindest teilweise innerhalb der
 * Zeichenfläche liegt. In letzterem Fall werden die Koordinaten \p x, \p y und die
 * Dimensionen des Rechtecks \p r so angepasst, dass durch die nachfolgende Blt-Funktion
 * nur der sichtbare Bereich an die korrekte Position gezeichnet wird.
 */
int Drawable::fitRect(int& x, int& y, Rect16& r)
{
    Rect16 screen(0, 0, my_width, my_height);
    Rect16 object(x, y, r.width(), r.height());
    Rect16 i = screen.intersected(object);

    if (i.isNull()) return 0;
    int16_t shiftx = i.x1 - object.x1;
    int16_t shifty = i.y1 - object.y1;
    x += shiftx;
    y += shifty;

    r.x1 += shiftx;
    r.y1 += shifty;
    r.x2 = r.x1 + i.width();
    r.y2 = r.y1 + i.height();
    return 1;
}

void Drawable::blt(const Drawable& source, int x, int y)
{
    blt(source, source.rect16(), x, y);
}

void Drawable::blt(const Drawable& source, const Rect16& srect, int x, int y)
{
    if (source.isEmpty()) return;
    // Quellrechteck
    Rect16 q;
    if (srect.isNull()) {
        q = source.rect();
    } else {
        q = srect;
        if (q.left() < 0) q.setLeft(0);
        if (q.width() > source.width()) q.setWidth(source.width());
        if (q.top() < 0) q.setTop(0);
        if (q.height() > source.height()) q.setHeight(source.height());
    }
    if (!fitRect(x, y, q)) return;
    for (int16_t yy = 0; yy <= q.height(); yy++) {
        for (int16_t xx = 0; xx <= q.width(); xx++) {
            putPixel(x + xx, y + yy, source.getPixel(q.x1 + xx, q.y1 + yy));
        }
    }
}

void Drawable::bltDiffuse(const Drawable& source, int x, int y, const Color& c)
{
    bltDiffuse(source, source.rect16(), x, y, c);
}

void Drawable::bltDiffuse(const Drawable& source, const Rect16& srect, int x, int y, const Color& c)
{
    if (source.isEmpty()) return;
    if (source.rgb_format.format() != RGBFormat::GREY8) return;
    // Quellrechteck
    Rect16 q;
    if (srect.isNull()) {
        q = source.rect();
    } else {
        q = srect;
        if (q.left() < 0) q.setLeft(0);
        if (q.width() > source.width()) q.setWidth(source.width());
        if (q.top() < 0) q.setTop(0);
        if (q.height() > source.height()) q.setHeight(source.height());
    }
    if (!fitRect(x, y, q)) return;
    uint32_t native_color = toNativeColor(c);

    for (int16_t yy = 0; yy <= q.height(); yy++) {
        for (int16_t xx = 0; xx <= q.width(); xx++) {
            uint8_t intensity = source.getPixelDirect(q.x1 + xx, q.y1 + yy);
            if (intensity > 0) {
                blendPixelDirect(x + xx, y + yy, native_color, intensity);
            }
        }
    }
}

void Drawable::bltAlpha(const Drawable& source, int x, int y)
{
    bltAlpha(source, source.rect16(), x, y);
}

void Drawable::bltAlpha(const Drawable& source, const Rect16& srect, int x, int y)
{
    if (source.isEmpty()) return;
    if (source.rgb_format.format() != RGBFormat::GREY8) return;
    // Quellrechteck
    Rect16 q;
    if (srect.isNull()) {
        q = source.rect();
    } else {
        q = srect;
        if (q.left() < 0) q.setLeft(0);
        if (q.width() > source.width()) q.setWidth(source.width());
        if (q.top() < 0) q.setTop(0);
        if (q.height() > source.height()) q.setHeight(source.height());
    }
    if (!fitRect(x, y, q)) return;
    for (int16_t yy = 0; yy <= q.height(); yy++) {
        for (int16_t xx = 0; xx <= q.width(); xx++) {
            Color pixel = source.getPixel(q.x1 + xx, q.y1 + yy);
            if (pixel.alpha() > 0) {
                blendPixel(x + xx, y + yy, pixel, pixel.alpha());
            }
        }
    }
}

void Drawable::bltBlend(const Drawable& source, float factor, int x, int y)
{
    bltBlend(source, factor, source.rect16(), x, y);
}

static inline uint8_t clamp_uint8_t(float value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (uint8_t)value;
}

void Drawable::bltBlend(const Drawable& source, float factor, const Rect16& srect, int x, int y)
{
    if (source.isEmpty()) return;
    if (source.rgb_format.format() != RGBFormat::GREY8) return;
    // Quellrechteck
    Rect16 q;
    if (srect.isNull()) {
        q = source.rect();
    } else {
        q = srect;
        if (q.left() < 0) q.setLeft(0);
        if (q.width() > source.width()) q.setWidth(source.width());
        if (q.top() < 0) q.setTop(0);
        if (q.height() > source.height()) q.setHeight(source.height());
    }
    if (!fitRect(x, y, q)) return;

    for (int16_t yy = 0; yy <= q.height(); yy++) {
        for (int16_t xx = 0; xx <= q.width(); xx++) {
            Color pixel = source.getPixel(q.x1 + xx, q.y1 + yy);
            pixel.setAlpha(clamp_uint8_t((float)pixel.alpha() * factor));
            if (pixel.alpha() > 0) {
                blendPixel(x + xx, y + yy, pixel, pixel.alpha());
            }
        }
    }
}

void Drawable::draw(const ImageList& iml, int nr, int x, int y)
{
    Rect16 r = iml.getRect(nr);
    switch (iml.method) {
    case DrawMethod::BLT:
        blt(iml.pixel, r, x, y);
        return;
    case DrawMethod::ALPHABLT:
        bltAlpha(iml.pixel, r, x, y);
        return;
    case DrawMethod::DIFFUSE:
        bltDiffuse(iml.pixel, r, x, y, iml.diffuse);
        return;
    }
}

void Drawable::draw(const ImageList& iml, int nr, int x, int y, const Color& diffuse)
{
    Rect16 r = iml.getRect(nr);
    switch (iml.method) {
    case DrawMethod::BLT:
        blt(iml.pixel, r, x, y);
        return;
    case DrawMethod::ALPHABLT:
        bltAlpha(iml.pixel, r, x, y);
        return;
    case DrawMethod::DIFFUSE:
        bltDiffuse(iml.pixel, r, x, y, diffuse);
        return;
    }
}

void Drawable::draw(const ImageReference& imgref, int x, int y)
{
    switch (imgref.draw_method) {
    case DrawMethod::BLT:
        blt(imgref.pixel, imgref.pixel.rect(), x, y);
        return;
    case DrawMethod::ALPHABLT:
        bltAlpha(imgref.pixel, x, y);
        return;
    case DrawMethod::DIFFUSE:
        bltDiffuse(imgref.pixel, x, y, imgref.diffuse_color);
        return;
    }
}

void Drawable::drawBlend(const ImageReference& imgref, int x, int y, float factor)
{
    switch (imgref.draw_method) {
    case DrawMethod::BLT:
        blt(imgref.pixel, imgref.pixel.rect(), x, y);
        return;
    case DrawMethod::ALPHABLT:
        bltBlend(imgref.pixel, factor, x, y);
        return;
    case DrawMethod::DIFFUSE:
        bltDiffuse(imgref.pixel, x, y, imgref.diffuse_color);
        return;
    }
}

/**************************************************************************
 * Kreise: Elipse, Circle                                                 *
 **************************************************************************/
void Drawable::elipse(int x, int y, int radx, int rady, const Color& c, bool fill)
{
    int d;
    int x2 = 0, y2 = 0;

    float pi = 3.1415926535f;
    float rad = pi / 180.0f;

    for (float i = 0.0f; i < 360.0f; i++) {
        int x1 = x + (int)(sinf(i * rad) * (float)radx);
        int y1 = y + (int)(cosf(i * rad) * (float)rady);
        //::printf ("x1=%i, y1=%i\n",x1,y1);
        if (i > 0) {
            d = abs(x2 - x1) + abs(y2 - y1);
            if (d > 1)
                line(x1, y1, x2, y2, c);
            else if (d == 1)
                putPixel(x1, y1, c);
        }
        x2 = x1;
        y2 = y1;
    }
    if (fill) floodFill(x, y, c, c);
}

void Drawable::elipse(int x, int y, int radx, int rady, const Color& c, bool fill, const Color& fillcolor, int start, int end)
{
    float st = (float)start / 360.0f;
    float en = (float)end / 360.0f;
    if (st != en) {
        int x2 = x + (int)(sinf(st) * (float)radx);
        int y2 = y + (int)(cosf(st) * (float)rady);
        putPixel(x2, y2, c);

        for (int i = start; i < end + 1; i++) {
            int x1 = x + (int)(sinf((float)i) * radx);
            int y1 = y + (int)(cosf((float)i) * rady);
            if (i > 0) {
                int d = abs(x2 - x1) + abs(y2 - y1);
                if (d > 1)
                    line(x1, y1, x2, y2, c);
                else if (d == 1)
                    putPixel(x1, y1, c);
            }
            x2 = x1;
            y2 = y1;
        }
        if (fill) {
            int x1 = x + (int)(sinf((float)(start + end) / 2) * (float)(radx - 2));
            int y1 = y + (int)(cosf((float)(start + end) / 2) * (float)(rady - 2));
            floodFill(x1, y1, fillcolor, c);
        }
    }
}

void Drawable::circle(int x, int y, int rad, const Color& c, bool fill)
{
    elipse(x, y, rad, rad, c, fill);
}

void Drawable::circle(const Point& p, int rad, const Color& c, bool fill)
{
    elipse(p.x, p.y, rad, rad, c, fill);
}

void Drawable::floodFill(int x, int y, const Color& color, const Color& border)
/*!\brief Fläche mit Farbe füllen
 *
 * \desc
 * Mit dieser Funktion kann eine beliebig geformte Fläche mit einer Farbe ausgefüllt werden.
 *
 * \param[in] x X-Koordinate eines beliebigen Punktes innerhalb der zu füllenden Fläche
 * \param[in] y Y-Koordinate eines beliebigen Punktes innerhalb der zu füllenden Fläche
 * \param[in] color Farbwert, mit der die Fläche gefüllt werden soll
 * \param[in] border Farbwert, der die Grenze der zu füllenden Fläche darstellt
 *
 * \remarks
 * Die Funktion ruft sich selbst mehrfach rekursiv auf. Der Code wurde im wesentlichen aus der freien
 * GD-Library, Version 1.2 entnommen
 *
 * \code
 * Portions copyright 1994, 1995, 1996, 1997, 1998, by Cold Spring
 * Harbor Laboratory. Funded under Grant P41-RR02188 by the National
 * Institutes of Health.
 *
 * Portions copyright 1996, 1997, 1998, by Boutell.Com, Inc.
 * \endcode
 */
{
    /*
     * Quelle des Codes: GD-Library Version 1.2
     */

    int lastBorder;
    /* Seek left */
    int leftLimit, rightLimit;
    int i;
    leftLimit = (-1);
    for (i = x; (i >= 0); i--) {
        if (getPixel(i, y) == border) {
            break;
        }
        putPixel(i, y, color);
        leftLimit = i;
    }
    if (leftLimit == (-1)) {
        return;
    }
    /* Seek right */
    rightLimit = x;
    for (i = (x + 1); (i < my_width); i++) {
        if (getPixel(i, y) == border) {
            break;
        }
        putPixel(i, y, color);
        rightLimit = i;
    }
    /* Look at lines above and below and start paints */
    /* Above */
    if (y > 0) {
        lastBorder = 1;
        for (i = leftLimit; (i <= rightLimit); i++) {
            Color c;
            c = getPixel(i, y - 1);
            if (lastBorder) {
                if ((c != border) && (c != color)) {
                    floodFill(i, y - 1, color, border);
                    lastBorder = 0;
                }
            } else if ((c == border) || (c == color)) {
                lastBorder = 1;
            }
        }
    }
    /* Below */
    if (y < ((my_height)-1)) {
        lastBorder = 1;
        for (i = leftLimit; (i <= rightLimit); i++) {
            Color c;
            c = getPixel(i, y + 1);
            if (lastBorder) {
                if ((c != border) && (c != color)) {
                    floodFill(i, y + 1, color, border);
                    lastBorder = 0;
                }
            } else if ((c == border) || (c == color)) {
                lastBorder = 1;
            }
        }
    }
    return;
}

} // namespace picopplib
