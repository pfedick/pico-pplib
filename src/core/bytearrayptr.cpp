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

ByteArrayPtr::ByteArrayPtr()
{
    ptradr = NULL;
    ptrsize = 0;
}

/*!\brief Copy-Konstruktor
 *
 * Mit diesem Konstruktor wird eine Referenz auf den Speicherbereich einer anderen
 * ByteArrayPtr- oder ByteArray-Klasse übernommen.
 *
 * @param[in] other Referenz auf eine andere ByteArrayReferenz-Klasse
 */
ByteArrayPtr::ByteArrayPtr(const ByteArrayPtr& other)
{
    ptradr = other.ptradr;
    ptrsize = other.ptrsize;
}

ByteArrayPtr::ByteArrayPtr(const String& data)
{
    ptradr = (void*)data.getPtr();
    ptrsize = data.size();
}

/*!\brief Konstruktor mit Angabe einer Speicheradresse und Größe
 *
 * Mit diesem Konstruktor wird eine neue Instanz der Klasse erzeugt, die eine Referenz auf den mit
 * \p adr und \p size angegebenen Speicherbereich enthält.
 *
 * @param[in] adr Pointer auf den Beginn des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 */
ByteArrayPtr::ByteArrayPtr(void* adr, size_t size)
{
    ptradr = adr;
    ptrsize = size;
}

ByteArrayPtr::ByteArrayPtr(const void* adr, size_t size)
{
    ptradr = (void*)adr;
    ptrsize = size;
}

/*!\brief Prüfen, ob Speicher referenziert ist
 *
 * Mit dieser Funktion kann geprüft werden, ob die Klasse zur Zeit eine Referenz auf einen
 * Speicherbereich enthält.
 *
 * \return
 * Enthält die Klasse keine Referenz auf einen Speicherbereich, liefert die Funktion
 * \c true zurück, andernfalls \c false.
 */
bool ByteArrayPtr::isNull() const
{
    if (!ptradr) return true;
    return false;
}

/*!\brief Prüfen, ob der Referenzierte Speicher eine Größe von 0 hat
 *
 * Mit dieser Funktion kann geprüft werden, ob die Klasse zur Zeit eine Referenz auf einen
 * Speicherbereich enthält und dieser größer als 0 Byte ist.
 *
 * \return
 * Enthält die Klasse keine Referenz auf einen Speicherbereich, der mindestens 1 Byte
 * gross ist, liefert die Funktion \c true zurück, andernfalls \c false.
 */
bool ByteArrayPtr::isEmpty() const
{
    if (!ptradr) return true;
    if (!ptrsize) return true;
    return false;
}

/*!\brief Größe des Speicherblocks auslesen
 *
 * Mit dieser Funktion kann die Größe des Speicherblocks ausgelesen werden.
 *
 * @return Größe des Speicherblocks oder 0, wenn kein Speicher zugeordnet ist.
 */
size_t ByteArrayPtr::size() const
{
    return ptrsize;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * Mit dieser Funktion wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
const void* ByteArrayPtr::adr() const
{
    return ptradr;
}

void ByteArrayPtr::truncate(size_t position)
{
    if (position > ptrsize)
        throw Exception("OverflowException", "ByteArrayPtr::truncate position exceeds size of ByteArray (%zu > %zu)", position, ptrsize);
    ptrsize = position;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * Mit dieser Funktion wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
const void* ByteArrayPtr::ptr() const
{
    return ptradr;
}

/*!\brief Speicherreferenz von anderem ByteArrayPtr-Objekt übernehmen
 *
 * Mit diesem Operator wird eine Referenz auf einen Speicherbereich von einer anderen
 * ByteArrayPtr- oder ByteArray Instanz übernommen.
 *
 * @param[in] other Referenz auf ein anderes ByteArrayPtr-Objekt.
 * @return Referenz auf das Objekt
 */
ByteArrayPtr& ByteArrayPtr::operator=(const ByteArrayPtr& other)
{
    ptradr = other.ptradr;
    ptrsize = other.ptrsize;
    return *this;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
ByteArrayPtr::operator const void*() const
{
    return ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
ByteArrayPtr::operator const unsigned char*() const
{
    return (const unsigned char*)ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
ByteArrayPtr::operator const char*() const
{
    return (const char*)ptradr;
}

/*!\brief Einzelnes Byte aus dem Speicherbereich auslesen
 *
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
unsigned char ByteArrayPtr::operator[](size_t pos) const
{
    if (ptradr != NULL && pos < ptrsize) return ((unsigned char*)ptradr)[pos];
    throw Exception("OutOfBoundsEception");
}

unsigned char& ByteArrayPtr::operator[](size_t pos)
{
    if (ptradr != NULL && pos < ptrsize) return static_cast<unsigned char*>(ptradr)[pos];
    throw Exception("OutOfBoundsEception");
}

unsigned char ByteArrayPtr::operator[](int pos) const
{
    if (ptradr != NULL && pos < ptrsize) return ((unsigned char*)ptradr)[pos];
    throw Exception("OutOfBoundsEception");
}

unsigned char& ByteArrayPtr::operator[](int pos)
{
    if (ptradr != NULL && pos < ptrsize) return static_cast<unsigned char*>(ptradr)[pos];
    throw Exception("OutOfBoundsEception");
}

void ByteArrayPtr::set(size_t pos, unsigned char value)
{
    if (pos < ptrsize)
        ((unsigned char*)ptradr)[pos] = value;
    else
        throw Exception("OutOfBoundsEception");
}

unsigned char ByteArrayPtr::get(size_t pos) const
{
    if (ptradr != NULL && pos < ptrsize) return ((unsigned char*)ptradr)[pos];
    throw Exception("OutOfBoundsEception");
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
const char* ByteArrayPtr::toCharPtr() const
{
    return (const char*)ptradr;
}

/*!\brief Speicher mit bestimmtem Wert füllen
 *
 * Diese Funktion füllt den gesamten verwalteten Speicher mit dem Bytewert \p value
 *
 * \param[in] value Bytewert
 */
void ByteArrayPtr::memset(int value)
{
    ::memset(ptradr, value, ptrsize);
}

const char* ByteArrayPtr::map(size_t position, size_t size) const
{
    if (position + size > ptrsize)
        throw Exception("OverflowException", "ByteArrayPtr::map position (%u) + size (%u) exceeds size of ByteArray (%u > %u)", position,
                        size, position + size, ptrsize);
    return (const char*)ptradr + position;
}

} // namespace picopplib
