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

int RGBFormat::bitdepth() const
{
    switch (format_id) {
    case Monochrome1Bit:
        return 1;
    case R5G6B5:
        return 16;
    case A8R8G8B8:
        return 32;
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