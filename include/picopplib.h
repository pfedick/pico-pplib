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

#ifndef PICOPPLIB_H_
#define PICOPPLIB_H_

#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>

#include <list>
#include <exception>

namespace picopplib
{

class String;
class ByteArray;
class ByteArrayPtr;

class ByteArrayPtr
{
    friend class ByteArray;

private:
    void* ptradr;
    size_t ptrsize;

public:
    ByteArrayPtr();
    ByteArrayPtr(const ByteArrayPtr& other);
    explicit ByteArrayPtr(const String& data);
    ByteArrayPtr(void* adr, size_t size);
    ByteArrayPtr(const void* adr, size_t size);
    bool isNull() const;
    bool isEmpty() const;
    size_t size() const;
    const void* adr() const;
    const void* ptr() const;
    ByteArrayPtr& operator=(const ByteArrayPtr& other);
    operator const void*() const;
    operator const char*() const;
    operator const unsigned char*() const;
    const char* toCharPtr() const;
    void truncate(size_t position);
    unsigned char operator[](size_t pos) const;
    void set(size_t pos, unsigned char value);
    unsigned char get(size_t pos) const;
    void memset(int value);
    const char* map(size_t position, size_t size) const;
};

class ByteArray : public ByteArrayPtr
{
private:
public:
    ByteArray();
    ByteArray(const ByteArrayPtr& other);
    ByteArray(const ByteArray& other);
    ByteArray(ByteArray&& other) noexcept;
    ByteArray(const String& str);
    ByteArray(const void* adr, size_t size);
    explicit ByteArray(size_t size);
    ~ByteArray();
    void useadr(void* adr, size_t size);
    void* copy(const void* adr, size_t size);
    void* copy(const ByteArrayPtr& other);
    void* append(void* adr, size_t size);
    void* append(const ByteArrayPtr& other);
    void* prepend(void* adr, size_t size);
    void* prepend(const ByteArrayPtr& other);
    void* malloc(size_t size);
    void* calloc(size_t size);
    void* realloc(size_t newsize);
    void free();
    void clear();
    ByteArray& operator=(const ByteArrayPtr& other);
    ByteArray& operator=(const ByteArray& other);
    ByteArray& operator=(ByteArray&& other) noexcept;
    ByteArray& operator=(const String& str);
    operator const void*() const;
    operator const unsigned char*() const;
    operator const char*() const;
    unsigned char operator[](size_t pos) const;
};

class Array;
class WideString;
class String
{
private:
    char* ptr;
    size_t s, stringlen;

public:
    String() throw();
    String(const char* str);
    String(const char* str, size_t size);
    String(const String& str);
    String(const WideString& str);
    explicit String(const ByteArrayPtr& str);
    String(String&& other) noexcept;            // move construct
    String& operator=(String&& other) noexcept; // move assignment

    ~String() throw();

    void clear() throw();
    size_t capacity() const;
    void reserve(size_t size);
    size_t size() const;
    bool isEmpty() const;
    bool notEmpty() const;
    bool isNumeric() const;
    bool isInteger() const;
    bool isTrue() const;
    bool isFalse() const;

    int strcmp(const String& str, size_t size = (size_t)-1) const;
    int strcmp(const char* str, size_t size = (size_t)-1) const;
    int strCaseCmp(const String& str, size_t size = (size_t)-1) const;
    int strCaseCmp(const char* str, size_t size = (size_t)-1) const;

    String left(size_t len) const;
    String right(size_t len) const;
    String mid(size_t start, size_t len = (size_t)-1) const;
    String substr(size_t start, size_t len = (size_t)-1) const;

    String& set(const char* str, size_t size = (size_t)-1);
    String& set(const wchar_t* str, size_t size = (size_t)-1);
    String& set(const String& str, size_t size = (size_t)-1);
    String& set(const WideString& str, size_t size = (size_t)-1);
    String& set(const ByteArrayPtr& str, size_t size = (size_t)-1);

    String& set(char c);
    String& set(size_t position, char c);
    String& setf(const char* fmt, ...);
    String& useadr(void* adr, size_t size, size_t stringlen = (size_t)-1);

    String& append(const char* str, size_t size = (size_t)-1);
    String& append(const String& str, size_t size = (size_t)-1);
    String& append(const WideString& str, size_t size = (size_t)-1);
    String& appendf(const char* fmt, ...);
    String& append(char c);

