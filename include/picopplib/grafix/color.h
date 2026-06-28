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

#ifndef PICOPPLIB_GRAFIX_COLOR_H_
#define PICOPPLIB_GRAFIX_COLOR_H_

#include <stdint.h>
#include <stddef.h>

namespace picopplib
{

/**@class Color
 * @ingroup PPLGroupGrafik
 * @brief Ein Datenobjekt zum Speichern eines Farbwertes
 *
 * Diese Klasse wird als Objekt zum Speichern einer Farbe verwendet, die sich aus den
 * Komponenten Rot, Grün, Blau und optional einem Transparanz-Wert (Alpha-Wert) zusammensetzt.
 * Der Farbwert ist 32-Bit breit, wobei jede der 4 Komponenten 8 Bit verwendet.
 *
 */
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
    /**@brief Konstruktor ohne Angabe von Werten
     *
     * Mit diesem Konstruktor wird eine Instanz der Klasse erzeugt, und mit dem Farbwert
     * für schwarz initialisiert.
     */
    Color() { c = 0; };

    /**@brief Konstruktor mit Angabe der Farbwerte
     *
     * Mit diesem Konstruktor wird eine Instanz der Klasse erzeugt, die mit den angegebenen
     * Farbkomponenten initialisiert wird.
     *
     * @param red Wert zwischen 0 und 255, der den rot-Anteil der Farbe festlegt
     * @param green Wert zwischen 0 und 255, der den grün-Anteil der Farbe festlegt
     * @param blue Wert zwischen 0 und 255, der den blau-Anteil der Farbe festlegt
     * @param alpha Optionaler Wert zwischen 0 und 255, der die Transparenz der Farbe
     * festlegt. 0 bedeutet komplett transparent, 255 komplett sichtbar.
     */
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    /**@brief Konstruktor mit Angabe eines Farbwertes als 32-Bit Wert
     *
     * Mit diesem Konstruktor wird eine Instanz der Klasse erzeugt, die mit
     * dem angegebenen 32-Bit-Farbwert initialisiert wird.
     *
     * @param rgba 32-Bit Farbwert
     */
    Color(uint32_t rgba) { c = rgba; }

    /**@brief rot-Anteil auslesen
     *
     * Mit dieser Funktion wird der rot-Anteil der Farbe ausgelesen.
     *
     * @return rot-Anteil zwischen 0 und 255
     */
    constexpr inline uint8_t red() const { return r; }

    /**@brief grün-Anteil auslesen
     *
     * Mit dieser Funktion wird der grün-Anteil der Farbe ausgelesen.
     *
     * @return grün-Anteil zwischen 0 und 255
     */
    constexpr inline uint8_t green() const { return g; }

    /**@brief blau-Anteil auslesen
     *
     * Mit dieser Funktion wird der blau-Anteil der Farbe ausgelesen.
     *
     * @return blau-Anteil zwischen 0 und 255
     */
    constexpr inline uint8_t blue() const { return b; }

    /**@brief alpha-Anteil auslesen
     *
     * Mit dieser Funktion wird der alpha-Anteil der Farbe ausgelesen.
     *
     * @return alpha-Anteil zwischen 0 und 255
     */
    constexpr inline uint8_t alpha() const { return a; }

    /**@brief Farbwert als 32-Bit-Wert auslesen
     *
     * Mit dieser Funktion kann der Farbwert als 32-Bit-Wert ausgelesen werden.
     *
     * @return 32-Bit Farbwert
     */
    constexpr inline uint32_t rgba() const { return c; }

    /**@brief rot-Anteil setzen
     *
     * Mit dieser Funktion wird der rot-Anteil der Farbe gesetzt.
     *
     * @param red rot-Anteil zwischen 0 und 255.
     */
    inline void setRed(uint8_t red) { r = red; }

    /**@brief grün-Anteil setzen
     *
     * Mit dieser Funktion wird der grün-Anteil der Farbe gesetzt.
     *
     * @param green grün-Anteil zwischen 0 und 255.
     */
    inline void setGreen(uint8_t green) { g = green; }

    /**@brief blau-Anteil setzen
     *
     * Mit dieser Funktion wird der blau-Anteil der Farbe gesetzt.
     *
     * @param blue blau-Anteil zwischen 0 und 255.
     */
    inline void setBlue(uint8_t blue) { b = blue; }

    /**@brief alpha-Anteil setzen
     *
     * Mit dieser Funktion wird der alpha-Anteil der Farbe gesetzt.
     *
     * @param alpha alpha-Anteil zwischen 0 und 255.
     */
    inline void setAlpha(uint8_t alpha) { a = alpha; }

