/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRING16_H_
#define BASE_STRING16_H_

// WHAT:
// A version of std::basic_string that provides 2-byte characters even when
// wchar_t is not implemented as a 2-byte type. You can access this class as
// string16. We also define char16, which string16 is based upon.
//
// WHY:
// On Windows, wchar_t is 2 bytes, and it can conveniently handle UTF-16/UCS-2
// data. Plenty of existing code operates on strings encoded as UTF-16.
//
// On many other platforms, sizeof(wchar_t) is 4 bytes by default. We can make
// it 2 bytes by using the GCC flag -fshort-wchar. But then std::wstring fails
// at run time, because it calls some functions (like wcslen) that come from
// the system's native C library -- which was built with a 4-byte wchar_t!
// It's wasteful to use 4-byte wchar_t strings to carry UTF-16 data, and it's
// entirely improper on those systems where the encoding of wchar_t is defined
// as UTF-32.
//
// Here, we define string16, which is similar to std::wstring but replaces all
// libc functions with custom, 2-byte-char compatible routines. It is capable
// of carrying UTF-16-encoded data.

#include <string>

#include "basictypes.h"

#define WCHAR_T_IS_UTF16

#if defined(WCHAR_T_IS_UTF16)

typedef wchar_t char16;
typedef std::wstring string16;

#elif defined(WCHAR_T_IS_UTF32)

typedef uint16 char16;

namespace base {

// char16 versions of the functions required by string16_char_traits; these
// are based on the wide character functions of similar names ("w" or "wcs"
// instead of "c16").
int c16memcmp(const char16* s1, const char16* s2, size_t n);
size_t c16len(const char16* s);
const char16* c16memchr(const char16* s, char16 c, size_t n);
char16* c16memmove(char16* s1, const char16* s2, size_t n);
char16* c16memcpy(char16* s1, const char16* s2, size_t n);
char16* c16memset(char16* s, char16 c, size_t n);

struct string16_char_traits {
    typedef char16 char_type;
    typedef int int_type;

    // int_type needs to be able to hold each possible value of char_type, and in
    // addition, the distinct value of eof().
    COMPILE_ASSERT(sizeof(int_type) > sizeof(char_type), unexpected_type_width);

    typedef std::streamoff off_type;
    typedef mbstate_t state_type;
    typedef std::fpos<state_type> pos_type;

    static void assign(char_type& c1, const char_type& c2) {
        c1 = c2;
    }

    static bool eq(const char_type& c1, const char_type& c2) {
        return c1 == c2;
    }
    static bool lt(const char_type& c1, const char_type& c2) {
        return c1 < c2;
    }

    static int compare(const char_type* s1, const char_type* s2, size_t n) {
        return c16memcmp(s1, s2, n);
    }

    static size_t length(const char_type* s) {
        return c16len(s);
    }

    static const char_type* find(const char_type* s, size_t n,
                                                             const char_type& a) {
        return c16memchr(s, a, n);
    }

    static char_type* move(char_type* s1, const char_type* s2, int_type n) {
        return c16memmove(s1, s2, n);
    }

    static char_type* copy(char_type* s1, const char_type* s2, size_t n) {
        return c16memcpy(s1, s2, n);
    }

    static char_type* assign(char_type* s, size_t n, char_type a) {
        return c16memset(s, a, n);
    }

    static int_type not_eof(const int_type& c) {
        return eq_int_type(c, eof()) ? 0 : c;
    }

    static char_type to_char_type(const int_type& c) {
        return char_type(c);
    }

    static int_type to_int_type(const char_type& c) {
        return int_type(c);
    }

    static bool eq_int_type(const int_type& c1, const int_type& c2) {
        return c1 == c2;
    }

    static int_type eof() {
        return static_cast<int_type>(EOF);
    }
};

}  // namespace base

typedef std::basic_string<char16, base::string16_char_traits> string16;

#endif  // WCHAR_T_IS_UTF32

#endif  // BASE_STRING16_H_