    String& prepend(const char* str, size_t size = (size_t)-1);
    String& prepend(const String& str, size_t size = (size_t)-1);
    String& prepend(const WideString& str, size_t size = (size_t)-1);
    String& prependf(const char* fmt, ...);
    String& prepend(char c);

    String& vasprintf(const char* fmt, va_list args);

    String& repeat(size_t num);
    String& repeat(char code, size_t num);
    String& repeat(const String& str, size_t num);
    String repeated(size_t num) const;

    void trim();
    String trimmed() const;
    void trimLeft();
    void trimRight();
    void trim(const String& chars);
    void trimLeft(const String& chars);
    void trimRight(const String& chars);
    void chopRight(size_t num = 1);
    void chop(size_t num = 1);
    void chopLeft(size_t num = 1);
    void chomp();
    void cut(size_t pos);
    void cut(const String& letter);

    void shl(char c, size_t size);
    void shr(char c, size_t size);

    void lowerCase();
    void upperCase();
    void upperCaseWords();
    String toLowerCase() const;
    String toUpperCase() const;
    String toUpperCaseWords() const;

    String strchr(char c) const;
    String strrchr(char c) const;
    String strstr(const String& needle) const;
    ssize_t find(const String& needle, ssize_t start = 0) const;
    ssize_t instr(const String& needle, size_t start = 0) const;
    bool has(const String& needle) const;

    String& replace(const String& search, const String& replacement);

    void print(bool withNewline = false) const throw();
    void printnl() const throw();
    char get(ssize_t pos) const;
    const char* getPtr() const;
    const char* c_str() const;

    int toInt() const;
    unsigned int toUnsignedInt() const;
    int64_t toInt64() const;
    uint64_t toUnsignedInt64() const;
    bool toBool() const;
    long toLong() const;
    unsigned long toUnsignedLong() const;
    long long toLongLong() const;
    unsigned long long toUnsignedLongLong() const;
    float toFloat() const;
    double toDouble() const;
    const char* toChar() const;
    WideString toWideString() const;

    //! @name Operatoren
    //@{
    operator const char*() const;
    operator const unsigned char*() const;
    operator int() const;
    operator unsigned int() const;
    operator bool() const;
    operator long() const;
    operator unsigned long() const;
    operator long long() const;
    operator unsigned long long() const;
    operator float() const;
    operator double() const;

    char operator[](ssize_t pos) const;

    String& operator=(const char* str);
    String& operator=(const String& str);
    String& operator=(const String&& str);
    String& operator=(const WideString& str);
    String& operator=(char c);
    String& operator+=(const char* str);
    String& operator+=(const String& str);
    String& operator+=(const WideString& str);
    String& operator+=(char c);
    bool operator<(const String& str) const;
    bool operator<=(const String& str) const;
    bool operator==(const String& str) const;
    bool operator!=(const String& str) const;
    bool operator>=(const String& str) const;
    bool operator>(const String& str) const;

    bool operator<(const char* str) const;
    bool operator<=(const char* str) const;
    bool operator==(const char* str) const;
    bool operator!=(const char* str) const;
    bool operator>=(const char* str) const;
    bool operator>(const char* str) const;
};

String operator+(const String& str1, const String& str2);
String operator+(const char* str1, const String& str2);
String operator+(const String& str1, const char* str2);

class WideString
{
private:
    wchar_t* ptr;
    size_t s, stringlen;

public:
    WideString() throw();
    WideString(const wchar_t* str);
    WideString(const wchar_t* str, size_t size);
    WideString(const WideString& str);
    WideString(const String& str);
    WideString(WideString&& other) noexcept;            // move construct
    WideString& operator=(WideString&& other) noexcept; // move assignment

    ~WideString() throw();

    void clear() throw();
    size_t capacity() const;
    void reserve(size_t size);
    size_t size() const;
    size_t byteLength() const;
    bool isEmpty() const;
    bool notEmpty() const;
    bool isNumeric() const;
    bool isInteger() const;
    bool isTrue() const;
    bool isFalse() const;

    int strcmp(const WideString& str, size_t size = (size_t)-1) const;
    int strcmp(const wchar_t* str, size_t size = (size_t)-1) const;
    int strCaseCmp(const WideString& str, size_t size = (size_t)-1) const;
    int strCaseCmp(const wchar_t* str, size_t size = (size_t)-1) const;

    WideString left(size_t len) const;
    WideString right(size_t len) const;
    WideString mid(size_t start, size_t len = (size_t)-1) const;
    WideString substr(size_t start, size_t len = (size_t)-1) const;

