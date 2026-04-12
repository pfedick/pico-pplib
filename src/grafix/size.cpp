
#include "ppl7-light.h"
#include "grafix.h"

namespace ppl7light {


Size::Size()
{
    width=0;
    height=0;
}

Size::Size(int width, int height)
{
    this->width=width;
    this->height=height;
}

Size::Size(const Size& other)
{
    width=other.width;
    height=other.height;
}


bool Size::isNull() const
{
    if (height == 0 && width == 0) return true;
    return false;
}

bool Size::isEmpty() const
{
    if (height == 0 || width == 0) return true;
    return false;
}

bool Size::isValid() const
{
    if (width < 0) return false;
    if (height < 0) return false;
    return true;
}

void Size::setHeight(int height)
{
    this->height=height;
}

void Size::setWidth(int width)
{
    this->width=width;
}

void Size::setSize(int width, int height)
{
    this->width=width;
    this->height=height;
}

void Size::setSize(const Size& other)
{
    width=other.width;
    height=other.height;
}


Size& Size::operator*= (double factor)
{
    width=(int)(width * factor);
    height=(int)(height * factor);
    return *this;
}

Size& Size::operator+= (const Size& size)
{
    width+=size.width;
    height+=size.height;
    return *this;
}

Size& Size::operator-= (const Size& size)
{
    width-=size.width;
    height-=size.height;
    return *this;
}

Size& Size::operator/= (double divisor)
{
    width=(int)(width / divisor);
    height=(int)(height / divisor);
    return *this;
}

bool operator!= (const Size& s1, const Size& s2)
{
    if (s1.width != s2.width) return true;
    if (s1.height != s2.height) return true;
    return false;
}

bool operator== (const Size& s1, const Size& s2)
{
    if (s1.width != s2.width) return false;
    if (s1.height != s2.height) return false;
    return true;
}

const Size operator* (const Size& size, double factor)
{
    return Size((int)(size.width * factor), (int)(size.height * factor));
}

const Size operator* (double factor, const Size& size)
{
    return Size((int)(size.width * factor), (int)(size.height * factor));
}

const Size operator+ (const Size& s1, const Size& s2)
{
    return Size(s1.width + s2.width, s1.height + s2.height);
}

const Size operator- (const Size& s1, const Size& s2)
{
    return Size(s1.width - s2.width, s1.height - s2.height);
}

const Size operator- (const Size& size)
{
    return Size(0 - size.width, 0 - size.height);
}

const Size operator/ (const Size& size, double divisor)
{
    return Size((int)(size.width / divisor), (int)(size.height * divisor));
}





} // EOF namespace
