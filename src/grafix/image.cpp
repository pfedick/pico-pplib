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
Image::Image() {}

Image::Image(const Image& other)
{
    myMemory = other.myMemory;
    Drawable::create((void*)myMemory.adr(), other.format().bytesForWidth(other.width()), other.width(), other.height(), other.format());
}

Image::Image(Image&& other) noexcept
    : Drawable(std::move(other)),
      myMemory(std::move(other.myMemory))
{
    Drawable::create((void*)myMemory.adr(), format().bytesForWidth(width()), width(), height(), format());
}

Image::Image(const Drawable& other)
{
    copy(other);
}

Image::Image(uint16_t width, uint16_t height, const RGBFormat& format)
{
    create(width, height, format);
}

void Image::create(uint16_t width, uint16_t height, const RGBFormat& format)
{
    size_t bytes = height * format.bytesForWidth(width);
    void* data = myMemory.malloc(bytes);
    Drawable::create(data, format.bytesForWidth(width), width, height, format);
}

void Image::copy(const Drawable& other)
{
    create(other.width(), other.height(), other.format());
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            putPixelDirect(x, y, other.getPixelDirect(x, y));
        }
    }
}

void Image::copy(const Image& other)
{
    myMemory = other.myMemory;
    Drawable::create((void*)myMemory.adr(), other.format().bytesForWidth(other.width()), other.width(), other.height(), other.format());
}

void Image::copy(const Drawable& other, const Rect& rect)
{
    create(rect.width(), rect.height(), other.format());
    for (int y = 0; y < rect.height(); y++) {
        for (int x = 0; x < rect.width(); x++) {
            putPixelDirect(x, y, other.getPixelDirect(rect.left() + x, rect.top() + y));
        }
    }
}

Image& Image::operator=(const Image& other)
{
    copy(other);
    return *this;
}

Image& Image::operator=(Image&& other) noexcept
{
    if (this != &other) {
        Drawable::operator=(std::move(other));
        myMemory = std::move(other.myMemory);
        Drawable::create((void*)myMemory.adr(), format().bytesForWidth(width()), width(), height(), format());
    }
    return *this;
}

Image& Image::operator=(const Drawable& other)
{
    copy(other);
    return *this;
}

size_t Image::numBytes() const
{
    return myMemory.size();
}

ByteArrayPtr Image::memory() const
{
    return myMemory;
}

Image::operator ByteArrayPtr() const
{
    return myMemory;
}

} // namespace picopplib