    WideString& set(const char* str, size_t size = (size_t)-1);
    WideString& set(const wchar_t* str, size_t size = (size_t)-1);
    WideString& set(const WideString& str, size_t size = (size_t)-1);
    WideString& set(const String& str, size_t size = (size_t)-1);
    WideString& set(wchar_t c);
    WideString& set(size_t position, wchar_t c);
    WideString& setf(const char* fmt, ...);
    WideString& useadr(void* adr, size_t size, size_t stringlen = (size_t)-1);

    WideString& append(const wchar_t* str, size_t size = (size_t)-1);
    WideString& append(const char* str, size_t size = (size_t)-1);
    WideString& append(const WideString& str, size_t size = (size_t)-1);
    WideString& append(const String& str, size_t size = (size_t)-1);
    WideString& appendf(const char* fmt, ...);
    WideString& append(wchar_t c);

    WideString& prepend(const wchar_t* str, size_t size = (size_t)-1);
    WideString& prepend(const WideString& str, size_t size = (size_t)-1);
    WideString& prepend(const String& str, size_t size = (size_t)-1);
    WideString& prependf(const char* fmt, ...);
    WideString& prepend(wchar_t c);

    WideString& vasprintf(const char* fmt, va_list args);

    WideString& repeat(size_t num);
    WideString& repeat(wchar_t code, size_t num);
    WideString& repeat(const WideString& str, size_t num);
    WideString repeated(size_t num) const;

    void trim();
    WideString trimmed() const;
    void trimLeft();
    void trimRight();
    void trim(const WideString& chars);
    void trimLeft(const WideString& chars);
    void trimRight(const WideString& chars);
    void chopRight(size_t num = 1);
    void chop(size_t num = 1);
    void chopLeft(size_t num = 1);
    void chomp();
    void cut(size_t pos);
    void cut(const WideString& letter);

    void shl(wchar_t c, size_t size);
    void shr(wchar_t c, size_t size);

    void lowerCase();
    void upperCase();
    void upperCaseWords();
    WideString toLowerCase() const;
    WideString toUpperCase() const;
    WideString toUpperCaseWords() const;

    WideString strchr(wchar_t c) const;
    WideString strrchr(wchar_t c) const;
    WideString strstr(const WideString& needle) const;
    ssize_t find(const WideString& needle, ssize_t start = 0) const;
    ssize_t findCase(const WideString& needle, ssize_t start) const;
    ssize_t instr(const WideString& needle, size_t start = 0) const;
    ssize_t instrCase(const WideString& needle, size_t start) const;
    bool has(const WideString& needle) const;

    bool startsWith(const WideString& prefix, size_t start = 0, size_t end = (size_t)-1) const;
    bool endsWith(const WideString& suffix, size_t start = 0, size_t end = (size_t)-1) const;

    WideString& replace(const WideString& search, const WideString& replacement);
    WideString join(const Array& iterable) const;

    void print(bool withNewline = false) const throw();
    void printnl() const throw();
    wchar_t get(ssize_t pos) const;
    const wchar_t* getPtr() const;
    const wchar_t* c_str() const;

    int toInt() const;
    unsigned int toUnsignedInt() const;
    int64_t toInt64() const;
    uint64_t toUnsignedInt64() const;
    bool toBool() const;
    long toLong() const;
    unsigned long toUnsignedLong() const;
    long long toLongLong() const;
    unsigned long long toUnsignedLongLong() const;
    float toFloat() const;
    double toDouble() const;
    const wchar_t* toWchart() const;
    String toString() const;

    //! @name Operatoren
    //@{
    operator const wchar_t*() const;
    operator int() const;
    operator unsigned int() const;
    operator bool() const;
    operator long() const;
    operator unsigned long() const;
    operator long long() const;
    operator unsigned long long() const;
    operator float() const;
    operator double() const;

    wchar_t operator[](ssize_t pos) const;

    WideString& operator=(const char* str);
    WideString& operator=(const wchar_t* str);
    WideString& operator=(const String& str);
    WideString& operator=(const WideString& str);
    WideString& operator=(const WideString&& str);
    WideString& operator=(wchar_t c);
    WideString& operator+=(const char* str);
    WideString& operator+=(const wchar_t* str);
    WideString& operator+=(const String& str);
    WideString& operator+=(const WideString& str);
    WideString& operator+=(wchar_t c);
    bool operator<(const WideString& str) const;
    bool operator<=(const WideString& str) const;
    bool operator==(const WideString& str) const;
    bool operator!=(const WideString& str) const;
    bool operator>=(const WideString& str) const;
    bool operator>(const WideString& str) const;

