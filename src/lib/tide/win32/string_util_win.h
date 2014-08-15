/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef BASE_STRING_UTIL_WIN_H_
#define BASE_STRING_UTIL_WIN_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

namespace base {

inline int strcasecmp(const char* s1, const char* s2) {
    return _stricmp(s1, s2);
}

inline int strncasecmp(const char* s1, const char* s2, size_t count) {
    return _strnicmp(s1, s2, count);
}

inline int vsnprintf(char* buffer, size_t size,
    const char* format, va_list arguments) {
    int length = vsnprintf_s(buffer, size, size - 1, format, arguments);
    if (length < 0)
        return _vscprintf(format, arguments);
    return length;
}

inline int vswprintf(wchar_t* buffer, size_t size,
    const wchar_t* format, va_list arguments) {
    //DCHECK(IsWprintfFormatPortable(format));

    int length = _vsnwprintf_s(buffer, size, size - 1, format, arguments);
    if (length < 0)
        return _vscwprintf(format, arguments);
    return length;
}

}  // namespace base

#endif  // BASE_STRING_UTIL_WIN_H_
