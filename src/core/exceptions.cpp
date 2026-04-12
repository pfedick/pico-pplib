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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "picopplib.h"

namespace picopplib
{

static const char* default_type = "PPLException";

Exception::Exception(const char* type) throw()
{
    ErrorText = NULL;
    ExceptionType = NULL;
    ExceptionType = strdup(type);
}

Exception::~Exception() throw()
{
    if (ErrorText) free(ErrorText);
    if (ExceptionType) free(ExceptionType);
}

const char* Exception::what() const throw()
{
    if (!ExceptionType) return default_type;
    return ExceptionType;
}

Exception::Exception(const Exception& other) throw()
{
    if (other.ExceptionType) {
        ExceptionType = strdup(other.ExceptionType);
    } else {
        ExceptionType = NULL;
    }
    if (other.ErrorText) {
        ErrorText = strdup(other.ErrorText);
    } else {
        ErrorText = NULL;
    }
}

Exception& Exception::operator=(const Exception& other) throw()
{
    if (&other == this) return *this;
    if (ErrorText) free(ErrorText);
    if (ExceptionType) free(ExceptionType);
    if (other.ExceptionType) {
        ExceptionType = strdup(other.ExceptionType);
    } else {
        ExceptionType = NULL;
    }
    if (other.ErrorText) {
        ErrorText = strdup(other.ErrorText);
    } else {
        ErrorText = NULL;
    }
    return *this;
}

Exception::Exception(const char* type, const char* msg, ...) throw()
{
    ErrorText = NULL;
    ExceptionType = NULL;
    if (type) ExceptionType = strdup(type);
    if (msg) {
        String Msg;
        va_list args;
        va_start(args, msg);
        try {
            Msg.vasprintf(msg, args);
            ErrorText = strdup((const char*)Msg);
        }
        catch (...) {
            ErrorText = NULL;
        }
        va_end(args);
    } else {
        ErrorText = NULL;
    }
}

const char* Exception::text() const throw()
{
    if (ErrorText)
        return ErrorText;
    else
        return "";
}

const char* Exception::type() const throw()
{
    if (ExceptionType)
        return ExceptionType;
    else
        return default_type;
}

String Exception::toString() const throw()
{
    String str;
    str.setf("%s", what());
    if (ErrorText) str.appendf(" [%s]", (const char*)ErrorText);
    return str;
}

void Exception::print() const
{
    PrintDebug("Exception: %s", what());
    if (ErrorText) PrintDebug(" [%s]", (const char*)ErrorText);
    PrintDebug("\n");
}
#ifdef DONTSKIP

/*!\brief %Exception anhand errno-Variable werfen
 *
 * \desc
 * Diese Funktion wird verwendet, um nach Auftreten eines Fehlers, anhand der globalen
 * "errno"-Variablen die passende Exception zu werfen.
 *
 * @param e Errorcode aus der errno-Variablen
 * @param info Zusätzliche Informationen zum Fehler (optional)
 */
void throwExceptionFromErrno(int e, const String& info)
{
    switch (e) {
    case ENOMEM:
        throw Exception("OutOfMemoryException");
    case EINVAL:
        throw Exception("InvalidArgumentsException");
    case ENOTDIR:
    case ENAMETOOLONG:
        throw Exception("InvalidFileNameException", info);
    case EACCES:
    case EPERM:
        throw Exception("PermissionDeniedException", info);
    case ENOENT:
        throw Exception("FileNotFoundException", info);
#ifdef ELOOP
    case ELOOP:
        throw Exception("TooManySymbolicLinksException", info);
#endif
    case EISDIR:
        throw Exception("NoRegularFileException", info);
    case EROFS:
        throw Exception("ReadOnlyException", info);
    case EMFILE:
        throw Exception("TooManyOpenFilesException");
#ifdef EOPNOTSUPP
    case EOPNOTSUPP:
        throw Exception("UnsupportedFileOperationException", info);
#endif
    case ENOSPC:
        throw Exception("FilesystemFullException");
#ifdef EDQUOT
    case EDQUOT:
        throw Exception("QuotaExceededException");
#endif
    case EIO:
        throw Exception("IOErrorException");
    case EBADF:
        throw Exception("BadFiledescriptorException");
    case EFAULT:
        throw Exception("BadAddressException");
#ifdef EOVERFLOW
    case EOVERFLOW:
        throw Exception("OverflowException");
#endif
    case EEXIST:
        throw Exception("FileExistsException");
    case EAGAIN:
        throw Exception("OperationBlockedException");
    case EDEADLK:
        throw Exception("DeadlockException");
    case EINTR:
        throw Exception("OperationInterruptedException");
    case ENOLCK:
        throw Exception("TooManyLocksException");
    case ESPIPE:
        throw Exception("IllegalOperationOnPipeException");
    case ETIMEDOUT:
        throw Exception("TimeoutException", info);

    case ENETDOWN:
        throw Exception("NetworkDownException", info);
    case ENETUNREACH:
        throw Exception("NetworkUnreachableException", info);
    case ENETRESET:
        throw Exception("NetworkDroppedConnectionOnResetException", info);
    case ECONNABORTED:
        throw Exception("SoftwareCausedConnectionAbortException", info);
    case ECONNRESET:
        throw Exception("ConnectionResetByPeerException", info);
    case ENOBUFS:
        throw Exception("NoBufferSpaceException", info);
    case EISCONN:
        throw Exception("SocketIsAlreadyConnectedException", info);
    case ENOTCONN:
        throw Exception("NotConnectedException", info);
#ifdef ETOOMANYREFS
    case ETOOMANYREFS:
        throw Exception("TooManyReferencesException", info);
#endif
    case ECONNREFUSED:
        throw Exception("ConnectionRefusedException", info);
#ifdef EHOSTDOWN
    case EHOSTDOWN:
        throw Exception("HostDownException", info);
#endif
    case EHOSTUNREACH:
        throw Exception("NoRouteToHostException", info);
    case ENOTSOCK:
        throw Exception("InvalidSocketException", info);
    case ENOPROTOOPT:
        throw Exception("UnknownOptionException", info);
    case EPIPE:
        throw Exception("BrokenPipeException", info);
    case EINPROGRESS:
        throw Exception("OperationBlockedException", info);
    case EALREADY:
        throw Exception("OperationAlreadyInProgressException", info);
    case EDESTADDRREQ:
        throw Exception("DestinationAddressRequiredException", info);
    case EMSGSIZE:
        throw Exception("MessageTooLongException", info);
    case EPROTOTYPE:
        throw Exception("ProtocolWrongTypeForSocketException", info);
    default: {
        String ret;
        ret = strerror(e);
        ret += ": " + info;
        throw Exception("UnknownException", ret);
    }
    }
}
#endif

} // namespace picopplib
