#include "picopplib-grafix.h"

namespace picopplib
{

RGBFormat::RGBFormat()
{
    format_id = unknown;
}

RGBFormat::RGBFormat(Identifier id)
{
    format_id = id;
}

void RGBFormat::setFormat(Identifier id)
{
    format_id = id;
}

RGBFormat::Identifier RGBFormat::format() const
{
    return format_id;
}

uint8_t RGBFormat::bitdepth() const
{
    switch (format_id) {
    case Monochrome1BitVertical:
        return 1;
    case GREY8:
        return 8;
    case R5G6B5:
        return 16;
    case A8R8G8B8:
        return 32;
    default:
        return 0;
    }
}

uint32_t RGBFormat::bytesForWidth(uint16_t width) const
{
    switch (format_id) {
    case Monochrome1BitVertical:
        return width; // 1 Bit pro Pixel, aber vertikal gepackt: 8 Pixel pro Byte, also width/8, aber da vertikal gepackt, ist es einfach
                      // width
    case GREY8:
        return width; // 1 Byte
    case R5G6B5:
        return width * 2; // 16 Bit pro Pixel = 2 Bytes
    case A8R8G8B8:
        return width * 4; // 32 Bit pro Pixel = 4 Bytes
    default:
        return 0;
    }
}

bool RGBFormat::operator==(const RGBFormat& other) const
{
    return format_id == other.format_id;
}

bool RGBFormat::operator==(Identifier id) const
{
    return format_id == id;
}

} // namespace picopplib