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

namespace picopplib
{

Font::Font()
{
    Name = "Default";
    fontSize = 12;
    flags = 0;
    ori = Orientation::TOP;
    rotationDegrees = 0.0;
    cForeground.setColor(255, 255, 255, 255);
}

Font::Font(const Font& other)
{
    Name = other.Name;
    cForeground = other.cForeground;
    cBorder = other.cBorder;
    cShadow = other.cShadow;
    fontSize = other.fontSize;
    flags = other.flags;
    ori = other.ori;
    rotationDegrees = other.rotationDegrees;
}

Font::Font(Font&& other) noexcept
{
    Name = std::move(other.Name);
    cForeground = other.cForeground;
    cBorder = other.cBorder;
    cShadow = other.cShadow;
    fontSize = other.fontSize;
    flags = other.flags;
    ori = other.ori;
    rotationDegrees = other.rotationDegrees;
}

Font& Font::operator=(const Font& other)
{
    Name = other.Name;
    cForeground = other.cForeground;
    cBorder = other.cBorder;
    cShadow = other.cShadow;
    fontSize = other.fontSize;
    flags = other.flags;
    ori = other.ori;
    rotationDegrees = other.rotationDegrees;
    return *this;
}

Font& Font::operator=(Font&& other) noexcept
{
    Name = std::move(other.Name);
    cForeground = other.cForeground;
    cBorder = other.cBorder;
    cShadow = other.cShadow;
    fontSize = other.fontSize;
    flags = other.flags;
    ori = other.ori;
    rotationDegrees = other.rotationDegrees;
    return *this;
}

const String& Font::name() const
{
    return Name;
}

const Color& Font::color() const
{
    return cForeground;
}

const Color& Font::borderColor() const
{
    return cBorder;
}

const Color& Font::shadowColor() const
{
    return cShadow;
}

bool Font::bold() const
{
    if (flags & fBold) return true;
    return false;
}

bool Font::italic() const
{
    if (flags & fItalic) return true;
    return false;
}

bool Font::antialias() const
{
    if (flags & fAntialias) return true;
    return false;
}

bool Font::drawBorder() const
{
    if (flags & fDrawBorder) return true;
    return false;
}

bool Font::drawShadow() const
{
    if (flags & fDrawShadow) return true;
    return false;
}

bool Font::drawUnderline() const
{
    if (flags & fUnderline) return true;
    return false;
}

bool Font::monospace() const
{
    if (flags & fMonospace) return true;
    return false;
}

uint16_t Font::size() const
{
    return fontSize;
}

Font::Orientation Font::orientation() const
{
    return (Orientation)ori;
}

uint16_t Font::rotation() const
{
    return rotationDegrees;
}

int Font::setName(const String& name)
{
    // TODO: Prüfen, ob der Font existiert
    Name = name;
    return 1;
}

void Font::setColor(const Color& c)
{
    cForeground = c;
}

void Font::setBorderColor(const Color& c)
{
    cBorder = c;
}

void Font::setShadowColor(const Color& c)
{
    cShadow = c;
}

void Font::setBold(bool enable)
{
    if (!enable)
        flags &= ~fBold;
    else
        flags |= fBold;
}

void Font::setItalic(bool enable)
{
    if (!enable)
        flags &= ~fItalic;
    else
        flags |= fItalic;
}

void Font::setAntialias(bool enable)
{
    if (!enable)
        flags &= ~fAntialias;
    else
        flags |= fAntialias;
}

void Font::setDrawBorder(bool enable)
{
    if (!enable)
        flags &= ~fDrawBorder;
    else
        flags |= fDrawBorder;
}

void Font::setDrawShadow(bool enable)
{
    if (!enable)
        flags &= ~fDrawShadow;
    else
        flags |= fDrawShadow;
}

void Font::setDrawUnderline(bool enable)
{
    if (!enable)
        flags &= ~fUnderline;
    else
        flags |= fUnderline;
}

void Font::setMonospace(bool enable)
{
    if (!enable)
        flags &= ~fMonospace;
    else
        flags |= fMonospace;
}

void Font::setSize(uint16_t size)
{
    fontSize = size;
}

void Font::setOrientation(Orientation o)
{
    ori = o;
}

void Font::setRotation(uint16_t degrees)
{
    rotationDegrees = degrees;
}

Size16 Font::measure(const String& text) const
{
    Size16 s;
    Grafix* gfx = GetGrafix();
    const FontFile* file = gfx->findFont(Name);
    if (!file) return s;
    if (!file->engine) return s;
    return file->engine->measure(*file, *this, text);
}

Rect16 Font::boundary(const String& text, int x, int y) const
{
    Rect16 r;
    Grafix* gfx = GetGrafix();
    const FontFile* file = gfx->findFont(Name);
    if (!file) return r;
    if (!file->engine) return r;
    return file->engine->boundary(*file, *this, text, x, y);
}

bool operator!=(const Font& f1, const Font& f2)
{
    return !(f1 == f2);
}

bool operator==(const Font& f1, const Font& f2)
{
    if (f1.Name != f2.Name) return false;
    if (f1.fontSize != f2.fontSize) return false;
    if (f1.flags != f2.flags) return false;
    if (f1.cForeground != f2.cForeground) return false;
    if (f1.cBorder != f2.cBorder) return false;
    if (f1.cShadow != f2.cShadow) return false;
    if (f1.ori != f2.ori) return false;
    if (f1.rotationDegrees != f2.rotationDegrees) return false;
    return true;
}

/*!\class FontFile
 * \ingroup PPLGroupGrafik
 * \brief Interne Klasse zur Verwaltung aller geladener Font-Dateien
 */
FontFile::FontFile()
{
    engine = NULL;
    priv = NULL;
}

FontFile::~FontFile()
{
    if (engine) engine->deleteFont(this);
    engine = NULL;
}

/*!\brief FontEngine hinzufügen
 *
 * Mit dieser Funktion wird eine FontEngine der Grafikengine hinzugefügt. Darunter
 * versteht man eine von FontEngine abgeleitete Klasse, die in der Lage ist Fontdateien
 * in einem bestimmten Format zu verarbeiten und darzustellen.
 *
 * Die FontEngine muss von der Anwendung mit "new" erstellt werden und als Pointer
 * an die Grafix-Engine übergeben werden. Grafix verwaltet die Engine ab diesem
 * Moment und kümmert sich auch um deren Löschung bei Programmende. Die Anwendung darf
 * die FontEngine nicht selbst löschen!
 *
 * \param engine Pointer auf die Klasse mit der FontEngine
 * \returns Liefert true (1) zurück, wenn die Engine erfolgreich aufgenommen werden
 * konnte, sonst false (0). Ein entsprechender Fehlercode wird gesetzt.
 */
void Grafix::addFontEngine(FontEngine* engine)
{
    if (!engine) throw Exception("NullPointerException");
    engine->init();
    FontEngineList.push_back(engine);
}

void Grafix::loadFont(const ByteArrayPtr& memory, const String& fontname)
{
    std::list<FontEngine*>::iterator it;
    // printf("check font engine, we have %d engines\n", (int)FontEngineList.size());
    for (it = FontEngineList.begin(); it != FontEngineList.end(); ++it) {
        // printf("checking engine...\n");
        FontEngine* engine = *it;
        // printf("Engine: %s\n", (const char*)engine->description());
        if (engine->ident(memory)) {
            // printf("Engine gefunden\n");
            FontFile* font = engine->loadFont(memory, fontname);
            if (!font) throw Exception("InvalidFontException");
            std::map<String, FontFile*>::const_iterator fit;
            fit = FontList.find(font->Name);
            if (fit != FontList.end()) {
                FontFile* old = fit->second;
                FontList.erase(fit);
                old->engine->deleteFont(old);
            }
            // printf("Font geladen\n");
            FontList.insert(std::pair<String, FontFile*>(font->Name, font));
            return;
        }
    }
    throw Exception("NoSuitableFontEngineException", "%d engines loaded", (int)FontEngineList.size());
}

void Grafix::clearFonts()
{
    std::map<String, FontFile*>::iterator fit;
    for (fit = FontList.begin(); fit != FontList.end(); ++fit) {
        fit->second->engine->deleteFont(fit->second);
    }
    FontList.clear();
    FontEngineList.clear();
}

void Grafix::clearFontEngines()
{
    std::list<FontEngine*>::iterator it;
    for (it = FontEngineList.begin(); it != FontEngineList.end(); ++it) {
        delete *it;
    }
    FontEngineList.clear();
}

const FontFile* Grafix::findFont(const String& fontname) const
{
    std::map<String, FontFile*>::const_iterator fit = FontList.find(fontname);
    if (fit != FontList.end()) return fit->second;
    return NULL;
}

const FontFile* Grafix::findFont(const Font& font) const
{
    return findFont(font.name());
}

/*!\brief Text ausgeben
 *
 * \desc
 * Mit dieser Funktion wird der Text \p text auf der Grafik an den
 * Koordinaten \p x und \p y unter Verwendeung des Fonts \p font ausgegeben.
 *
 * @param font Zu verwendende Font-Parameter
 * @param x X-Koordinate
 * @param y Y-Koordinate
 * @param text Der auszugebende Text
 */
void Drawable::print(const Font& font, int x, int y, const String& text)
{
    Grafix* gfx = GetGrafix();
    const FontFile* file = gfx->findFont(font.name());
    if (!file) return;
    if (font.drawShadow()) {
        file->engine->render(*file, font, *this, x + 2, y + 2, text, toNativeColor(font.shadowColor()));
    }
    if (font.drawBorder()) {
        for (int a = -1; a < 2; a++) {
            for (int b = -1; b < 2; b++) {
                if (a == 0 && b == 0) continue;
                file->engine->render(*file, font, *this, x + a, y + b, text, toNativeColor(font.borderColor()));
            }
        }
    }
    file->engine->render(*file, font, *this, x, y, text, toNativeColor(font.color()));
}

/*!\brief Formatierten Text ausgeben
 *
 * \desc
 * Mit dieser Funktion wird zunächst ein Text anhand des Formatstrings
 * \p fmt erstellt und dann auf der Grafik an den
 * Koordinaten \p x und \p y unter Verwendeung des Fonts \p font ausgegeben.
 *
 * @param font Zu verwendende Font-Parameter
 * @param x X-Koordinate
 * @param y Y-Koordinate
 * @param fmt Formatstring
 * @param ... optionale Parameter für den Formatstring
 */
void Drawable::printf(const Font& font, int x, int y, const char* fmt, ...)
{
    String s;
    va_list args;
    va_start(args, fmt);
    s.vasprintf(fmt, args);
    va_end(args);
    print(font, x, y, s);
}

} // namespace picopplib