    /**@brief Farbwert anhand eines 32-Bit-Wertes setzen
     *
     * Mit dieser Funktgion wird die Farbe anhand des angegebenen
     * 32-Bit-Farbwertes gesetzt.
     *
     * @param rgba 32-Bit Farbwert
     */
    inline void setColor(uint32_t rgba) { c = rgba; }

    /**@brief Farbwert anhand der einzelnen Farbkomponenten setzen
     *
     * Mit dieser Funktion wird die Farbe anhand ihrer einzelnen
     * Komponenten gesetzt.

    * @param red Wert zwischen 0 und 255, der den rot-Anteil der Farbe festlegt
    * @param green Wert zwischen 0 und 255, der den grün-Anteil der Farbe festlegt
    * @param blue Wert zwischen 0 und 255, der den blau-Anteil der Farbe festlegt
    * @param alpha Optionaler Wert zwischen 0 und 255, der die Transparenz der Farbe
    * festlegt. 0 bedeutet komplett transparent, 255 komplett sichtbar. Wird der
    * Parameter nicht angegeben, wird automatisch 255 gesetzt.
    */
    inline void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    /**@brief Helligkeit der Farbe
     *
     * Mit dieser Funktion kann die Helligkeit des Farbwerts ausgelesen werden. Um die Helligkeit
     * zu berechnen, werden die 3 Komponenten der Farbe rot, grün und blau unterschiedlich gewichtet.
     * Da grün am hellsten ist, wird dieser Komponente am höchsten bewertet, gefolgt von rot und
     * zuletzt blau. Die genaue Formel lautet:
     * \f$helligkeit = (rot * 11 + grün * 16 + blau * 5)/32\f$.
     * Der Alpha-Kanal (Transparenz) spielt bei der Berechnung keine Rolle.
     *
     * @return Helligkeit der Farbe auf einer Skala von 0-255
     */
    int brightness() const;

    /**@brief Grauwert der Farbe
     *
     * Diese Funktion ist identisch zu Color::brightness, nur wird hier nicht der
     * Helligkeitswert zurückgegeben, sondern eine neue Color-Klasse, deren Farbkomponenten
     * jeweils mit dem gleichen Helligkeitswert initialisiert werden. Der Transparenz-Wert wird
     * übernommen.
     *
     * @return Grau-Farbe
     */
    Color grey() const;

    /**@brief Negativ der Farbe
     *
     * Mit dieser Funktion kann das Negativ der Farbe erzeugt werden. Dabei werden die Farbkomponenten
     * rot, grün und blau invertiert, während der Alpha-Kanal unverändert bleibt.
     *
     * @return Negativ-Farbe
     */
    Color negativ() const;

    /**@\brief Farbübereinstimmung prüfen
     *
     * Diese Funktion prüft, ob die Farbe \p other mit dieser übereinstimmt, wobei jeder der
     * drei Farbwerte (rot, grün, blau) eine maximale Abweichung von \p tolerance haben darf.
     * Der Alpha-Kanal wird dabei nicht berücksichtigt.
     *
     * @param other Der zu vergleichende Farbwert
     * @param tolerance Ein optionaler Parameter, der die maximale Toleranz auf einer Skala
     * von 0-255 angibt. Der Default ist 0
     * @return Stimmen die Farbwerte überein, liefert die Funktion \c true zurück, andernfalls \c false.
     */
    bool match(const Color& other, int tolerance = 0) const;

    /**@brief Farben überblenden
     *
     * Mit dieser Funktion werden die zwei Farbwerte \p background und \p foreground
     * anhand des dritten Parameters \p intensity vermischt. Dabei gilt, je höher der Wert
     * \p intensity, desto stärker kommt der Farbwert \p foreground zur Geltung.
     *
     * @param background Hintergrundfarbe
     * @param foreground Vordergrundfarbe
     * @param intensity Intensität der Vordergrundfarbe auf einer Skala von 0-255
     * @return Neuer Farbwert
     */
    Color& blend(const Color& background, const Color& foreground, int intensity);

    /**@brief Farben überblenden
     *
     * Mit dieser Funktion werden die zwei Farbwerte \p background und \p foreground
     * anhand des dritten Parameters \p intensity vermischt. Dabei gilt, je höher der Wert
     * \p intensity, desto stärker kommt der Farbwert \p foreground zur Geltung.
     *
     * @param background Hintergrundfarbe
     * @param foreground Vordergrundfarbe
     * @param intensity Intensität der Vordergrundfarbe auf einer Skala von 0.0 bis 1.0
     * @return Neuer Farbwert
     */
    Color& blendf(const Color& background, const Color& foreground, float intensity);

