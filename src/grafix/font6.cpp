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

#include "picopplib-grafix.h"
#include <stdio.h>
#include <map>

namespace picopplib
{

class Font6Glyph
{
private:
public:
    Font6Glyph();
    short width;
    short height;
    short bearingX;
    short bearingY;
    short advance;
    const char* bitmap;
    // std::map<wchar_t, int16_t> Hints;
    // int getHint(wchar_t nextGlyph) const;
};

class Font6Face
{
private:
public:
    unsigned char Flags;
    short Pixelformat;
    short Size;
    short MaxBearingY;
    short MaxHeight;
    short Underscore;
    std::map<wchar_t, Font6Glyph> Glyphs;

    const Font6Glyph* getGlyph(wchar_t code) const;
};

class Font6Renderer
{
private:
    std::map<uint32_t, Font6Face> Faces;
    PFPFile pfp;

    void loadFace(const char* data, size_t size);
    void loadGlyph(Font6Face& Face, const char* data, size_t size);

    const Font6Face* getFaceInternal(int size, int flags);

    const Font6Face* getFace(int size, int flags);

    void renderInternal(const Font6Face& face, Drawable& draw, const Font& font, int x, int y, const WideString& text, int color);

public:
    Font6Renderer();
    ~Font6Renderer();

    void loadFont(const ByteArrayPtr& memory);

    const String& name() const;
    const String& author() const;
    const String& copyright() const;
    const String& description() const;
    size_t numFaces() const;

    void render(Drawable& draw, const Font& font, int x, int y, const String& text, int color);
    Size16 measure(const Font& font, const String& text);
    Rect16 boundary(const Font& font, const String& text, int x, int y);
};

Font6Glyph::Font6Glyph()
{
    width = 0;
    height = 0;
    bearingX = 0;
    bearingY = 0;
    advance = 0;
    bitmap = NULL;
}

static void DrawGlyphMono8_0(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            v = bitmap[0];
            if (v) data.putPixelDirect(xx + x, yy + y, c);
            bitmap++;
        }
    }
}

static void DrawGlyphMono8_90(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            v = bitmap[0];
            if (v) data.putPixelDirect(x - yy, y + xx, c);
            bitmap++;
        }
    }
}

static void DrawGlyphMono8_180(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            v = bitmap[0];
            if (v) data.putPixelDirect(x - xx, y - yy, c);
            bitmap++;
        }
    }
}

static void DrawGlyphMono8_270(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            v = bitmap[0];
            if (v) data.putPixelDirect(x + yy, y - xx, c);
            bitmap++;
        }
    }
}

static void DrawGlyphMono1_0(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if (v & 128) {
                data.putPixelDirect(xx + x, yy + y, c);
            }
            v = v << 1;
            bitcount--;
        }
    }
}

static void DrawGlyphMono1_90(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if (v & 128) {
                data.putPixelDirect(x - yy, y + xx, c);
            }
            v = v << 1;
            bitcount--;
        }
    }
}

static void DrawGlyphMono1_180(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if (v & 128) {
                data.putPixelDirect(x - xx, y - yy, c);
            }
            v = v << 1;
            bitcount--;
        }
    }
}

static void DrawGlyphMono1_270(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if (v & 128) {
                data.putPixelDirect(x + yy, y - xx, c);
            }
            v = v << 1;
            bitcount--;
        }
    }
}

static void DrawGlyphAA2_0(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0, v2 = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if ((v2 = v & 192)) {
                if (v2 == 192)
                    data.blendPixelDirect(x + xx, y + yy, c, 255);
                else if (v2 == 128)
                    data.blendPixelDirect(x + xx, y + yy, c, 192); // 200
                else
                    data.blendPixelDirect(x + xx, y + yy, c, 100); // 100
            }
            v = v << 2;
            bitcount -= 2;
        }
    }
}

static void DrawGlyphAA2_90(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0, v2 = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if ((v2 = v & 192)) {
                if (v2 == 192)
                    data.blendPixelDirect(x - yy, y + xx, c, 255);
                else if (v2 == 128)
                    data.blendPixelDirect(x - yy, y + xx, c, 192); // 192
                else
                    data.blendPixelDirect(x - yy, y + xx, c, 100); // 100
            }
            v = v << 2;
            bitcount -= 2;
        }
    }
}

