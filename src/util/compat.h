/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include <stdbool.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <limits.h>
#include <math.h>
#include <complex.h>

#ifdef _WIN32
	// Include the god-awful windows.h header here so that we can fight the obnoxious namespace pollution it introduces.
	// We obviously don't need it in every source file, but it's easier to do it globally and early than to try to infer
	// just where down the maze of includes some of our dependencies happens to smuggle it in.
	//
	// *sigh*
	//
	// Goddamn it.

	// Make sure we get the "unicode" (actually UTF-16) versions of win32 APIs; it defaults to legacy crippled ones.
	#ifndef UNICODE
		#define UNICODE
	#endif
	#ifndef _UNICODE
		#define _UNICODE
	#endif

	// Ask windows.h to include a little bit less of the stupid crap we'll never use.
	// Some of it actually clashes with our names.
	#define WIN32_LEAN_AND_MEAN
	#define NOGDI

	#include <windows.h>

	// far/near pointers are obviously very relevant for modern CPUs and totally deserve their very own, unprefixed keywords!
	#undef near
	#undef far
#endif

// This macro should be provided by stddef.h, but in practice it sometimes is not.
#ifndef offsetof
	#ifdef __GNUC__
		#define offsetof(type, field) __builtin_offsetof(type, field)
	#else
		#define offsetof(type, field) ((size_t)&(((type *)0)->field))
	#endif
#endif

#ifndef __GNUC__ // clang defines this too
	#define __attribute__(...)
	#define __extension__
	#define PRAGMA(p)
	#define UNREACHABLE
#else
	#define PRAGMA(p) _Pragma(#p)
	#define USE_GNU_EXTENSIONS
	#define UNREACHABLE __builtin_unreachable()
#endif

#ifndef __has_attribute
	#ifdef __GNUC__
		#define __has_attribute(attr) 1
	#else
		#define __has_attribute(attr) 0
	#endif
#endif

// On windows, use the MinGW implementations of printf and friends instead of the crippled mscrt ones.
#ifdef __USE_MINGW_ANSI_STDIO
	#define FORMAT_ATTR __MINGW_PRINTF_FORMAT
#else
	#define FORMAT_ATTR printf
#endif

#undef uint
typedef unsigned int uint;

#undef ushort
typedef unsigned short ushort;

#undef ulong
typedef unsigned long ulong;

#undef uchar
typedef unsigned char uchar;

#undef schar
typedef signed char schar;

// These definitions are common but non-standard, so we provide our own
#undef M_PI
#undef M_PI_2
#undef M_PI_4
#undef M_E
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.78539816339744830962
#define M_E 2.7182818284590452354

// This is a workaround to properly specify the type of our "complex" variables...
// Taisei code always uses just "complex" when it actually means "complex double", which is not really correct.
// gcc doesn't seem to care, other compilers do (e.g. clang)
#undef complex
#define complex _Complex double

// Meeded for mingw; presumably yet again defined somewhere in windoze headers.
#undef min
#undef max

/*
 * Abstract away the nasty GNU attribute syntax.
 */

// Function is a hot spot.
#define attr_hot \
	__attribute__ ((hot))

// Function has no side-effects.
#define attr_pure \
	__attribute__ ((pure))

// Function has no side-effects, return value depends on arguments only.
// Must not take pointer parameters, must not return void.
#define attr_const \
	__attribute__ ((const))

// Function never returns NULL.
#define attr_returns_nonnull \
	__attribute__ ((returns_nonnull))

// Function must be called with NULL as the last argument (for varargs functions).
#define attr_sentinel \
	__attribute__ ((sentinel))

// Identifier is meant to be possibly unused.
#define attr_unused \
	__attribute__ ((unused))

// Function or type is deprecated and should not be used.
#define attr_deprecated(msg) \
	__attribute__ ((deprecated(msg)))

// Function parameters at specified positions must not be NULL.
#define attr_nonnull(...) \
	__attribute__ ((nonnull(__VA_ARGS__)))

// The return value of this function must not be ignored.
#define attr_nodiscard \
	__attribute__ ((warn_unused_result))

// Function takes a printf-style format string and variadic arguments.
#define attr_printf(fmt_index, firstarg_index) \
	__attribute__ ((format(FORMAT_ATTR, fmt_index, firstarg_index)))

// Function must be inlined regardless of optimization settings.
#define attr_must_inline \
	__attribute__ ((always_inline))

// Structure must not be initialized with an implicit (non-designated) initializer.
#if __has_attribute(designated_init)
	#define attr_designated_init \
		__attribute__ ((designated_init))
#else
	#define attr_designated_init
#endif