    /**@brief Farbwert anhand eines anderen Farbwertes setzen
     *
     * Mit dieser Funktion wird die Farbe anhand eines anderen Farbwertes gesetzt.
     *
     * @param other Farbwert, der übernommen werden soll
     */
    void setColor(const Color& other);

    /**@brief Lineare Interpolation zwischen zwei Farben
     *
     * Mit dieser Funktion kann eine lineare Interpolation zwischen zwei Farben durchgeführt werden.
     * Dabei wird der Farbwert \p c1 mit dem Faktor \p factor gewichtet, während der Farbwert \p c2
     * mit dem Faktor (1.0 - \p factor) gewichtet wird. Der Faktor \p factor muss dabei im Bereich
     * von 0.0 bis 1.0 liegen.
     *
     * @param c1 Erste Farbe
     * @param c2 Zweite Farbe
     * @param factor Gewichtungsfaktor für die erste Farbe auf einer Skala von 0.0 bis 1.0
     * @return Interpolierte Farbe
     */
    static Color lerp(const Color& c1, const Color& c2, float t);

    Color& operator*=(float factor);
    Color& operator+=(const Color& other);

    bool operator<(const Color& other) const;
    bool operator<=(const Color& other) const;

    /**@brief Vergleich zweier Farben
     *
     * Mit den Operatoren \p == und \p != können zwei Farbwerte miteinander verglichen werden.
     *
     * @param other Anderer Farbwert
     * @return Bei Verwendung des Operators \p == wird \c true zurückgegeben, wenn beide Farbwerte
     * identisch sind und \c false, wenn dies nicht der Fall ist. Bei Verwendung des Operators
     * \p != wird \c true zurückgegeben, wenn die Farbwerte unterschiedlich sind und \c false, wenn
     * sie identisch sind.
     */
    bool operator==(const Color& other) const;

    /**@brief Vergleich zweier Farben
     *
     * Mit den Operatoren \p == und \p != können zwei Farbwerte miteinander verglichen werden.
     *
     * @param other Anderer Farbwert
     * @return Bei Verwendung des Operators \p == wird \c true zurückgegeben, wenn beide Farbwerte
     * identisch sind und \c false, wenn dies nicht der Fall ist. Bei Verwendung des Operators
     * \p != wird \c true zurückgegeben, wenn die Farbwerte unterschiedlich sind und \c false, wenn
     * sie identisch sind.
     */
    bool operator!=(const Color& other) const;
    bool operator>=(const Color& other) const;
    bool operator>(const Color& other) const;

    /**@brief Farbwert als 32-Bit-Wert auslesen
     *
     * Mit diesem Operator kann der Farbwert der Klasse als 32-Bit-Wert ausgelesen werden.
     *
     * @return 32-Bit Farbwert
     */
    constexpr inline operator uint32_t() const { return c; }
};

/**@brief Multiplikation einer Farbe mit einem Faktor
 *
 * Mit diesem Operator wird die angegebene Farbe \p color mit dem Faktor \p factor
 * multipliziert und das Ergebnis als neuer Farbwert zurückgegeben. Ein Wert größer 1.0
 * bewirkt, dass die Farbe heller wird, bei Werten kleiner 1,0 wird die Farbe dunkler.
 *
 * Ist das Ergebnis der Multiplikation eines Farbwertes größer 255, wird er auf 255 gesetzt.
 * Ein negativer Faktor wird zu 0.
 *
 * @param color Farbwert
 * @param factor Multiplikations-Faktor
 * @return Neuer Farbwert
 */
const Color operator*(const Color& color, float factor);

/**@brief Multiplikation einer Farbe mit einem Faktor
 *
 * Mit diesem Operator wird die angegebene Farbe \p color mit dem Faktor \p factor
 * multipliziert und das Ergebnis als neuer Farbwert zurückgegeben. Ein Wert größer 1.0
 * bewirkt, dass die Farbe heller wird, bei Werten kleiner 1,0 wird die Farbe dunkler.
 *
 * Ist das Ergebnis der Multiplikation eines Farbwertes größer 255, wird er auf 255 gesetzt.
 * Ein negativer Faktor wird zu 0.
 *
 * @param color Farbwert
 * @param factor Multiplikations-Faktor
 * @return Neuer Farbwert
 */
const Color operator*(float factor, const Color& color);
const Color operator+(const Color& color1, const Color& color2);

Color blendColor(const Color& background, const Color& foreground, int intensity);
Color blendColor(const Color& background, const Color& foreground, float intensity);

} // namespace picopplib

#endif