    bool operator<(const wchar_t* str) const;
    bool operator<=(const wchar_t* str) const;
    bool operator==(const wchar_t* str) const;
    bool operator!=(const wchar_t* str) const;
    bool operator>=(const wchar_t* str) const;
    bool operator>(const wchar_t* str) const;
};

WideString operator+(const WideString& str1, const WideString& str2);
WideString operator+(const wchar_t* str1, const WideString& str2);
WideString operator+(const WideString& str1, const wchar_t* str2);

class WideStringRepresentation
{
private:
    wchar_t* ptr;
    size_t stringlen;

public:
    WideStringRepresentation(const String& str) throw();
    WideStringRepresentation(const WideStringRepresentation& other) throw();
    WideStringRepresentation(WideStringRepresentation&& other) noexcept;

    ~WideStringRepresentation() throw();
    const wchar_t* c_str() const throw();
    size_t size() const;
    wchar_t operator[](ssize_t pos) const;

    WideStringRepresentation& operator=(const WideStringRepresentation& other) throw();
    WideStringRepresentation& operator=(WideStringRepresentation&& other) noexcept;
};

class Array
{
private:
    typedef struct
    {
        String* value;
    } ROW;

    size_t numElements;
    size_t numCapacity;
    void* rows;

public:
    static String EmptyString;

    class Iterator
    {
        friend class Array;

    private:
        size_t pos;

    public:
        Iterator();
    };

    //! @name Konstruktoren und Destruktor
    //@{
    Array();
    Array(const Array& other);
    Array(Array&& other) noexcept;
    Array(const String& str, const String& delimiter = String("\n"), size_t limit = 0, bool skipemptylines = false);
    ~Array();
    //@}

    //! @name Elemente hinzufügen/verändern
    //@{
    void add(const String& value);
    void add(const String& value, size_t size);
    void add(const char* value, size_t size);
    void add(const Array& other);
    void addf(const char* fmt, ...);
    void copy(const Array& other);
    void set(size_t index, const String& value);
    void setf(size_t index, const char* fmt, ...);
    void insert(size_t index, const String& value);
    void insert(size_t index, const Array& other);
    void insertf(size_t index, const char* fmt, ...);
    Array& fromArgs(int argc, const char** argv);
    Array& fromArgs(const String& args);
    Array& explode(const String& text, const String& delimiter = "\n", size_t limit = 0, bool skipemptylines = false);
    //@}

    //! @name Elemente löschen
    //@{
    void clear();
    String erase(size_t index);
    String shift();
    String pop();
    //@}

    //! @name Elemente auslesen
    //@{
    const String& get(ssize_t index) const;
    const String& getRandom() const;
    String& get(ssize_t index);
    String& getRandom();
    String getRest(size_t index, const String& delimiter = " ");
    const char* getPtr(ssize_t index) const;
    const char* getRandomPtr() const;
    void reset(Iterator& it) const;
    const String& getFirst(Iterator& it) const;
    const String& getNext(Iterator& it) const;
    String implode(const String& delimiter = "\n") const;
    //@}

    //! @name Sonstiges
    //@{
    size_t count() const;
    size_t size() const;
    size_t capacity() const;
    bool empty() const;
    void reserve(size_t size);
    void list(const String& prefix = String()) const;
    void sort();
    void sortReverse();
    void sortUnique();
    void makeUnique();

    size_t indexOf(const String& search);
    bool has(const String& search);
    //@}

    //! @name Operatoren
    //@{
    String& operator[](ssize_t index);
    const String& operator[](ssize_t index) const;
    Array& operator=(const Array& other);
    Array& operator=(Array&& other) noexcept;
    Array& operator+=(const Array& other);
    bool operator==(const Array& other) const;
    bool operator!=(const Array& other) const;
    //@}

    //! @name Iteratoren
    //@{
    class ptr_iterator
    {
        void* ptr;

    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef String value_type;
        typedef std::ptrdiff_t difference_type;
        typedef String* pointer;
        typedef String& reference;
        ptr_iterator(void* p)
            : ptr(p)
        {
        }
        reference operator*() const
        {
            String* s = (*(ROW*)ptr).value;
            if (!s) return Array::EmptyString;
            return *s;
        }
        pointer operator->() const
        {
            String* s = (*(ROW*)ptr).value;
            if (!s) return &Array::EmptyString;
            return s;
        }
        ptr_iterator& operator++()
        {
            ptr = (ROW*)ptr + 1;
            return *this;
        }
        ptr_iterator operator++(int)
        {
            ptr_iterator tmp = *this;
            ptr = (ROW*)ptr + 1;
            return tmp;
        }
        bool operator==(const ptr_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const ptr_iterator& other) const { return ptr != other.ptr; }
    };

