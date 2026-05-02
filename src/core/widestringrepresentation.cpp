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

WideStringRepresentation::WideStringRepresentation(const String& str) throw()
{
    stringlen = 0;
    ptr = NULL;
    if (str.isEmpty()) return;

    size_t len = mbstowcs(NULL, str.c_str(), str.size());
    if (len == (size_t)-1) return;
    ptr = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    if (!ptr) return;
    stringlen = mbstowcs(ptr, str.c_str(), len + 1);
    if (stringlen == (size_t)-1) {
        free(ptr);
        ptr = NULL;
        stringlen = 0;
    }
}

WideStringRepresentation::WideStringRepresentation(const WideStringRepresentation& other) throw()
{
    stringlen = other.stringlen;
    if (stringlen == 0) {
        ptr = NULL;
        return;
    }
    ptr = (wchar_t*)malloc((stringlen + 1) * sizeof(wchar_t));
    if (!ptr) {
        stringlen = 0;
        return;
    }
    memcpy(ptr, other.ptr, (stringlen + 1) * sizeof(wchar_t));
}

WideStringRepresentation::WideStringRepresentation(WideStringRepresentation&& other) noexcept
{
    ptr = other.ptr;
    stringlen = other.stringlen;
    other.ptr = NULL;
    other.stringlen = 0;
}

WideStringRepresentation::~WideStringRepresentation() throw()
{
    if (ptr) free(ptr);
}

const wchar_t* WideStringRepresentation::c_str() const throw()
{
    if (!ptr) return L"";
    return ptr;
}

size_t WideStringRepresentation::size() const
{
    return stringlen;
}

wchar_t WideStringRepresentation::operator[](ssize_t pos) const
{
    if (!ptr || pos < 0 || (size_t)pos >= stringlen) return 0;
    return ptr[pos];
}

WideStringRepresentation& WideStringRepresentation::operator=(const WideStringRepresentation& other) throw()
{
    if (this == &other) return *this;
    if (ptr) free(ptr);
    stringlen = other.stringlen;
    if (stringlen == 0) {
        ptr = NULL;
        return *this;
    }
    ptr = (wchar_t*)malloc((stringlen + 1) * sizeof(wchar_t));
    if (!ptr) {
        stringlen = 0;
        return *this;
    }
    memcpy(ptr, other.ptr, (stringlen + 1) * sizeof(wchar_t));
    return *this;
}

WideStringRepresentation& WideStringRepresentation::operator=(WideStringRepresentation&& other) noexcept
{
    if (this == &other) return *this;
    if (ptr) free(ptr);
    ptr = other.ptr;
    stringlen = other.stringlen;
    other.ptr = NULL;
    other.stringlen = 0;
    return *this;
}

} // namespace picopplib