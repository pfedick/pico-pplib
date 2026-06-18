#ifndef PICOPPLIB_GRAFIX_IMAGE_H
#define PICOPPLIB_GRAFIX_IMAGE_H

#include <stdint.h>
#include <stddef.h>

#include "picopplib.h"
#include "picopplib/grafix/drawable.h"
namespace picopplib
{

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

} // namespace picopplib

#endif // PICOPPLIB_GRAFIX_IMAGE_H