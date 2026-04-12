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

#include "picopplib.h"

namespace picopplib
{

/*
** Translation Table as described in RFC1113
*/
static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
static void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
    out[0] = cb64[in[0] >> 2];
    out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
    out[2] = (unsigned char)(len > 1 ? cb64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] : '=');
    out[3] = (unsigned char)(len > 2 ? cb64[in[2] & 0x3f] : '=');
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
String ToBase64(const ByteArrayPtr& bin)
{
    String res;
    unsigned char in[3], out[4];
    size_t p = 0, filelen = bin.size();

    while (p < filelen) {
        int len = 0;
        for (int i = 0; i < 3; i++) {
            if (p < filelen) {
                in[i] = (unsigned char)bin.get(p++);
                len++;
            }
        }
        if (len) {
            encodeblock(in, out, len);
            for (int i = 0; i < 4; i++) {
                res.appendf("%c", out[i]);
            }
        }
    }
    return res;
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
static void decodeblock(unsigned char in[4], unsigned char out[3])
{
    out[0] = (unsigned char)(in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char)(in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char)(((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
ByteArray FromBase64(const String& str)
{
    ByteArray res;
    unsigned char in[4], out[3], v;
    int i, len;
    size_t p = 0, filelen = str.size();
    while (p < filelen) {
        for (len = 0, i = 0; i < 4 && p < filelen; i++) {
            v = 0;
            while (p < filelen && v == 0) {
                v = str.get(p++);
                v = ((v < 43 || v > 122) ? 0 : cd64[v - 43]);
                if (v) {
                    v = ((v == '$') ? 0 : v - 61);
                }
            }
            if (p < filelen + 1) {
                if (v) {
                    len++;
                    in[i] = (unsigned char)(v - 1);
                }
            } else {
                in[i] = 0;
            }
        }
        if (len) {
            decodeblock(in, out);
            res.append(out, len - 1);
            len = 0;
        }
    }
    return res;
}

/*!\brief Schneidet Leerzeichen, Tabs Returns und Linefeeds am Anfang und Ende des Strings ab
 * \relates String
 *
 * \desc
 * Schneidet Leerzeichen, Tabs Returns und Linefeeds am Anfang und Ende des Strings ab
 * @param str Eingabe-String
 * @return Neuer String
 */
String Trim(const String& str)
{
    String ret = str;
    ret.trim();
    return ret;
}

String UpperCase(const String& str)
{
    String ret = str;
    ret.upperCase();
    return ret;
}

String LowerCase(const String& str)
{
    String ret = str;
    ret.lowerCase();
    return ret;
}

int StrCmp(const String& s1, const String& s2)
{
    int cmp = s1.strcmp(s2);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

int StrCaseCmp(const String& s1, const String& s2)
{
    int cmp = s1.strCaseCmp(s2);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

/*!\brief Sucht nach Zeichen in einem String
 * \relates String
 *
 * \code
ssize_t Instr (const char * haystack, const char * needle, size_t start);
ssize_t Instr (const wchar_t * haystack, const wchar_t * needle, size_t start);
ssize_t Instr (const String &haystack, const String &needle, size_t start);
ssize_t Instrcase (const char * haystack, const char * needle, size_t start);
ssize_t Instrcase (const wchar_t * haystack, const wchar_t * needle, size_t start);
ssize_t Instrcase (const String &haystack, const String &needle, size_t start);
\endcode
 * \desc
 * Diese Funktionen suchen nach einer Zeichenkette innerhalb eines Strings, wobei
 * die Funktion \c Instr zwischen Gross- und Kleinschreibung unterschiedet, und
 * die Funktion \c Instrcase nicht.
 *
 * \param haystack ist ein Pointer auf einen Null-terminierten String, der
 * den zu durchsuchenden String enthält.
 * \param needle ist ein Pointer auf einen Null-terminierten String, der
 * den zu suchenden String enthält.
 * \param start ist ein optionaler Parameter, der die Startposition innerhalb des Suchstrings angibt.
 * Der Default ist 0, das heißt der String wurd vom Anfang an durchsucht.
 * \return Wird der String \p needle gefunden, wird seine Position zurückgegeben. Befindet sich der
 * String gleich an erster Stelle des Suchstrings, wird 0 zurückgegeben. Wird der String nicht gefunden,
 * wird -1 zurückgegeben.
 *
 */
ssize_t Instr(const char* haystack, const char* needle, size_t start)
{
    if (!haystack) return -1;
    if (!needle) return -1;
    if (start < strlen(haystack)) {
        const char* _t = strstr((haystack + start), needle);
        if (_t != NULL) {
            return ((ssize_t)(_t - haystack));
        }
    }
    return (-1);
}

#ifndef HAVE_STRCASESTR
static const char* mystrcasestr(const char* haystack, const char* needle)
{
    char c;
    if ((c = *needle++) != 0) {
        c = tolower((unsigned char)c);
        size_t len = strlen(needle);
        do {
            char sc;
            do {
                if ((sc = *haystack++) == 0) return (NULL);
            } while ((char)tolower((unsigned char)sc) != c);
        } while (strncasecmp(haystack, needle, len) != 0);
        haystack--;
    }
    return ((char*)haystack);
}
#endif

/*!\brief Sucht nach Zeichen in einem String
 * \relates String
 *
 * \copydoc Instr(const char * haystack, const char * needle, size_t start)
 */
ssize_t Instr(const String& haystack, const String& needle, size_t start)
{
    return haystack.instr(needle, start);
}

String Left(const String& str, size_t num)
{
    return str.left(num);
}

String Right(const String& str, size_t num)
{
    return str.right(num);
}

String Mid(const String& str, size_t start, size_t num)
{
    return str.mid(start, num);
}

String SubStr(const String& str, size_t start, size_t num)
{
    return str.substr(start, num);
}

String ToString(const char* fmt, ...)
{
    String str;
    va_list args;
    va_start(args, fmt);
    str.vasprintf(fmt, args);
    va_end(args);
    return str;
}

String Replace(const String& string, const String& search, const String& replace)
{
    String Tmp = string;
    Tmp.replace(search, replace);
    return Tmp;
}

bool IsTrue(const String& str)
{
    return str.isTrue();
}

} // namespace picopplib