static void DrawGlyphAA2_180(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0, v2 = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if ((v2 = v & 192)) {
                if (v2 == 192)
                    data.blendPixelDirect(x - xx, y - yy, c, 255);
                else if (v2 == 128)
                    data.blendPixelDirect(x - xx, y - yy, c, 192); // 192
                else
                    data.blendPixelDirect(x - xx, y - yy, c, 100); // 100
            }
            v = v << 2;
            bitcount -= 2;
        }
    }
}

static void DrawGlyphAA2_270(Drawable& data, const Font6Glyph& glyph, int x, int y, int c)
{
    const char* bitmap = glyph.bitmap;
    int v = 0, v2 = 0;
    uint8_t bitcount = 0;
    for (int yy = 0; yy < glyph.height; yy++) {
        for (int xx = 0; xx < glyph.width; xx++) {
            if (!bitcount) {
                v = bitmap[0];
                bitcount = 8;
                bitmap++;
            }
            if ((v2 = v & 192)) {
                if (v2 == 192)
                    data.blendPixelDirect(x + yy, y - xx, c, 255);
                else if (v2 == 128)
                    data.blendPixelDirect(x + yy, y - xx, c, 192); // 192
                else
                    data.blendPixelDirect(x + yy, y - xx, c, 100); // 100
            }
            v = v << 2;
            bitcount -= 2;
        }
    }
}

Font6Renderer::Font6Renderer() {}

Font6Renderer::~Font6Renderer() {}

size_t Font6Renderer::numFaces() const
{
    return Faces.size();
}

void Font6Renderer::loadFont(const ByteArrayPtr& memory)
{
    if (!pfp.ident(memory)) throw Exception("InvalidFontFormatException");
    if (pfp.getID() != "FONT") throw Exception("InvalidFontFormatException");
    if (pfp.getMainVersion() != 6 || pfp.getSubVersion() != 0) throw Exception("InvalidFontFormatException");
    pfp.load(memory);
    // printf("pfpfile loaded loaded\n");

    // Die Faces laden
    PFPFile::const_iterator it;
    for (it = pfp.begin(); it != pfp.end(); ++it) {
        const PFPChunk& c = *it;
        // printf("Loading Chunk: %s, size: %u\n", (const char*)c->name(), c->size());
        if (c.name() == "FACE") loadFace((const char*)c.data(), c.size());
    }
}

void Font6Renderer::loadFace(const char* data, size_t size)
{
    if (size < 12) throw Exception("InvalidFontFaceException");
    Font6Face Face;
    Face.Flags = Peek8(data + 0);
    Face.Pixelformat = Peek8(data + 1);
    Face.Size = Peek16(data + 2);
    Face.MaxBearingY = Peek16(data + 4);
    Face.MaxHeight = Peek16(data + 6);
    Face.Underscore = Peek16(data + 8);
    size_t numGlyphs = Peek16(data + 10);
    uint32_t id;
    id = (Face.Flags & 7) << 16;
    id |= Face.Size;
    size_t p = 12;
    // printf("Lade Face mit id %u, Size: %d, Flags: %d, Bytes: %u\n", id, Face.Size, Face.Flags, (unsigned int)size);

    // Wir fügen zuerst das Face in die Map ein
    Faces.insert(std::pair<uint32_t, Font6Face>(id, Face));

    // Damit wir im nächsten Schritt die Glyphen direkt innerhalb des Eintrags in der Map
    // ergänzen können, was uns eine ganze Menge Kopiererei erspart
    Font6Face& mapFace = Faces[id];
    // printf("Num Glyphs: %zi\n", numGlyphs);
    for (size_t i = 0; i < numGlyphs; i++) {
        size_t s = Peek32(data + p);
        loadGlyph(mapFace, data + p, s);
        p += s;
    }
    // printf ("%zi Bytes gelesen\n",p);
}

