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

#ifndef PICOPPLIB_GRAFIX_H_
#define PICOPPLIB_GRAFIX_H_

#include <stdint.h>
#include <stddef.h>
#include "picopplib.h"
#include <list>
#include <map>

#include "picopplib/grafix/rect.h"
#include "picopplib/grafix/point.h"
#include "picopplib/grafix/size.h"
#include "picopplib/grafix/color.h"
#include "picopplib/grafix/fonts.h"

namespace picopplib
{

/**@class RGBFormat
 * @brief Datentyp, der das Farbformat einer Zeichenfläche repräsentiert
 *
 * Mit dieser Klasse wird das Farbformat einer Zeichenfläche repräsentiert. Sie enthält nur
 * einen einzigen Wert aus der Enumeration RGBFormat::Identifier, der das verwendete
 * Farbformat festlegt.
 */
class RGBFormat
{
public:
    /// @brief Enumeration der unterstützten Farbformate
    enum Identifier
    {
        unknown = 0,            ///< 0: Unbekanntes Format
        Monochrome1BitVertical, ///< 1 Bit pro Pixel, vertikal gepackt (z.B. SSD1322)
        R5G6B5,                 ///< 16 Bit pro Pixel: 5 Bit Rot, 6 Bit Grün, 5 Bit Blau
        A8R8G8B8,               ///< 32 Bit pro Pixel: 8 Bit Alpha, 8 Bit Rot, 8 Bit Grün, 8 Bit Blau
        GREY8,                  ///< 8 Bit pro Pixel: 8 Bit Graustufe
        MaxIdentifiers          ///< Obergrenze der Identifikatoren
    };

private:
    /**
     * @brief Das aktuell gesetzte Farbformat.
     */
    Identifier format_id;

public:
    RGBFormat();
    RGBFormat(Identifier id);

    /**@brief Farbformat Anhand einer ID festlegen
     *
     * Mit dieser Funktion wird das Farbformat anhand eines Wertes aus der Enumeration
     * RGBFormat::Identifier gesetzt.
     *
     * @param[in] id Wert aus der Enumeration RGBFormat::Identifier
     */
    void setFormat(Identifier id);

    /**@brief Integer-Wert des Farbformats auslesen
     *
     * Mit dieser Funktion wird der Integer-Wert des Farbformates ausgelesen.
     * Der Wert entspricht der Position des Farbformates innerhalb der Enumeration RGBFormat::Identifier.
     *
     * @return Integer-Wert, der das Farbformat repräsentiert.
     */
    Identifier format() const;

    /**@brief Bittiefe des Farbformates
     *
     * Diese Funktion liefert die Anzahl Bits zurück, die zur Darstellung des gewählten
     * Farbformats erforderlich sind. Ist in der Regel 8, 16, 24, 32 oder 64. Die Funktion ist
     * identisch mit RGBFormat::bitsPerPixel.
     *
     * @return Anzahl erforderlicher Bits oder 0, wenn kein Farbformat gesetzt ist.
     */
    uint8_t bitdepth() const;

    /**@brief Anzahl Bytes pro Pixel
     *
     * Diese Funktion liefert die Anzahl Bytes für eine bestimmte Anzahl von Pixeln zurück,
     * abhängig vom gewählten Farbformat.
     *
     * @param[in] width Anzahl Pixel, für die die erforderlichen Bytes berechnet werden sollen.
     * @return Anzahl erforderlicher Bytes pro Pixel oder 0, wenn kein Farbformat gesetzt ist.
     * @note Sonderfall ist das Farbformat Monochrome1BitVertical, bei dem die Pixel
     * vertikal gepackt sind. In diesem Fall entspricht die Anzahl Bytes der Anzahl Pixel,
     * da 8 Pixel pro Byte gespeichert werden, aber vertikal gepackt, so dass es einfach
     * width ist.
     */
    uint32_t bytesForWidth(uint16_t width) const;

    bool operator==(const RGBFormat& other) const;
    bool operator==(Identifier id) const;
};
inline bool operator==(RGBFormat::Identifier id, const RGBFormat& fmt)
{
    return fmt == id;
}

class Image;
class ImageList;
class ImageReference;

class Drawable
{
private:
    uint8_t* buffer;
    uint32_t pitch;
    uint16_t my_width;
    uint16_t my_height;
    RGBFormat rgb_format;

