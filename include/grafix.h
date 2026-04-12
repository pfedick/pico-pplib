
#ifndef PPL7GRAFIX_H_
#define PPL7GRAFIX_H_

#include <stdint.h>
#include <stddef.h>
#include "ppl7-light.h"
#include <list>
#include <map>

namespace ppl7light
{

// Font6 Exceptions
/*
PPL7EXCEPTION(InvalidFontFormatException, Exception);
PPL7EXCEPTION(InvalidFontFaceException, Exception);
PPL7EXCEPTION(UnknownFontFaceException, Exception);
*/

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
    Color(int red, int green, int blue);
    Color(uint32_t rgb);

    uint32_t color() const;
    uint32_t rgb() const;
    int red() const;
    int green() const;
    int blue() const;
    int brightness() const;
    Color grey() const;
    Color negativ() const;
    bool match(const Color& other, int tolerance = 0) const;

    void setRed(int red);
    void setGreen(int green);
    void setBlue(int blue);
    void setColor(int red, int green, int blue);
    void setColor(const Color& other);
    void setColor(uint32_t rgb);

    Color& blend(const Color& background, const Color& foreground, int intensity);
    Color& blendf(const Color& background, const Color& foreground, float intensity);

    Color& operator*=(float factor);
    Color& operator+=(const Color& other);
    operator uint32_t() const;
    operator int32_t() const;

    operator uint16_t() const;

    bool operator<(const Color& other) const;
    bool operator<=(const Color& other) const;
    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;
    bool operator>=(const Color& other) const;
    bool operator>(const Color& other) const;

    static Color getBlended(const Color& background, const Color& foreground, int intensity);
    static Color getBlendedf(const Color& background, const Color& foreground, float intensity);
};
const Color operator*(const Color& color, float factor);
const Color operator*(float factor, const Color& color);
const Color operator+(const Color& color1, const Color& color2);

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
    int cForeground;
    int cBorder;
    int cShadow;
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
    int color() const;
    int borderColor() const;
    int shadowColor() const;
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
    void setColor(int c);
    void setBorderColor(int c);
    void setShadowColor(int c);
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

class Drawable
{
private:
    uint8_t* buffer;
    // uint32_t pitch;
    size_t buffer_size;
    unsigned int my_width;
    unsigned int my_height;

public:
    Drawable();
    Drawable(unsigned int width, unsigned int height);
    ~Drawable();
    void create(unsigned int width, unsigned int height);
    void clear(int color);

    unsigned int width() const;
    unsigned int height() const;
    bool isEmpty() const;

    uint8_t* ptr() const;
    size_t size() const;

    void putPixel(int x, int y, int color);
    int getPixel(int x, int y) const;

    void blendPixel(int x, int y, int c, int brightness);

    void drawRect(int x1, int y1, int x2, int y2, int color);
    void fillRect(int x1, int y1, int x2, int y2, int color);
    void invertRect(int x1, int y1, int x2, int y2);
    void line(int x1, int y1, int x2, int y2, int color);

    void print(const Font& font, int x, int y, const String& text);
    void printf(const Font& font, int x, int y, const char* fmt, ...);
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
    virtual void render(const FontFile& file, const Font& font, Drawable& draw, int x, int y, const String& text,
                        int color) const = 0;
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
    void render(const FontFile& file, const Font& font, Drawable& draw, int x, int y, const String& text,
                int color) const override;
    Size measure(const FontFile& file, const Font& font, const String& text) const override;
    Rect boundary(const FontFile& file, const Font& font, const String& text, int x, int y) const override;
    String name() const override;
    String description() const override;
};

} // namespace ppl7light

#endif // PPL7GRAFIX_H_