void Font6Renderer::loadGlyph(Font6Face& Face, const char* data, size_t size)
{
    Font6Glyph NewGlyph;
    wchar_t unicode = Peek16(data + 4);
    // Um spätere Kopiererei bei einfügen des Glyphs in die Map zu vermeiden,
    // fügen wir den Glyph zuerst in die Map ein
    Face.Glyphs.insert(std::pair<wchar_t, Font6Glyph>(unicode, NewGlyph));
    // Und arbeiten dann auf dem Objekt innerhalb der Map
    Font6Glyph& Glyph = Face.Glyphs[unicode];
    Glyph.width = Peek16(data + 6);
    Glyph.height = Peek16(data + 8);
    Glyph.bearingX = (short)Peek16(data + 10);
    Glyph.bearingY = (short)Peek16(data + 12);
    Glyph.advance = Peek16(data + 14);
    size_t p = 16;
    if (Face.Flags & 8) { // Wir haben Hints
        wchar_t c;
        while ((c = Peek16(data + p))) {
            // Glyph.Hints.insert(std::pair<wchar_t, int16_t>(c, (int16_t)Peek16(data + p + 2)));
            p += 4;
        }
        p += 4;
    }
    Glyph.bitmap = data + p;
    /*
    printf("Reading Glyph: Size: %zi, Unicode: %i = %lc, width: %i, height: %i, advance: %i, Hints: %zi\n",
        size, unicode, unicode, Glyph.width, Glyph.height, Glyph.advance, Glyph.Hints.size());
     */
}

const Font6Face* Font6Renderer::getFaceInternal(int size, int flags)
{
    uint32_t id;

    id = (flags & 7) << 16;
    id |= (size & 0xffff);
    // printf("Suche id %i aus %zi Faces\n",id,Faces.size());
    std::map<uint32_t, Font6Face>::const_iterator it;
    it = Faces.find(id);
    if (it == Faces.end()) return NULL;
    return &it->second;
}

const Font6Face* Font6Renderer::getFace(int size, int flags)
{
    const Font6Face* face = getFaceInternal(size, flags);
    if (!face && (flags & 1)) {
        // Fallback für nicht-antialiaste Schrift, wenn kein Face für Antialias enthalten ist
        flags -= 1;
        face = getFaceInternal(size, flags);
    } else if (!face && !(flags & 1)) {
        // Fallback für antialiaste Schrift, wenn kein Face für Nicht-Antialias enthalten ist
        flags += 1;
        face = getFaceInternal(size, flags);
    }
    return face;
}

const Font6Glyph* Font6Face::getGlyph(wchar_t code) const
{
    std::map<wchar_t, Font6Glyph>::const_iterator it;
    it = Glyphs.find(code);
    if (it == Glyphs.end()) return NULL;
    return &it->second;
}

/*
int Font6Glyph::getHint(wchar_t nextGlyph) const
{
    if (Hints.empty()) return 0;
    std::map<wchar_t, int16_t>::const_iterator it;
    it = Hints.find(nextGlyph);
    if (it == Hints.end()) return 0;
    return it->second;
}
*/

void Font6Renderer::render(Drawable& draw, const Font& font, int x, int y, const String& text, int color)
{
    int flags = 0;
    if (font.antialias()) flags |= 1;
    if (font.bold()) flags |= 2;
    if (font.italic()) flags |= 4;
    const Font6Face* face = getFace(font.size(), flags);

    WideString wtext(text);
    if (face) {
        renderInternal(*face, draw, font, x, y, wtext, color);
        return;
    }
    // Fallback für fette Schrift, wenn kein Face für Bold enthalten ist
    if (flags & 2) {
        flags -= 2;
        face = getFace(font.size(), flags);
        if (face) {
            renderInternal(*face, draw, font, x, y, wtext, color);
            uint16_t rotate = (uint16_t)font.rotation();
            switch (rotate) {
            case 0:
                renderInternal(*face, draw, font, x + 1, y, wtext, color);
                break;
            case 90:
                renderInternal(*face, draw, font, x, y + 1, wtext, color);
                break;
            case 180:
                renderInternal(*face, draw, font, x - 1, y, wtext, color);
                break;
            case 270:
                renderInternal(*face, draw, font, x, y - 1, wtext, color);
                break;
            }
            return;
        }
    }
    // throw Exception("UnknownFontFaceException");
}

typedef void (*BltGlyphFunction)(Drawable& data, const Font6Glyph& glyph, int x, int y, int c);

static BltGlyphFunction getBlitter(const Font6Face& face, const Drawable& draw, uint16_t rotate)
{
    switch (face.Pixelformat) {
    case 1: // Monochrom, 8 Bit pro Pixel
        switch (rotate) {
        case 0:
            return &DrawGlyphMono8_0;
        case 90:
            return &DrawGlyphMono8_90;
        case 180:
            return &DrawGlyphMono8_180;
        case 270:
            return &DrawGlyphMono8_270;
        }
        break;
    case 2: // Monochrom, 1 Bit pro Pixel
        switch (rotate) {
        case 0:
            return &DrawGlyphMono1_0;
        case 90:
            return &DrawGlyphMono1_90;
        case 180:
            return &DrawGlyphMono1_180;
        case 270:
            return &DrawGlyphMono1_270;
        }
        break;
    case 4: // Antialiased, 2 Bit pro Pixel
        switch (rotate) {
        case 0:
            return &DrawGlyphAA2_0;
        case 90:
            return &DrawGlyphAA2_90;
        case 180:
            return &DrawGlyphAA2_180;
        case 270:
            return &DrawGlyphAA2_270;
        }
        break;
    default:
        return NULL;
        // throw Exception("InvalidFontException");
    };
    return NULL;
    // throw Exception("InvalidFontException");
}

