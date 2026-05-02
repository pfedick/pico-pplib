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

#ifndef PICOPPLIBGRAFIX_H_
#define PICOPPLIBGRAFIX_H_

#include <stdint.h>
#include <stddef.h>
#include "picopplib.h"
#include <list>
#include <map>

namespace picopplib
{

// Microsoft kompatible Strukturen
typedef struct _RECT
{
    long left;
    long top;
    long right;
    long bottom;
} RECT, *PRECT;

typedef struct tagPOINT
{
    long x;
    long y;
} POINT, *PPOINT;

typedef struct tagSIZE
{
    long cx;
    long cy;
} SIZE, *PSIZE;

class Rect;

class Point
{
public:
    int x, y;

    Point();
    Point(int x, int y);
    Point(const Point& other);
    bool isNull() const;
    bool inside(const Rect& r) const;
    double vectorLength() const;
    int manhattanLength() const;
    void setX(int x);
    void setY(int y);
    void setPoint(int x, int y);
    void setPoint(const Point& other);
    Point& operator*=(double factor);
    Point& operator+=(const Point& point);
    Point& operator-=(const Point& point);
    Point& operator/=(double divisor);

    bool operator<(const Point& other) const;
    bool operator<=(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    bool operator>=(const Point& other) const;
    bool operator>(const Point& other) const;
};

const Point operator*(const Point& point, double factor);
const Point operator*(double factor, const Point& point);
const Point operator+(const Point& p1, const Point& p2);
const Point operator-(const Point& p1, const Point& p2);
const Point operator-(const Point& point);
const Point operator/(const Point& point, double divisor);
double Distance(const Point& p1, const Point& p2);

class Size
{
public:
    int width, height;

    Size();
    Size(int width, int height);
    Size(const Size& other);
    bool isNull() const;
    bool isEmpty() const;
    bool isValid() const;
    void setHeight(int height);
    void setWidth(int width);
    void setSize(int width, int height);
    void setSize(const Size& s);
    Size& operator*=(double factor);
    Size& operator+=(const Size& size);
    Size& operator-=(const Size& size);
    Size& operator/=(double divisor);
};

bool operator!=(const Size& s1, const Size& s2);
bool operator==(const Size& s1, const Size& s2);
const Size operator*(const Size& size, double factor);
const Size operator*(double factor, const Size& size);
const Size operator+(const Size& s1, const Size& s2);
const Size operator-(const Size& s1, const Size& s2);
const Size operator-(const Size& size);
const Size operator/(const Size& size, double divisor);

class Rect
{
    friend bool operator!=(const Rect& r1, const Rect& r2);
    friend bool operator==(const Rect& r1, const Rect& r2);

private:
public:
    int x1, y1;
    int x2, y2;
    Rect();
    Rect(const Point& p1, const Point& p2);
    Rect(const Point& p, const Size& s);
    Rect(int x, int y, int width, int height);
    Rect(const Rect& r);
    Rect(const RECT& r);

    bool isNull() const;
    int left() const;
    int right() const;
    int top() const;
    int bottom() const;
    int width() const;
    int height() const;
    Size size() const;

    Point topLeft() const;
    Point topRight() const;
    Point bottomLeft() const;
    Point bottomRight() const;

    Rect normalized() const;
    Rect intersected(const Rect& other);
    bool intersects(const Rect& other);

    void setTopLeft(const Point& topLeft);
    void setBottomRight(const Point& bottomRight);
    void setRect(const RECT& r);
    void setRect(const Rect& r);
    void setRect(int x, int y, int width, int height);
    void setCoords(int x1, int y1, int x2, int y2);
    void setCoords(const Point& p1, const Point& p2);
    void setLeft(int left);
    void setRight(int right);
    void setTop(int top);
    void setBottom(int bottom);
    void setX(int x);
    void setY(int y);
    void setSize(const Size& size);
    void setWidth(int width);
    void setHeight(int height);
};

bool operator!=(const Rect& r1, const Rect& r2);
bool operator==(const Rect& r1, const Rect& r2);

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
    inline uint8_t red() const { return r; }
    inline uint8_t green() const { return g; }
    inline uint8_t blue() const { return b; }
    inline uint8_t alpha() const { return a; }
    inline void setRed(uint8_t red) { r = red; }
    inline void setGreen(uint8_t green) { g = green; }
    inline void setBlue(uint8_t blue) { b = blue; }
    inline void setAlpha(uint8_t alpha) { a = alpha; }

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
};
const Color operator*(const Color& color, float factor);
const Color operator*(float factor, const Color& color);
const Color operator+(const Color& color1, const Color& color2);

Color blendColor(const Color& background, const Color& foreground, int intensity);
Color blendColor(const Color& background, const Color& foreground, float intensity);

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
    double rotationDegrees;

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

    const String& name() const;
    Color color() const;
    Color borderColor() const;
    Color shadowColor() const;
    bool bold() const;
    bool italic() const;
    bool antialias() const;
    bool drawBorder() const;
    bool drawShadow() const;
    bool drawUnderline() const;
    bool monospace() const;
    int size() const;
    Orientation orientation() const;
    Size measure(const String& text) const;
    Rect boundary(const String& text, int x = 0, int y = 0) const;
    double rotation() const;

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
    void setSize(int size);
    void setOrientation(Orientation o);
    void setRotation(double degrees);

    Font& operator=(const Font& other);
};
bool operator!=(const Font& f1, const Font& f2);
bool operator==(const Font& f1, const Font& f2);

