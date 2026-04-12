#include "grafix.h"
#include <stdlib.h>
#include <string.h>

namespace ppl7light
{

Drawable::Drawable()
{
    buffer = NULL;
    buffer_size = 0;
    my_width = my_height = 0;
    // pitch=0;
}

Drawable::Drawable(unsigned int width, unsigned int height)
{
    buffer = NULL;
    buffer_size = 0;
    this->my_width = this->my_height = 0;
    create(width, height);
}

Drawable::~Drawable()
{
    free(buffer);
}

void Drawable::create(unsigned int width, unsigned int height)
{
    free(buffer);
    buffer_size = width * height / 8;
    buffer = (uint8_t*)malloc(buffer_size);
    if (!buffer) {
        buffer_size = 0;
        return;
    }
    memset(buffer, 0, buffer_size);
    this->my_width = width;
    this->my_height = height;
}

unsigned int Drawable::width() const
{
    return my_width;
}

unsigned int Drawable::height() const
{
    return my_height;
}

bool Drawable::isEmpty() const
{
    if (!buffer) return true;
    return false;
}

void Drawable::clear(int color)
{
    if (color)
        memset(buffer, 255, buffer_size);
    else
        memset(buffer, 0, buffer_size);
}

uint8_t* Drawable::ptr() const
{
    return buffer;
}

size_t Drawable::size() const
{
    return buffer_size;
}

void Drawable::putPixel(int x, int y, int color)
{
    if (!buffer) return;
    if (x < 0 || x >= my_width || y < 0 || y >= my_height) return;
    int adr = x + (y / 8) * my_width;
    if (adr >= buffer_size) return;
    if (color)
        buffer[adr] |= (1 << (y & 7));
    else
        buffer[adr] &= -(1 << (y & 7));
}

int Drawable::getPixel(int x, int y) const
{
    if (!buffer) return 0;
    if (x < 0 || x >= my_width || y < 0 || y >= my_height) return 0;
    int adr = x + (y / 8) * my_width;
    if (adr >= buffer_size) return 0;
    if (buffer[adr] & (1 << (y & 7))) return 1;
    return 0;
}

void Drawable::drawRect(int x1, int y1, int x2, int y2, int color)
{
    for (int x = x1; x <= x2; x++) {
        putPixel(x, y1, color);
        putPixel(x, y2, color);
    }
    for (int y = y1 + 1; y <= y2; y++) {
        putPixel(x1, y, color);
        putPixel(x2, y, color);
    }
}

void Drawable::fillRect(int x1, int y1, int x2, int y2, int color)
{
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            putPixel(x, y, color);
        }
    }
}

void Drawable::invertRect(int x1, int y1, int x2, int y2)
{
    int c;
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            int c = getPixel(x, y);
            if (c)
                putPixel(x, y, 0);
            else
                putPixel(x, y, 1);
        }
    }
}

/* signum function */
static int sgn(int x)
{
    return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

void Drawable::line(int x1, int y1, int x2, int y2, int color)
{
    int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

    /* Entfernung in beiden Dimensionen berechnen */
    dx = x2 - x1;
    dy = y2 - y1;

    /* Vorzeichen des Inkrements bestimmen */
    incx = sgn(dx);
    incy = sgn(dy);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    /* feststellen, welche Entfernung größer ist */
    if (dx > dy) {
        /* x ist schnelle Richtung */
        pdx = incx;
        pdy = 0; /* pd. ist Parallelschritt */
        ddx = incx;
        ddy = incy; /* dd. ist Diagonalschritt */
        es = dy;
        el = dx; /* Fehlerschritte schnell, langsam */
    } else {
        /* y ist schnelle Richtung */
        pdx = 0;
        pdy = incy; /* pd. ist Parallelschritt */
        ddx = incx;
        ddy = incy; /* dd. ist Diagonalschritt */
        es = dx;
        el = dy; /* Fehlerschritte schnell, langsam */
    }

    /* Initialisierungen vor Schleifenbeginn */
    x = x1;
    y = y1;
    err = el / 2;
    putPixel(x, y, color);

    /* Pixel berechnen */
    for (t = 0; t < el; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
    {
        /* Aktualisierung Fehlerterm */
        err -= es;
        if (err < 0) {
            /* Fehlerterm wieder positiv (>=0) machen */
            err += el;
            /* Schritt in langsame Richtung, Diagonalschritt */
            x += ddx;
            y += ddy;
        } else {
            /* Schritt in schnelle Richtung, Parallelschritt */
            x += pdx;
            y += pdy;
        }
        putPixel(x, y, color);
    }
}

void Drawable::blendPixel(int x, int y, int c, int brightness)
{
}

} // namespace ppl7light
