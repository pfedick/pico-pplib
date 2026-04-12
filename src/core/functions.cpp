#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <stdio.h>
#include <string.h>
#include "pico/rand.h"

#include "ppl7-light.h"
#include "grafix.h"

namespace ppl7light
{
void PrintDebug(const char* format, ...)
{
    if (!format) return;
    char* buff = NULL;
    va_list args;
    va_start(args, format);
    if (vasprintf(&buff, format, args) < 0) {
        va_end(args);
        return;
    }
    va_end(args);
    if (!buff) return;
    printf("%s", buff);
    free(buff);
}

void HexDump(const void* address, size_t bytes, bool skipheader)
{
    char buff[1024], tmp[10], cleartext[20];
    if (!skipheader) {
        printf("HEXDUMP: %d Bytes starting at Address %p:\n", (int)bytes, address);
    }
    const char* _adresse = (const char*)address;
    const char* start_adr = _adresse;
    int spalte = 0;
    // sprintf (buff,"%p: ",_adresse);
    buff[0] = 0;
    memset(cleartext, 0, 20);
    for (size_t i = 0; i < bytes; i++) {
        sprintf(tmp, "%02X ", (uint8_t)_adresse[i]);
        strcat(buff, tmp);
        if ((uint8_t)_adresse[i] > 31 && (uint8_t)_adresse[i] < 128)
            cleartext[spalte] = (uint8_t)_adresse[i];
        else
            cleartext[spalte] = '.';
        spalte++;
        if (spalte > 15) {
            buff[16 * 3 - 1] = 0;
            printf("%p: %s: %s\n", start_adr, buff, cleartext);
            buff[0] = 0;
            memset(cleartext, 0, 20);
            spalte = 0;
            start_adr = _adresse + i + 1;
        }
    }

    if (spalte > 0) {
        strcat(buff, "                                                               ");
        buff[16 * 3 - 1] = 0;
        printf("%p: %s: %s\n", start_adr, buff, cleartext);
    }
    if (!skipheader) printf("\n");
}

void HexDump(const void* address, size_t bytes)
{
    HexDump(address, bytes, false);
}

/*!\brief 8-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke8(void* Adresse, uint8_t Wert)
{
    ((uint8_t*)Adresse)[0] = Wert;
}

/*!\brief 16-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke16(void* Adresse, uint16_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)(Wert & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 8) & 255);
}

/*!\brief 24-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 24 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke24(void* Adresse, uint32_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)(Wert & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 8) & 255);
    ((uint8_t*)Adresse)[2] = (uint8_t)((Wert >> 16) & 255);
}

/*!\brief 32-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke32(void* Adresse, uint32_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)(Wert & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 8) & 255);
    ((uint8_t*)Adresse)[2] = (uint8_t)((Wert >> 16) & 255);
    ((uint8_t*)Adresse)[3] = (uint8_t)((Wert >> 24) & 255);
}

/*!\brief 64-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke64(void* Adresse, uint64_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)(Wert & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 8) & 255);
    ((uint8_t*)Adresse)[2] = (uint8_t)((Wert >> 16) & 255);
    ((uint8_t*)Adresse)[3] = (uint8_t)((Wert >> 24) & 255);
    ((uint8_t*)Adresse)[4] = (uint8_t)((Wert >> 32) & 255);
    ((uint8_t*)Adresse)[5] = (uint8_t)((Wert >> 40) & 255);
    ((uint8_t*)Adresse)[6] = (uint8_t)((Wert >> 48) & 255);
    ((uint8_t*)Adresse)[7] = (uint8_t)((Wert >> 56) & 255);
}

/*!\brief 32-Bit-Float-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die Inhalt des Floats \p Wert wird in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeFloat(void* Adresse, float Wert)
{
    // Immer Little-Endian ablegen, Byte für Byte
    uint8_t* dst = (uint8_t*)Adresse;
    uint8_t* src = (uint8_t*)&Wert;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

/*!\brief 8-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint8_t Peek8(const void* Adresse)
{
    return (uint32_t)((uint8_t*)Adresse)[0];
}

/*!\brief 16-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint16_t Peek16(const void* Adresse)
{
    uint8_t wert1 = ((uint8_t*)Adresse)[0];
    uint8_t wert2 = ((uint8_t*)Adresse)[1];
    return ((uint16_t)wert1 | ((uint16_t)wert2 << 8));
}

/*!\brief 24-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 24 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint32_t Peek24(const void* Adresse)
{
    uint8_t wert1, wert2, wert3;
    wert1 = ((uint8_t*)Adresse)[0];
    wert2 = ((uint8_t*)Adresse)[1];
    wert3 = ((uint8_t*)Adresse)[2];
    return ((uint32_t)wert1 | (wert2 << 8) | (wert3 << 16));
}

/*!\brief 32-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint32_t Peek32(const void* Adresse)
{
    uint8_t wert1, wert2, wert3, wert4;
    wert1 = ((uint8_t*)Adresse)[0];
    wert2 = ((uint8_t*)Adresse)[1];
    wert3 = ((uint8_t*)Adresse)[2];
    wert4 = ((uint8_t*)Adresse)[3];

    return ((uint32_t)(uint32_t)wert1 | ((uint32_t)wert2 << 8) | ((uint32_t)wert3 << 16) | ((uint32_t)wert4 << 24));
}

/*!\brief 64-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint64_t Peek64(const void* Adresse)
{
    uint8_t wert1, wert2, wert3, wert4, wert5, wert6, wert7, wert8;
    wert1 = ((uint8_t*)Adresse)[0];
    wert2 = ((uint8_t*)Adresse)[1];
    wert3 = ((uint8_t*)Adresse)[2];
    wert4 = ((uint8_t*)Adresse)[3];
    wert5 = ((uint8_t*)Adresse)[4];
    wert6 = ((uint8_t*)Adresse)[5];
    wert7 = ((uint8_t*)Adresse)[6];
    wert8 = ((uint8_t*)Adresse)[7];

    return ((uint64_t)(uint64_t)wert1 | ((uint64_t)wert2 << 8) | ((uint64_t)wert3 << 16) | ((uint64_t)wert4 << 24) |
            ((uint64_t)wert5 << 32) | ((uint64_t)wert6 << 40) | ((uint64_t)wert7 << 48) | ((uint64_t)wert8 << 56));
}

/*!\brief 32-Bit-Float-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Float-Wert zurückgegeben.
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
float PeekFloat(const void* Adresse)
{
    // Immer aus Little-Endian zusammensetzen
    float Wert;
    uint8_t* dst = (uint8_t*)&Wert;
    const uint8_t* src = (const uint8_t*)Adresse;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    return Wert;
}

/*!\brief 8-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN8(void* Adresse, uint8_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)(Wert & 255);
}

/*!\brief 16-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN16(void* Adresse, uint16_t Wert)
{
    ((uint8_t*)Adresse)[1] = (uint8_t)(Wert & 255);
    ((uint8_t*)Adresse)[0] = (uint8_t)((Wert >> 8) & 255);
}

/*!\brief 24-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 24 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN24(void* Adresse, uint32_t Wert)
{
    // Immer als Big-Endian (Network Order) ablegen: [0]=MSB ... [2]=LSB
    ((uint8_t*)Adresse)[0] = (uint8_t)((Wert >> 16) & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 8) & 255);
    ((uint8_t*)Adresse)[2] = (uint8_t)(Wert & 255);
}

/*!\brief 32-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN32(void* Adresse, uint32_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)((Wert >> 24) & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 16) & 255);
    ((uint8_t*)Adresse)[2] = (uint8_t)((Wert >> 8) & 255);
    ((uint8_t*)Adresse)[3] = (uint8_t)(Wert & 255);
}

/*!\brief 64-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN64(void* Adresse, uint64_t Wert)
{
    ((uint8_t*)Adresse)[0] = (uint8_t)((Wert >> 56) & 255);
    ((uint8_t*)Adresse)[1] = (uint8_t)((Wert >> 48) & 255);
    ((uint8_t*)Adresse)[2] = (uint8_t)((Wert >> 40) & 255);
    ((uint8_t*)Adresse)[3] = (uint8_t)((Wert >> 32) & 255);
    ((uint8_t*)Adresse)[4] = (uint8_t)((Wert >> 24) & 255);
    ((uint8_t*)Adresse)[5] = (uint8_t)((Wert >> 16) & 255);
    ((uint8_t*)Adresse)[6] = (uint8_t)((Wert >> 8) & 255);
    ((uint8_t*)Adresse)[7] = (uint8_t)(Wert & 255);
}

/*!\brief 8-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint8_t PeekN8(const void* Adresse)
{
    return (uint8_t)((uint8_t*)Adresse)[0];
}

/*!\brief 16-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint16_t PeekN16(const void* Adresse)
{
    uint8_t wert1, wert2;
    wert1 = ((uint8_t*)Adresse)[1];
    wert2 = ((uint8_t*)Adresse)[0];
    return ((uint16_t)(uint16_t)wert1 | ((uint16_t)wert2 << 8));
}

/*!\brief 24-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 24 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint32_t PeekN24(const void* Adresse)
{
    uint8_t msb = ((uint8_t*)Adresse)[0];
    uint8_t mid = ((uint8_t*)Adresse)[1];
    uint8_t lsb = ((uint8_t*)Adresse)[2];
    return ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | (uint32_t)lsb;
}

/*!\brief 32-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint32_t PeekN32(const void* Adresse)
{
    uint8_t wert1, wert2, wert3, wert4;
    wert1 = ((uint8_t*)Adresse)[3];
    wert2 = ((uint8_t*)Adresse)[2];
    wert3 = ((uint8_t*)Adresse)[1];
    wert4 = ((uint8_t*)Adresse)[0];
    return ((uint32_t)(uint32_t)wert1 | ((uint32_t)wert2 << 8) | ((uint32_t)wert3 << 16) | ((uint32_t)wert4 << 24));
}

/*!\brief 64-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
uint64_t PeekN64(const void* Adresse)
{
    uint8_t wert1, wert2, wert3, wert4, wert5, wert6, wert7, wert8;
    wert1 = ((uint8_t*)Adresse)[7];
    wert2 = ((uint8_t*)Adresse)[6];
    wert3 = ((uint8_t*)Adresse)[5];
    wert4 = ((uint8_t*)Adresse)[4];
    wert5 = ((uint8_t*)Adresse)[3];
    wert6 = ((uint8_t*)Adresse)[2];
    wert7 = ((uint8_t*)Adresse)[1];
    wert8 = ((uint8_t*)Adresse)[0];
    return ((uint64_t)(uint64_t)wert1 | ((uint64_t)wert2 << 8) | ((uint64_t)wert3 << 16) | ((uint64_t)wert4 << 24) |
            ((uint64_t)wert5 << 32) | ((uint64_t)wert6 << 40) | ((uint64_t)wert7 << 48) | ((uint64_t)wert8 << 56));
}

size_t rand(size_t min, size_t max)
{
    uint64_t r64 = get_rand_64();
    return min + (r64 % (max - min + 1));
}

double randf(double min, double max)
{
    uint64_t r64 = get_rand_64();
    double fraction = (double)r64 / (double)UINT64_MAX;
    return min + fraction * (max - min);
}

} // namespace ppl7light
