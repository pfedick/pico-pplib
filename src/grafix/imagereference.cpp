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

ImageReference::ImageReference()
{
    draw_method = DrawMethod::ALPHABLT;
}

ImageReference::ImageReference(const Drawable& draw, DrawMethod method, const Color& diffuse)
{
    this->draw_method = method;
    this->diffuse_color = diffuse;
    this->pixel = draw;
}

Size ImageReference::size() const
{
    return Size(pixel.width(), pixel.height());
}

DrawMethod ImageReference::drawMethod() const
{
    return draw_method;
}

const Drawable& ImageReference::getDrawable() const
{
    return pixel;
}

Color ImageReference::diffuseColor() const
{
    return diffuse_color;
}

void ImageReference::setDrawMethod(DrawMethod method)
{
    this->draw_method = method;
}

void ImageReference::setDiffuseColor(const Color& c)
{
    this->diffuse_color = c;
}

void ImageReference::setDrawable(const Drawable& draw)
{
    this->pixel = draw;
}

void ImageReference::useDrawable(const Drawable& draw, DrawMethod method, const Color& diffuse)
{
    this->pixel = draw;
    this->draw_method = method;
    this->diffuse_color = diffuse;
}

} // namespace picopplib