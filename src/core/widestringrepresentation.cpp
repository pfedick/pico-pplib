/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: http://www.pfp.de/ppl/
 *
 *******************************************************************************
 * Copyright (c) 2013, Patrick Fedick <patrick@pfp.de>
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

#include "ppl7-light.h"

namespace ppl7light
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

} // namespace ppl7light