    typedef void (*PutPixelFunc)(Drawable& self, int x, int y, uint32_t c);
    typedef void (*BlendPixelFunc)(Drawable& self, int x, int y, uint32_t c, uint8_t intensity);
    typedef void (*FillRectFunc)(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c);
    typedef uint32_t (*GetPixelFunc)(const Drawable& self, int x, int y);
    PutPixelFunc putPixelImpl;
    GetPixelFunc getPixelImpl;
    BlendPixelFunc blendPixelImpl;
    FillRectFunc fillRectImpl;

    // Static Implementierungen für verschiedene Formate
    static void putPixelMonochrome1BitVertical(Drawable& self, int x, int y, uint32_t c);
    static void blendPixelMonochrome1BitVertical(Drawable& self, int x, int y, uint32_t c, uint8_t intensity);
    static uint32_t getPixelMonochrome1BitVertical(const Drawable& self, int x, int y);
    static void fillRectMonochrome1BitVertical(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c);

    static void putPixel16BitR5G6B5(Drawable& self, int x, int y, uint32_t c);
    static void blendPixel16BitR5G6B5(Drawable& self, int x, int y, uint32_t c, uint8_t intensity);
    static uint32_t getPixel16BitR5G6B5(const Drawable& self, int x, int y);
    static void fillRect16BitR5G6B5(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c);

    static void putPixel32BitA8R8G8B8(Drawable& self, int x, int y, uint32_t c);
    static void blendPixel32BitA8R8G8B8(Drawable& self, int x, int y, uint32_t c, uint8_t intensity);
    static uint32_t getPixel32BitA8R8G8B8(const Drawable& self, int x, int y);
    static void fillRect32BitA8R8G8B8(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c);

    static void putPixel8BitGREY(Drawable& self, int x, int y, uint32_t c);
    static void blendPixel8BitGREY(Drawable& self, int x, int y, uint32_t c, uint8_t intensity);
    static uint32_t getPixel8BitGREY(const Drawable& self, int x, int y);
    static void fillRect8BitGREY(Drawable& self, int x1, int y1, int x2, int y2, uint32_t c);

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
    void use(const Drawable& other);

    Drawable getDrawable(const Rect& rect) const;
    Drawable getDrawable(const Rect16& rect) const;
    Drawable getDrawable(const Point& p, const Size& s) const;
    Drawable getDrawable(const Point16& p, const Size16& s) const;
    Drawable getDrawable(int x1, int y1, int x2, int y2) const;

    inline uint16_t width() const { return my_width; };
    inline uint16_t height() const { return my_height; };
    inline RGBFormat format() const { return rgb_format; };

    bool isEmpty() const;

    uint8_t* ptr() const;
    Size size() const;
    Size16 size16() const;
    Rect rect() const;
    Rect16 rect16() const;

    inline void putPixel(int x, int y, const Color& color) { putPixelImpl(*this, x, y, toNativeColor(color)); }
    inline void blendPixel(int x, int y, const Color& color, uint8_t intensity)
    {
        blendPixelImpl(*this, x, y, toNativeColor(color), intensity);
    }
    inline Color getPixel(int x, int y) const { return fromNativeColor(getPixelImpl(*this, x, y)); }

    inline void putPixelDirect(int x, int y, uint32_t native_color) { putPixelImpl(*this, x, y, native_color); }
    inline void blendPixelDirect(int x, int y, uint32_t native_color, uint8_t intensity)
    {
        blendPixelImpl(*this, x, y, native_color, intensity);
    }
    inline uint32_t getPixelDirect(int x, int y) const { return getPixelImpl(*this, x, y); }

    void clear(const Color& color = Color(0, 0, 0, 0));

    // void blendPixel(int x, int y, const Color& c, int brightness);

    void drawRect(int x1, int y1, int x2, int y2, const Color& color);
    inline void fillRect(int x1, int y1, int x2, int y2, const Color& color) { fillRectImpl(*this, x1, y1, x2, y2, toNativeColor(color)); }
    void invertRect(int x1, int y1, int x2, int y2);

