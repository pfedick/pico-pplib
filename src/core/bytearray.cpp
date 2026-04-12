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

// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "picopplib.h"

namespace picopplib
{

/*!\brief Konstruktor ohne Argumente
 *
 * \desc
 * Dieser Konstruktor erstellt eine Inszanz der Klasse ohne einen zugeweisenen Speicherbereich
 *
 */
ByteArray::ByteArray()
{
    ptradr = NULL;
    ptrsize = 0;
}

/*!\brief Destruktor der Klasse
 *
 * \desc
 * Der Destruktor sort dafür, dass der allokierte Speicher mittes "free" wieder freigegeben wird.
 *
 */
ByteArray::~ByteArray()
{
    ::free(ptradr);
    ptradr = NULL;
}

/*!\brief Copy-Konstruktor
 *
 * \desc
 * Mit diesem Konstruktor wird der Speicherbereich eines anderen ByteArray- oder ByteArrayPtr-
 * Objekts kopiert.
 *
 * @param[in] other Referenz auf eine andere ByteArray- oder ByteArrayPtr-Klasse
 */
ByteArray::ByteArray(const ByteArrayPtr& other)
{
    if (other.ptradr) {
        ptradr = ::malloc(other.ptrsize + 1);
        if (!ptradr) throw Exception("OutOfMemoryException");
        memcpy(ptradr, other.ptradr, other.ptrsize);
        ptrsize = other.ptrsize;
        ((char*)ptradr)[ptrsize] = 0;
    } else {
        ptradr = NULL;
        ptrsize = 0;
    }
}

/*!\brief Konstruktor durch String
 *
 * \desc
 * Mit diesem Konstruktor wird der Speicherbereich eines Strings in das Objekt kopiert.
 *
 * @param[in] other Referenz auf eine String-Klasse
 */
ByteArray::ByteArray(const String& str)
{
    if (str.notEmpty()) {
        ptradr = ::malloc(str.size() + 1);
        if (!ptradr) throw Exception("OutOfMemoryException");
        memcpy(ptradr, str.getPtr(), str.size());
        ptrsize = str.size();
        ((char*)ptradr)[ptrsize] = 0;
    } else {
        ptradr = NULL;
        ptrsize = 0;
    }
}

/*!\brief Copy-Konstruktor
 *
 * \desc
 * Mit diesem Konstruktor wird der Speicherbereich eines anderen ByteArray- oder ByteArrayPtr-
 * Objekts kopiert.
 *
 * @param[in] other Referenz auf eine andere ByteArray- oder ByteArrayPtr-Klasse
 */
ByteArray::ByteArray(const ByteArray& other)
{
    if (other.ptradr) {
        ptradr = ::malloc(other.ptrsize + 1);
        if (!ptradr) throw Exception("OutOfMemoryException");
        memcpy(ptradr, other.ptradr, other.ptrsize);
        ptrsize = other.ptrsize;
        ((char*)ptradr)[ptrsize] = 0;
    } else {
        ptradr = NULL;
        ptrsize = 0;
    }
}

/*!\brief Konstruktor mit Angabe einer Speicheradresse und Größe
 *
 * \desc
 * Mit diesem Konstruktor wird eine neue Instanz der Klasse erzeugt, und der
 * mit \p adr und \p size angegebenen Speicherbereich kopiert.
 *
 * @param[in] adr Pointer auf den Beginn des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 */
ByteArray::ByteArray(const void* adr, size_t size)
{
    ptradr = NULL;
    ptrsize = 0;
    copy(adr, size);
}

/*!\brief Konstruktor mit Speicher-Allokierung
 *
 * \desc
 * Mit diesem Konstruktor wird eine Instanz der Klasse erzeugt, die einen Speicherblock mit
 * der Größe \p size allokiert und diesen selbst verwaltet.
 *
 * @param[in] size Gewünschte Größe des Speicherblocks in Bytes
 * \exception OutOfMemoryException Diese Exception wird geworfen, wenn der Speicher nicht allokiert werden konnte
 */
ByteArray::ByteArray(size_t size)
{
    ptrsize = 0;
    ptradr = ::malloc(size + 1);
    if (!ptradr) throw Exception("OutOfMemoryException");
    ptrsize = size;
    ((char*)ptradr)[ptrsize] = 0;
}

/*!\brief Speicherverwaltung übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Klasse die Verwaltung des Speicherbereich mit der Adresse \p adr und der
 * Größe \p size übergeben. Der Speicher muss zuvor mit "malloc" bzw. "calloc" allokiert worden sein
 * und darf von der Anwendung selbst nicht mehr freigegeben werden.
 *
 * @param[in] adr Startadresse des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 *
 * \note
 * Falls die Klasse vor Aufruf bereits Speicher verwaltet hat, wird dieser zuerst freigegeben.
 *
 * \attention
 * Bei Verwendung dieser Funktion wird kein 0-Byte am Ende des Speicherbereichs hinzugefügt.
 * Fall das Objekt als Eingabe für eine C-String-Funktion verwendet werden soll, muss die
 * Anwendung selbst dafür sorgen, dass am Ende ein 0-Byte vorhanden ist.
 */
void ByteArray::useadr(void* adr, size_t size)
{
    ::free(ptradr);
    ptradr = adr;
    ptrsize = size;
}

/*!\brief Speicherbereich kopieren
 *
 * \desc
 * Mit dieser Funktion wird der durch \p adr und \p size angegebene Speicherbereich in
 * die ByteArray-Klasse kopiert. Die Klasse allokiert dafür zunächst den erforderlichen
 * Speicher und übernimmt dessen Verwaltung.
 *
 * @param[in] adr
 * @param[in] size
 * \return Bei erfolgreichem Kopieren liefert die Funktion einen Pointer auf den
 * neu allokierten Speicherbereich zurück. Im Fehlerfall wird eine Exception geworfen.
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 * \note
 * Falls die Klasse vor Aufruf bereits Speicher verwaltet hat, wird dieser zuerst freigegeben.
 */
void* ByteArray::copy(const void* adr, size_t size)
{
    ::free(ptradr);
    ptrsize = 0;
    ptradr = NULL;
    if (adr != NULL && size > 0) {
        ptradr = ::malloc(size + 4);
        if (!ptradr) {
            throw Exception("OutOfMemoryException");
        }
        if (memcpy(ptradr, adr, size) != ptradr) {
            ::free(ptradr);
            ptradr = NULL;
            ptrsize = 0;
            throw Exception("Exception");
        }
        ptrsize = size;
        ((char*)ptradr)[ptrsize] = 0;
        ((char*)ptradr)[ptrsize + 1] = 0;
        ((char*)ptradr)[ptrsize + 2] = 0;
        ((char*)ptradr)[ptrsize + 3] = 0;
    }
    return ptradr;
}

/*!\brief Speicherbereich aus einem anderen ByteArray-Objekt kopieren
 *
 * \desc
 * Mit dieser Funktion wird der durch \p other referenzierte Speicherbereich
 * in diese Instanz der ByteArray-Klasse kopiert. Die Klasse allokiert dafür zunächst
 * den dafür erforderlichen Speicher und übernimmt dessen Verwaltung.
 *
 * @param[in] other Referenz auf das ByteArray- oder ByteArrayPtr-Objekt, von dem der
 * Speicher kopiert werden soll.
 * \return Bei erfolgreichem Kopieren liefert die Funktion einen Pointer auf den
 * neu allokierten Speicherbereich zurück. Im Fehlerfall wird eine Exception
 * geworfen.
 *
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 *
 */
void* ByteArray::copy(const ByteArrayPtr& other)
{
    return copy(other.ptradr, other.ptrsize);
}

/*!\brief Speicherbereich anhängen
 *
 * \desc
 * Mit dieser Funktion wird der durch \p adr und \p size angegebene Speicherbereich an die
 * bereits in der Klasse vorhandenen Daten angehangen. Die Klasse allokiert dafür zunächst
 * den erforderlichen Speicher und kopiert dann die Daten.
 *
 * @param[in] adr
 * @param[in] size
 * \return Bei erfolgreichem Kopieren liefert die Funktion einen Pointer auf den
 * neu allokierten Speicherbereich zurück. Im Fehlerfall wird eine Exception geworfen.
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 * \note
 * Falls die Klasse vor Aufruf bereits Speicher verwaltet hat, wird dieser zuerst freigegeben.
 */
void* ByteArray::append(void* adr, size_t size)
{
    if (adr == NULL || size == 0) {
        throw Exception("NullPointerException");
    }

    if (!ptradr) return copy(adr, size);
    size_t newsize = ptrsize + size;
    void* p = ::realloc(ptradr, newsize + 4);
    if (!p) throw Exception("OutOfMemoryException");
    ptradr = p;
    void* target = (char*)ptradr + ptrsize;
    if (memcpy(target, adr, size) != target) {
        ::free(ptradr);
        ptradr = NULL;
        ptrsize = 0;
        throw Exception("Exception");
    }
    ptrsize = newsize;
    ((char*)ptradr)[ptrsize] = 0;
    ((char*)ptradr)[ptrsize + 1] = 0;
    ((char*)ptradr)[ptrsize + 2] = 0;
    ((char*)ptradr)[ptrsize + 3] = 0;
    return ptradr;
}

/*!\brief Speicherbereich aus einem ByteArray-Objekt anhängen
 *
 * \desc
 * Mit dieser Funktion wird der durch \p other referenzierte Speicherbereich
 * an die Daten dieser Instanz der ByteArray-Klasse angehangen. Dazu wird
 * Speicher reallokiert und kopiert.
 *
 * @param[in] other Referenz auf das ByteArray- oder ByteArrayPtr-Objekt, von dem der
 * Speicher kopiert werden soll.
 * \return Bei erfolgreichem Kopieren liefert die Funktion einen Pointer auf den
 * neu allokierten Speicherbereich zurück. Im Fehlerfall wird eine Exception
 * geworfen.
 *
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 *
 */
void* ByteArray::append(const ByteArrayPtr& other)
{
    return append(other.ptradr, other.ptrsize);
}

/*!\brief Speicherbereich davor hängen
 *
 * \desc
 * Mit dieser Funktion wird der durch \p adr und \p size angegebene Speicherbereich vor die
 * bereits in der Klasse vorhandenen Daten gehangen. Die Klasse allokiert dafür zunächst
 * den erforderlichen Speicher und kopiert dann die Daten.
 *
 * @param[in] adr
 * @param[in] size
 * \return Bei erfolgreichem Kopieren liefert die Funktion einen Pointer auf den
 * neu allokierten Speicherbereich zurück. Im Fehlerfall wird eine Exception geworfen.
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 * \note
 * Falls die Klasse vor Aufruf bereits Speicher verwaltet hat, wird dieser zuerst freigegeben.
 */
void* ByteArray::prepend(void* adr, size_t size)
{
    if (adr == NULL || size == 0) {
        throw Exception("NullPointerException");
    }

    if (!ptradr) return copy(adr, size);
    size_t newsize = ptrsize + size;
    void* p = ::malloc(newsize + 4);
    if (!p) throw Exception("OutOfMemoryException");
    if (memcpy(p, adr, size) != p) {
        ::free(ptradr);
        ptradr = NULL;
        ptrsize = 0;
        ::free(p);
        throw Exception("Exception");
    }
    void* target = (char*)p + size;
    if (memcpy(target, ptradr, ptrsize) != target) {
        ::free(ptradr);
        ptradr = NULL;
        ptrsize = 0;
        ::free(p);
        throw Exception("Exception");
    }
    ::free(ptradr);
    ptradr = p;
    ptrsize = newsize;
    ((char*)ptradr)[ptrsize] = 0;
    ((char*)ptradr)[ptrsize + 1] = 0;
    ((char*)ptradr)[ptrsize + 2] = 0;
    ((char*)ptradr)[ptrsize + 3] = 0;

    return ptradr;
}

/*!\brief Speicherbereich aus einem ByteArray-Objekt davorhängen
 *
 * \desc
 * Mit dieser Funktion wird der durch \p other referenzierte Speicherbereich
 * vor die Daten dieser Instanz der ByteArray-Klasse gehangen. Dazu wird
 * Speicher reallokiert und kopiert.
 *
 * @param[in] other Referenz auf das ByteArray- oder ByteArrayPtr-Objekt, von dem der
 * Speicher kopiert werden soll.
 * \return Bei erfolgreichem Kopieren liefert die Funktion einen Pointer auf den
 * neu allokierten Speicherbereich zurück. Im Fehlerfall wird eine Exception
 * geworfen.
 *
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 *
 */
void* ByteArray::prepend(const ByteArrayPtr& other)
{
    return prepend(other.ptradr, other.ptrsize);
}

/*!\brief Speicherreferenz von anderem ByteArray-Objekt kopieren
 *
 * \desc
 * Mit diesem Operator wird der Speicherbereich eines anderen ByteArray- oder ByteArrayPtr-Objekts
 * kopiert. Er ist identisch zur Funktion ByteArray::copy.
 *
 * @param[in] other Referenz auf ein anderes ByteArray- oder ByteArrayPtr-Objekt.
 * @return Referenz auf das Objekt
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 *
 */
ByteArray& ByteArray::operator=(const ByteArrayPtr& other)
{
    copy(other.ptradr, other.ptrsize);
    return *this;
}

/*!\brief Speicherreferenz von anderem ByteArray-Objekt kopieren
 *
 * \desc
 * Mit diesem Operator wird der Speicherbereich eines anderen ByteArray- oder ByteArrayPtr-Objekts
 * kopiert. Er ist identisch zur Funktion ByteArray::copy.
 *
 * @param[in] other Referenz auf ein anderes ByteArray- oder ByteArrayPtr-Objekt.
 * @return Referenz auf das Objekt
 * \exception OutOfMemoryException Speicher konnte nicht allokiert werden
 * \exception NullPointerException Ein übergebener Parameter war 0
 * \exception Exception Speicherbereich konnte nicht kopiert werden
 *
 */
ByteArray& ByteArray::operator=(const ByteArray& other)
{
    copy(other.ptradr, other.ptrsize);
    return *this;
}

ByteArray& ByteArray::operator=(const String& str)
{
    clear();
    if (str.notEmpty()) {
        ptradr = ::malloc(str.size() + 1);
        if (!ptradr) throw Exception("OutOfMemoryException");
        memcpy(ptradr, str.getPtr(), str.size());
        ptrsize = str.size();
        ((char*)ptradr)[ptrsize] = 0;
    }
    return *this;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen.
 *
 * @return Adresse des Speicherblocks
 * \attention
 * Die Adresse kann NULL sein, wenn kein Speicherblock allokiert ist!
 */
ByteArray::operator const void*() const
{
    return ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen.
 *
 * @return Adresse des Speicherblocks
 * \attention
 * Die Adresse kann NULL sein, wenn kein Speicherblock allokiert ist!
 */
ByteArray::operator const unsigned char*() const
{
    return (const unsigned char*)ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen.
 *
 * @return Adresse des Speicherblocks
 * \attention
 * Die Adresse kann NULL sein, wenn kein Speicherblock allokiert ist!
 */
ByteArray::operator const char*() const
{
    return (const char*)ptradr;
}

/*!\brief Einzelnes Byte aus dem Speicherbereich kopieren
 *
 * \desc
 * Mit dem Operator [] kann ein bestimmtes Byte \p pos aus dem Speicherbereich
 * ausgelesen werden. Ist kein Speicher referenziert oder ist \p pos größer als
 * der Speicherblock, wird eine Exception ausgelöst.
 *
 * @param [in] pos Auszulesendes Byte, beginnend mit 0.
 * @return Wert der Speicherstelle
 * \exception OutOfBoundsEception Diese Exception wird geworfen, wenn die mit
 * \p pos angegebene Speicherstelle ausseralb des referenzierten Speichers liegt oder
 * kein Speicher referenziert ist.
 */
unsigned char ByteArray::operator[](size_t pos) const
{
    if (ptradr != NULL && pos < ptrsize) return ((unsigned char*)ptradr)[pos];
    throw Exception("OutOfBoundsEception");
}

/*!\brief Speicher allokieren
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock mit der Größe von \p size Bytes allokiert
 * und dessen Adresse als Ergebnis zurückgeliefert. Das ByteArray-Objekt übernimmt die Verwaltung
 * des Speicherblocks.
 *
 * @param[in] size Gewünschte Größe des Speicherblocks.
 * \return
 * Pointer auf den Beginn des allokierten Speichers, oder NULL, wenn ein Fehler beim
 * Allokieren aufgetreten ist. Ein entsprechender Fehlercode wird gesetzt.
 */
void* ByteArray::malloc(size_t size)
{
    ::free(ptradr);
    ptradr = ::malloc(size + 1);
    if (ptradr) {
        ptrsize = size;
    } else {
        throw Exception("OutOfMemoryException");
    }
    ((char*)ptradr)[ptrsize] = 0;
    return ptradr;
}

/*!\brief Speicher allokieren und mit 0 initialisieren
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock mit der Größe von \p size Bytes allokiert,
 * der Inhalt mit 0 initialisiert
 * und dessen Adresse als Ergebnis zurückgeliefert. Das ByteArray-Objekt übernimmt die Verwaltung
 * des Speicherblocks.
 *
 * @param[in] size Gewünschte Größe des Speicherblocks.
 * \return
 * Pointer auf den Beginn des allokierten Speichers, oder NULL, wenn ein Fehler beim
 * Allokieren aufgetreten ist. Ein entsprechender Fehlercode wird gesetzt.
 */
void* ByteArray::calloc(size_t size)
{
    ::free(ptradr);
    ptradr = ::calloc(size + 1, 1);
    if (ptradr) {
        ptrsize = size;
    } else {
        throw Exception("OutOfMemoryException");
    }
    return ptradr;
}

/*!\brief Speicher freigeben
 *
 * \desc
 * Wird der Speicher vom ByteArray-Objekt verwaltet, wird dieser durch Aufruf dieser Funktion
 * wieder freigegeben. Bei nichtverwaltetem Speicher wird lediglich die interne Referenz auf
 * NULL gesetzt, aber der Speicher nicht freigegeben.
 */
void ByteArray::free()
{
    ::free(ptradr);
    ptradr = NULL;
    ptrsize = 0;
}

/*!\brief Speicher freigeben
 *
 * \desc
 * Wird der Speicher vom ByteArray-Objekt verwaltet, wird dieser durch Aufruf dieser Funktion
 * wieder freigegeben. Bei nichtverwaltetem Speicher wird lediglich die interne Referenz auf
 * NULL gesetzt, aber der Speicher nicht freigegeben.
 */
void ByteArray::clear()
{
    ::free(ptradr);
    ptradr = NULL;
    ptrsize = 0;
}

} // namespace picopplib
