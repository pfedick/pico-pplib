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

#ifndef PICOPPLIB_GRAFIX_POINT_H_
#define PICOPPLIB_GRAFIX_POINT_H_

#include <stdint.h>
#include <stddef.h>
namespace picopplib
{

class Point;
class Point16;
class Size;
class Size16;
class Rect;
class Rect16;

class Point
{
public:
    int x, y;

    Point();
    Point(int x, int y);
    Point(const Point16& other) noexcept;
    bool isNull() const;
    bool inside(const Rect& r) const;
    float vectorLength() const;
    int manhattanLength() const;
    void setX(int x);
    void setY(int y);
    void setPoint(int x, int y);
    void setPoint(const Point& other);

    Point& operator=(const Point16& other) noexcept;

    Point& operator*=(float factor);
    Point& operator+=(const Point& point);
    Point& operator-=(const Point& point);
    Point& operator/=(float divisor);

    bool operator<(const Point& other) const;
    bool operator<=(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    bool operator>=(const Point& other) const;
    bool operator>(const Point& other) const;
};

const Point operator*(const Point& point, float factor);
const Point operator*(float factor, const Point& point);
const Point operator+(const Point& p1, const Point& p2);
const Point operator-(const Point& p1, const Point& p2);
const Point operator-(const Point& point);
const Point operator/(const Point& point, float divisor);
float Distance(const Point& p1, const Point& p2);

class Point16
{
public:
    int16_t x, y;

    Point16();
    Point16(int16_t x, int16_t y);
    Point16(const Point& other) noexcept;
    bool isNull() const;
    bool inside(const Rect16& r) const;
    float vectorLength() const;
    int manhattanLength() const;
    void setX(int16_t x);
    void setY(int16_t y);
    void setPoint(int16_t x, int16_t y);
    void setPoint(const Point16& other);
    void setPoint(const Point& other);
    Point16& operator=(const Point& other) noexcept;
    Point16& operator*=(float factor);
    Point16& operator+=(const Point16& point);
    Point16& operator-=(const Point16& point);
    Point16& operator/=(float divisor);

    bool operator<(const Point16& other) const;
    bool operator<=(const Point16& other) const;
    bool operator==(const Point16& other) const;
    bool operator!=(const Point16& other) const;
    bool operator>=(const Point16& other) const;
    bool operator>(const Point16& other) const;
};

const Point16 operator*(const Point16& point, float factor);
const Point16 operator*(float factor, const Point16& point);
const Point16 operator+(const Point16& p1, const Point16& p2);
const Point16 operator-(const Point16& p1, const Point16& p2);
const Point16 operator-(const Point16& point);
const Point16 operator/(const Point16& point, float divisor);
float Distance(const Point16& p1, const Point16& p2);

} // namespace picopplib
#endif // PICOPPLIB_GRAFIX_POINT_H_