void Font6Renderer::renderInternal(const Font6Face& face, Drawable& draw, const Font& font, int x, int y, const WideString& text, int color)
{
    const Font6Glyph *glyph = NULL, *previous = NULL;
    void (*BltGlyph)(Drawable& draw, const Font6Glyph& glyph, int x, int y, int c) = NULL;
    int startx = x;
    int starty = y;
    int lastx = x;
    int lasty = y;
    int kerningx = 0;
    uint16_t rotate = (uint16_t)font.rotation();

    size_t textlen = text.size();
    size_t p = 0;
    BltGlyph = getBlitter(face, draw, rotate);
    if (!BltGlyph) return;
    if (font.orientation() == Font::TOP) {
        lasty += face.MaxBearingY;
    }
    while (p < textlen) {
        wchar_t code = text[p++];
        if (code == 10) { // Newline
            switch (rotate) {
            case 0:
                lastx = startx;
                lasty += face.MaxHeight;
                break;
            case 90:
                lasty = starty;
                lastx -= face.MaxHeight;
                break;
            case 180:
                lastx = startx;
                lasty -= face.MaxHeight;
                break;
            case 270:
                lasty = starty;
                lastx += face.MaxHeight;
                break;
            }
            glyph = NULL;
        } else if (code == '\t') { // Tab
            glyph = face.getGlyph(32);
            if (glyph) {
                switch (rotate) {
                case 0:
                    lastx += 4 * glyph->advance;
                    break;
                case 90:
                    lasty += 4 * glyph->advance;
                    break;
                case 180:
                    lastx -= 4 * glyph->advance;
                    break;
                case 270:
                    lasty -= 4 * glyph->advance;
                    break;
                }
            }
        } else {
            glyph = face.getGlyph(code);
            if (glyph == NULL || BltGlyph == NULL) {
                x = lastx;
                y = lasty;
                draw.drawRect(x, y - face.MaxBearingY, x + 4, y, font.color());
                lastx += 5;
            } else {
                kerningx = 0;
                // if (previous) kerningx += previous->getHint(code);
                switch (rotate) {
                case 0:
                    x = lastx + glyph->bearingX + kerningx;
                    y = lasty - glyph->bearingY;
                    BltGlyph(draw, *glyph, x, y, color);
                    lastx += glyph->advance + kerningx;
                    break;
                case 90:
                    x = lastx + glyph->bearingY;
                    y = lasty + glyph->bearingX + kerningx;
                    BltGlyph(draw, *glyph, x, y, color);
                    lasty += glyph->advance + kerningx;
                    break;
                case 180:
                    x = lastx - glyph->bearingX - kerningx;
                    y = lasty + glyph->bearingY;
                    BltGlyph(draw, *glyph, x, y, color);
                    lastx -= glyph->advance + kerningx;
                    break;
                case 270:
                    x = lastx - glyph->bearingY;
                    y = lasty - glyph->bearingX - kerningx;
                    BltGlyph(draw, *glyph, x, y, color);
                    lasty -= glyph->advance + kerningx;
                    break;
                }
            }
        }
        previous = glyph;
    }
}

