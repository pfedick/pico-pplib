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

Rect::Rect()
{
    x = 0;
    y = 0;
    w = 0;
    h = 0;
}

Rect::Rect(const Point& p1, const Point& p2)
{
    x = p1.x;
    y = p1.y;
    w = p2.x - p1.x + 1;
    h = p2.y - p1.y + 1;
}

Rect::Rect(const Point& p, const Size& s)
{
    x = p.x;
    y = p.y;
    w = s.width;
    h = s.height;
}

Rect::Rect(const Rect16& other)
{
    x = other.x;
    y = other.y;
    w = other.w;
    h = other.h;
}

Rect::Rect(int x, int y, int width, int height)
{
    this->x = x;
    this->y = y;
    this->w = width;
    this->h = height;
}

bool Rect::isNull() const
{
    if (w == 0 || h == 0) return true;
    return false;
}

int Rect::left() const
{
    return x;
}

int Rect::right() const
{
    return x + w - 1;
}

int Rect::top() const
{
    return y;
}

int Rect::bottom() const
{
    return y + h - 1;
}

int Rect::width() const
{
    return w;
}

int Rect::height() const
{
    return h;
}

Size Rect::size() const
{
    return Size(w, h);
}

Point Rect::topLeft() const
{
    return Point(x, y);
}

Point Rect::topRight() const
{
    return Point(x + w - 1, y);
}

Point Rect::bottomLeft() const
{
    return Point(x, y + h - 1);
}

Point Rect::bottomRight() const
{
    return Point(x + w - 1, y + h - 1);
}

inline static void swap_int(int& i1, int& i2)
{
    int t = i1;
    i1 = i2;
    i2 = t;
}

Rect Rect::normalized() const
{
    Rect r(*this);
    if (x2 < x1) swap_int(r.x1, r.x2);
    if (y2 < y1) swap_int(r.y1, r.y2);
    return r;
}

bool Rect::intersects(const Rect& other)
{
    return (other.x1 < x2 && other.x2 > x1 && other.y1 < y2 && other.y2 > y1);
}

#ifndef max
inline static int max(int v1, int v2)
{
    if (v1 > v2) return v1;
    return v2;
}
#endif

#ifndef min
inline static int min(int v1, int v2)
{
    if (v1 < v2) return v1;
    return v2;
}
#endif

Rect Rect::intersected(const Rect& other)
{
    Rect r;
    if (isNull() == true || other.isNull() == true) return r;
    if (!intersects(other)) return r;
    r.x1 = max(x1, other.x1);
    r.y1 = max(y1, other.y1);
    r.x2 = min(x2, other.x2);
    r.y2 = min(y2, other.y2);
    return r;
}

void Rect::setTopLeft(const Point& topLeft)
{
    x1 = topLeft.x;
    y1 = topLeft.y;
}

void Rect::setBottomRight(const Point& bottomRight)
{
    x2 = bottomRight.x;
    y2 = bottomRight.y;
}

void Rect::setRect(int x, int y, int width, int height)
{
    x1 = x;
    y1 = y;
    x2 = x + width;
    y2 = y + height;
}

void Rect::setRect(const Rect& other)
{
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
}

void Rect::setCoords(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

void Rect::setCoords(const Point& p1, const Point& p2)
{
    x1 = p1.x;
    y1 = p1.y;
    x2 = p2.x;
    y2 = p2.y;
}

void Rect::setLeft(int left)
{
    x1 = left;
}

void Rect::setRight(int right)
{
    x2 = right;
}

void Rect::setTop(int top)
{
    y1 = top;
}

void Rect::setBottom(int bottom)
{
    y2 = bottom;
}

void Rect::setX(int x)
{
    x1 = x;
}

void Rect::setY(int y)
{
    y1 = y;
}

void Rect::setSize(const Size& size)
{
    x2 = x1 + size.width;
    y2 = y1 + size.height;
}

void Rect::setWidth(int width)
{
    x2 = x1 + width;
}

void Rect::setHeight(int height)
{
    y2 = y1 + height;
}

bool operator!=(const Rect& r1, const Rect& r2)
{
    if (r1.x1 != r2.x1) return true;
    if (r1.y1 != r2.y1) return true;
    if (r1.x2 != r2.x2) return true;
    if (r1.y2 != r2.y2) return true;
    return false;
}

bool operator==(const Rect& r1, const Rect& r2)
{
    if (r1.x1 != r2.x1) return false;
    if (r1.y1 != r2.y1) return false;
    if (r1.x2 != r2.x2) return false;
    if (r1.y2 != r2.y2) return false;
    return true;
}

} // namespace picopplib
