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

/*!\class ImageList
 * \ingroup PPLGroupGrafik
 * \brief Ein Container für eine Grafik mit mehreren gleichgroßen Icons
 * @return
 */

ImageList::ImageList()
{
    method = DrawMethod::BLT;
    width = height = 0;
    numIcons = 0;
    numX = numY = 0;
}

ImageList::ImageList(const Drawable& draw, int icon_width, int icon_height, DrawMethod method)
{
    useDrawable(draw, icon_width, icon_height, method);
}

ImageList::~ImageList() {}

void ImageList::clear()
{
    method = DrawMethod::BLT;
    width = height = 0;
    numIcons = 0;
    numX = numY = 0;
}

void ImageList::useDrawable(const Drawable& draw, int icon_width, int icon_height, DrawMethod method)
{
    pixel = draw;
    this->method = method;
    numX = draw.width() / icon_width;
    numY = draw.height() / icon_height;
    numIcons = numX * numY;
}

void ImageList::setDrawMethod(DrawMethod method)
{
    this->method = method;
}

void ImageList::setDiffuseColor(const Color& c)
{
    diffuse = c;
}

void ImageList::setIconSize(int width, int height)
{
    this->width = width;
    this->height = height;
    numX = pixel.width() / width;
    numY = pixel.height() / height;
    numIcons = numX * numY;
}

size_t ImageList::num() const
{
    return numIcons;
}

Size ImageList::iconSize() const
{
    return Size(width, height);
}

Rect ImageList::getRect(size_t nr) const
{
    Rect r;
    if (numIcons == 0 || nr >= numIcons) return r;
    int h, w;
    h = (int)(nr / numX);
    w = (int)(nr % numX);
    r.setRect(w * width, h * height, width, height);
    return r;
}

ImageList::DrawMethod ImageList::drawMethod() const
{
    return method;
}

Drawable ImageList::getDrawable(size_t nr) const
{
    Rect r = getRect(nr);
    return pixel.getDrawable(r);
}

Color ImageList::diffuseColor() const
{
    return diffuse;
}

} // namespace picopplib