Size16 Font6Renderer::measure(const Font& font, const String& text)
{
    Size16 s;
    const Font6Glyph *glyph = NULL, *previous = NULL;
    int lastx = 0;
    int lasty = 0;
    int kerningx = 0;
    uint16_t rotate = (uint16_t)font.rotation();
    WideString wtext(text);
    size_t textlen = wtext.size();
    size_t p = 0;
    int flags = 0;
    if (font.antialias()) flags |= 1;
    if (font.bold()) flags |= 2;
    if (font.italic()) flags |= 4;
    const Font6Face* face = getFace(font.size(), flags);
    if (!face) face = getFace(font.size(), 0);
    if (!face) return s;
    lasty = face->MaxHeight;
    while (p < textlen) {
        wchar_t code = wtext[p++];
        if (code == 10) { // Newline
            lastx = 0;
            lasty += face->MaxHeight;
            glyph = NULL;
        } else if (code == '\t') { // Tab
            glyph = face->getGlyph(32);
            if (glyph) {
                lastx += 4 * glyph->advance;
            }
        } else {
            glyph = face->getGlyph(code);
            if (glyph == NULL) {
                lastx += 5;
            } else {
                kerningx = 0;
                // if (previous) kerningx += previous->getHint(code);
                lastx += glyph->advance + kerningx;
            }
        }
        previous = glyph;
        if (lastx > s.width) s.width = lastx;
        if (lasty > s.height) s.height = lasty;
    }
    if (rotate == 90 || rotate == 270) {
        s.setSize(s.height, s.width);
    }
    return s;
}

Rect16 Font6Renderer::boundary(const Font& font, const String& text, int x, int y)
{
    Rect16 r;
    int flags = 0;
    if (font.antialias()) flags |= 1;
    if (font.bold()) flags |= 2;
    if (font.italic()) flags |= 4;
    const Font6Face* face = getFace(font.size(), flags);
    if (!face) return r;

    Size16 s = measure(font, text);
    uint16_t rotate = (uint16_t)font.rotation();
    switch (rotate) {
    case 0:
        r.setRect(x, y, s.width, s.height);
        break;
    case 90:
        r.setRect(x - s.width, y, s.width, s.height);
        break;
    case 180:
        r.setRect(x - s.width, y - s.height, s.width, s.height);
        break;
    case 270:
        r.setRect(x, y - s.height, s.width, s.height);
        break;
    }
    return r;
}

// ##################################################################################################################################################################
// ##################################################################################################################################################################
// ##################################################################################################################################################################

/*!\class FontEngineFont6
 * \ingroup PPLGroupGrafik
 * \brief Font-Engine für PFP Version 6 Fonts
 */

FontEngineFont6::FontEngineFont6() {}

FontEngineFont6::~FontEngineFont6() {}

String FontEngineFont6::name() const
{
    return "FontEngineFont6";
}

String FontEngineFont6::description() const
{
    return "Rendering of PPLib Version 6 Fonts";
}

void FontEngineFont6::init()
{
    // Es gibt nichts zu tun
}

bool FontEngineFont6::ident(const ByteArrayPtr& memory) const throw()
{
    PFPFile ff;
    // HexDump(memory.ptr(), 100);
    if (!ff.ident(memory)) return 0;
    if (ff.getID() != "FONT") return 0;
    if (ff.getMainVersion() == 6 && ff.getSubVersion() == 0) {
        return 1;
    }
    return 0;
}

FontFile* FontEngineFont6::loadFont(const ByteArrayPtr& memory, const String& fontname)
{
    Font6Renderer* render = new Font6Renderer;
    if (!render) throw Exception("OutOfMemoryException");
    try {
        render->loadFont(memory);
    }
    catch (...) {
        delete render;
        throw;
    }
    FontFile* ff = new FontFile;
    if (!ff) {
        delete render;
        throw Exception("OutOfMemoryException");
    }
    ff->Name = fontname;
    ff->engine = this;
    ff->priv = render;
    return ff;
}

void FontEngineFont6::deleteFont(FontFile* file)
{
    if (!file) throw Exception("NullPointerException");
    if (file->engine != this) throw Exception("InvalidFontEngineException");
    Font6Renderer* render = static_cast<Font6Renderer*>(file->priv);
    delete render;
    file->priv = NULL;
    file->engine = NULL;
}

void FontEngineFont6::render(
    const FontFile& file, const Font& font, Drawable& draw, int x, int y, const String& text, int native_color) const
{
    Font6Renderer* render = static_cast<Font6Renderer*>(file.priv);
    render->render(draw, font, x, y, text, native_color);
}

Size16 FontEngineFont6::measure(const FontFile& file, const Font& font, const String& text) const
{
    Font6Renderer* render = static_cast<Font6Renderer*>(file.priv);
    return render->measure(font, text);
}

Rect16 FontEngineFont6::boundary(const FontFile& file, const Font& font, const String& text, int x, int y) const
{
    Font6Renderer* render = static_cast<Font6Renderer*>(file.priv);
    return render->boundary(font, text, x, y);
}

} // namespace picopplib
