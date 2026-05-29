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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

#include "picopplib.h"

namespace picopplib
{
static size_t InitialBuffersize = 64;
static char* empty_string = (char*)"";

/*!\class String
 * \ingroup PPLGroupDataTypes
 * \ingroup PPLGroupStrings
 * \brief String-Klasse
 *
 * \desc
 * Diese Klasse kann verwendet werden, um beliebige Strings zu speichern und zu verarbeiten. Dabei
 * braucht sich der Anwender keine Gedanken um den verwendeten Speicher zu machen.
 * Die einzelnen Zeichen des Strings werden intern im Unicode-Format gespeichert. Bei Übernahme eines
 * C-Strings wird erwartet, dass dieser im UTF-8 Format vorliegt, mit der statischen Funktion
 * String::setGlobalEncoding kann jedoch auch eine andere Kodierung vorgegeben werden.
 *
 */

/*!\brief Konstruktor für leeren String
 *
 * \desc
 * Es wird ein leerer String erstellt.
 */
String::String() throw()
{
    ptr = empty_string;
    stringlen = 0;
    s = 0;
}

/*!\brief Konstruktor aus C-String
 *
 * \desc
 * Ein String wird aus einem C-String erstellt.
 *
 * @param str C-String mit 0-Byte am Ende
 * @exception OutOfMemoryException
 * @exception UnsupportedFeatureException
 * @exception UnsupportedCharacterEncodingException
 * @exception CharacterEncodingException
 */
String::String(const char* str)
{
    ptr = empty_string;
    stringlen = 0;
    s = 0;
    set(str);
}

/*!\brief Konstruktor aus Wide-Character-String mit bestimmer Länge
 *
 * \desc
 * Ein String wird aus dem Wide-Character-String \p str erstellt, von dem maximal
 * \p size Zeichen übernommen werden.
 *
 * @param str Wide-Character-String, der mit einem 0-Wert Endet
 * @param size Maximale Anzahl Zeichen, die übernommen werden sollen
 * @exception OutOfMemoryException
 */
String::String(const char* str, size_t size)
{
    ptr = empty_string;
    stringlen = 0;
    s = 0;
    set(str, size);
}

/*!\brief Konstruktor aus anderem String (Copy-Konstruktor)
 *
 * \desc
 * Ein String wird aus einem anderen String erstellt.
 *
 * @param str Referenz auf einen anderen String
 * @exception OutOfMemoryException
 */
String::String(const String& str)
{
    ptr = empty_string;
    stringlen = 0;
    s = 0;
    set(str);
}

String::String(String&& other) noexcept
{
    ptr = other.ptr;
    s = other.s;
    stringlen = other.stringlen;
    other.ptr = empty_string;
    other.s = 0;
    other.stringlen = 0;
}

String& String::operator=(String&& other) noexcept
{
    if (this != &other) {
        if (ptr != empty_string) free(ptr);
        ptr = other.ptr;
        s = other.s;
        stringlen = other.stringlen;
        other.ptr = empty_string;
        other.s = 0;
        other.stringlen = 0;
    }
    return *this;
}

String::String(const ByteArrayPtr& str)
{
    ptr = empty_string;
    stringlen = 0;
    s = 0;
    set(str);
}

/*!\brief Destruktor
 *
 * \desc
 * Der Destructor gibt den durch den String belegten Speicher wieder frei.
 *
 */
String::~String() throw()
{
    if (ptr != empty_string) free(ptr);
}

/*!\brief String leeren
 *
 * \desc
 * Mit dieser Funktion wird der String geleert und der bisher allokierte Speicher wieder
 * freigegeben.
 */
void String::clear() throw()
{
    if (ptr != empty_string) free(ptr);
    ptr = empty_string;
    stringlen = 0;
    s = 0;
}

/*!\brief Anzahl Zeichen, die in den bereits allokierten Speicher passen
 *
 * \desc
 * Diese Funktion liefert die Anzahl Zeichen zurück, die in den derzeitig allokierten
 * Puffer passen, ohne dass neuer Speicher allokiert werden muss.
 *
 * @return Anzahl Zeichen
 */
size_t String::capacity() const
{
    if (!s) return 0;
    return (s / sizeof(char)) - 1;
}

/*!\brief Reserviert Speicher für den String
 *
 * \desc
 * Mit dieser Funktion kann vor Verwendung des Strings vorgegeben werden, wieviel
 * Speicher initial reserviert werden soll. Dies ist insbesondere dann sinnvoll,
 * wenn der String während seiner Lebenszeit häufig verlängert wird.
 *
 * @param[in] size Anzahl Zeichen, für die Speicher reserviert werden soll.
 *
 * \note
 * Enthält der String bereits Zeichen, gehen diese nicht verloren, der existierende
 * Speicherbereich kann aber zwecks Vergrößerung umkopiert werden. Der Aufruf
 * der Funktion String::clear führt dazu, dass der Speicher wieder freigegeben wird.
 *
 */
void String::reserve(size_t size)
{
    size_t bytes = (size + 1) * sizeof(char);
    if (s >= bytes) return; // Nothing to do
    char* p;
    if (ptr == empty_string)
        p = (char*)malloc(bytes);
    else
        p = (char*)realloc(ptr, bytes);
    if (!p) throw Exception("OutOfMemoryException");
    ptr = p;
    s = bytes;
}

/*!\brief Länge des Strings
 *
 * \desc
 * Diese Funktion gibt die Anzahl Zeichen zurück, aus denen der String besteht.
 *
 * \note
 * Die Funktionen String::len, String::length und String::size sind identisch.
 * \see
 * String::capacity
 *
 * @return Anzahl Zeichen
 */
size_t String::size() const
{
    return stringlen;
}

/*! \brief Prüft, ob der String leer ist.
 *
 * \desc
 * Diese Funktion prüft, ob der String leer ist.
 *
 * \returns Ist der String leer, liefert die Funktion \c true zurück, sonst \c false.
 * \see String::notEmpty
 */
bool String::isEmpty() const
{
    if (stringlen == 0) return true;
    return false;
}

/*! \brief Prüft, ob der String Zeichen enthält
 *
 * \desc
 * Diese Funktion prüft, ob der String Zeichen enthält.
 *
 * \returns Enthält der String Zeichen, liefert die Funktion \c true zurück, sonst \c false.
 * \see String::isEmpty
 */
bool String::notEmpty() const
{
    if (stringlen == 0) return false;
    return true;
}

/*!\brief Prüft, ob der String nummerisch ist
 *
 * \desc
 * Diese Funktion prüft, ob im String nur nummerische Zeichen vorhanden sind, also die Ziffern
 * 0-9, Punkt, Komma und Minus.
 *
 * @return Ist der String nummerisch, wird 1 zurückgegeben. Ist er es nicht oder ist der String
 * leer, wird 0 zurückgegeben.
 */
bool String::isNumeric() const
{
    if (!stringlen) return false;
    size_t dotcount = 0;
    for (size_t i = 0; i < stringlen; i++) {
        int c = ((char*)ptr)[i];
        if (c < '0' || c > '9') {
            if (c != '.' && c != ',' && c != '-') return false;
            if (c == '-' && i > 0) return false;
            if (c == '.' || c == ',') {
                dotcount++;
                if (dotcount > 1) return false;
            }
        }
    }
    if (ptr[stringlen - 1] == '.') return false;
    return (true);
}

/*!\brief Prüft, ob der String einen Integer Wert enthält
 *
 * \desc
 * Diese Funktion prüft, ob im String einen integer Wert enthält, also nur die Ziffern
 * 0-9 und optional ein Minus am Anfang enthalten sind
 *
 * @return Ist der String ein Integer, wird 1 zurückgegeben. Ist er es nicht oder ist der String
 * leer, wird 0 zurückgegeben.
 */
bool String::isInteger() const
{
    if (!stringlen) return false;
    for (size_t i = 0; i < stringlen; i++) {
        int c = ((char*)ptr)[i];
        if (c < '0' || c > '9') {
            if (c == '-' && i == 0) continue; // Minus am Anfang ist erlaubt
            return false;
        }
    }
    return true;
}

/*!\brief Prüft, ob der String "wahr" ist
 *
 * Diese Funktion überprüft den aktuellen String, ob er "wahr" ist. Dies ist der Fall,
 * wenn eine der folgenden Bedingungen erfüllt ist:
 * - Der String enthält eine Ziffer ungleich 0
 * - Der String enthält das Wort "true" (Gross- oder Kleingeschrieben)
 * - Der String enthält das Wort "wahr" (Gross- oder Kleingeschrieben)
 * - Der String enthält das Wort "yes" (Gross- oder Kleingeschrieben)
 * - Der String enthält das Wort "ja" (Gross- oder Kleingeschrieben)
 *
 * \returns Liefert true (1) zurück, wenn der String "wahr" ist, sonst false (0). Ein Fehlercode wird nicht gesetzt
 * \see CWString::IsFalse()
 */
bool String::isTrue() const
{
    if (!stringlen) return false;
    if (atol(ptr) != 0) return true;
    if (strCaseCmp("true") == 0) return true;
    if (strCaseCmp("wahr") == 0) return true;
    if (strCaseCmp("ja") == 0) return true;
    if (strCaseCmp("yes") == 0) return true;
    if (strCaseCmp("t") == 0) return true;
    return false;
}

/*!\brief Prüft, ob der String "unwahr" ist
 *
 * Diese Funktion überprüft den aktuellen String, ob er "unwahr" ist. Dies ist der Fall,
 * wenn eine der folgenden Bedingungen erfüllt ist:
 * - Der String zeigt auf NULL
 * - Die Länge des Strings ist 0
 * - Der String enthält die Ziffer 0
 * - Der String enthält nicht das Wort "true", "wahr", "yes" oder "ja" (Gross-/Kleinschreibung egal)
 *
 * \returns Liefert true (1) zurück, wenn der String "unwahr" ist, sonst false (0). Ein Fehlercode wird nicht
 * gesetzt
 * \see CWString::IsTrue()
 */
bool String::isFalse() const
{
    if (isTrue()) return false;
    return true;
}

/*!\brief String anhand eines C-Strings setzen
 *
 * \desc
 * Mit dieser Funktion wird der String anhand eines char * gesetzt. Dabei wird er
 * intern nach Unicode konvertiert.
 *
 * \param str Pointer auf einen String
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 * \exception UnsupportedFeatureException
 * \exception UnsupportedCharacterEncodingException
 * \exception CharacterEncodingException
 *
 * \note
 * Multibyte-Characters zählen als ein Zeichen.
 *
 */
String& String::set(const char* str, size_t size)
{
    if (!str) {
        clear();
        return *this;
    }
    size_t inbytes;
    if (size != (size_t)-1)
        inbytes = size;
    else
        inbytes = ::strlen(str);
    if (inbytes == 0) {
        clear();
        return *this;
    }
    size_t outbytes = inbytes * sizeof(char) + 1;
    if (outbytes >= s) {
        if (ptr != empty_string) free(ptr);
        stringlen = 0;
        s = InitialBuffersize;
        if (s <= outbytes) s = ((outbytes / InitialBuffersize) + 1) * InitialBuffersize + 1;
        ptr = (char*)malloc(s);
        if (!ptr) {
            s = 0;
            throw Exception("OutOfMemoryException");
        }
    }
#ifdef HAVE_STRNCPY_S
    strncpy_s((char*)ptr, s, str, inbytes);
#else
    strncpy((char*)ptr, str, inbytes);
#endif
    stringlen = inbytes;
    ((char*)ptr)[stringlen] = 0;
    return *this;
}

/*!\brief Wert eines anderen Strings übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des Strings \p str übernommen.
 *
 * \param str Referenz auf einen anderen String
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 */
String& String::set(const String& str, size_t size)
{
    size_t inbytes;
    if (size != (size_t)-1)
        inbytes = size;
    else
        inbytes = str.stringlen;
    if (inbytes > str.stringlen) inbytes = str.stringlen;
    return set(str.ptr, inbytes);
}

String& String::set(const ByteArrayPtr& str, size_t size)
{
    size_t inbytes;
    if (size != (size_t)-1)
        inbytes = size;
    else
        inbytes = str.size();
    if (inbytes > str.size()) inbytes = str.size();
    return set((const char*)str.adr(), inbytes);
}

/*!\brief Einzelnes Zeichen ersetzen
 *
 * \desc
 * Mit dieser Funktion wird ein einzelnes Zeichen eines Strings an der Position
 * \p position durch das Zeichen \p c ersetzt.
 *
 * @param position Position innerhalb des Strings (Zählung beginnt bei 0)
 * @param c Unicode-Wert, der gesetzt werden soll
 * @return Referenz auf den String
 * \throw OutOfBoundsEception: Wird geworfen, wenn \p position größer ist, als die
 * Länge des Strings
 */
String& String::set(size_t position, char c)
{
    if (position >= stringlen) throw Exception("OutOfBoundsEception");
    ptr[position] = c;
    return *this;
}

/*! \brief Erzeugt einen formatierten String
 *
 * \desc
 * Erzeugt einen String anhand des übergebenen Formatstrings \p fmt
 * und den optionalen Parametern \p ...
 *
 * \param fmt Der Formatstring
 * \param ... Optionale Parameter
 *
 * @return Referenz auf den String
 *
 * \par Example:
 * \code
int main(int argc, char **argv)
{
    String s;
    s.setf ("Anzahl Parameter: %i\n",argc);
    s.printnl();
    return 0;
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String& String::setf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* buff = NULL;
    if (::vasprintf(&buff, (char*)fmt, args) >= 0 && buff != NULL) {
        try {
            set(buff);
            free(buff);
        }
        catch (...) {
            free(buff);
            va_end(args);
            throw;
        }
        return *this;
    }
    va_end(args);
    free(buff);
    throw Exception("String::setf");
}

/*!\brief Einzelnes ASCII-Zeichen übernehmen
 *
 * \desc
 * Ein einzelnes ASCII-Zeichen \p c wird in den String übernommen.
 *
 * @param c ASCII-Wert des gewünschten Zeichens
 *
 * @return Referenz auf den String
 */
String& String::set(char c)
{
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    return set(buffer, 1);
}

/*! \brief Erzeugt einen formatierten String
 *
 * Erzeugt einen String anhand des übergebenen Formatstrings \p fmt
 * und den optionalen Parametern in \p args.
 *
 * \param fmt Der Formatstring
 * \param args Pointer auf Liste der Parameter. Muss zuvor mit va_start initialisiert worden sein.
 * @return Referenz auf den String
 * \par Example:
 * \code
void MyFunction(const char *fmt, ...)
{
    String s;
    va_list args;
    va_start(args, fmt);
    s.vasprintf(fmt,args);
    va_end(args);
    printf ("Ergebnis: %s",(const char*)s);
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String& String::vasprintf(const char* fmt, va_list args)
{
    char* buff = NULL;
    int res = ::vasprintf(&buff, (char*)fmt, args);
    if (res >= 0 && buff != NULL) {
        set(buff, res);
        free(buff);
        return *this;
    }
    if (buff) free(buff);
    throw Exception("OperationFailed", "vasprintf");
}

/*!\brief String-Speicher übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Klasse die Verwaltung des Speicherbereich mit der Adresse \p adr und der
 * Größe \p size übergeben. Der Speicher muss zuvor mit "malloc" bzw. "calloc" allokiert worden sein
 * und darf von der Anwendung selbst nicht mehr freigegeben werden.
 *
 * @param[in] adr Startadresse des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 * @param[in] stringlen Optionaler Parameter, der die Länge des Strings innerhalb des übergebenen
 * Speicherbereichs angibt. Darf maximal \b size-1 groß sein. Ist der Wert nicht angegeben, wird die
 * Länge des Strings mit \b strlen berechnet
 *
 * \note Der String muss mit einem Null-Byte enden. Um dies sicherzustellen überschreibt die Methode
 * das letzte Byte des übergebenen Speicherbereichs mit 0.
 *
 * \return Referenz auf den String
 *
 */
String& String::useadr(void* adr, size_t size, size_t stringlen)
{
    if (adr == NULL || size == 0) throw Exception("IllegalArgumentException", "adr and size must not be 0");
    if (ptr != empty_string) free(ptr);
    ptr = (char*)adr;
    s = size;
    ptr[s - 1] = 0;
    this->stringlen = stringlen;
    if (stringlen == (size_t)-1) stringlen = strlen(ptr);
    if (stringlen >= size) stringlen = size - 1;
    return *this;
}

/*!\brief Fügt einen C-String an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen C-String an das Ende des bestehenden an. Der String muss entweder
 * UTF-8 kodiert sein, oder es muss mit der statischen Funktion String::setGlobalEncoding
 * zuvor eine andere Kodierung gesetzt worden sein.
 *
 * \param[in] str Pointer auf einen Wide-Character String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 * \exception UnsupportedFeatureException
 * \exception UnsupportedCharacterEncodingException
 * \exception CharacterEncodingException
 *
 */
String& String::append(const char* str, size_t size)
{
    if (str == NULL || size == 0) return *this;
    if (ptr == empty_string) {
        return set(str, size);
    }
    size_t inchars;
    if (size != (size_t)-1) {
        inchars = size;
        if (inchars > strlen(str)) inchars = strlen(str);
    } else
        inchars = strlen(str);
    size_t outbytes = (inchars + stringlen) * sizeof(char) + 1;
    if (outbytes >= s) {
        size_t newbuffersize = ((outbytes / InitialBuffersize) + 1) * InitialBuffersize + 16;
        char* t = (char*)realloc(ptr, newbuffersize);
        if (!t) throw Exception("OutOfMemoryException");
        ptr = t;
        s = newbuffersize;
    }
    memcpy(((char*)ptr) + stringlen, str, inchars);
    stringlen += inchars;
    ptr[stringlen] = 0;
    return *this;
}

/*!\brief Fügt einen String an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen String an das Ende des bestehenden an.
 *
 * \param[in] str Referenz auf ein String-Objekt
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String& String::append(const String& str, size_t size)
{
    return append(str.ptr, size);
}

/*!\brief Fügt einen Formatierten String an das Ende des bestehenden an
 *
 * Anhand des übergebenen Formatstrings \p fmt und den optionalen Parametern \p ...
 * wird ein neuer String gebildet, der an das Ende des bestehenden angehangen wird
 *
 * \param fmt Der Formatstring
 * \param ... Optionale Parameter
  * @return Referenz auf den String
 *
 * \par Example:
 * \code
int main(int argc, char **argv)
{
    String s;
    s="Hallo Welt!";
    s.appendf (" Es wurden %i Parameter übergeben\n",argc);
    s.printnl();
    return 0;
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String& String::appendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* buff = NULL;
    int res = ::vasprintf(&buff, (const char*)fmt, args);
    va_end(args);
    if (res >= 0 && buff != NULL) {
        append(buff, res);
        free(buff);
        return *this;
    }
    if (buff) free(buff);
    throw Exception("OperationFailed", "vasprintf");
}

/*!\brief Einzelnes ASCII-Zeichen anhängen
 *
 * \desc
 * Ein einzelnes ASCII-Zeichen \p c wird in an den String angehangen.
 *
 * @param c ASCII-Wert des gewünschten Zeichens
 *
 * @return Referenz auf den String
 */
String& String::append(char c)
{
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    return append(buffer, 1);
}

/*!\brief Fügt einen String am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen String am Anfang des bestehenden Strings ein
 *
 * \param[in] str Referenz auf einen String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String& String::prepend(const String& str, size_t size)
{
    if (ptr == empty_string) {
        set(str, size);
        return *this;
    }
    String a;
    a.set(str, size);
    return prepend(a.ptr, a.stringlen);
}

/*!\brief Fügt einen C-String am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen C-String am Anfang des bestehenden Strings ein
 *
 * \param[in] str Pointer auf einen C-String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String& String::prepend(const char* str, size_t size)
{
    if (str == NULL || size == 0) return *this;
    if (ptr == empty_string) {
        set(str, size);
        return *this;
    }
    size_t inchars;
    if (size != (size_t)-1) {
        inchars = size;
        if (inchars > strlen(str)) inchars = strlen(str);
    } else
        inchars = strlen(str);
    size_t outbytes = (inchars + stringlen) * sizeof(char) + 1;
    if (outbytes >= s) {
        size_t newbuffersize = ((outbytes / InitialBuffersize) + 1) * InitialBuffersize + 16;
        char* t = (char*)realloc(ptr, newbuffersize);
        if (!t) throw Exception("OutOfMemoryException");
        ptr = t;
        s = newbuffersize;
    }
    // Bestehenden Speicherblock nach hinten moven
    memmove(((char*)ptr) + inchars, ptr, stringlen);
    // Neuen Speicherblock davor kopieren
    memcpy(ptr, str, inchars);
    stringlen += inchars;
    ptr[stringlen] = 0;
    return *this;
}

/*!\brief Fügt einen Formatierten String am Anfang bestehenden ein
 *
 * Anhand des übergebenen Formatstrings \p fmt und den optionalen Parametern \p ...
 * wird ein neuer String gebildet, der am Anfang des bestehenden eingehangen wird.
 *
 * \param fmt Der Formatstring
 * \param ... Optionale Parameter
 * \par Example:
 * \code
int main(int argc, char **argv)
{
    String s;
    s="Vielen Dank!\n";
    s.prependf ("Es wurden %i Parameter übergeben. ",argc);
    s.printnl();
    return 0;
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String& String::prependf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* buff = NULL;
    if (::vasprintf(&buff, (const char*)fmt, args) >= 0 && buff != NULL) {
        try {
            String a;
            a.set(buff);
            free(buff);
            prepend(a.ptr, a.stringlen);
        }
        catch (...) {
            free(buff);
            va_end(args);
            throw;
        }
        return *this;
    }
    va_end(args);
    free(buff);
    throw Exception("OperationFailed", "vasprintf");
}

/*!\brief Einzelnes ASCII-Zeichen am Anfang einfügen
 *
 * \desc
 * Ein einzelnes ASCII-Zeichen \p c wird in am Anfang des Strings eingefügt.
 * Die nachfolgenden Zeichen des Strings verschieben sich nach rechts.
 *
 * @param c ASCII-Wert des gewünschten Zeichens
 *
 * @return Referenz auf den String
 */
String& String::prepend(char c)
{
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    return prepend(buffer, 1);
}

/*!\brief Einzelnes Zeichen auslesen
 *
 * \desc
 * Mit dieser Funktion kann Bytewert eines einzelnen Zeichens an der Position
 * \p pos ausgelesen werden. Enthält \p pos einen positiven Wert, wird die Position des
 * Zeichens vom Anfang des Strings ermittelt, wobei 0 dem ersten Zeichen entspricht.
 * Ist der Wert negativ, wird das Zeichen vom Ende des Strings ermittelt, wobei -1
 * dem letzten Zeichen des Strings entspricht.
 *
 * @param pos Position des Zeichens innerhalb des Strings
 * @return Bytewert des Zeichens
 * \exception OutOfBoundsEception Wird geworfen, wenn die angegebene Position \p pos
 * ausserhalb des Strings liegt oder der String leer ist.
 */
char String::get(ssize_t pos) const
{
    if (pos >= 0 && stringlen > (size_t)pos) return ((char*)ptr)[pos];
    if (pos < 0 && (size_t)(0 - pos) < stringlen) return ((char*)ptr)[stringlen + pos];
    throw Exception("OutOfBoundsEception");
}

/*!\brief Einzelnes Zeichen auslesen
 *
 * \desc
 * Mit diesem Operator kann der Bytewert eines einzelnen Zeichens an der Position
 * \p pos ausgelesen werden. Enthält \p pos einen positiven Wert, wird die Position des
 * Zeichens vom Anfang des Strings ermittelt, wobei 0 dem ersten Zeichen entspricht.
 * Ist der Wert negativ, wird das Zeichen vom Ende des Strings ermittelt, wobei -1
 * dem letzten Zeichen des Strings entspricht.
 *
 * @param pos Position des Zeichens innerhalb des Strings
 * @return Bytewert des Zeichens
 * \exception OutOfBoundsEception Wird geworfen, wenn die angegebene Position \p pos
 * ausserhalb des Strings liegt oder der String leer ist.
 */
char String::operator[](ssize_t pos) const
{
    if (pos >= 0 && stringlen > (size_t)pos) return ((char*)ptr)[pos];
    if (pos < 0 && (size_t)(0 - pos) <= stringlen) return ((char*)ptr)[stringlen + pos];
    throw Exception("OutOfBoundsEception");
}

/*!\brief String auf STDOUT ausgeben
 *
 * Diese Funktion gibt den aktuellen String auf STDOUT aus. Dazu ist es notwendig den String vom internen
 * Unicode-Format in das Encoding des Betriebssystems umzurechnen. Da dieses von den lokalen Einstellungen
 * des Betriebssystems und des Users abhängig ist, wird die Environment-Variable "LANG" ausgewertet.
 * Ist diese nicht gesetzt oder enthält ein unbekanntes Encoding, wird der String immer in UTF-8 ausgegeben.
 *
 * \param withNewline Ein optionaler Parameter, der angibt, ob nach der Ausgabe ein Zeilenumbruch
 * angehangen werden soll (default=false)
 * \par Exceptions:
 * Keine
 *
 */
void String::print(bool withNewline) const throw()
{
    if (stringlen > 0) {
        if (withNewline)
            printf("%s\n", (char*)ptr);
        else
            printf("%s", (char*)ptr);
    } else if (withNewline) {
        printf("\n");
    }
}

/*!\brief String auf STDOUT mit abschließendem Zeilenumbruch ausgeben
 *
 * Diese Funktion gibt den aktuellen String mit abschließendem Zeilenumbruch auf STDOUT aus.
 * Dazu ist es notwendig den String vom internen
 * Unicode-Format in das Encoding des Betriebssystems umzurechnen. Da dieses von den lokalen Einstellungen
 * des Betriebssystems und des Users abhängig ist, wird die Environment-Variable "LANG" ausgewertet.
 * Ist diese nicht gesetzt oder enthält ein unbekanntes Encoding, wird der String immer in UTF-8 ausgegeben.
 *
 * \par Exceptions:
 * Keine
 *
 */
void String::printnl() const throw()
{
    print(true);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const char* str)
{
    return set(str);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const String& str)
{
    return set(str);
}

/*!\brief Zeichen übernehmen
 *
 * \desc
 * Mit diesem Operator wird ein einzelnes Zeichen in den String kopiert.
 *
 * @param[in] c Unicode Wert des zu übernehmenden Zeichens
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(char c)
{
    return set(c);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const char* str)
{
    return append(str);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const String& str)
{
    return append(str);
}

/*!\brief Zeichen anhängen
 *
 * \desc
 * Mit diesem Operator wird das angegebene Zeichen \p c an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] c Unicode-Wert des anzuhängenden Zeichens
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(char c)
{
    return append(c);
}

/*!\brief Führt einen Vergleich mit einem anderen String durch
 *
 * \desc
 * Führt einen Vergleich mit einem anderen String durch.
 *
 * \param str String, mit dem verglichen werden soll
 * \param size Optionaler Parameter, der die Anzahl zu berücksichtigender Zeichen innerhalb des
 * Strings \p str angibt. Wird er nicht angegeben, wird ein vergleich mit dem kompletten String
 * \p str durchgeführt.
 *
 * \return Ist der String innerhalb dieses Objekts kleiner als der mit \a str angegebene, wird ein
 * negativer Wert zurückgegeben, ist er größer, erfolgt ein positiver Return-Wert,
 * sind beide identisch, wird 0 zurückgegeben.
 *
 * \see strCaseCmp Vergleich zweier Strings unter Ignorierung der Gross-/Kleinschreibung
 */
int String::strcmp(const String& str, size_t size) const
{
    if (size != (size_t)-1) return ::strncmp(ptr, str.ptr, size);
    return ::strcmp(ptr, str.ptr);
}

int String::strcmp(const char* str, size_t size) const
{
    if (size != (size_t)-1) return ::strncmp(ptr, str, size);
    return ::strcmp(ptr, str);
}

/*!\brief Stringvergleich mit Ignorierung von Gross-/Kleinschreibung
 *
 * \desc
 * Führt einen Vergleich mit einem anderen String durch, unter Ignorierung der
 * Gross-/Kleinschreibung.
 *
 * \param str String, mit dem verglichen werden soll
 * \param size Optionaler Parameter, der die Anzahl zu berücksichtigender Zeichen innerhalb des
 * Strings \p str angibt. Wird er nicht angegeben, wird ein vergleich mit dem kompletten String
 * \p str durchgeführt.
 *
 *
 * \return Ist der String innerhalb dieses Objekts kleiner als der mit \a str angegebene, wird ein
 * negativer Wert zurückgegeben, ist er größer, erfolgt ein positiver Return-Wert,
 * sind beide identisch, wird 0 zurückgegeben.
 *
 * \see strcmp Vergleich zweier Strings unter Berücksichtigung der Gross-/Kleinschreibung
 */
int String::strCaseCmp(const String& str, size_t size) const
{
    if (size != (size_t)-1) return strncasecmp(ptr, str.ptr, size);
    return strcasecmp(ptr, str.ptr);
}

int String::strCaseCmp(const char* str, size_t size) const
{
    if (size != (size_t)-1) return strncasecmp(ptr, str, size);
    return strcasecmp(ptr, str);
}

/*!\brief Linken Teilstring zurückgeben
 *
 * \desc
 * Gibt die ersten \p len Zeichen des Strings als neuen zurück.
 *
 * @param len Länge des Teilstrings
 * @return Neuer String
 */
String String::left(size_t len) const
{
    if (stringlen > 0) {
        if (len > stringlen) len = stringlen;
        return String(ptr, len);
    }
    return String();
}

/*!\brief Rechten Teilstring zurückgeben
 *
 * \desc
 * Gibt die letzten \p len Zeichen des Strings als neuen zurück.
 *
 * @param len Länge des Teilstrings
 * @return Neuer String
 */
String String::right(size_t len) const
{
    if (stringlen > 0) {
        if (len > stringlen) len = stringlen;
        return String(ptr + stringlen - len, len);
    }
    return String();
}

/*!\brief Teilstring zurückgeben
 *
 * \desc
 * Gibt \p len Zeichen des Strings, beginnend ab Position \p start als
 * neuen String zurück.
 *
 * @param start Startposition
 * @param len Optionale Länge des Teilstrings. Ist der Parameter nicht angegeben, wird
 * der komplette String ab Position \p start zurückgegeben.
 * @return Neuer String
 */
String String::mid(size_t start, size_t len) const
{
    if (len == (size_t)-1) len = stringlen;
    if (start < stringlen && stringlen > 0 && len > 0) {
        if (start + len > stringlen) len = stringlen - start;
        return String(ptr + start, len);
    }
    return String();
}

/*!\brief Teilstring zurückgeben
 *
 * \desc
 * Gibt \p len Zeichen des Strings, beginnend ab Position \p start als
 * neuen String zurück.
 *
 * @param start Startposition
 * @param len Optionale Länge des Teilstrings. Ist der Parameter nicht angegeben, wird
 * der komplette String ab Position \p start zurückgegeben.
 * @return Neuer String
 */
String String::substr(size_t start, size_t len) const
{
    if (len == (size_t)-1) len = stringlen;
    if (start < stringlen && stringlen > 0 && len > 0) {
        if (start + len > stringlen) len = stringlen - start;
        return String(ptr + start, len);
    }
    return String();
}

//! \brief Schneidet Leerzeichen, Tabs, Returns und Linefeeds am Anfang und Ende des Strings ab
void String::trim()
{
    if (stringlen > 0) {
        size_t i, start, ende, s;
        start = 0;
        s = 0;
        ende = stringlen;
        for (i = 0; i < stringlen; i++) {
            if (ptr[i] == 13 || ptr[i] == 10 || ptr[i] == 32 || ptr[i] == '\t') {
                if (s == 0) start = i + 1;
            } else {
                s = 1;
                ende = i;
            }
        }
        ptr[ende + 1] = 0;
        if (start > 0) memmove(ptr, ptr + start, (ende - start + 2) * sizeof(char));
        stringlen = strlen(ptr);
        ptr[stringlen] = 0;
    }
}

/*!\brief Schneidet Leerzeichen, Tabs, Returns und Linefeeds am Anfang und Ende des Strings ab
 *
 * \desc
 * Es wird eine Kopie des Strings angelegt und bei dieser alle Leerzeichen, Tabs, Returns und
 * Linefeeds am Anfang und Ende des Strings abgeschnitten. Das Ergebnis wird als Returnwert
 * zurückgegeben. Der Original-String bleibt unverändert.
 */
String String::trimmed() const
{
    String ret(*this);
    ret.trim();
    return ret;
}

//! \brief Schneidet Leerzeichen, Tabs Returns und Linefeeds am Anfang des Strings ab
void String::trimLeft()
{
    if (stringlen > 0) {
        size_t i, start, s;
        start = 0;
        s = 0;
        // ende=stringlen;
        for (i = 0; i < stringlen; i++) {
            if (ptr[i] == 13 || ptr[i] == 10 || ptr[i] == 32 || ptr[i] == '\t') {
                if (s == 0) start = i + 1;
            } else {
                s = 1; // ende=i;
            }
        }
        if (start > 0) memmove(ptr, ptr + start, (stringlen - start + 1) * sizeof(char));
        stringlen = strlen(ptr);
        ptr[stringlen] = 0;
    }
}

//! \brief Schneidet Leerzeichen, Tabs Returns und Linefeeds am Ende des Strings ab
void String::trimRight()
{
    if (stringlen > 0) {
        size_t i, ende;
        ende = 0;
        for (i = stringlen; i > 0; i--) {
            char w = ptr[i - 1];
            if (w != 13 && w != 10 && w != 32 && w != '\t') {
                ende = i;
                break;
            }
        }
        ptr[ende] = 0;
        stringlen = strlen(ptr);
        ptr[stringlen] = 0;
    }
}

//! \brief Schneidet die definierten Zeichen am Anfang des Strings ab
void String::trimLeft(const String& chars)
{
    if (stringlen > 0 && chars.stringlen > 0) {
        size_t i, start, s, z;
        start = 0;
        s = 0;
        for (i = 0; i < stringlen; i++) {
            int match = 0;
            for (z = 0; z < chars.stringlen; z++) {
                if (ptr[i] == chars.ptr[z]) {
                    if (s == 0) start = i + 1;
                    match = 1;
                    break;
                }
            }
            if (!match) {
                s = 1;
            }
        }
        if (start > 0) {
            memmove(ptr, ptr + start, (stringlen - start + 1) * sizeof(char));
            stringlen = strlen(ptr);
        }
    }
}

//! \brief Schneidet die definierten Zeichen am Ende des Strings ab
void String::trimRight(const String& chars)
{
    if (stringlen > 0 && chars.stringlen > 0) {
        size_t i, ende, z;
        ende = 0;
        for (i = stringlen; i > 0; i--) {
            char w = ptr[i - 1];
            int match = 0;
            for (z = 0; z < chars.stringlen; z++) {
                if (w == chars.ptr[z]) {
                    // if (s==0) start=i+1;
                    match = 1;
                    break;
                }
            }
            if (!match) {
                ende = i;
                break;
            }
        }
        ptr[ende] = 0;
        stringlen = strlen(ptr);
    }
}

//! \brief Schneidet die definierten Zeichen am Anfang und Ende des Strings ab
void String::trim(const String& chars)
{
    trimLeft(chars);
    trimRight(chars);
}

/*!\brief Schneidet Zeichen am Ende des Strings ab
 *
 * \desc
 * Diese Funktion schneidet \p num Zeichen vom Ende des Strings ab. Falls \p num
 * größer als der String ist, bleibt ein leerer String zurück.
 *
 * @param num Anzahl Zeichen, die abgeschnitten werden sollen
 */
void String::chopRight(size_t num)
{
    if (stringlen > 0) {
        if (stringlen < num) num = stringlen;
        stringlen -= num;
        ptr[stringlen] = 0;
    }
}

/*!\brief Schneidet Zeichen am Ende des Strings ab
 *
 * \desc
 * Diese Funktion schneidet \p num Zeichen vom Ende des Strings ab. Falls \p num
 * größer als der String ist, bleibt ein leerer String zurück.
 *
 * @param num Anzahl Zeichen, die abgeschnitten werden sollen
 *
 * \see
 * Die Funktion ist identisch zu String::chopRight
 */
void String::chop(size_t num)
{
    if (stringlen > 0) {
        if (stringlen < num) num = stringlen;
        stringlen -= num;
        ptr[stringlen] = 0;
    }
}

/*!\brief Schneidet Zeichen am Anfang des Strings ab
 *
 * \desc
 * Diese Funktion schneidet \p num Zeichen vom Anfang des Strings ab. Falls \p num
 * größer als der String ist, bleibt ein leerer String zurück.
 *
 * @param num Anzahl Zeichen, die abgeschnitten werden sollen
 */
void String::chopLeft(size_t num)
{
    if (stringlen > 0) {
        if (stringlen < num) num = stringlen;
        memmove(ptr, ptr + num, (stringlen - num) * sizeof(char));
        stringlen -= num;
        ptr[stringlen] = 0;
    }
}

/*!\brief Schneidet Returns und Linefeeds am Anfanng und Ende des Strings ab
 *
 * \desc
 * Schneidet Returns und Linefeeds am Anfanng und Ende des Strings ab
 */
void String::chomp()
{
    trim(String("\n\r"));
}

/*!\brief Schneidet den String an einer bestimmten Stelle ab
 *
 * \desc
 * Der String wird an einer bestimmten Stelle einfach abgeschnitten
 * \param pos Die Position, an der der String abgeschnitten wird. Bei Angabe von 0 ist der String anschließend
 * komplett leer. Ist \c pos größer als die Länge des Strings, passiert nichts.
 */
void String::cut(size_t pos)
{
    if (stringlen == 0) return;
    if (pos > stringlen) return;
    ptr[pos] = 0;
    stringlen = pos;
}

/*! \brief Schneidet den String beim ersten Auftauchen eines Zeichens/Strings ab
 *
 * Der String wird beim ersten Auftauchen eines Zeichens oder eines Strings abgeschnitten.
 * \param[in] letter Buchstabe oder Buchstabenkombination, an der der String abgeschnitten werden
 * soll. Zeigt der Pointer auf NULL oder ist der String leer, passiert nichts.
 */
void String::cut(const String& letter)
{
    if (stringlen == 0) return;
    if (letter.isEmpty()) return;
    ssize_t p = instr(letter, 0);
    if (p >= 0) {
        ptr[p] = 0;
        stringlen = p;
    }
}

String String::strchr(char c) const
{
    String ret;
    if (stringlen > 0) {
        char* p = ::strchr(ptr, c);
        if (p) ret.set(p);
    }
    return ret;
}

String String::strrchr(char c) const
{
    String ret;
    if (stringlen > 0) {
        char* p = ::strrchr(ptr, c);
        if (p) ret.set(p);
    }
    return ret;
}

/*!\brief Teilstring finden
 *
 * \desc
 * Diese Funktion findet die erste Position der Zeichenfolge \p needle
 * innerhalb des Strings. Abschließende `\0'-Zeichen werden nicht
 * miteinander verglichen.
 *
 * @param needle
 * @return
 * Die Funktion gibt einen String zurück, der mit der gefundenen
 * Zeichenkette beginnt und den Rest des Strings bis zum Ende enthält.
 * Wurde die Zeichenkette nicht gefunden, wird ein leerer String
 * zurückgegeben.
 * \note
 * Ein Sonderfall besteht, wenn \p needle leer ist. In diesem Fall wird
 * der komplette String zurückgegeben.
 */
String String::strstr(const String& needle) const
{
    String ret;
    if (stringlen > 0) {
        if (needle.size() == 0) return *this;
        char* p = ::strstr(ptr, needle.ptr);
        if (p) ret.set(p);
    }
    return ret;
}

/*! \brief Sucht nach einem String
 *
 * Find sucht nach dem Suchstring \a needle ab der gewünschten Position \a start.
 *
 * \param[in] needle Gesuchter Teilstring
 * \param[in] start Optionale Startposition innerhalb des Suchstrings. Ist der Parameter 0 oder wird er weggelassen,
 * wird der String vom Anfang an durchsucht. Ist der Wert jedoch negativ, wird rückwärts vom
 * Ende des Strings gesucht.
 *
 * \return Liefert die Position innerhalb des Strings, an der der Suchstring gefunden wurde
 * oder -1 wenn er nicht gefunden wurde. Ist \p needle ein leerer String, liefert die
 * Funktion immer 0 zurück.
 */
ssize_t String::find(const String& needle, ssize_t start) const
{
    if (stringlen == 0) return -1;
    if (needle.stringlen == 0) return 0;
    if (start > 0 && (size_t)start >= stringlen) return -1;
    if (start < 0 && ((size_t)((ssize_t)stringlen + start)) >= stringlen) return -1;

    // Position to return
    size_t p = -1;
    // Length of the string to search for
    size_t lstr = needle.stringlen;
    // Current position to search from and position of found string
    char *found = NULL, *tmp = NULL;

    // Search forward
    if (start >= 0) {
        // Search first occurence, starting at the given position...
        found = ::strstr(ptr + start, needle.ptr);
        //...and calculate the position to return if str was found
        if (found != NULL) {
            p = found - ptr;
        }
    } else {
        // Search backward
        // Start counting from behind at 0, not -1
        start++;

        /* Beginning at the start of the contained string, start searching for
               every occurence of the str and make it the position last found as long
               as the found string doesn't exceed the defined end of the search */
        while ((found = ::strstr((tmp == NULL ? ptr : tmp + 1), needle.ptr)) != NULL && found - ptr + lstr <= stringlen + start)
            tmp = found;

        // Calculate the position to return if str was found
        if (tmp != NULL) {
            p = tmp - ptr;
        }
    }
    return p;
}

/*! \brief Sucht nach einem String
 *
 * \desc
 * Diese Funktion sucht nach dem Suchstring \a needle ab der gewünschten Position \a start.
 *
 * \param[in] needle Gesuchter Teilstring
 * \param[in] start Optionale Startposition innerhalb des Suchstrings. Ist der Parameter 0
 * oder nicht angegeben, wird der String vom Anfang an gesucht.
 *
 * \return Liefert die Position innerhalb des Strings, an der der Suchstring gefunden wurde
 * oder -1 wenn er nicht gefunden wurde. Ist \p needle ein leerer String, liefert die
 * Funktion immer 0 zurück.
 */
ssize_t String::instr(const String& needle, size_t start) const
{
    if (stringlen == 0) return -1;
    if (needle.stringlen == 0) return 0;
    if (start >= stringlen) return -1;
    const char* p;
    p = ::strstr((ptr + start), needle.ptr);
    if (p != NULL) {
        return ((ssize_t)(p - ptr));
    }
    return -1;
}

bool String::has(const String& needle) const
{
    if (stringlen == 0) return false;
    if (needle.stringlen == 0) return false;
    const char* p;
    p = ::strstr(ptr, needle.ptr);
    if (p != NULL) return true;
    return false;
}

/*!\brief String wiederholen
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des Strings mehrfach wiederholt.
 *
 * @param num Anzahl Wiederholungen. Falls \p num 0 ist, ist der String anschließend leer.
 *
 * @return Referenz auf den verlängerten String.
 */
String& String::repeat(size_t num)
{
    if (stringlen == 0) return *this;
    if (num == 0) {
        clear();
        return *this;
    }
    size_t newsize = (stringlen * num + 16) * sizeof(char);
    char* buf = (char*)malloc(newsize);
    if (!buf) throw Exception("OutOfMemoryException");
    char* tmp = buf;
    for (size_t i = 0; i < num; i++) {
#ifdef HAVE_STRNCPY_S
        strncpy_s(tmp, newsize, ptr, stringlen);
#else
        strncpy(tmp, ptr, stringlen);
#endif
        tmp += stringlen;
    }
    free(ptr);
    ptr = buf;
    stringlen = stringlen * num;
    ptr[stringlen] = 0;
    s = newsize;
    return *this;
}

/*! \brief Füllt den String mit einem Zeichen
 *
 * Der String wird mit einem gewünschten Zeichen gefüllt
 * \param unicode Der Unicode des Zeichens, mit dem der String gefüllt werden soll
 * \param num Die Länge des gewünschten Strings
 * \return Referenz auf den neuen String
 */
String& String::repeat(char code, size_t num)
{
    if (!code) {
        throw Exception("IllegalArgumentException");
    }
    if (!num) {
        clear();
        return *this;
    }
    size_t newsize = (num + 16) * sizeof(char);
    char* buf = (char*)malloc(newsize);
    if (!buf) throw Exception("OutOfMemoryException");
    for (size_t i = 0; i < num; i++)
        buf[i] = code;
    if (ptr != empty_string) free(ptr);
    ptr = buf;
    stringlen = num;
    ptr[stringlen] = 0;
    s = newsize;
    return *this;
}

/*!\brief String wiederholen
 *
 * \desc
 * Mit dieser Funktion wird der übergebene String \p str \p num mal wiederholt und
 * das Ergebnis in diesem String gespeichert.
 *
 * @param str Der zu wiederholende String
 * @param num Anzahl wiederholungen
 * @return Referenz auf den String
 */
String& String::repeat(const String& str, size_t num)
{
    if (str.stringlen == 0 || num == 0) {
        clear();
        return *this;
    }
    size_t newsize = (str.stringlen * num + 16) * sizeof(char);
    char* buf = (char*)malloc(newsize);
    if (!buf) throw Exception("OutOfMemoryException");
    char* tmp = buf;
#ifdef HAVE_STRNCPY_S
    size_t buffer_left = newsize;
#endif
    for (size_t i = 0; i < num; i++) {
#ifdef HAVE_STRNCPY_S
        strncpy_s((char*)tmp, buffer_left, str.ptr, str.stringlen);
        buffer_left -= str.stringlen;
#else
        strncpy(tmp, str.ptr, str.stringlen);
#endif
        tmp += str.stringlen;
    }
    if (ptr != empty_string) free(ptr);
    ptr = buf;
    stringlen = num;
    ptr[stringlen] = 0;
    s = newsize;
    return *this;
}

/*!\brief String multiplizieren
 *
 * \desc
 * Der aktuelle String wird \p count mal hintereinander wiederholt und
 * als neuer String zurückgegeben.
 *
 * @param[in] count Anzahl wiederholungen
 * @return Neuer String
 * \exception OutOfMemoryException Tritt auf, wenn kein Speicher mehr verfügbar ist.
 */
String String::repeated(size_t count) const
{
    String ret;
    for (size_t i = 0; i < count; i++)
        ret.append(ptr, stringlen);
    return ret;
}

String& String::replace(const String& search, const String& replacement)
//! \brief Ersetzt einen Teilstring durch einen anderen
{
    if (stringlen == 0 || search.stringlen == 0) return *this;
    size_t start = 0, slen = search.stringlen;
    ssize_t end;
    // collect the result
    String ms;
    // Do while str is found in the contained string
    while ((end = find(search, start)) >= 0) {
        // The result is built from the parts that don't match str and the replacement string
        ms += mid(start, end - start);
        ms += replacement;
        // New start for search is behind the replaced part
        start = end + slen;
    }
    // Add the remaining part of the contained string to the result
    ms += mid(start);
    // The result is assigned to this mstring
    return set(ms);
}

/*! \brief Schiebt den String nach links
 *
 * Der String wird um die mit \c size angegebenen Zeichen nach links verschoben und rechts mit dem durch \c c
 * angegebenen Zeichen aufgefüllt.
 * \param c Das Zeichen, mit dem der String auf der rechten Seite aufgefüllt werden soll. Wird der Wert 0 verwendet,
 * findet keine Auffüllung statt, d.h. der String verkürzt sich einfach.
 * \param size Die Anzahl Zeichen, um die der String nach links verschoben werden soll. Ist \c size größer als die
 * Länge des Strings, wird der String komplett geleert und ist anschließend so groß wie size, sofern c>0 war.
 */
void String::shl(char c, size_t size)
{
    if (!stringlen) return;
    if (!size) return;
    if (size > stringlen) size = stringlen;
    String t = mid(size);
    if (c) {
        String a;
        a.repeat(c, size);
        t += a;
    }
    set(t);
}

/*! \brief Schiebt den String nach rechts
 *
 * Der String wird um die mit \c size angegebenen Zeichen nach rechts verschoben und links mit dem durch \c c
 * angegebenen Zeichen aufgefüllt.
 * \param c Das Zeichen, mit dem der String auf der linken Seite aufgefüllt werden soll. \c c muß größer 0 sein.
 * \param size Die Anzahl Zeichen, um die der String nach rechts verschoben werden soll. Ist \c size größer als die
 * Länge des Strings, wird der String komplett geleert und ist anschließend so groß wie size.
 */
void String::shr(char c, size_t size)
{
    if (!stringlen) return;
    if (!size) return;
    if (!c) return;
    if (size > stringlen) size = stringlen;
    String t;
    t.repeat(c, size);
    t += left(stringlen - size);
    t.cut(size);
    set(t);
}

/*!\brief Kleiner als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner dem des
 * rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<(const String& str) const
{
    if (::strcmp(ptr, str.ptr) < 0) return true;
    return false;
}

/*!\brief Kleiner oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner oder gleich
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<=(const String& str) const
{
    if (strcmp(str) <= 0) return true;
    return false;
}

/*!\brief Gleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters mit dem des
 * rechten identisch ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator==(const String& str) const
{
    if (strcmp(str) == 0) return true;
    return false;
}

/*!\brief Ungleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters nicht dem des
 * rechten entspricht.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator!=(const String& str) const
{
    if (strcmp(str) == 0) return false;
    return true;
}

/*!\brief Größer oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer oder
 * gleich dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>=(const String& str) const
{
    if (strcmp(str) >= 0) return true;
    return false;
}

/*!\brief Größer als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>(const String& str) const
{
    if (strcmp(str) > 0) return true;
    return false;
}

/*!\brief Kleiner als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner dem des
 * rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<(const char* str) const
{
    if (strcmp(str) < 0) return true;
    return false;
}

/*!\brief Kleiner oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner oder gleich
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<=(const char* str) const
{
    if (strcmp(str) <= 0) return true;
    return false;
}

/*!\brief Gleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters mit dem des
 * rechten identisch ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator==(const char* str) const
{
    if (strcmp(str) == 0) return true;
    return false;
}

/*!\brief Ungleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters nicht dem des
 * rechten entspricht.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator!=(const char* str) const
{
    if (strcmp(str) == 0) return false;
    return true;
}

/*!\brief Größer oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer oder
 * gleich dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>=(const char* str) const
{
    if (strcmp(str) >= 0) return true;
    return false;
}

/*!\brief Größer als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>(const char* str) const
{
    if (strcmp(str) > 0) return true;
    return false;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \desc
 * Diese Funktion liefert einen %Pointer im Format "const char*" auf den internen
 * C-String der Klasse zurück. Falls der %String leer ist, wird ein
 * %Pointer auf einen leeren %String zurückgegeben. Das Ergebnis kann in \b printf und
 * verwandten Funktionen mit dem Formatstring "%s" verwendet werden.
 *
 * @return %Pointer auf den internen C-String der Klasse
 * \example
 * \code
void PrintString(const ppl7::String &text)
{
    printf ("Der String lautet: %s\n",text.getPtr());
    // oder mittels Operator:
    printf ("Der String lautet: %s\n",(const char*)text);
}
 * \endcode
 * \see
 * Die folgenden Funktionen erfüllen den gleichen Zweck:
 * - const char * String::getPtr() const
 * - const char * String::c_str() const
 * - const char * String::toChar() const
 * - String::operator const char *() const
 */
const char* String::getPtr() const
{
    return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
const char* String::c_str() const
{
    return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
const char* String::toChar() const
{
    return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
String::operator const char*() const
{
    return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
String::operator const unsigned char*() const
{
    return (const unsigned char*)ptr;
}

String::operator bool() const
{
    if (isTrue()) return true;
    return false;
}

String::operator int() const
{
    if (!stringlen) return 0;
    return strtol(ptr, NULL, 0);
}

String::operator unsigned int() const
{
    if (!stringlen) return 0;
    return strtoul(ptr, NULL, 0);
}

String::operator long() const
{
    return toLong();
}

String::operator unsigned long() const
{
    return toUnsignedLong();
}

String::operator long long() const
{
    return toLongLong();
}

String::operator unsigned long long() const
{
    return toUnsignedLongLong();
}

String::operator float() const
{
    return toFloat();
}

String::operator double() const
{
    return toDouble();
}

int String::toInt() const
{
    if (!stringlen) return 0;
    return strtol(ptr, NULL, 10);
}

unsigned int String::toUnsignedInt() const
{
    if (!stringlen) return 0;
    return strtoul(ptr, NULL, 10);
}

int64_t String::toInt64() const
{
    if (!stringlen) return 0;
#ifdef HAVE_STRTOLL
    return (int64_t)strtoll(ptr, NULL, 10);
#elif defined WIN32
    return (int64_t)_strtoi64(ptr, NULL, 10);
#else
    throw Exception("TypeConversionException");
#endif
}

uint64_t String::toUnsignedInt64() const
{
    if (!stringlen) return 0;
#ifdef HAVE_STRTOULL
    return (uint64_t)strtoull(ptr, NULL, 10);
#elif defined HAVE_STRTOLL
    return (uint64_t)strtoll(ptr, NULL, 10);
#elif defined WIN32
    return (uint64_t)_strtoi64(ptr, NULL, 10);
#else
    throw Exception("TypeConversionException");
#endif
}

bool String::toBool() const
{
    if (isTrue()) return true;
    return false;
}

long String::toLong() const
{
    if (!stringlen) return 0;
    return strtol(ptr, NULL, 10);
}

unsigned long String::toUnsignedLong() const
{
    if (!stringlen) return 0;
    return strtoul(ptr, NULL, 10);
}

long long String::toLongLong() const
{
    if (!stringlen) return 0;
#ifdef HAVE_STRTOLL
    return (long long)strtoll(ptr, NULL, 10);
#elif defined WIN32
    return (long long)_strtoi64(ptr, NULL, 10);
#else
    throw Exception("TypeConversionException");
#endif
}

unsigned long long String::toUnsignedLongLong() const
{
    if (!stringlen) return 0;
#ifdef HAVE_STRTOULL
    return (unsigned long long)strtoull(ptr, NULL, 10);
#elif defined HAVE_STRTOLL
    return (unsigned long long)strtoll(ptr, NULL, 10);
#elif defined WIN32
    return (unsigned long long)_strtoi64(ptr, NULL, 10);
#else
    throw Exception("TypeConversionException");
#endif
}

float String::toFloat() const
{
    if (!stringlen) return 0.0f;
    return (float)atof(ptr);
}

double String::toDouble() const
{
    if (!stringlen) return 0.0;
    return atof(ptr);
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String& str1, const String& str2)
{
    String s(str1);
    s.append(str2);
    return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const char* str1, const String& str2)
{
    String s(str1);
    s.append(str2);
    return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String& str1, const char* str2)
{
    String s(str1);
    s.append(str2);
    return s;
}

} // namespace picopplib