class RGBFormat
{
public:
    enum Identifier
    {
        unknown = 0,
        Monochrome1BitVertical, // 1 Bit pro Pixel, vertikal gepackt (z.B. SSD1322)
        R5G6B5,
        A8R8G8B8,
        MaxIdentifiers
    };

private:
    Identifier format_id;

public:
    RGBFormat();
    RGBFormat(Identifier id);

    void setFormat(Identifier id);

    Identifier format() const;
    uint8_t bitdepth() const;
    uint32_t bytesForWidth(uint16_t width) const;

    bool operator==(const RGBFormat& other) const;
    bool operator==(Identifier id) const;
};
inline bool operator==(RGBFormat::Identifier id, const RGBFormat& fmt)
{
    return fmt == id;
}

class Drawable
{
private:
    uint8_t* buffer;
    uint32_t pitch;
    uint16_t my_width;
    uint16_t my_height;
    RGBFormat rgb_format;

    typedef void (*PutPixelFunc)(Drawable& self, int x, int y, uint32_t c);
    typedef uint32_t (*GetPixelFunc)(const Drawable& self, int x, int y);
    PutPixelFunc putPixelImpl;
    GetPixelFunc getPixelImpl;

    // Static Implementierungen für verschiedene Formate
    static void putPixelMonochrome1BitVertical(Drawable& self, int x, int y, uint32_t c);
    static uint32_t getPixelMonochrome1BitVertical(const Drawable& self, int x, int y);

    static void putPixel16BitR5G6B5(Drawable& self, int x, int y, uint32_t c);
    static uint32_t getPixel16BitR5G6B5(const Drawable& self, int x, int y);

    static void putPixel32BitA8R8G8B8(Drawable& self, int x, int y, uint32_t c);
    static uint32_t getPixel32BitA8R8G8B8(const Drawable& self, int x, int y);

    uint32_t toNativeColor(const Color& c) const;
    Color fromNativeColor(uint32_t c) const;

public:
    Drawable();
    Drawable(const Drawable& other);
    Drawable(Drawable&& other) noexcept;
    Drawable(void* buffer, uint32_t pitch, uint16_t width, uint16_t height, const RGBFormat& format);

    Drawable& operator=(const Drawable& other);
    Drawable& operator=(Drawable&& other) noexcept;

    void create(void* buffer, uint32_t pitch, uint16_t width, uint16_t height, const RGBFormat& format);

    Drawable getDrawable(const Rect& rect) const;
    Drawable getDrawable(const Point& p, const Size& s) const;
    Drawable getDrawable(int x1, int y1, int x2, int y2) const;

    inline uint16_t width() const { return my_width; };
    inline uint16_t height() const { return my_height; };
    inline RGBFormat format() const { return rgb_format; };

    bool isEmpty() const;

    uint8_t* ptr() const;
    Size size() const;

    inline void putPixel(int x, int y, const Color& color) { putPixelImpl(*this, x, y, toNativeColor(color)); }
    inline Color getPixel(int x, int y) const { return fromNativeColor(getPixelImpl(*this, x, y)); }

    inline void putPixelDirect(int x, int y, uint32_t native_color) { putPixelImpl(*this, x, y, native_color); }
    inline uint32_t getPixelDirect(int x, int y) const { return getPixelImpl(*this, x, y); }

    void clear(const Color& color = Color(0, 0, 0, 0));

    // void blendPixel(int x, int y, const Color& c, int brightness);

    void drawRect(int x1, int y1, int x2, int y2, const Color& color);
    void fillRect(int x1, int y1, int x2, int y2, const Color& color);
    void invertRect(int x1, int y1, int x2, int y2);
    void line(int x1, int y1, int x2, int y2, const Color& color);

    void print(const Font& font, int x, int y, const String& text);
    void printf(const Font& font, int x, int y, const char* fmt, ...);
};

class Image : public Drawable
{
private:
    ByteArray myMemory;

public:
    Image();
    Image(const Image& other);
    Image(Image&& other) noexcept;
    Image(const Drawable& other);
    Image(uint16_t width, uint16_t height, const RGBFormat& format = RGBFormat::A8R8G8B8);
    void create(uint16_t width, uint16_t height, const RGBFormat& format = RGBFormat::A8R8G8B8);
    void copy(const Image& other);
    void copy(const Drawable& other);
    void copy(const Drawable& other, const Rect& rect);
    Image& operator=(const Image& other);
    Image& operator=(Image&& other) noexcept;

    Image& operator=(const Drawable& other);
    size_t numBytes() const;
    ByteArrayPtr memory() const;
    operator ByteArrayPtr() const;
};

class ImageFilter;
class FontEngine;
class FontFile;

class Grafix
{
private:
    // std::list<ImageFilter*>	ImageFilterList;
    std::list<FontEngine*> FontEngineList;
    std::map<String, FontFile*> FontList;

    void clearFonts();
    void clearFontEngines();

public:
    Grafix();
    ~Grafix();
    // void addImageFilter(ImageFilter* filter);
    // ImageFilter* findImageFilter(const String& name);
    void addFontEngine(FontEngine* engine);
    void loadFont(const ByteArrayPtr& memory, const String& fontname = String());
    const FontFile* findFont(const String& fontname) const;
    const FontFile* findFont(const Font& font) const;
};

Grafix* GetGrafix();

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
    virtual Size measure(const FontFile& file, const Font& font, const String& text) const = 0;
    virtual Rect boundary(const FontFile& file, const Font& font, const String& text, int x, int y) const = 0;
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
    Size measure(const FontFile& file, const Font& font, const String& text) const override;
    Rect boundary(const FontFile& file, const Font& font, const String& text, int x, int y) const override;
    String name() const override;
    String description() const override;
};

} // namespace picopplib

#endif // PICOPPLIBGRAFIX_H_