    typedef ptr_iterator iterator;
    typedef const ptr_iterator const_iterator;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    //@}
};

Array operator+(const Array& a1, const Array& a2);

class PFPChunk
{
    friend class PFPFile;

private:
    String chunkname;
    const void* chunkdata;
    size_t chunksize;

public:
    PFPChunk();
    ~PFPChunk();
    size_t size() const;
    const void* data() const;
    const String& name() const;
};

class PFPFile
{
private:
    std::list<PFPChunk> Chunks;
    String id;
    uint8_t mainversion, subversion;
    uint8_t comp;

public:
    typedef std::list<PFPChunk>::iterator iterator;
    typedef std::list<PFPChunk>::const_iterator const_iterator;

    PFPFile();
    void clear();

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    void load(const ByteArrayPtr& data);
    bool ident(const ByteArrayPtr& data);

    const String& getID() const;
    int getMainVersion() const;
    int getSubVersion() const;
    int getCompression() const;
};

/*****************************************************************
 * Functions
 *****************************************************************/

void PrintDebug(const char* format, ...);
void HexDump(const void* address, size_t bytes, bool skipheader);
void HexDump(const void* address, size_t bytes);

// Speicherzugriff
void Poke8(void* Adresse, uint8_t Wert);
void Poke16(void* Adresse, uint16_t Wert);
void Poke24(void* Adresse, uint32_t Wert);
void Poke32(void* Adresse, uint32_t Wert);
void Poke64(void* Adresse, uint64_t Wert);
void PokeFloat(void* Adresse, float Wert);
uint8_t Peek8(const void* Adresse);
uint16_t Peek16(const void* Adresse);
uint32_t Peek24(const void* Adresse);
uint32_t Peek32(const void* Adresse);
uint64_t Peek64(const void* Adresse);
float PeekFloat(const void* Adresse);

// Network-Byte-Order
void PokeN8(void* Adresse, uint8_t Wert);
void PokeN16(void* Adresse, uint16_t Wert);
void PokeN32(void* Adresse, uint32_t Wert);
void PokeN64(void* Adresse, uint64_t Wert);

uint8_t PeekN8(const void* Adresse);
uint16_t PeekN16(const void* Adresse);
uint32_t PeekN32(const void* Adresse);
uint64_t PeekN64(const void* Adresse);

size_t rand(size_t min, size_t max);
double randf(double min, double max);
uint32_t crc32(const void* buffer, size_t size, uint32_t initial_crc = 0xFFFFFFFF);
uint16_t crc16(const void* buffer, size_t size, uint16_t initial_crc = 0xFFFF);

String ToBase64(const ByteArrayPtr& bin);
ByteArray FromBase64(const String& str);
String ToString(const char* fmt, ...);
String Trim(const String& str);
String UpperCase(const String& str);
String LowerCase(const String& str);
int StrCmp(const String& s1, const String& s2);
int StrCaseCmp(const String& s1, const String& s2);
ssize_t Instr(const String& haystack, const String& needle, size_t start = 0);
ssize_t Instr(const char* haystack, const char* needle, size_t start = 0);

String Left(const String& str, size_t num);
String Right(const String& str, size_t num);
String Mid(const String& str, size_t start, size_t num = (size_t)-1);
String SubStr(const String& str, size_t start, size_t num = (size_t)-1);
String ToString(const char* fmt, ...);
String Replace(const String& string, const String& search, const String& replace);
bool IsTrue(const String& str);

/*****************************************************************
 * Exceptions
 *****************************************************************/

class Exception : std::exception
{
private:
    char* ErrorText;
    char* ExceptionType;

public:
    explicit Exception(const char* type) noexcept;
    Exception(const Exception& other) noexcept;
    Exception(Exception&& other) noexcept;
    Exception& operator=(const Exception& other) noexcept;
    Exception& operator=(Exception&& other) noexcept;
    Exception(const char* type, const char* msg, ...) noexcept;
    virtual ~Exception() noexcept;
    virtual const char* what() const noexcept;
    const char* text() const noexcept;
    const char* type() const noexcept;
    String toString() const noexcept;
    void print() const;
};

} // namespace picopplib

#endif
