#ifndef PICOPPLIB_GRAFIX_DRAWABLE_H
#define PICOPPLIB_GRAFIX_DRAWABLE_H

#include <stdint.h>
#include <stddef.h>

#include "picopplib/grafix/rgbformat.h"
#include "picopplib/grafix/color.h"

namespace picopplib
{
class Point;
class Point16;
class Size;
class Size16;
class Rect;
class Rect16;
class Image;
class ImageList;
class ImageReference;
class String;
class Font;

enum class Orientation : uint8_t
{
    Horizontal = 0,
    Vertical = 1
};

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

    /**@brief getDrawable
     *
     * Diese Funktion gibt ein Drawable zurück, das einen bestimmten Bereich des aktuellen Drawables repräsentiert.
     *
     * @param x1 X-Koordinate der oberen linken Ecke des Bereichs
     * @param y1 Y-Koordinate der oberen linken Ecke des Bereichs
     * @param x2 X-Koordinate der unteren rechten Ecke des Bereichs. Liegt 1 Pixel außerhalb des Bereichs
     * @param y2 Y-Koordinate der unteren rechten Ecke des Bereichs. Liegt 1 Pixel außerhalb des Bereichs
     * @return Ein Drawable, das den angegebenen Bereich repräsentiert.
     *
     * @note Wenn der Bereich außerhalb der Grenzen des aktuellen Drawables liegt, wird er entsprechend angepasst.
     * Liegt er vollständig außerhalb, wird ein leeres Drawable zurückgegeben.
     */
    Drawable getDrawable(int x1, int y1, int x2, int y2) const;

    inline uint16_t width() const { return my_width; };
    inline uint16_t height() const { return my_height; };
    inline RGBFormat format() const { return rgb_format; };

    constexpr inline bool isEmpty() const { return (!buffer); }
    constexpr inline bool notEmpty() const { return (buffer); }

    uint8_t* ptr() const;
    Size size() const;
    Size16 size16() const;
    Rect rect() const;
    Rect16 rect16() const;

    inline void putPixel(int x, int y, const Color& color)
    {
        if (putPixelImpl) putPixelImpl(*this, x, y, toNativeColor(color));
    }
    inline void blendPixel(int x, int y, const Color& color, uint8_t intensity)
    {
        if (blendPixelImpl) blendPixelImpl(*this, x, y, toNativeColor(color), intensity);
    }
    inline Color getPixel(int x, int y) const
    {
        if (!getPixelImpl) return Color();
        return fromNativeColor(getPixelImpl(*this, x, y));
    }

    inline void putPixelDirect(int x, int y, uint32_t native_color)
    {
        if (putPixelImpl) putPixelImpl(*this, x, y, native_color);
    }
    inline void blendPixelDirect(int x, int y, uint32_t native_color, uint8_t intensity)
    {
        if (blendPixelImpl) blendPixelImpl(*this, x, y, native_color, intensity);
    }
    inline uint32_t getPixelDirect(int x, int y) const
    {
        if (!getPixelImpl) return 0;
        return getPixelImpl(*this, x, y);
    }

    void clear(const Color& color = Color(0, 0, 0, 0));

    // void blendPixel(int x, int y, const Color& c, int brightness);

    void drawRect(int x1, int y1, int x2, int y2, const Color& color);
    inline void fillRect(int x1, int y1, int x2, int y2, const Color& color)
    {
        if (fillRectImpl) fillRectImpl(*this, x1, y1, x2, y2, toNativeColor(color));
    }
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

    void colorGradient(const Rect16& rect, const Color& c1, const Color& c2, Orientation orientation);
    void colorGradient(int x1, int y1, int x2, int y2, const Color& c1, const Color& c2, Orientation orientation);

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

} // namespace picopplib

#endif // PICOPPLIB_GRAFIX_DRAWABLE_H