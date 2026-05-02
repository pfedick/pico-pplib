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

#include <string.h>
#include <stdio.h>

#include "picopplib.h"

namespace picopplib
{

/*!\class PFPChunk
 * \brief Daten-Chunk eines PFP-File Version 3
 *
 * \desc
 * In dieser Klasse werden die Daten eines einzelnen Chunks eines PFP-Files Version 3
 * gespeichert. Um einen Chunk zu erstellen, wird mit "new" eine neue Instanz
 * der Klasse generiert und mit der Funktion PFPFile::addChunk einem PFP-File
 * hinzugefügt. Um einen existierenden Chunk zu löschen, muß die Funktion
 * PFPFile::deleteChunk aufgerufen werden.
 *
 * \see PFPFile
 *
 */

PFPChunk::PFPChunk()
/*!\brief Konstruktor des PFPChunk
 *
 * \desc
 * Der Konstruktor initialisiert den Datenbereich mit NULL und setzt als
 * Name "UNKN" ein. Ein derartiger Chunk würde durch die PFPFile-Klasse
 * nicht gespeichert.
 *
 */
{
    chunkname = "UNKN";
    chunkdata = NULL;
    chunksize = 0;
}

PFPChunk::~PFPChunk()
/*!\brief Destruktor des PFPChunk
 *
 * \desc
 * Der Destruktor sorgt dafür, dass der Datenbereich des Chunks freigegeben
 * wird, sofern er definiert wurde, und wenn er einem PFPFile zugeordnet
 * wurde, wird er daraus entfernt.
 *
 */
{
    chunkdata = NULL;
}

size_t PFPChunk::size() const
/*!\brief Größe des Chunks auslesen
 *
 * \desc
 * Mit dieser Funktion kann die Größe des Chunks ausgelesen werden.
 *
 * \returns Enthält der Chunk Daten, wird die Anzahl Bytes zurückgegeben.
 * Ist er leer, wird 0 zurückgegeben und der Fehlercode 424 gesetzt.
 *
 */
{
    return chunksize;
}

const void* PFPChunk::data() const
/*!\brief Pointer auf die Daten des Chunks holen
 *
 * \desc
 * Mit dieser Funktion wir der Pointer auf die Nutzdaten des Chunks
 * ausgelesen.
 *
 * \returns Enthält der Chunk Daten, wird ein Pointer darauf zurückgegeben.
 * Wenn nicht, wird NULL zurückgegeben.
 *
 */
{
    return chunkdata;
}

const String& PFPChunk::name() const
/*!\brief Name des Chunks auslesen
 *
 * \desc
 * Diese Funktion liefert den Namen des Chunks zurück. Dieser ist
 * immer 4 Byte groß.
 *
 * \returns String mit dem Namen des Chunks.
 *
 */
{
    return chunkname;
}

/*!\class PFPFile
 * \brief Klasse zum Lesen und schreiben von PFP-Files Version 3
 *
 * Mit dieser Klasse können Dateien mit "PFP-File"-Header der Version 3 gelesen und geschrieben werden.
 * Mit Version 3 wurde ein mehr generisches Format definiert, als in den beiden Vorgängerversionen.
 * Jedes File, ganz gleich welchen Inhalt es hat, hat bis zum Ende den gleichen Aufbau. Wichtigste
 * Neuerung dabei sind die sogenannten Chunks. Ein File kann aus bliebig vielen Chunks bestehen.
 * Diese werden von der Klasse PFPChunk abgeleitet, bekommen einen Namen und einen beliebigen
 * Inhalt. Diese können dann mit PFPFile::Add in das File hinzugefügt werden.
 *
 * \copydoc PFPFileVersion3
 *
 * \since Version 6.1.0
 *
 */

/*!\var PFPFile::Chunks
 * \brief Verwaltung aller Chunks in einer Liste
 *
 * \since Version 6.1.0
 */

/*!\var PFPFile::id
 * \brief enthält die ID des Chunks. Die ID ist immer 4 Byte lang, gefolgt von einem 0-Byte
 *
 * \since Version 6.1.0
 */

/*!\var PFPFile::mainversion
 * \brief Hier wird die Hauptversion des Files gespeichert
 *
 * \since Version 6.1.0
 */

/*!\var PFPFile::subversion
 * \brief Hier wird die Unterversion des Files gespeichert
 *
 * \since Version 6.1.0
 */

/*!\var PFPFile::comp
 * \brief Kompressions-Flag
 *
 * In dieser Variable wird die Art der Komprimierung gespeichert:
 * - 0 = keine Komprimierung
 * - 1 = Zlib
 * - 2 = Bzip2
 *
 * \since Version 6.1.0
 */

/*!\var PFPFile::findchunk
 * \brief Chunkname für FindFirstChunk und FindNextChunk
 *
 * \since Version 6.1.0
 */

PFPFile::PFPFile()
/*!\brief Konstruktor der Klasse
 *
 * Hier werden einige interne Variablen initialisert, die ID wird auf "UNKN" gesetzt, Version
 * auf 0 und Kompression abgeschaltet
 *
 * \since Version 6.1.0
 */
{
    id = "UNKN";
    mainversion = subversion = comp = 0;
}

void PFPFile::clear()
/*!\brief Inhalt der Klasse löschen
 *
 * \desc
 * Mit dieser Funktion werden alle Chunks im Speicher freigegeben und die Klasse auf den
 * Ursprungszustand zurückgesetzt, das heisst sie ist anschließend leer
 *
 */
{
    Chunks.clear();
    id = "UNKN";
    mainversion = subversion = comp = 0;
}

const String& PFPFile::getID() const
/*!\brief ID auslesen
 *
 * Diese Funktion liefert einen Pointer auf die ID der Datei zurück
 * \returns Pointer auf die ID der Datei. Diese ist immer 4 Byte groß und mit einem 0-Byte
 * terminiert
 *
 * \since Version 6.1.0
 */
{
    return id;
}

int PFPFile::getMainVersion() const
/*!\brief Hauptversion auslesen
 *
 * Mit dieser Funktion wird die Hauptversion der Datei ausgelesen.
 * \returns Hauptversion als Interger
 *
 * \since Version 6.1.0
 */
{
    return mainversion;
}

int PFPFile::getSubVersion() const
/*!\brief Unterversion auslesen
 *
 * Mit dieser Funktion wird die Unterversion der Datei ausgelesen.
 * \returns Unterversion als Interger
 *
 * \since Version 6.1.0
 *
 */
{
    return subversion;
}

int PFPFile::getCompression() const
{
    return comp;
}

PFPFile::iterator PFPFile::begin()
{
    return Chunks.begin();
}

PFPFile::const_iterator PFPFile::begin() const
{
    return Chunks.begin();
}

PFPFile::iterator PFPFile::end()
{
    return Chunks.end();
}

PFPFile::const_iterator PFPFile::end() const
{
    return Chunks.end();
}

/*!\brief Prüfen, ob es sich um ein PFP-File handelt
 *
 * \desc
 * Diese Funktion prüft, ob es sich bei der geöffneten Datei \p ff um eine Datei
 * im \ref PFPFileVersion3 PFP-Format Version 3 handelt. Ist dies der Fall, wird deren
 * ID und Version eingelesen.
 *
 * @param ff Referenz auf eine geöffnete Datei
 * @return Gibt \c true zurück, wenn es sich um eine Datei im PFP-Format handelt. Deren
 * ID kann anschließend mit PFPFile::getID ausgelesen werden, Version mit PFPFile::getVersion bzw.
 * PFPFile::getMainVersion und PFPFile::getSubVersion. Handelt es sich nicht um eine Datei
 * im PFP-Format, gibt die Funktion \c false zurück. Es wird keine Exception geworfen.
 */
bool PFPFile::ident(const ByteArrayPtr& data)
{
    try {
        const char* p;
        p = data.map(0, 24);
        if (strncmp(p, "PFP-File", 8) != 0) return false;
        if (Peek8(p + 8) != 3) return false;
        id.set(p + 10, 4);
        mainversion = Peek8(p + 15);
        subversion = Peek8(p + 14);
        comp = Peek8(p + 16);
        return true;
    }
    catch (...) {
        return false;
    }
    return false;
}

void PFPFile::load(const ByteArrayPtr& data)
/*!\brief PFP-File laden
 *
 * Mit dieser Funktion wird ein PFP-File in die Klasse geladen. Dabei wird zuerst der Header geladen
 * und überprüft, ob es sich um ein gültiges PFP-File handelt. Dann wird die virtuelle Funktion
 * PFPFile::LoadRequest mit ID, Haupt- und Unterversion als Parameter aufgerufen. Liefert diese
 * nicht true (1) zurück, wird der Ladevorgang abgebrochen. Andernfalls wird fortgeführt
 * und geprüft, ob der
 * Datenbereich komprimiert ist und gegebenenfalls dekomprimiert. Erst danach werden die
 * einzelnen Chunks eingelesen. Kommt es dabei zu Fehlern durch ungültige Chunks, werden diese
 * ignoriert und die Funktion gibt den Fehlercode 434 zurück.
 *
 * \param ff Pointer auf eine CFile-Klasse, mit der die einzulesende Datei geöffnet wurde.
 * \returns Konnte die Datei fehlerfrei eingelesen werden, gibt die Funktion true (1) zurück,
 * im Fehlerfall false (0). Ein entsprechender Fehlercode wird gesetzt.
 *
 * \remarks
 * Vor dem Laden der Datei wird die Funktion PFPFile::Clear aufgerufen, so dass eventuell vorher
 * vorhandene Daten verloren gehen.
 *
 * \since Version 6.1.0
 */
{
    const char* p;
    try {
        p = data.map(0, 24);
    }
    catch (...) {
        throw Exception("InvalidFormatException");
    }
    if (strncmp(p, "PFP-File", 8) != 0) Exception("InvalidFormatException");
    if (Peek8(p + 8) != 3) Exception("InvalidFormatException");
    size_t z, fsize;

    char tmpid[5];
    tmpid[4] = 0;
    strncpy(tmpid, p + 10, 4);
    int t1, t2;
    t1 = Peek8(p + 15);
    t2 = Peek8(p + 14);
    clear();
    id.set(p + 10, 4);
    mainversion = Peek8(p + 15);
    subversion = Peek8(p + 14);
    comp = Peek8(p + 16);
    size_t hsize = Peek8(p + 9);
    if (comp) {
        throw Exception("Compression is not supported");
    }
    p = data.map(hsize, data.size() - hsize);
    fsize = data.size() - hsize;

    // Wir haben nun den ersten Chunk ab Pointer p
    // printf("PFPFile: lade chunks, fsize=%zu\n", fsize);
    z = 0;
    try {
        size_t size = 0;
        while (z < fsize) {
            size = Peek32((p + z + 4));
            // HexDump(p + z, size);
            if (strncmp(p + z, "ENDF", 4) == 0) break;
            if (!size) break;
            // Falls z+size über das Ende der Datei geht, stimmt mit diesem Chunk was nicht
            if (z + size > fsize) break;
            PFPChunk chunk;
            chunk.chunkname.set(p + z, 4);
            chunk.chunkdata = (p + z + 8);
            chunk.chunksize = size - 8;
            // printf("add chunk\n");
            if (chunk.chunkname == "UNKN") throw Exception("IllegalArgumentException");
            Chunks.push_back(std::move(chunk));
            z += size;
        }
    }
    catch (...) {
        clear();
        throw;
    }
    // printf("done loading chunks\n\n");
}

} // namespace picopplib
