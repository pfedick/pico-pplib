#ifndef PICOPPLIB_GRAFIX_RGBFORMAT_H_
#define PICOPPLIB_GRAFIX_RGBFORMAT_H_

#include <stdint.h>
#include <stddef.h>

namespace picopplib
{

/**@class RGBFormat
 * @brief Datentyp, der das Farbformat einer Zeichenfläche repräsentiert
 *
 * Mit dieser Klasse wird das Farbformat einer Zeichenfläche repräsentiert. Sie enthält nur
 * einen einzigen Wert aus der Enumeration RGBFormat::Identifier, der das verwendete
 * Farbformat festlegt.
 */
class RGBFormat
{
public:
    /// @brief Enumeration der unterstützten Farbformate
    enum Identifier
    {
        unknown = 0,            ///< 0: Unbekanntes Format
        Monochrome1BitVertical, ///< 1 Bit pro Pixel, vertikal gepackt (z.B. SSD1322)
        R5G6B5,                 ///< 16 Bit pro Pixel: 5 Bit Rot, 6 Bit Grün, 5 Bit Blau
        A8R8G8B8,               ///< 32 Bit pro Pixel: 8 Bit Alpha, 8 Bit Rot, 8 Bit Grün, 8 Bit Blau
        GREY8,                  ///< 8 Bit pro Pixel: 8 Bit Graustufe
        MaxIdentifiers          ///< Obergrenze der Identifikatoren
    };

private:
    /**
     * @brief Das aktuell gesetzte Farbformat.
     */
    Identifier format_id;

public:
    RGBFormat();
    RGBFormat(Identifier id);

    /**@brief Farbformat Anhand einer ID festlegen
     *
     * Mit dieser Funktion wird das Farbformat anhand eines Wertes aus der Enumeration
     * RGBFormat::Identifier gesetzt.
     *
     * @param[in] id Wert aus der Enumeration RGBFormat::Identifier
     */
    void setFormat(Identifier id);

    /**@brief Integer-Wert des Farbformats auslesen
     *
     * Mit dieser Funktion wird der Integer-Wert des Farbformates ausgelesen.
     * Der Wert entspricht der Position des Farbformates innerhalb der Enumeration RGBFormat::Identifier.
     *
     * @return Integer-Wert, der das Farbformat repräsentiert.
     */
    Identifier format() const;

    /**@brief Bittiefe des Farbformates
     *
     * Diese Funktion liefert die Anzahl Bits zurück, die zur Darstellung des gewählten
     * Farbformats erforderlich sind. Ist in der Regel 8, 16, 24, 32 oder 64. Die Funktion ist
     * identisch mit RGBFormat::bitsPerPixel.
     *
     * @return Anzahl erforderlicher Bits oder 0, wenn kein Farbformat gesetzt ist.
     */
    uint8_t bitdepth() const;

    /**@brief Anzahl Bytes pro Pixel
     *
     * Diese Funktion liefert die Anzahl Bytes für eine bestimmte Anzahl von Pixeln zurück,
     * abhängig vom gewählten Farbformat.
     *
     * @param[in] width Anzahl Pixel, für die die erforderlichen Bytes berechnet werden sollen.
     * @return Anzahl erforderlicher Bytes pro Pixel oder 0, wenn kein Farbformat gesetzt ist.
     * @note Sonderfall ist das Farbformat Monochrome1BitVertical, bei dem die Pixel
     * vertikal gepackt sind. In diesem Fall entspricht die Anzahl Bytes der Anzahl Pixel,
     * da 8 Pixel pro Byte gespeichert werden, aber vertikal gepackt, so dass es einfach
     * width ist.
     */
    uint32_t bytesForWidth(uint16_t width) const;

    bool operator==(const RGBFormat& other) const;
    bool operator==(Identifier id) const;
};
inline bool operator==(RGBFormat::Identifier id, const RGBFormat& fmt)
{
    return fmt == id;
}

} // namespace picopplib

#endif // PICOPPLIB_GRAFIX_RGBFORMAT_H_