    void line(int x1, int y1, int x2, int y2, const Color& color);
    void line(const Point& start, const Point& end, const Color& c);
    void lineAA(int x1, int y1, int x2, int y2, const Color& c, int strength = 1);
    void lineAA(const Point& start, const Point& end, const Color& c, int strength = 1);

    void floodFill(int x, int y, const Color& color, const Color& border);
    void elipse(int x, int y, int radx, int rady, const Color& c, bool fill = false);
    void elipse(int x, int y, int radx, int rady, const Color& c, bool fill, const Color& fillcolor, int startangle, int endangle);
    void circle(int x, int y, int rad, const Color& c, bool fill = false);
    void circle(const Point& p, int rad, const Color& c, bool fill = false);

    void print(const Font& font, int x, int y, const String& text);
    void printf(const Font& font, int x, int y, const char* fmt, ...);

    void draw(const ImageList& iml, int nr, int x, int y);
    void draw(const ImageList& iml, int nr, int x, int y, const Color& diffuse);
    void draw(const ImageReference& imgref, int x, int y);
    void drawBlend(const ImageReference& imgref, int x, int y, float factor);

    void colorGradient(const Rect16& rect, const Color& c1, const Color& c2, int direction);
    void colorGradient(int x1, int y1, int x2, int y2, const Color& c1, const Color& c2, int direction);

    // Blit-Funktionen
    int fitRect(int& x, int& y, Rect16& r);
    void blt(const Drawable& source, int x = 0, int y = 0);
    void blt(const Drawable& source, const Rect16& srect, int x = 0, int y = 0);
    void bltDiffuse(const Drawable& source, int x = 0, int y = 0, const Color& c = Color());
    void bltDiffuse(const Drawable& source, const Rect16& srect, int x = 0, int y = 0, const Color& c = Color());
    void bltAlpha(const Drawable& source, int x = 0, int y = 0);
    void bltAlpha(const Drawable& source, const Rect16& srect, int x = 0, int y = 0);
    void bltBlend(const Drawable& source, float factor, int x = 0, int y = 0);
    void bltBlend(const Drawable& source, float factor, const Rect16& srect, int x = 0, int y = 0);
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

enum class DrawMethod : uint8_t
{
    BLT = 1,
    ALPHABLT,
    DIFFUSE
};

class ImageReference
{
    friend class Drawable;

private:
    Color diffuse_color;
    Drawable pixel;
    DrawMethod draw_method;

public:
    ImageReference();
    ImageReference(const Drawable& draw, DrawMethod method = DrawMethod::ALPHABLT, const Color& diffuse = Color());
    Size16 size() const;
    DrawMethod drawMethod() const;
    const Drawable& getDrawable() const;
    Color diffuseColor() const;
    void setDrawMethod(DrawMethod method);
    void setDiffuseColor(const Color& c);
    void setDrawable(const Drawable& draw);
    void useDrawable(const Drawable& draw, DrawMethod method, const Color& diffuse = Color());
    inline uint16_t width() const { return pixel.width(); };
    inline uint16_t height() const { return pixel.height(); };
    inline RGBFormat format() const { return pixel.format(); };
    inline bool isEmpty() const { return pixel.isEmpty(); };
};

class ImageList
{
    friend class Drawable;

public:
private:
    uint16_t numIcons;
    uint16_t width, height;
    uint16_t numX, numY;
    Color diffuse;
    DrawMethod method;
    Drawable pixel;

public:
    ImageList();
    ImageList(const Drawable& draw, int icon_width, int icon_height, DrawMethod method);
    ~ImageList();

    void clear();
    void setDrawMethod(DrawMethod method);
    void setDiffuseColor(const Color& c);
    void setIconSize(int width, int height);
    void useDrawable(const Drawable& draw, int icon_width, int icon_height, DrawMethod method);

    size_t num() const;
    Size iconSize() const;
    Rect16 getRect(size_t nr) const;
    DrawMethod drawMethod() const;
    Drawable getDrawable(size_t nr) const;
    Color diffuseColor() const;
    ImageReference getImageReference(size_t nr) const;
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

} // namespace picopplib

#endif // PICOPPLIB_GRAFIX_H_
