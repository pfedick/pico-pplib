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

#ifndef PICOPPLIB_GRAFIX_FONTS_H_
#define PICOPPLIB_GRAFIX_FONTS_H_

#include <stdint.h>
#include <stddef.h>
#include "picopplib.h"

#ifndef PICOPPLIB_GRAFIX_COLOR_H
#include "picopplib/grafix/color.h"
#endif

namespace picopplib
{
class Size16;
class Rect16;
class Drawable;
class FontEngine;
class FontFile;

class Font
{
    friend bool operator!=(const Font& f1, const Font& f2);
    friend bool operator==(const Font& f1, const Font& f2);

private:
    enum FontFlags
    {
        fBold = 1,
        fItalic = 2,
        fAntialias = 4,
        fDrawBorder = 8,
        fDrawShadow = 16,
        fUnderline = 32,
        fMonospace = 64
    };

    String Name;
    Color cForeground;
    Color cBorder;
    Color cShadow;
    uint16_t fontSize;
    uint8_t flags;
    uint8_t ori;
    uint16_t rotationDegrees;

public:
    enum Orientation
    {
        LEFT = 1,
        MIDDLE,
        RIGHT,
        TOP,
        BOTTOM,
        BASE
    };

    Font();
    Font(const Font& other);
    Font(Font&& other) noexcept;

    const String& name() const;
    const Color& color() const;
    const Color& borderColor() const;
    const Color& shadowColor() const;
    bool bold() const;
    bool italic() const;
    bool antialias() const;
    bool drawBorder() const;
    bool drawShadow() const;
    bool drawUnderline() const;
    bool monospace() const;
    uint16_t size() const;
    Orientation orientation() const;
    Size16 measure(const String& text) const;
    Rect16 boundary(const String& text, int x = 0, int y = 0) const;
    uint16_t rotation() const;

    int setName(const String& name);
    void setColor(const Color& c);
    void setBorderColor(const Color& c);
    void setShadowColor(const Color& c);
    void setBold(bool enable);
    void setItalic(bool enable);
    void setAntialias(bool enable);
    void setDrawBorder(bool enable);
    void setDrawShadow(bool enable);
    void setDrawUnderline(bool enable);
    void setMonospace(bool enable);
    void setSize(uint16_t size);
    void setOrientation(Orientation o);
    void setRotation(uint16_t degrees);

    Font& operator=(const Font& other);
    Font& operator=(Font&& other) noexcept;
};
bool operator!=(const Font& f1, const Font& f2);
bool operator==(const Font& f1, const Font& f2);

class FontFile
{
    friend class Grafix;

private:
public:
    String Name;
    ByteArray Memory;
    FontEngine* engine;
    void* priv;

    FontFile();
    ~FontFile();
};

class FontEngine
{
    friend class Grafix;

private:
public:
    virtual void init() = 0;
    virtual bool ident(const ByteArrayPtr& memory) const throw() = 0;
    virtual FontFile* loadFont(const ByteArrayPtr& memory, const String& fontname) = 0;
    virtual void deleteFont(FontFile* file) = 0;
    virtual void render(const FontFile& file, const Font& font, Drawable& draw, int x, int y, const String& text, int color) const = 0;
    virtual Size16 measure(const FontFile& file, const Font& font, const String& text) const = 0;
    virtual Rect16 boundary(const FontFile& file, const Font& font, const String& text, int x, int y) const = 0;
    virtual String name() const = 0;
    virtual String description() const = 0;
};

class FontEngineFont6 : public FontEngine
{
private:
public:
    FontEngineFont6();
    ~FontEngineFont6();
    void init() override;
    bool ident(const ByteArrayPtr& memory) const throw() override;
    FontFile* loadFont(const ByteArrayPtr& memory, const String& fontname) override;
    void deleteFont(FontFile* file) override;
    void render(const FontFile& file, const Font& font, Drawable& draw, int x, int y, const String& text, int native_color) const override;
    Size16 measure(const FontFile& file, const Font& font, const String& text) const override;
    Rect16 boundary(const FontFile& file, const Font& font, const String& text, int x, int y) const override;
    String name() const override;
    String description() const override;
};

} // namespace picopplib

#endif