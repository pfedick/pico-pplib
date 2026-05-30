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

namespace picopplib
{

static inline int16_t clamp16(int value)
{
    if (value < -32768) return -32768;
    if (value > 32767) return 32767;
    return value;
}

Rect16::Rect16()
{
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

Rect16::Rect16(const Point16& p1, const Point16& p2)
{
    x1 = p1.x;
    y1 = p1.y;
    x2 = p2.x;
    y2 = p2.y;
}

Rect16::Rect16(const Point16& p, const Size16& s)
{
    x1 = p.x;
    y1 = p.y;
    x2 = x1 + s.width;
    y2 = y1 + s.height;
}

Rect16::Rect16(const Rect16& other)
{
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
}

Rect16::Rect16(const Rect& other)
{
    x1 = clamp16(other.x1);
    y1 = clamp16(other.y1);
    x2 = clamp16(other.x2);
    y2 = clamp16(other.y2);
}

Rect16::Rect16(int16_t x, int16_t y, int16_t width, int16_t height)
{
    x1 = x;
    y1 = y;
    x2 = x + width;
    y2 = y + height;
}

bool Rect16::isNull() const
{
    if (x1 == x2 && y1 == y2) return true;
    return false;
}

int16_t Rect16::left() const
{
    return x1;
}

int16_t Rect16::right() const
{
    return x2 - 1;
}

int16_t Rect16::top() const
{
    return y1;
}

int16_t Rect16::bottom() const
{
    return y2 - 1;
}

int16_t Rect16::width() const
{
    return x2 - x1;
}

int16_t Rect16::height() const
{
    return y2 - y1;
}

Size16 Rect16::size() const
{
    return Size16(x2 - x1, y2 - y1);
}

Point16 Rect16::topLeft() const
{
    return Point16(x1, y1);
}

Point16 Rect16::topRight() const
{
    return Point16(x2, y1);
}

Point16 Rect16::bottomLeft() const
{
    return Point16(x1, y2);
}

Point16 Rect16::bottomRight() const
{
    return Point16(x2, y2);
}

inline static void swap_int(int16_t& i1, int16_t& i2)
{
    int16_t t = i1;
    i1 = i2;
    i2 = t;
}

Rect16 Rect16::normalized() const
{
    Rect16 r(*this);
    if (x2 < x1) swap_int(r.x1, r.x2);
    if (y2 < y1) swap_int(r.y1, r.y2);
    return r;
}

bool Rect16::intersects(const Rect16& other)
{
    return (other.x1 < x2 && other.x2 > x1 && other.y1 < y2 && other.y2 > y1);
}

#ifndef max
inline static int16_t max(int16_t v1, int16_t v2)
{
    if (v1 > v2) return v1;
    return v2;
}
#endif

#ifndef min
inline static int16_t min(int16_t v1, int16_t v2)
{
    if (v1 < v2) return v1;
    return v2;
}
#endif

Rect16 Rect16::intersected(const Rect16& other)
{
    Rect16 r;
    if (isNull() == true || other.isNull() == true) return r;
    if (!intersects(other)) return r;
    r.x1 = max(x1, other.x1);
    r.y1 = max(y1, other.y1);
    r.x2 = min(x2, other.x2);
    r.y2 = min(y2, other.y2);
    return r;
}

void Rect16::setTopLeft(const Point16& topLeft)
{
    x1 = topLeft.x;
    y1 = topLeft.y;
}

void Rect16::setBottomRight(const Point16& bottomRight)
{
    x2 = bottomRight.x;
    y2 = bottomRight.y;
}

void Rect16::setRect(int16_t x, int16_t y, int16_t width, int16_t height)
{
    x1 = x;
    y1 = y;
    x2 = x + width;
    y2 = y + height;
}

void Rect16::setRect(const Rect16& other)
{
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
}

void Rect16::setCoords(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

void Rect16::setCoords(const Point16& p1, const Point16& p2)
{
    x1 = p1.x;
    y1 = p1.y;
    x2 = p2.x;
    y2 = p2.y;
}

void Rect16::setLeft(int16_t left)
{
    x1 = left;
}

void Rect16::setRight(int16_t right)
{
    x2 = right;
}

void Rect16::setTop(int16_t top)
{
    y1 = top;
}

void Rect16::setBottom(int16_t bottom)
{
    y2 = bottom;
}

void Rect16::setX(int16_t x)
{
    x1 = x;
}

void Rect16::setY(int16_t y)
{
    y1 = y;
}

void Rect16::setSize(const Size16& size)
{
    x2 = x1 + size.width;
    y2 = y1 + size.height;
}

void Rect16::setWidth(int16_t width)
{
    x2 = x1 + width;
}

void Rect16::setHeight(int16_t height)
{
    y2 = y1 + height;
}

bool operator!=(const Rect16& r1, const Rect16& r2)
{
    if (r1.x1 != r2.x1) return true;
    if (r1.y1 != r2.y1) return true;
    if (r1.x2 != r2.x2) return true;
    if (r1.y2 != r2.y2) return true;
    return false;
}

bool operator==(const Rect16& r1, const Rect16& r2)
{
    if (r1.x1 != r2.x1) return false;
    if (r1.y1 != r2.y1) return false;
    if (r1.x2 != r2.x2) return false;
    if (r1.y2 != r2.y2) return false;
    return true;
}

} // namespace picopplib
