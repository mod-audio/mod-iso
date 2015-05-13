//#include "juce_Config.h"

/********* Start of inlined file: juce_StandardHeader.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_STANDARDHEADER_JUCEHEADER__
#define __JUCE_STANDARDHEADER_JUCEHEADER__

/** Current Juce version number.

    See also SystemStats::getJUCEVersion() for a string version.
*/
#define JUCE_MAJOR_VERSION      1
#define JUCE_MINOR_VERSION      46

/** Current Juce version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release (not currently used).

    See also SystemStats::getJUCEVersion() for a string version.
*/
#define JUCE_VERSION            ((JUCE_MAJOR_VERSION << 16) + (JUCE_MINOR_VERSION << 8))

//#include "../../../juce_Config.h"

#ifdef JUCE_NAMESPACE
  #define BEGIN_JUCE_NAMESPACE    namespace JUCE_NAMESPACE {
  #define END_JUCE_NAMESPACE      }
#else
  #define BEGIN_JUCE_NAMESPACE
  #define END_JUCE_NAMESPACE
#endif

// This sets up the JUCE_WIN32, JUCE_MAC, or JUCE_LINUX macros

/********* Start of inlined file: juce_PlatformDefs.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_PLATFORMDEFS_JUCEHEADER__
#define __JUCE_PLATFORMDEFS_JUCEHEADER__

/*  This file figures out which platform is being built, and defines some macros
    that the rest of the code can use for OS-specific compilation.

    Macros that will be set here are:

    - One of JUCE_WIN32, JUCE_MAC or JUCE_LINUX.
    - Either JUCE_32BIT or JUCE_64BIT, depending on the architecture.
    - Either JUCE_LITTLE_ENDIAN or JUCE_BIG_ENDIAN.
    - Either JUCE_INTEL or JUCE_PPC
    - Either JUCE_GCC or JUCE_MSVC

    On the Mac, it also defines MACOS_10_2_OR_EARLIER if the build is targeting OSX10.2,
    and MACOS_10_3_OR_EARLIER if it is targeting either 10.2 or 10.3

    It also includes a set of macros for debug console output and assertions.

*/

#if (defined (_WIN32) || defined (_WIN64))
  #define       JUCE_WIN32 1
#else
  #if defined (LINUX) || defined (__linux__)
    #define     JUCE_LINUX 1
  #else
    #define     JUCE_MAC 1
  #endif
#endif

#if JUCE_WIN32
  #ifdef _MSC_VER
    #ifdef _WIN64
      #define JUCE_64BIT 1
    #else
      #define JUCE_32BIT 1
    #endif
  #endif

  #ifdef _DEBUG
    #define JUCE_DEBUG 1
  #endif

  /** If defined, this indicates that the processor is little-endian. */
  #define JUCE_LITTLE_ENDIAN 1

  #define JUCE_INTEL 1
#endif

#if JUCE_MAC

  #include <CoreFoundation/CoreFoundation.h>

  #ifndef NDEBUG
    #define JUCE_DEBUG 1
  #endif

  #ifdef __LITTLE_ENDIAN__
    #define JUCE_LITTLE_ENDIAN 1
  #else
    #define JUCE_BIG_ENDIAN 1
  #endif

  #if defined (__ppc__) || defined (__ppc64__)
    #define JUCE_PPC 1
  #else
    #define JUCE_INTEL 1
  #endif

  #ifdef __LP64__
    #define JUCE_64BIT 1
  #else
    #define JUCE_32BIT 1
  #endif

  #if (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3)
    #define MACOS_10_2_OR_EARLIER 1
  #endif

  #if (! defined (MAC_OS_X_VERSION_10_4)) || (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4)
    #define MACOS_10_3_OR_EARLIER 1
  #endif

  #if (! defined (MAC_OS_X_VERSION_10_5)) || (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5)
    #define MACOS_10_4_OR_EARLIER 1
  #endif
#endif

#if JUCE_LINUX

  #ifdef _DEBUG
    #define JUCE_DEBUG 1
  #endif

  // Allow override for big-endian Linux platforms
  #ifndef JUCE_BIG_ENDIAN
    #define JUCE_LITTLE_ENDIAN 1
  #endif

  #if defined (__LP64__) || defined (_LP64)
    #define JUCE_64BIT 1
  #else
    #define JUCE_32BIT 1
  #endif

  #define JUCE_INTEL 1
#endif

#ifdef JUCE_FORCE_DEBUG
  #undef JUCE_DEBUG

  #if JUCE_FORCE_DEBUG
    #define JUCE_DEBUG 1
  #endif
#endif

// Compiler type macros.

#ifdef __GNUC__
  #define JUCE_GCC 1
#elif defined (_MSC_VER)
  #define JUCE_MSVC 1

  #if _MSC_VER >= 1400
    #define JUCE_USE_INTRINSICS 1
  #endif
#else
  #error unknown compiler
#endif

/** This macro defines the C calling convention used as the standard for Juce calls. */
#if JUCE_MSVC
  #define JUCE_CALLTYPE             __stdcall
#else
  #define JUCE_CALLTYPE
#endif

// Debugging and assertion macros

// (For info about JUCE_LOG_ASSERTIONS, have a look in juce_Config.h)
#if JUCE_LOG_ASSERTIONS
  #define juce_LogCurrentAssertion    juce_LogAssertion (__FILE__, __LINE__);
#elif defined (JUCE_DEBUG)
  #define juce_LogCurrentAssertion    fprintf (stderr, "JUCE Assertion failure in %s, line %d\n", __FILE__, __LINE__);
#else
  #define juce_LogCurrentAssertion
#endif

#ifdef JUCE_DEBUG

  // If debugging is enabled..

  /** Writes a string to the standard error stream.

    This is only compiled in a debug build.

    @see Logger::outputDebugString
  */
  #define DBG(dbgtext)                  Logger::outputDebugString (dbgtext);

  /** Printf's a string to the standard error stream.

    This is only compiled in a debug build.

    @see Logger::outputDebugString
  */
  #define DBG_PRINTF(dbgprintf)         Logger::outputDebugPrintf dbgprintf;

  // Assertions..

  #if JUCE_WIN32 || DOXYGEN

    #if JUCE_USE_INTRINSICS
      #pragma intrinsic (__debugbreak)

      /** This will try to break the debugger if one is currently hosting this app.
          @see jassert()
      */
      #define juce_breakDebugger            __debugbreak();

    #elif JUCE_GCC
      /** This will try to break the debugger if one is currently hosting this app.
          @see jassert()
      */
      #define juce_breakDebugger            asm("int $3");
    #else
      /** This will try to break the debugger if one is currently hosting this app.
          @see jassert()
      */
      #define juce_breakDebugger            { __asm int 3 }
    #endif
  #elif JUCE_MAC
    #define juce_breakDebugger              Debugger();
  #elif JUCE_LINUX
    #define juce_breakDebugger              kill (0, SIGTRAP);
  #endif

  /** This will always cause an assertion failure.

      It is only compiled in a debug build, (unless JUCE_LOG_ASSERTIONS is enabled
      in juce_Config.h).

      @see jassert()
  */
  #define jassertfalse                  { juce_LogCurrentAssertion; if (JUCE_NAMESPACE::juce_isRunningUnderDebugger()) juce_breakDebugger; }

  /** Platform-independent assertion macro.

      This gets optimised out when not being built with debugging turned on.

      Be careful not to call any functions within its arguments that are vital to
      the behaviour of the program, because these won't get called in the release
      build.

      @see jassertfalse
  */
  #define jassert(expression)           { if (! (expression)) jassertfalse }

#else

  // If debugging is disabled, these dummy debug and assertion macros are used..

  #define DBG(dbgtext)
  #define DBG_PRINTF(dbgprintf)

  #define jassertfalse                  { juce_LogCurrentAssertion }

  #if JUCE_LOG_ASSERTIONS
    #define jassert(expression)         { if (! (expression)) jassertfalse }
  #else
    #define jassert(a)                  { }
  #endif

#endif

#ifndef DOXYGEN
  template <bool b> struct JuceStaticAssert;
  template <> struct JuceStaticAssert <true> { static void dummy() {} };
#endif

/** A compile-time assertion macro.

    If the expression parameter is false, the macro will cause a compile error.
*/
#define static_jassert(expression)      JuceStaticAssert<expression>::dummy();

#if JUCE_CATCH_UNHANDLED_EXCEPTIONS

  #define JUCE_TRY try

  /** Used in try-catch blocks, this macro will send exceptions to the JUCEApplication
      object so they can be logged by the application if it wants to.
  */
  #define JUCE_CATCH_EXCEPTION \
    catch (const std::exception& e)  \
    { \
        JUCEApplication::sendUnhandledException (&e, __FILE__, __LINE__); \
    } \
    catch (...) \
    { \
        JUCEApplication::sendUnhandledException (0, __FILE__, __LINE__); \
    }

  #define JUCE_CATCH_ALL            catch (...) {}
  #define JUCE_CATCH_ALL_ASSERT     catch (...) { jassertfalse }

#else

  #define JUCE_TRY
  #define JUCE_CATCH_EXCEPTION
  #define JUCE_CATCH_ALL
  #define JUCE_CATCH_ALL_ASSERT

#endif

// Macros for inlining.

#if JUCE_MSVC
  /** A platform-independent way of forcing an inline function.

      Use the syntax: @code
      forcedinline void myfunction (int x)
      @endcode
  */
  #ifdef JUCE_DEBUG
    #define forcedinline  __forceinline
  #else
    #define forcedinline  inline
  #endif

  /** A platform-independent way of stopping the compiler inlining a function.

      Use the syntax: @code
      juce_noinline void myfunction (int x)
      @endcode
  */
  #define juce_noinline

#else
  /** A platform-independent way of forcing an inline function.

      Use the syntax: @code
      forcedinline void myfunction (int x)
      @endcode
  */
  #ifndef JUCE_DEBUG
    #define forcedinline  inline __attribute__((always_inline))
  #else
    #define forcedinline  inline
  #endif

  /** A platform-independent way of stopping the compiler inlining a function.

      Use the syntax: @code
      juce_noinline void myfunction (int x)
      @endcode
  */
  #define juce_noinline __attribute__((noinline))

#endif

#endif   // __JUCE_PLATFORMDEFS_JUCEHEADER__
/********* End of inlined file: juce_PlatformDefs.h *********/

// Now we'll include any OS headers we need.. (at this point we are outside the Juce namespace).
#if JUCE_MSVC
  #pragma warning (push)
  #pragma warning (disable: 4514 4245 4100)
#endif

#include <cstdlib>
#include <cstdarg>
#include <climits>
#include <cmath>
#include <cwchar>
#include <stdexcept>
#include <typeinfo>
#include <cstring>
#include <cstdio>

#if JUCE_MAC || JUCE_LINUX
  #include <pthread.h>
#endif

#if JUCE_USE_INTRINSICS
  #include <intrin.h>
#endif

#if JUCE_MAC
  #if MACOS_10_3_OR_EARLIER
    #include <CoreServices/CoreServices.h>
  #else
    #include <libkern/OSAtomic.h>
  #endif
#endif

#if JUCE_LINUX
  #include <signal.h>
#endif

#if JUCE_MSVC && JUCE_DEBUG
  #include <crtdbg.h>
#endif

#if JUCE_MSVC
  #pragma warning (pop)
#endif

// DLL building settings on Win32
#if JUCE_MSVC
  #ifdef JUCE_DLL_BUILD
    #define JUCE_API __declspec (dllexport)
    #pragma warning (disable: 4251)
  #elif defined (JUCE_DLL)
    #define JUCE_API __declspec (dllimport)
    #pragma warning (disable: 4251)
  #endif
#endif

#ifndef JUCE_API
  /** This macro is added to all juce public class declarations. */
  #define JUCE_API
#endif

/** This macro is added to all juce public function declarations. */
#define JUCE_PUBLIC_FUNCTION        JUCE_API JUCE_CALLTYPE

// Now include some basics that are needed by most of the Juce classes...
BEGIN_JUCE_NAMESPACE

extern bool JUCE_API JUCE_CALLTYPE juce_isRunningUnderDebugger() throw();

#if JUCE_LOG_ASSERTIONS
  extern void JUCE_API juce_LogAssertion (const char* filename, const int lineNum) throw();
#endif

/********* Start of inlined file: juce_Memory.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_MEMORY_JUCEHEADER__
#define __JUCE_MEMORY_JUCEHEADER__

/*
    This file defines the various juce_malloc(), juce_free() macros that should be used in
    preference to the standard calls.
*/

#if defined (JUCE_DEBUG) && JUCE_MSVC && JUCE_CHECK_MEMORY_LEAKS
  #ifndef JUCE_DLL

    // Win32 debug non-DLL versions..

    /** This should be used instead of calling malloc directly. */
    #define juce_malloc(numBytes)                 _malloc_dbg  (numBytes, _NORMAL_BLOCK, __FILE__, __LINE__)
    /** This should be used instead of calling calloc directly. */
    #define juce_calloc(numBytes)                 _calloc_dbg  (1, numBytes, _NORMAL_BLOCK, __FILE__, __LINE__)
    /** This should be used instead of calling realloc directly. */
    #define juce_realloc(location, numBytes)      _realloc_dbg (location, numBytes, _NORMAL_BLOCK, __FILE__, __LINE__)
    /** This should be used instead of calling free directly. */
    #define juce_free(location)                   _free_dbg    (location, _NORMAL_BLOCK)

  #else

    // Win32 debug DLL versions..

    // For the DLL, we'll define some functions in the DLL that will be used for allocation - that
    // way all juce calls in the DLL and in the host API will all use the same allocator.
    extern JUCE_API void* juce_DebugMalloc (const int size, const char* file, const int line);
    extern JUCE_API void* juce_DebugCalloc (const int size, const char* file, const int line);
    extern JUCE_API void* juce_DebugRealloc (void* const block, const int size, const char* file, const int line);
    extern JUCE_API void juce_DebugFree (void* const block);

    /** This should be used instead of calling malloc directly. */
    #define juce_malloc(numBytes)                 JUCE_NAMESPACE::juce_DebugMalloc (numBytes, __FILE__, __LINE__)
    /** This should be used instead of calling calloc directly. */
    #define juce_calloc(numBytes)                 JUCE_NAMESPACE::juce_DebugCalloc (numBytes, __FILE__, __LINE__)
    /** This should be used instead of calling realloc directly. */
    #define juce_realloc(location, numBytes)      JUCE_NAMESPACE::juce_DebugRealloc (location, numBytes, __FILE__, __LINE__)
    /** This should be used instead of calling free directly. */
    #define juce_free(location)                   JUCE_NAMESPACE::juce_DebugFree (location)
  #endif

  #if ! defined (_AFXDLL)
    /** This macro can be added to classes to add extra debugging information to the memory
        allocated for them, so you can see the type of objects involved when there's a dump
        of leaked objects at program shutdown. (Only works on win32 at the moment).
    */
    #define juce_UseDebuggingNewOperator \
      static void* operator new (size_t sz)           { void* const p = juce_malloc ((int) sz); return (p != 0) ? p : ::operator new (sz); } \
      static void* operator new (size_t sz, void* p)  { return ::operator new (sz, p); } \
      static void operator delete (void* p)           { juce_free (p); }
  #endif

#elif defined (JUCE_DLL)

  // Win32 DLL (release) versions..

  // For the DLL, we'll define some functions in the DLL that will be used for allocation - that
  // way all juce calls in the DLL and in the host API will all use the same allocator.
  extern JUCE_API void* juce_Malloc (const int size);
  extern JUCE_API void* juce_Calloc (const int size);
  extern JUCE_API void* juce_Realloc (void* const block, const int size);
  extern JUCE_API void juce_Free (void* const block);

  /** This should be used instead of calling malloc directly. */
  #define juce_malloc(numBytes)                 JUCE_NAMESPACE::juce_Malloc (numBytes)
  /** This should be used instead of calling calloc directly. */
  #define juce_calloc(numBytes)                 JUCE_NAMESPACE::juce_Calloc (numBytes)
  /** This should be used instead of calling realloc directly. */
  #define juce_realloc(location, numBytes)      JUCE_NAMESPACE::juce_Realloc (location, numBytes)
  /** This should be used instead of calling free directly. */
  #define juce_free(location)                   JUCE_NAMESPACE::juce_Free (location)

  #define juce_UseDebuggingNewOperator \
    static void* operator new (size_t sz)           { void* const p = juce_malloc ((int) sz); return (p != 0) ? p : ::operator new (sz); } \
    static void* operator new (size_t sz, void* p)  { return ::operator new (sz, p); } \
    static void operator delete (void* p)           { juce_free (p); }

#else

  // Mac, Linux and Win32 (release) versions..

  /** This should be used instead of calling malloc directly. */
  #define juce_malloc(numBytes)                 malloc (numBytes)
  /** This should be used instead of calling calloc directly. */
  #define juce_calloc(numBytes)                 calloc (1, numBytes)
  /** This should be used instead of calling realloc directly. */
  #define juce_realloc(location, numBytes)      realloc (location, numBytes)
  /** This should be used instead of calling free directly. */
  #define juce_free(location)                   free (location)

#endif

/** This macro can be added to classes to add extra debugging information to the memory
    allocated for them, so you can see the type of objects involved when there's a dump
    of leaked objects at program shutdown. (Only works on win32 at the moment).

    Note that if you create a class that inherits from a class that uses this macro,
    your class must also use the macro, otherwise you'll probably get compile errors
    because of ambiguous new operators.

    Most of the JUCE classes use it, so see these for examples of where it should go.
*/
#ifndef juce_UseDebuggingNewOperator
  #define juce_UseDebuggingNewOperator
#endif

#if JUCE_MSVC
  /** This is a compiler-indenpendent way of declaring a variable as being thread-local.

      E.g.
      @code
      juce_ThreadLocal int myVariable;
      @endcode
  */
  #define juce_ThreadLocal    __declspec(thread)
#else
  #define juce_ThreadLocal    __thread
#endif

/** Clears a block of memory. */
#define zeromem(memory, numBytes)               memset (memory, 0, numBytes)

/** Clears a reference to a local structure. */
#define zerostruct(structure)                   memset (&structure, 0, sizeof (structure))

/** A handy macro that calls delete on a pointer if it's non-zero, and
    then sets the pointer to null.
*/
#define deleteAndZero(pointer)                  { delete (pointer); (pointer) = 0; }

#endif   // __JUCE_MEMORY_JUCEHEADER__
/********* End of inlined file: juce_Memory.h *********/

/********* Start of inlined file: juce_MathsFunctions.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_MATHSFUNCTIONS_JUCEHEADER__
#define __JUCE_MATHSFUNCTIONS_JUCEHEADER__

/*
    This file sets up some handy mathematical typdefs and functions.
*/

// Definitions for the int8, int16, int32, int64 and pointer_sized_int types.

/** A platform-independent 8-bit signed integer type. */
typedef signed char                 int8;
/** A platform-independent 8-bit unsigned integer type. */
typedef unsigned char               uint8;
/** A platform-independent 16-bit signed integer type. */
typedef signed short                int16;
/** A platform-independent 16-bit unsigned integer type. */
typedef unsigned short              uint16;
/** A platform-independent 32-bit signed integer type. */
typedef signed int                  int32;
/** A platform-independent 32-bit unsigned integer type. */
typedef unsigned int                uint32;

#if JUCE_MSVC
  /** A platform-independent 64-bit integer type. */
  typedef __int64                   int64;
  /** A platform-independent 64-bit unsigned integer type. */
  typedef unsigned __int64          uint64;
  /** A platform-independent macro for writing 64-bit literals, needed because
      different compilers have different syntaxes for this.

      E.g. writing literal64bit (0x1000000000) will translate to 0x1000000000LL for
      GCC, or 0x1000000000 for MSVC.
  */
  #define literal64bit(longLiteral)     ((__int64) longLiteral)
#else
  /** A platform-independent 64-bit integer type. */
  typedef long long                 int64;
  /** A platform-independent 64-bit unsigned integer type. */
  typedef unsigned long long        uint64;
  /** A platform-independent macro for writing 64-bit literals, needed because
      different compilers have different syntaxes for this.

      E.g. writing literal64bit (0x1000000000) will translate to 0x1000000000LL for
      GCC, or 0x1000000000 for MSVC.
  */
  #define literal64bit(longLiteral)     (longLiteral##LL)
#endif

#if JUCE_64BIT
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef int64                     pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef uint64                    pointer_sized_uint;
#elif _MSC_VER >= 1300
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef _W64 int                  pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef _W64 unsigned int         pointer_sized_uint;
#else
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef int                       pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef unsigned int              pointer_sized_uint;
#endif

/** A platform-independent unicode character type. */
typedef wchar_t                     juce_wchar;

// Some indispensible min/max functions

/** Returns the larger of two values. */
forcedinline int    jmax (const int a, const int b) throw()                                   { return (a < b) ? b : a; }
/** Returns the larger of two values. */
forcedinline int64  jmax (const int64 a, const int64 b) throw()                               { return (a < b) ? b : a; }
/** Returns the larger of two values. */
forcedinline float  jmax (const float a, const float b) throw()                               { return (a < b) ? b : a; }
/** Returns the larger of two values. */
forcedinline double jmax (const double a, const double b) throw()                             { return (a < b) ? b : a; }

/** Returns the larger of three values. */
inline int    jmax (const int a, const int b, const int c) throw()                            { return (a < b) ? ((b < c) ? c : b) : ((a < c) ? c : a); }
/** Returns the larger of three values. */
inline int64  jmax (const int64 a, const int64 b, const int64 c) throw()                      { return (a < b) ? ((b < c) ? c : b) : ((a < c) ? c : a); }
/** Returns the larger of three values. */
inline float  jmax (const float a, const float b, const float c) throw()                      { return (a < b) ? ((b < c) ? c : b) : ((a < c) ? c : a); }
/** Returns the larger of three values. */
inline double jmax (const double a, const double b, const double c) throw()                   { return (a < b) ? ((b < c) ? c : b) : ((a < c) ? c : a); }

/** Returns the larger of four values. */
inline int    jmax (const int a, const int b, const int c, const int d) throw()               { return jmax (a, jmax (b, c, d)); }
/** Returns the larger of four values. */
inline int64  jmax (const int64 a, const int64 b, const int64 c, const int64 d) throw()       { return jmax (a, jmax (b, c, d)); }
/** Returns the larger of four values. */
inline float  jmax (const float a, const float b, const float c, const float d) throw()       { return jmax (a, jmax (b, c, d)); }
/** Returns the larger of four values. */
inline double jmax (const double a, const double b, const double c, const double d) throw()   { return jmax (a, jmax (b, c, d)); }

/** Returns the smaller of two values. */
inline int    jmin (const int a, const int b) throw()                                         { return (a > b) ? b : a; }
/** Returns the smaller of two values. */
inline int64  jmin (const int64 a, const int64 b) throw()                                     { return (a > b) ? b : a; }
/** Returns the smaller of two values. */
inline float  jmin (const float a, const float b) throw()                                     { return (a > b) ? b : a; }
/** Returns the smaller of two values. */
inline double jmin (const double a, const double b) throw()                                   { return (a > b) ? b : a; }

/** Returns the smaller of three values. */
inline int    jmin (const int a, const int b, const int c) throw()                            { return (a > b) ? ((b > c) ? c : b) : ((a > c) ? c : a); }
/** Returns the smaller of three values. */
inline int64  jmin (const int64 a, const int64 b, const int64 c) throw()                      { return (a > b) ? ((b > c) ? c : b) : ((a > c) ? c : a); }
/** Returns the smaller of three values. */
inline float  jmin (const float a, const float b, const float c) throw()                      { return (a > b) ? ((b > c) ? c : b) : ((a > c) ? c : a); }
/** Returns the smaller of three values. */
inline double jmin (const double a, const double b, const double c) throw()                   { return (a > b) ? ((b > c) ? c : b) : ((a > c) ? c : a); }

/** Returns the smaller of four values. */
inline int    jmin (const int a, const int b, const int c, const int d) throw()               { return jmin (a, jmin (b, c, d)); }
/** Returns the smaller of four values. */
inline int64  jmin (const int64 a, const int64 b, const int64 c, const int64 d) throw()       { return jmin (a, jmin (b, c, d)); }
/** Returns the smaller of four values. */
inline float  jmin (const float a, const float b, const float c, const float d) throw()       { return jmin (a, jmin (b, c, d)); }
/** Returns the smaller of four values. */
inline double jmin (const double a, const double b, const double c, const double d) throw()   { return jmin (a, jmin (b, c, d)); }

/** Constrains a value to keep it within a given range.

    This will check that the specified value lies between the lower and upper bounds
    specified, and if not, will return the nearest value that would be in-range. Effectively,
    it's like calling jmax (lowerLimit, jmin (upperLimit, value)).

    Note that it expects that lowerLimit <= upperLimit. If this isn't true,
    the results will be unpredictable.

    @param lowerLimit           the minimum value to return
    @param upperLimit           the maximum value to return
    @param valueToConstrain     the value to try to return
    @returns    the closest value to valueToConstrain which lies between lowerLimit
                and upperLimit (inclusive)
    @see jlimit0To, jmin, jmax
*/
template <class Type>
inline Type jlimit (const Type lowerLimit,
                    const Type upperLimit,
                    const Type valueToConstrain) throw()
{
    jassert (lowerLimit <= upperLimit); // if these are in the wrong order, results are unpredictable..

    return (valueToConstrain < lowerLimit) ? lowerLimit
                                           : ((valueToConstrain > upperLimit) ? upperLimit
                                                                              : valueToConstrain);
}

/** Handy function to swap two values over.
*/
template <class Type>
inline void swapVariables (Type& variable1, Type& variable2) throw()
{
    const Type tempVal = variable1;
    variable1 = variable2;
    variable2 = tempVal;
}

/** Handy macro for getting the number of elements in a simple const C array.

    E.g.
    @code
    static int myArray[] = { 1, 2, 3 };

    int numElements = numElementsInArray (myArray) // returns 3
    @endcode
*/
#define numElementsInArray(a)   ((int) (sizeof (a) / sizeof ((a)[0])))

// Some useful maths functions that aren't always present with all compilers and build settings.

#if JUCE_WIN32 || defined (DOXYGEN)
  /** Using juce_hypot and juce_hypotf is easier than dealing with all the different
      versions of these functions of various platforms and compilers. */
  forcedinline double juce_hypot (double a, double b)           { return _hypot (a, b); }

  /** Using juce_hypot and juce_hypotf is easier than dealing with all the different
      versions of these functions of various platforms and compilers. */
  forcedinline float juce_hypotf (float a, float b)             { return (float) _hypot (a, b); }
#elif MACOS_10_2_OR_EARLIER
  /** Using juce_hypot and juce_hypotf is easier than dealing with all the different
      versions of these functions of various platforms and compilers. */
  forcedinline double juce_hypot (double a, double b)           { return hypot (a, b); }

  /** Using juce_hypot and juce_hypotf is easier than dealing with all the different
      versions of these functions of various platforms and compilers. */
  forcedinline float juce_hypotf (float a, float b)             { return (float) hypot (a, b); }
  forcedinline float sinf (const float a)                       { return (float) sin (a); }
  forcedinline float cosf (const float a)                       { return (float) cos (a); }
  forcedinline float tanf (const float a)                       { return (float) tan (a); }
  forcedinline float atan2f (const float a, const float b)      { return (float) atan2 (a, b); }
  forcedinline float sqrtf (const float a)                      { return (float) sqrt (a); }
  forcedinline float logf (const float a)                       { return (float) log (a); }
  forcedinline float powf (const float a, const float b)        { return (float) pow (a, b); }
  forcedinline float expf (const float a)                       { return (float) exp (a); }
#else
  /** Using juce_hypot and juce_hypotf is easier than dealing with all the different
      versions of these functions of various platforms and compilers. */
  forcedinline double juce_hypot (double a, double b)           { return hypot (a, b); }

  /** Using juce_hypot and juce_hypotf is easier than dealing with all the different
      versions of these functions of various platforms and compilers. */
  forcedinline float juce_hypotf (float a, float b)             { return hypotf (a, b); }
#endif

inline int64 abs64 (const int64 n) throw()                      { return (n >= 0) ? n : -n; }

/** A predefined value for Pi, at double-precision.

    @see float_Pi
*/
const double  double_Pi  = 3.1415926535897932384626433832795;

/** A predefined value for Pi, at sngle-precision.

    @see double_Pi
*/
const float   float_Pi   = 3.14159265358979323846f;

/** The isfinite() method seems to vary greatly between platforms, so this is a
    platform-independent macro for it.
*/
#if JUCE_LINUX
  #define juce_isfinite(v)      std::isfinite(v)
#elif JUCE_MAC
  #if MACOS_10_2_OR_EARLIER
    #define juce_isfinite(v)    __isfinite(v)
  #elif MACOS_10_3_OR_EARLIER
    #ifdef isfinite
      #define juce_isfinite(v)    isfinite(v)
    #else
      // no idea why the isfinite macro is sometimes impossible to include, so just copy the built-in one..
      static __inline__ int juce_isfinite (double __x) { return __x == __x && __builtin_fabs (__x) != __builtin_inf(); }
    #endif
  #else
    #define juce_isfinite(v)    std::isfinite(v)
  #endif
#elif JUCE_WIN32 && ! defined (isfinite)
  #define juce_isfinite(v)      _finite(v)
#else
  #define juce_isfinite(v)      isfinite(v)
#endif

#endif   // __JUCE_MATHSFUNCTIONS_JUCEHEADER__
/********* End of inlined file: juce_MathsFunctions.h *********/

/********* Start of inlined file: juce_DataConversions.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_DATACONVERSIONS_JUCEHEADER__
#define __JUCE_DATACONVERSIONS_JUCEHEADER__

/* #include "juce_PlatformDefs.h" */

#if JUCE_USE_INTRINSICS
  #pragma intrinsic (_byteswap_ulong)
#endif

// Endianness conversions..

/** Swaps the byte-order in an integer from little to big-endianness or vice-versa. */
forcedinline uint32 swapByteOrder (uint32 n) throw()
{
#if JUCE_MAC
    // Mac version
    return CFSwapInt32 (n);
#elif JUCE_GCC
    // Inpenetrable GCC version..
    asm("bswap %%eax" : "=a"(n) : "a"(n));
    return n;
#elif JUCE_USE_INTRINSICS
    // Win32 intrinsics version..
    return _byteswap_ulong (n);
#else
    // Win32 version..
    __asm {
        mov eax, n
        bswap eax
        mov n, eax
    }
    return n;
#endif
}

/** Swaps the byte-order of a 16-bit short. */
inline uint16 swapByteOrder (const uint16 n) throw()
{
#if JUCE_USE_INTRINSICSxxx // agh - the MS compiler has an internal error when you try to use this intrinsic!
    // Win32 intrinsics version..
    return (uint16) _byteswap_ushort (n);
#else
    return (uint16) ((n << 8) | (n >> 8));
#endif
}

inline uint64 swapByteOrder (const uint64 value) throw()
{
#if JUCE_MAC
    return CFSwapInt64 (value);
#elif JUCE_USE_INTRINSICS
    return _byteswap_uint64 (value);
#else
    return (((int64) swapByteOrder ((uint32) value)) << 32)
            | swapByteOrder ((uint32) (value >> 32));
#endif
}

#if JUCE_LITTLE_ENDIAN
  /** Swaps the byte order of a 16-bit int if the CPU is big-endian */
  inline uint16     swapIfBigEndian (const uint16 v) throw()             { return v; }
  /** Swaps the byte order of a 32-bit int if the CPU is big-endian */
  inline uint32     swapIfBigEndian (const uint32 v) throw()             { return v; }
  /** Swaps the byte order of a 64-bit int if the CPU is big-endian */
  inline uint64     swapIfBigEndian (const uint64 v) throw()             { return v; }

  /** Swaps the byte order of a 16-bit int if the CPU is little-endian */
  inline uint16     swapIfLittleEndian (const uint16 v) throw()          { return swapByteOrder (v); }
  /** Swaps the byte order of a 32-bit int if the CPU is little-endian */
  inline uint32     swapIfLittleEndian (const uint32 v) throw()          { return swapByteOrder (v); }
  /** Swaps the byte order of a 64-bit int if the CPU is little-endian */
  inline uint64     swapIfLittleEndian (const uint64 v) throw()          { return swapByteOrder (v); }

  /** Turns 4 bytes into a little-endian integer. */
  inline uint32     littleEndianInt (const char* const bytes) throw()    { return *(uint32*) bytes; }
  /** Turns 2 bytes into a little-endian integer. */
  inline uint16     littleEndianShort (const char* const bytes) throw()  { return *(uint16*) bytes; }

  /** Turns 4 bytes into a big-endian integer. */
  inline uint32     bigEndianInt (const char* const bytes) throw()       { return swapByteOrder (*(uint32*) bytes); }
  /** Turns 2 bytes into a big-endian integer. */
  inline uint16     bigEndianShort (const char* const bytes) throw()     { return swapByteOrder (*(uint16*) bytes); }

#else
  /** Swaps the byte order of a 16-bit int if the CPU is big-endian */
  inline uint16     swapIfBigEndian (const uint16 v) throw()             { return swapByteOrder (v); }
  /** Swaps the byte order of a 32-bit int if the CPU is big-endian */
  inline uint32     swapIfBigEndian (const uint32 v) throw()             { return swapByteOrder (v); }
  /** Swaps the byte order of a 64-bit int if the CPU is big-endian */
  inline uint64     swapIfBigEndian (const uint64 v) throw()             { return swapByteOrder (v); }

  /** Swaps the byte order of a 16-bit int if the CPU is little-endian */
  inline uint16     swapIfLittleEndian (const uint16 v) throw()          { return v; }
  /** Swaps the byte order of a 32-bit int if the CPU is little-endian */
  inline uint32     swapIfLittleEndian (const uint32 v) throw()          { return v; }
  /** Swaps the byte order of a 64-bit int if the CPU is little-endian */
  inline uint64     swapIfLittleEndian (const uint64 v) throw()          { return v; }

  /** Turns 4 bytes into a little-endian integer. */
  inline uint32     littleEndianInt (const char* const bytes) throw()    { return swapByteOrder (*(uint32*) bytes); }
  /** Turns 2 bytes into a little-endian integer. */
  inline uint16     littleEndianShort (const char* const bytes) throw()  { return swapByteOrder (*(uint16*) bytes); }

  /** Turns 4 bytes into a big-endian integer. */
  inline uint32     bigEndianInt (const char* const bytes) throw()       { return *(uint32*) bytes; }
  /** Turns 2 bytes into a big-endian integer. */
  inline uint16     bigEndianShort (const char* const bytes) throw()     { return *(uint16*) bytes; }
#endif

/** Converts 3 little-endian bytes into a signed 24-bit value (which is sign-extended to 32 bits). */
inline int littleEndian24Bit (const char* const bytes) throw()                          { return (((int) bytes[2]) << 16) | (((uint32) (uint8) bytes[1]) << 8) | ((uint32) (uint8) bytes[0]); }
/** Converts 3 big-endian bytes into a signed 24-bit value (which is sign-extended to 32 bits). */
inline int bigEndian24Bit (const char* const bytes) throw()                             { return (((int) bytes[0]) << 16) | (((uint32) (uint8) bytes[1]) << 8) | ((uint32) (uint8) bytes[2]); }

/** Copies a 24-bit number to 3 little-endian bytes. */
inline void littleEndian24BitToChars (const int value, char* const destBytes) throw()   { destBytes[0] = (char)(value & 0xff); destBytes[1] = (char)((value >> 8) & 0xff); destBytes[2] = (char)((value >> 16) & 0xff); }
/** Copies a 24-bit number to 3 big-endian bytes. */
inline void bigEndian24BitToChars (const int value, char* const destBytes) throw()      { destBytes[0] = (char)((value >> 16) & 0xff); destBytes[1] = (char)((value >> 8) & 0xff); destBytes[2] = (char)(value & 0xff); }

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a double to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently. For a more accurate conversion,
    see roundDoubleToIntAccurate().
*/
inline int roundDoubleToInt (const double value) throw()
{
    union { int asInt[2]; double asDouble; } n;
    n.asDouble = value + 6755399441055744.0;

#if JUCE_BIG_ENDIAN
    return n.asInt [1];
#else
    return n.asInt [0];
#endif
}

/** Fast floating-point-to-integer conversion.

    This is a slightly slower and slightly more accurate version of roundDoubleToInt(). It works
    fine for values above zero, but negative numbers are rounded the wrong way.
*/
inline int roundDoubleToIntAccurate (const double value) throw()
{
    return roundDoubleToInt (value + 1.5e-8);
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
*/
inline int roundFloatToInt (const float value) throw()
{
    union { int asInt[2]; double asDouble; } n;
    n.asDouble = value + 6755399441055744.0;

#if JUCE_BIG_ENDIAN
    return n.asInt [1];
#else
    return n.asInt [0];
#endif
}

#endif   // __JUCE_DATACONVERSIONS_JUCEHEADER__
/********* End of inlined file: juce_DataConversions.h *********/

/********* Start of inlined file: juce_Logger.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_LOGGER_JUCEHEADER__
#define __JUCE_LOGGER_JUCEHEADER__

//#include "../text/juce_String.h"

/**
    Acts as an application-wide logging class.

    A subclass of Logger can be created and passed into the Logger::setCurrentLogger
    method and this will then be used by all calls to writeToLog.

    The logger class also contains methods for writing messages to the debugger's
    output stream.

    @see FileLogger
*/
class JUCE_API  Logger
{
public:

    /** Destructor. */
    virtual ~Logger();

    /** Sets the current logging class to use.

        Note that the object passed in won't be deleted when no longer needed.
        A null pointer can be passed-in to disable any logging.

        If deleteOldLogger is set to true, the existing logger will be
        deleted (if there is one).
    */
    static void JUCE_CALLTYPE setCurrentLogger (Logger* const newLogger,
                                                const bool deleteOldLogger = false);

    /** Writes a string to the current logger.

        This will pass the string to the logger's logMessage() method if a logger
        has been set.

        @see logMessage
    */
    static void JUCE_CALLTYPE writeToLog (const String& message);

    /** Writes a message to the standard error stream.

        This can be called directly, or by using the DBG() macro in
        juce_PlatformDefs.h (which will avoid calling the method in non-debug builds).
    */
    static void JUCE_CALLTYPE outputDebugString (const String& text) throw();

    /** Writes a message to the standard error stream.

        This can be called directly, or by using the DBG_PRINTF() macro in
        juce_PlatformDefs.h (which will avoid calling the method in non-debug builds).
    */
    static void JUCE_CALLTYPE outputDebugPrintf (const tchar* format, ...) throw();

protected:

    Logger();

    /** This is overloaded by subclasses to implement custom logging behaviour.

        @see setCurrentLogger
    */
    virtual void logMessage (const String& message) = 0;
};

#endif   // __JUCE_LOGGER_JUCEHEADER__
/********* End of inlined file: juce_Logger.h *********/

END_JUCE_NAMESPACE

#endif   // __JUCE_STANDARDHEADER_JUCEHEADER__
/********* End of inlined file: juce_StandardHeader.h *********/

/* #include "juce_Memory.h" */
/* #include "juce_MathsFunctions.h" */
/* #include "juce_DataConversions.h" */
/* #include "juce_Logger.h" */
/* #include "juce_PlatformDefs.h" */

/********* Start of inlined file: juce_String.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_STRING_JUCEHEADER__
#define __JUCE_STRING_JUCEHEADER__

/********* Start of inlined file: juce_CharacterFunctions.h *********/
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_CHARACTERFUNCTIONS_JUCEHEADER__
#define __JUCE_CHARACTERFUNCTIONS_JUCEHEADER__

/* The String class can either use wchar_t unicode characters, or 8-bit characters
   (in the default system encoding) as its internal representation.

   To use unicode, define the JUCE_STRINGS_ARE_UNICODE macro in juce_Config.h

   Be sure to use "tchar" for characters rather than "char", and always wrap string
   literals in the T("abcd") macro, so that it all works nicely either way round.
*/
#if JUCE_STRINGS_ARE_UNICODE

  #define JUCE_T(stringLiteral)     (L##stringLiteral)
  typedef juce_wchar                tchar;
  #define juce_tcharToWideChar(c)   (c)

#else

  #define JUCE_T(stringLiteral)     (stringLiteral)
  typedef char                      tchar;
  #define juce_tcharToWideChar(c)   ((juce_wchar) (unsigned char) (c))

#endif

#if ! JUCE_DONT_DEFINE_MACROS

/** The 'T' macro allows a literal string to be compiled using either 8-bit characters
    or unicode.

    If you write your string literals in the form T("xyz"), this will either be compiled
    as "xyz" for non-unicode builds, or L"xyz" for unicode builds, depending on whether the
    JUCE_STRINGS_ARE_UNICODE macro has been set in juce_Config.h

    Because the 'T' symbol is occasionally used inside 3rd-party library headers which you
    may need to include after juce.h, you can use the juce_withoutMacros.h file (in
    the juce/src directory) to avoid defining this macro. See the comments in
    juce_withoutMacros.h for more info.
*/
#define T(stringLiteral)            JUCE_T(stringLiteral)

#endif

/**
    A set of methods for manipulating characters and character strings, with
    duplicate methods to handle 8-bit and unicode characters.

    These are defined as wrappers around the basic C string handlers, to provide
    a clean, cross-platform layer, (because various platforms differ in the
    range of C library calls that they provide).

    @see String
*/
class JUCE_API  CharacterFunctions
{
public:
    static int length (const char* const s) throw();
    static int length (const juce_wchar* const s) throw();

    static void copy (char* dest, const char* src, const int maxBytes) throw();
    static void copy (juce_wchar* dest, const juce_wchar* src, const int maxChars) throw();

    static void copy (juce_wchar* dest, const char* src, const int maxChars) throw();
    static void copy (char* dest, const juce_wchar* src, const int maxBytes) throw();
    static int bytesRequiredForCopy (const juce_wchar* src) throw();

    static void append (char* dest, const char* src) throw();
    static void append (juce_wchar* dest, const juce_wchar* src) throw();

    static int compare (const char* const s1, const char* const s2) throw();
    static int compare (const juce_wchar* s1, const juce_wchar* s2) throw();

    static int compare (const char* const s1, const char* const s2, const int maxChars) throw();
    static int compare (const juce_wchar* s1, const juce_wchar* s2, int maxChars) throw();

    static int compareIgnoreCase (const char* const s1, const char* const s2) throw();
    static int compareIgnoreCase (const juce_wchar* s1, const juce_wchar* s2) throw();

    static int compareIgnoreCase (const char* const s1, const char* const s2, const int maxChars) throw();
    static int compareIgnoreCase (const juce_wchar* s1, const juce_wchar* s2, int maxChars) throw();

    static const char* find (const char* const haystack, const char* const needle) throw();
    static const juce_wchar* find (const juce_wchar* haystack, const juce_wchar* const needle) throw();

    static int indexOfChar (const char* const haystack, const char needle, const bool ignoreCase) throw();
    static int indexOfChar (const juce_wchar* const haystack, const juce_wchar needle, const bool ignoreCase) throw();

    static int indexOfCharFast (const char* const haystack, const char needle) throw();
    static int indexOfCharFast (const juce_wchar* const haystack, const juce_wchar needle) throw();

    static int getIntialSectionContainingOnly (const char* const text, const char* const allowedChars) throw();
    static int getIntialSectionContainingOnly (const juce_wchar* const text, const juce_wchar* const allowedChars) throw();

    static int ftime (char* const dest, const int maxChars, const char* const format, const struct tm* const tm) throw();
    static int ftime (juce_wchar* const dest, const int maxChars, const juce_wchar* const format, const struct tm* const tm) throw();

    static int getIntValue (const char* const s) throw();
    static int getIntValue (const juce_wchar* s) throw();

    static int64 getInt64Value (const char* s) throw();
    static int64 getInt64Value (const juce_wchar* s) throw();

    static double getDoubleValue (const char* const s) throw();
    static double getDoubleValue (const juce_wchar* const s) throw();

    static char toUpperCase (const char character) throw();
    static juce_wchar toUpperCase (const juce_wchar character) throw();
    static void toUpperCase (char* s) throw();

    static void toUpperCase (juce_wchar* s) throw();
    static bool isUpperCase (const char character) throw();
    static bool isUpperCase (const juce_wchar character) throw();

    static char toLowerCase (const char character) throw();
    static juce_wchar toLowerCase (const juce_wchar character) throw();
    static void toLowerCase (char* s) throw();
    static void toLowerCase (juce_wchar* s) throw();
    static bool isLowerCase (const char character) throw();
    static bool isLowerCase (const juce_wchar character) throw();

    static bool isWhitespace (const char character) throw();
    static bool isWhitespace (const juce_wchar character) throw();

    static bool isDigit (const char character) throw();
    static bool isDigit (const juce_wchar character) throw();

    static bool isLetter (const char character) throw();
    static bool isLetter (const juce_wchar character) throw();

    static bool isLetterOrDigit (const char character) throw();
    static bool isLetterOrDigit (const juce_wchar character) throw();

    /** Returns 0 to 16 for '0' to 'F", or -1 for characters that aren't a legel
        hex digit.
    */
    static int getHexDigitValue (const tchar digit) throw();

    static int printf (char* const dest, const int maxLength, const char* const format, ...) throw();
    static int printf (juce_wchar* const dest, const int maxLength, const juce_wchar* const format, ...) throw();

    static int vprintf (char* const dest, const int maxLength, const char* const format, va_list& args) throw();
    static int vprintf (juce_wchar* const dest, const int maxLength, const juce_wchar* const format, va_list& args) throw();
};

#endif   // __JUCE_CHARACTERFUNCTIONS_JUCEHEADER__
/********* End of inlined file: juce_CharacterFunctions.h *********/

/**
    The JUCE String class!

    Using a reference-counted internal representation, these strings are fast
    and efficient, and there are methods to do just about any operation you'll ever
    dream of.

    @see StringArray, StringPairArray
*/
class JUCE_API  String
{
public:

    /** Creates an empty string.

        @see empty
    */
    String() throw();

    /** Creates a copy of another string. */
    String (const String& other) throw();

    /** Creates a string from a zero-terminated text string.

        The string is assumed to be stored in the default system encoding.
    */
    String (const char* const text) throw();

    /** Creates a string from an string of characters.

        This will use up the the first maxChars characters of the string (or
        less if the string is actually shorter)
    */
    String (const char* const text,
            const int maxChars) throw();

    /** Creates a string from a zero-terminated unicode text string. */
    String (const juce_wchar* const unicodeText) throw();

    /** Creates a string from a unicode text string.

        This will use up the the first maxChars characters of the string (or
        less if the string is actually shorter)
    */
    String (const juce_wchar* const unicodeText,
            const int maxChars) throw();

    /** Creates a string from a single character. */
    static const String charToString (const tchar character) throw();

    /** Destructor. */
    ~String() throw();

    /** This is an empty string that can be used whenever one is needed.

        It's better to use this than String() because it explains what's going on
        and is more efficient.
    */
    static const String empty;

    /** Generates a probably-unique 32-bit hashcode from this string. */
    int hashCode() const throw();

    /** Generates a probably-unique 64-bit hashcode from this string. */
    int64 hashCode64() const throw();

    /** Returns the number of characters in the string. */
    int length() const throw();

    // Assignment and concatenation operators..

    /** Replaces this string's contents with another string. */
    const String& operator= (const tchar* const other) throw();

    /** Replaces this string's contents with another string. */
    const String& operator= (const String& other) throw();

    /** Appends another string at the end of this one. */
    const String& operator+= (const tchar* const textToAppend) throw();
    /** Appends another string at the end of this one. */
    const String& operator+= (const String& stringToAppend) throw();
    /** Appends a character at the end of this string. */
    const String& operator+= (const char characterToAppend) throw();
    /** Appends a character at the end of this string. */
    const String& operator+= (const juce_wchar characterToAppend) throw();

    /** Appends a string at the end of this one.

        @param textToAppend     the string to add
        @param maxCharsToTake   the maximum number of characters to take from the string passed in
    */
    void append (const tchar* const textToAppend,
                 const int maxCharsToTake) throw();

    /** Appends a string at the end of this one.
        @returns     the concatenated string
    */
    const String operator+ (const String& stringToAppend) const throw();

    /** Appends a string at the end of this one.
        @returns     the concatenated string
    */
    const String operator+ (const tchar* const textToAppend) const throw();

    /** Appends a character at the end of this one.
        @returns     the concatenated string
    */
    const String operator+ (const tchar characterToAppend) const throw();

    /** Appends a character at the end of this string. */
    String& operator<< (const char n) throw();
    /** Appends a character at the end of this string. */
    String& operator<< (const juce_wchar n) throw();
    /** Appends another string at the end of this one. */
    String& operator<< (const char* const text) throw();
    /** Appends another string at the end of this one. */
    String& operator<< (const juce_wchar* const text) throw();
    /** Appends another string at the end of this one. */
    String& operator<< (const String& text) throw();

    /** Appends a decimal number at the end of this string. */
    String& operator<< (const short number) throw();
    /** Appends a decimal number at the end of this string. */
    String& operator<< (const int number) throw();
    /** Appends a decimal number at the end of this string. */
    String& operator<< (const unsigned int number) throw();
    /** Appends a decimal number at the end of this string. */
    String& operator<< (const float number) throw();
    /** Appends a decimal number at the end of this string. */
    String& operator<< (const double number) throw();

    // Comparison methods..

    /** Returns true if the string contains no characters.

        Note that there's also an isNotEmpty() method to help write readable code.

        @see containsNonWhitespaceChars()
    */
    inline bool isEmpty() const throw()                     { return text->text[0] == 0; }

    /** Returns true if the string contains at least one character.

        Note that there's also an isEmpty() method to help write readable code.

        @see containsNonWhitespaceChars()
    */
    inline bool isNotEmpty() const throw()                  { return text->text[0] != 0; }

    /** Case-sensitive comparison with another string. */
    bool operator== (const String& other) const throw();
    /** Case-sensitive comparison with another string. */
    bool operator== (const tchar* const other) const throw();

    /** Case-sensitive comparison with another string. */
    bool operator!= (const String& other) const throw();
    /** Case-sensitive comparison with another string. */
    bool operator!= (const tchar* const other) const throw();

    /** Case-insensitive comparison with another string. */
    bool equalsIgnoreCase (const String& other) const throw();
    /** Case-insensitive comparison with another string. */
    bool equalsIgnoreCase (const tchar* const other) const throw();

    /** Case-sensitive comparison with another string. */
    bool operator> (const String& other) const throw();
    /** Case-sensitive comparison with another string. */
    bool operator< (const tchar* const other) const throw();

    /** Case-sensitive comparison with another string. */
    bool operator>= (const String& other) const throw();
    /** Case-sensitive comparison with another string. */
    bool operator<= (const tchar* const other) const throw();

    /** Case-sensitive comparison with another string.
        @returns     0 if the two strings are identical; negative if this string
                     comes before the other one alphabetically, or positive if it
                     comes after it.
    */
    int compare (const tchar* const other) const throw();

    /** Case-insensitive comparison with another string.
        @returns     0 if the two strings are identical; negative if this string
                     comes before the other one alphabetically, or positive if it
                     comes after it.
    */
    int compareIgnoreCase (const tchar* const other) const throw();

    /** Lexicographic comparison with another string.

        The comparison used here is case-insensitive and ignores leading non-alphanumeric
        characters, making it good for sorting human-readable strings.

        @returns     0 if the two strings are identical; negative if this string
                     comes before the other one alphabetically, or positive if it
                     comes after it.
    */
    int compareLexicographically (const tchar* const other) const throw();

    /** Tests whether the string begins with another string.

        Uses a case-sensitive comparison.
    */
    bool startsWith (const tchar* const text) const throw();

    /** Tests whether the string begins with a particular character.

        Uses a case-sensitive comparison.
    */
    bool startsWithChar (const tchar character) const throw();

    /** Tests whether the string begins with another string.

        Uses a case-insensitive comparison.
    */
    bool startsWithIgnoreCase (const tchar* const text) const throw();

    /** Tests whether the string ends with another string.

        Uses a case-sensitive comparison.
    */
    bool endsWith (const tchar* const text) const throw();

    /** Tests whether the string ends with a particular character.

        Uses a case-sensitive comparison.
    */
    bool endsWithChar (const tchar character) const throw();

    /** Tests whether the string ends with another string.

        Uses a case-insensitive comparison.
    */
    bool endsWithIgnoreCase (const tchar* const text) const throw();

    /** Tests whether the string contains another substring.

        Uses a case-sensitive comparison.
    */
    bool contains (const tchar* const text) const throw();

    /** Tests whether the string contains a particular character.

        Uses a case-sensitive comparison.
    */
    bool containsChar (const tchar character) const throw();

    /** Tests whether the string contains another substring.

        Uses a case-insensitive comparison.
    */
    bool containsIgnoreCase (const tchar* const text) const throw();

    /** Tests whether the string contains another substring as a distict word.

        @returns    true if the string contains this word, surrounded by
                    non-alphanumeric characters
        @see indexOfWholeWord, containsWholeWordIgnoreCase
    */
    bool containsWholeWord (const tchar* const wordToLookFor) const throw();

    /** Tests whether the string contains another substring as a distict word.

        @returns    true if the string contains this word, surrounded by
                    non-alphanumeric characters
        @see indexOfWholeWordIgnoreCase, containsWholeWord
    */
    bool containsWholeWordIgnoreCase (const tchar* const wordToLookFor) const throw();

    /** Finds an instance of another substring if it exists as a distict word.

        @returns    if the string contains this word, surrounded by non-alphanumeric characters,
                    then this will return the index of the start of the substring. If it isn't
                    found, then it will return -1
        @see indexOfWholeWordIgnoreCase, containsWholeWord
    */
    int indexOfWholeWord (const tchar* const wordToLookFor) const throw();

    /** Finds an instance of another substring if it exists as a distict word.

        @returns    if the string contains this word, surrounded by non-alphanumeric characters,
                    then this will return the index of the start of the substring. If it isn't
                    found, then it will return -1
        @see indexOfWholeWord, containsWholeWordIgnoreCase
    */
    int indexOfWholeWordIgnoreCase (const tchar* const wordToLookFor) const throw();

    /** Looks for any of a set of characters in the string.

        Uses a case-sensitive comparison.

        @returns    true if the string contains any of the characters from
                    the string that is passed in.
    */
    bool containsAnyOf (const tchar* const charactersItMightContain) const throw();

    /** Looks for a set of characters in the string.

        Uses a case-sensitive comparison.

        @returns    true if the all the characters in the string are also found in the
                    string that is passed in.
    */
    bool containsOnly (const tchar* const charactersItMightContain) const throw();

    /** Returns true if this string contains any non-whitespace characters.

        This will return false if the string contains only whitespace characters, or
        if it's empty.

        It is equivalent to calling "myString.trim().isNotEmpty()".
    */
    bool containsNonWhitespaceChars() const throw();

    /** Returns true if the string matches this simple wildcard expression.

        So for example String ("abcdef").matchesWildcard ("*DEF", true) would return true.

        This isn't a full-blown regex though! The only wildcard characters supported
        are "*" and "?". It's mainly intended for filename pattern matching.
    */
    bool matchesWildcard (const tchar* wildcard, const bool ignoreCase) const throw();

    // Substring location methods..

    /** Searches for a character inside this string.

        Uses a case-sensitive comparison.

        @returns    the index of the first occurrence of the character in this
                    string, or -1 if it's not found.
    */
    int indexOfChar (const tchar characterToLookFor) const throw();

    /** Searches for a character inside this string.

        Uses a case-sensitive comparison.

        @param startIndex           the index from which the search should proceed
        @param characterToLookFor   the character to look for
        @returns            the index of the first occurrence of the character in this
                            string, or -1 if it's not found.
    */
    int indexOfChar (const int startIndex, const tchar characterToLookFor) const throw();

    /** Returns the index of the first character that matches one of the characters
        passed-in to this method.

        This scans the string, beginning from the startIndex supplied, and if it finds
        a character that appears in the string charactersToLookFor, it returns its index.

        If none of these characters are found, it returns -1.

        If ignoreCase is true, the comparison will be case-insensitive.

        @see indexOfChar, lastIndexOfAnyOf
    */
    int indexOfAnyOf (const tchar* const charactersToLookFor,
                      const int startIndex = 0,
                      const bool ignoreCase = false) const throw();

    /** Searches for a substring within this string.

        Uses a case-sensitive comparison.

        @returns    the index of the first occurrence of this substring, or -1 if it's not found.
    */
    int indexOf (const tchar* const text) const throw();

    /** Searches for a substring within this string.

        Uses a case-sensitive comparison.

        @param startIndex       the index from which the search should proceed
        @param textToLookFor    the string to search for
        @returns                the index of the first occurrence of this substring, or -1 if it's not found.
    */
    int indexOf (const int startIndex,
                 const tchar* const textToLookFor) const throw();

    /** Searches for a substring within this string.

        Uses a case-insensitive comparison.

        @returns    the index of the first occurrence of this substring, or -1 if it's not found.
    */
    int indexOfIgnoreCase (const tchar* const textToLookFor) const throw();

    /** Searches for a substring within this string.

        Uses a case-insensitive comparison.

        @param startIndex       the index from which the search should proceed
        @param textToLookFor    the string to search for
        @returns                the index of the first occurrence of this substring, or -1 if it's not found.
    */
    int indexOfIgnoreCase (const int startIndex,
                           const tchar* const textToLookFor) const throw();

    /** Searches for a character inside this string (working backwards from the end of the string).

        Uses a case-sensitive comparison.

        @returns            the index of the last occurrence of the character in this
                            string, or -1 if it's not found.
    */
    int lastIndexOfChar (const tchar character) const throw();

    /** Searches for a substring inside this string (working backwards from the end of the string).

        Uses a case-sensitive comparison.

        @returns            the index of the start of the last occurrence of the
                            substring within this string, or -1 if it's not found.
    */
    int lastIndexOf (const tchar* const textToLookFor) const throw();

    /** Searches for a substring inside this string (working backwards from the end of the string).

        Uses a case-insensitive comparison.

        @returns            the index of the start of the last occurrence of the
                            substring within this string, or -1 if it's not found.
    */
    int lastIndexOfIgnoreCase (const tchar* const textToLookFor) const throw();

    /** Returns the index of the last character in this string that matches one of the
        characters passed-in to this method.

        This scans the string backwards, starting from its end, and if it finds
        a character that appears in the string charactersToLookFor, it returns its index.

        If none of these characters are found, it returns -1.

        If ignoreCase is true, the comparison will be case-insensitive.

        @see lastIndexOf, indexOfAnyOf
    */
    int lastIndexOfAnyOf (const tchar* const charactersToLookFor,
                          const bool ignoreCase = false) const throw();

    // Substring extraction and manipulation methods..

    /** Returns the character at this index in the string.

        No checks are made to see if the index is within a valid range, so be careful!
    */
    inline const tchar& operator[] (const int index) const throw()  { jassert (((unsigned int) index) <= (unsigned int) length()); return text->text [index]; }

    /** Returns a character from the string such that it can also be altered.

        This can be used as a way of easily changing characters in the string.

        Note that the index passed-in is not checked to see whether it's in-range, so
        be careful when using this.
    */
    tchar& operator[] (const int index) throw();

    /** Returns the final character of the string.

        If the string is empty this will return 0.
    */
    tchar getLastCharacter() const throw();

    /** Returns a subsection of the string.

        If the range specified is beyond the limits of the string, as much as
        possible is returned.

        @param startIndex   the index of the start of the substring needed
        @param endIndex     all characters from startIndex up to (but not including)
                            this index are returned
        @see fromFirstOccurrenceOf, dropLastCharacters, upToFirstOccurrenceOf
    */
    const String substring (int startIndex,
                            int endIndex) const throw();

    /** Returns a section of the string, starting from a given position.

        @param startIndex   the first character to include. If this is beyond the end
                            of the string, an empty string is returned. If it is zero or
                            less, the whole string is returned.
        @returns            the substring from startIndex up to the end of the string
        @see dropLastCharacters, fromFirstOccurrenceOf, upToFirstOccurrenceOf, fromLastOccurrenceOf
    */
    const String substring (const int startIndex) const throw();

    /** Returns a version of this string with a number of characters removed
        from the end.

        @param numberToDrop     the number of characters to drop from the end of the
                                string. If this is greater than the length of the string,
                                an empty string will be returned. If zero or less, the
                                original string will be returned.
        @see substring, fromFirstOccurrenceOf, upToFirstOccurrenceOf, fromLastOccurrenceOf, getLastCharacter
    */
    const String dropLastCharacters (const int numberToDrop) const throw();

    /** Returns a section of the string starting from a given substring.

        This will search for the first occurrence of the given substring, and
        return the section of the string starting from the point where this is
        found (optionally not including the substring itself).

        e.g. for the string "123456", fromFirstOccurrenceOf ("34", true) would return "3456", and
                                      fromFirstOccurrenceOf ("34", false) would return "56".

        If the substring isn't found, the method will return an empty string.

        If ignoreCase is true, the comparison will be case-insensitive.

        @see upToFirstOccurrenceOf, fromLastOccurrenceOf
    */
    const String fromFirstOccurrenceOf (const tchar* const substringToStartFrom,
                                        const bool includeSubStringInResult,
                                        const bool ignoreCase) const throw();

    /** Returns a section of the string starting from the last occurrence of a given substring.

        Similar to fromFirstOccurrenceOf(), but using the last occurrence of the substring, and
        unlike fromFirstOccurrenceOf(), if the substring isn't found, this method will
        return the whole of the original string.

        @see fromFirstOccurrenceOf, upToLastOccurrenceOf
    */
    const String fromLastOccurrenceOf (const tchar* const substringToFind,
                                       const bool includeSubStringInResult,
                                       const bool ignoreCase) const throw();

    /** Returns the start of this string, up to the first occurrence of a substring.

        This will search for the first occurrence of a given substring, and then
        return a copy of the string, up to the position of this substring,
        optionally including or excluding the substring itself in the result.

        e.g. for the string "123456", upTo ("34", false) would return "12", and
                                      upTo ("34", true) would return "1234".

        If the substring isn't found, this will return the whole of the original string.

        @see upToLastOccurrenceOf, fromFirstOccurrenceOf
    */
    const String upToFirstOccurrenceOf (const tchar* const substringToEndWith,
                                        const bool includeSubStringInResult,
                                        const bool ignoreCase) const throw();

    /** Returns the start of this string, up to the last occurrence of a substring.

        Similar to upToFirstOccurrenceOf(), but this finds the last occurrence rather than the first.

        @see upToFirstOccurrenceOf, fromFirstOccurrenceOf
    */
    const String upToLastOccurrenceOf (const tchar* substringToFind,
                                       const bool includeSubStringInResult,
                                       const bool ignoreCase) const throw();

    /** Returns a copy of this string with any whitespace characters removed from the start and end. */
    const String trim() const throw();
    /** Returns a copy of this string with any whitespace characters removed from the start. */
    const String trimStart() const throw();
    /** Returns a copy of this string with any whitespace characters removed from the end. */
    const String trimEnd() const throw();

    /** Returns an upper-case version of this string. */
    const String toUpperCase() const throw();

    /** Returns an lower-case version of this string. */
    const String toLowerCase() const throw();

    /** Replaces a sub-section of the string with another string.

        This will return a copy of this string, with a set of characters
        from startIndex to startIndex + numCharsToReplace removed, and with
        a new string inserted in their place.

        Note that this is a const method, and won't alter the string itself.

        @param startIndex               the first character to remove. If this is beyond the bounds of the string,
                                        it will be constrained to a valid range.
        @param numCharactersToReplace   the number of characters to remove. If zero or less, no
                                        characters will be taken out.
        @param stringToInsert           the new string to insert at startIndex after the characters have been
                                        removed.
    */
    const String replaceSection (int startIndex,
                                 int numCharactersToReplace,
                                 const tchar* const stringToInsert) const throw();

    /** Replaces all occurrences of a substring with another string.

        Returns a copy of this string, with any occurrences of stringToReplace
        swapped for stringToInsertInstead.

        Note that this is a const method, and won't alter the string itself.
    */
    const String replace (const tchar* const stringToReplace,
                          const tchar* const stringToInsertInstead,
                          const bool ignoreCase = false) const throw();

    /** Returns a string with all occurrences of a character replaced with a different one. */
    const String replaceCharacter (const tchar characterToReplace,
                                   const tchar characterToInsertInstead) const throw();

    /** Replaces a set of characters with another set.

        Returns a string in which each character from charactersToReplace has been replaced
        by the character at the equivalent position in newCharacters (so the two strings
        passed in must be the same length).

        e.g. translate ("abc", "def") replaces 'a' with 'd', 'b' with 'e', etc.

        Note that this is a const method, and won't affect the string itself.
    */
    const String replaceCharacters (const String& charactersToReplace,
                                    const tchar* const charactersToInsertInstead) const throw();

    /** Returns a version of this string that only retains a fixed set of characters.

        This will return a copy of this string, omitting any characters which are not
        found in the string passed-in.

        e.g. for "1122334455", retainCharacters ("432") would return "223344"

        Note that this is a const method, and won't alter the string itself.
    */
    const String retainCharacters (const tchar* const charactersToRetain) const throw();

    /** Returns a version of this string with a set of characters removed.

        This will return a copy of this string, omitting any characters which are
        found in the string passed-in.

        e.g. for "1122334455", removeCharacters ("432") would return "1155"

        Note that this is a const method, and won't alter the string itself.
    */
    const String removeCharacters (const tchar* const charactersToRemove) const throw();

    /** Returns a section from the start of the string that only contains a certain set of characters.

        This returns the leftmost section of the string, up to (and not including) the
        first character that doesn't appear in the string passed in.
    */
    const String initialSectionContainingOnly (const tchar* const permittedCharacters) const throw();

    /** Returns a section from the start of the string that only contains a certain set of characters.

        This returns the leftmost section of the string, up to (and not including) the
        first character that occurs in the string passed in.
    */
    const String initialSectionNotContaining (const tchar* const charactersToStopAt) const throw();

    /** Checks whether the string might be in quotation marks.

        @returns    true if the string begins with a quote character (either a double or single quote).
                    It is also true if there is whitespace before the quote, but it doesn't check the end of the string.
        @see unquoted, quoted
    */
    bool isQuotedString() const throw();

    /** Removes quotation marks from around the string, (if there are any).

        Returns a copy of this string with any quotes removed from its ends. Quotes that aren't
        at the ends of the string are not affected. If there aren't any quotes, the original string
        is returned.

        Note that this is a const method, and won't alter the string itself.

        @see isQuotedString, quoted
    */
    const String unquoted() const throw();

    /** Adds quotation marks around a string.

        This will return a copy of the string with a quote at the start and end, (but won't
        add the quote if there's already one there, so it's safe to call this on strings that
        may already have quotes around them).

        Note that this is a const method, and won't alter the string itself.

        @param quoteCharacter   the character to add at the start and end
        @see isQuotedString, unquoted
    */
    const String quoted (const tchar quoteCharacter = JUCE_T('"')) const throw();

    /** Writes text into this string, using printf style-arguments.

        This will replace the contents of the string with the output of this
        formatted printf.

        Note that using the %s token with a juce string is probably a bad idea, as
        this may expect differect encodings on different platforms.

        @see formatted
    */
    void printf (const tchar* const format, ...) throw();

    /** Returns a string, created using arguments in the style of printf.

        This will return a string which is the result of a sprintf using the
        arguments passed-in.

        Note that using the %s token with a juce string is probably a bad idea, as
        this may expect differect encodings on different platforms.

        @see printf, vprintf
    */
    static const String formatted (const tchar* const format, ...) throw();

    /** Writes text into this string, using a printf style, but taking a va_list argument.

        This will replace the contents of the string with the output of this
        formatted printf. Used by other methods, this is public in case it's
        useful for other purposes where you want to pass a va_list through directly.

        Note that using the %s token with a juce string is probably a bad idea, as
        this may expect differect encodings on different platforms.

        @see printf, formatted
    */
    void vprintf (const tchar* const format, va_list& args) throw();

    /** Creates a string which is a version of a string repeated and joined together.

        @param stringToRepeat         the string to repeat
        @param numberOfTimesToRepeat  how many times to repeat it
    */
    static const String repeatedString (const tchar* const stringToRepeat,
                                        int numberOfTimesToRepeat) throw();

    /** Creates a string from data in an unknown format.

        This looks at some binary data and tries to guess whether it's Unicode
        or 8-bit characters, then returns a string that represents it correctly.

        Should be able to handle Unicode endianness correctly, by looking at
        the first two bytes.
    */
    static const String createStringFromData (const void* const data,
                                              const int size) throw();

    // Numeric conversions..

    /** Creates a string containing this signed 32-bit integer as a decimal number.

        @see getIntValue, getFloatValue, getDoubleValue, toHexString
    */
    explicit String (const int decimalInteger) throw();

    /** Creates a string containing this unsigned 32-bit integer as a decimal number.

        @see getIntValue, getFloatValue, getDoubleValue, toHexString
    */
    explicit String (const unsigned int decimalInteger) throw();

    /** Creates a string containing this signed 16-bit integer as a decimal number.

        @see getIntValue, getFloatValue, getDoubleValue, toHexString
    */
    explicit String (const short decimalInteger) throw();

    /** Creates a string containing this unsigned 16-bit integer as a decimal number.

        @see getIntValue, getFloatValue, getDoubleValue, toHexString
    */
    explicit String (const unsigned short decimalInteger) throw();

    /** Creates a string containing this signed 64-bit integer as a decimal number.

        @see getLargeIntValue, getFloatValue, getDoubleValue, toHexString
    */
    explicit String (const int64 largeIntegerValue) throw();

    /** Creates a string containing this unsigned 64-bit integer as a decimal number.

        @see getLargeIntValue, getFloatValue, getDoubleValue, toHexString
    */
    explicit String (const uint64 largeIntegerValue) throw();

    /** Creates a string representing this floating-point number.

        @param floatValue               the value to convert to a string
        @param numberOfDecimalPlaces    if this is > 0, it will format the number using that many
                                        decimal places, and will not use exponent notation. If 0 or
                                        less, it will use exponent notation if necessary.
        @see getDoubleValue, getIntValue
    */
    explicit String (const float floatValue,
                     const int numberOfDecimalPlaces = 0) throw();

    /** Creates a string representing this floating-point number.

        @param doubleValue              the value to convert to a string
        @param numberOfDecimalPlaces    if this is > 0, it will format the number using that many
                                        decimal places, and will not use exponent notation. If 0 or
                                        less, it will use exponent notation if necessary.

        @see getFloatValue, getIntValue
    */
    explicit String (const double doubleValue,
                     const int numberOfDecimalPlaces = 0) throw();

    /** Parses this string to find its numerical value (up to 32 bits in size).

        @returns the value of the string as a 32 bit signed base-10 integer.
        @see getTrailingIntValue, getHexValue32, getHexValue64
    */
    int getIntValue() const throw();

    /** Parses this string to find its numerical value (up to 64 bits in size).

        @returns the value of the string as a 64 bit signed base-10 integer.
    */
    int64 getLargeIntValue() const throw();

    /** Parses a decimal number from the end of the string.

        This will look for a value at the end of the string.
        e.g. for "321 xyz654" it will return 654; for "2 3 4" it'll return 4.

        Negative numbers are not handled, so "xyz-5" returns 5.

        @see getIntValue
    */
    int getTrailingIntValue() const throw();

    /** Parses this string as a floating point number.

        @returns    the value of the string as a 32-bit floating point value.
        @see getDoubleValue
    */
    float getFloatValue() const throw();

    /** Parses this string as a floating point number.

        @returns    the value of the string as a 64-bit floating point value.
        @see getFloatValue
    */
    double getDoubleValue() const throw();

    /** Parses the string as a hexadecimal number.

        Non-hexadecimal characters in the string are ignored.

        If the string contains too many characters, then the lowest significant
        digits are returned, e.g. "ffff12345678" would produce 0x12345678.

        @returns    a 32-bit number which is the value of the string in hex.
    */
    int getHexValue32() const throw();

    /** Parses the string as a hexadecimal number.

        Non-hexadecimal characters in the string are ignored.

        If the string contains too many characters, then the lowest significant
        digits are returned, e.g. "ffff1234567812345678" would produce 0x1234567812345678.

        @returns    a 64-bit number which is the value of the string in hex.
    */
    int64 getHexValue64() const throw();

    /** Creates a string representing this 32-bit value in hexadecimal. */
    static const String toHexString (const int number) throw();

    /** Creates a string representing this 64-bit value in hexadecimal. */
    static const String toHexString (const int64 number) throw();

    /** Creates a string representing this 16-bit value in hexadecimal. */
    static const String toHexString (const short number) throw();

    /** Creates a string containing a hex dump of a block of binary data.

        @param data         the binary data to use as input
        @param size         how many bytes of data to use
        @param groupSize    how many bytes are grouped together before inserting a
                            space into the output. e.g. group size 0 has no spaces,
                            group size 1 looks like: "be a1 c2 ff", group size 2 looks
                            like "bea1 c2ff".
    */
    static const String toHexString (const unsigned char* data,
                                     const int size,
                                     const int groupSize = 1) throw();

    // Casting to character arrays..

#if JUCE_STRINGS_ARE_UNICODE
    /** Returns a version of this string using the default 8-bit system encoding.

        Because it returns a reference to the string's internal data, the pointer
        that is returned must not be stored anywhere, as it can be deleted whenever the
        string changes.
    */
    operator const char*() const throw();

    /** Returns a unicode version of this string.

        Because it returns a reference to the string's internal data, the pointer
        that is returned must not be stored anywhere, as it can be deleted whenever the
        string changes.
    */
    inline operator const juce_wchar*() const throw()   { return text->text; }
#else
    /** Returns a version of this string using the default 8-bit system encoding.

        Because it returns a reference to the string's internal data, the pointer
        that is returned must not be stored anywhere, as it can be deleted whenever the
        string changes.
    */
    inline operator const char*() const throw()         { return text->text; }

    /** Returns a unicode version of this string.

        Because it returns a reference to the string's internal data, the pointer
        that is returned must not be stored anywhere, as it can be deleted whenever the
        string changes.
    */
    operator const juce_wchar*() const throw();
#endif

    /** Copies the string to a buffer.

        @param destBuffer       the place to copy it to
        @param maxCharsToCopy   the maximum number of characters to copy to the buffer,
                                not including the tailing zero, so this shouldn't be
                                larger than the size of your destination buffer - 1
    */
    void copyToBuffer (char* const destBuffer,
                       const int maxCharsToCopy) const throw();

    /** Copies the string to a unicode buffer.

        @param destBuffer       the place to copy it to
        @param maxCharsToCopy   the maximum number of characters to copy to the buffer,
                                not including the tailing zero, so this shouldn't be
                                larger than the size of your destination buffer - 1
    */
    void copyToBuffer (juce_wchar* const destBuffer,
                       const int maxCharsToCopy) const throw();

    /** Copies the string to a buffer as UTF-8 characters.

        Returns the number of bytes copied to the buffer, including the terminating null
        character.

        @param destBuffer       the place to copy it to; if this is a null pointer,
                                the method just returns the number of bytes required
                                (including the terminating null character).
        @param maxBufferSizeBytes  the size of the destination buffer, in bytes. If the
                                string won't fit, it'll put in as many as it can while
                                still allowing for a terminating null char at the end,
                                and will return the number of bytes that were actually
                                used. If this value is < 0, no limit is used.
    */
    int copyToUTF8 (uint8* const destBuffer, const int maxBufferSizeBytes = 0x7fffffff) const throw();

    /** Returns a pointer to a UTF-8 version of this string.

        Because it returns a reference to the string's internal data, the pointer
        that is returned must not be stored anywhere, as it can be deleted whenever the
        string changes.
    */
    const char* toUTF8() const throw();

    /** Creates a String from a UTF-8 encoded buffer.

        If the size is < 0, it'll keep reading until it hits a zero.
    */
    static const String fromUTF8 (const uint8* const utf8buffer,
                                  int bufferSizeBytes = -1) throw();

    /** Increases the string's internally allocated storage.

        Although the string's contents won't be affected by this call, it will
        increase the amount of memory allocated internally for the string to grow into.

        If you're about to make a large number of calls to methods such
        as += or <<, it's more efficient to preallocate enough extra space
        beforehand, so that these methods won't have to keep resizing the string
        to append the extra characters.

        @param numCharsNeeded   the number of characters to allocate storage for. If this
                                value is less than the currently allocated size, it will
                                have no effect.
    */
    void preallocateStorage (const int numCharsNeeded) throw();

    juce_UseDebuggingNewOperator // (adds debugging info to find leaked objects)

private:

    struct InternalRefCountedStringHolder
    {
        int refCount;
        int allocatedNumChars;

#if JUCE_STRINGS_ARE_UNICODE
          wchar_t text[1];
#else
          char text[1];
#endif
    };

    InternalRefCountedStringHolder* text;
    static InternalRefCountedStringHolder emptyString;

    // internal constructor that preallocates a certain amount of memory
    String (const int numChars, const int dummyVariable) throw();

    void deleteInternal() throw();
    void createInternal (const int numChars) throw();
    void createInternal (const tchar* const text, const tchar* const textEnd) throw();
    void appendInternal (const tchar* const text, const int numExtraChars) throw();
    void doubleToStringWithDecPlaces (double n, int numDecPlaces) throw();
    void dupeInternalIfMultiplyReferenced() throw();
};

/** Global operator to allow a String to be appended to a string literal.

    This allows the use of expressions such as "abc" + String (x)

    @see String
 */
const String JUCE_PUBLIC_FUNCTION   operator+ (const char* const string1,
                                               const String& string2) throw();

/** Global operator to allow a String to be appended to a string literal.

    This allows the use of expressions such as "abc" + String (x)

    @see String
 */
const String JUCE_PUBLIC_FUNCTION   operator+ (const juce_wchar* const string1,
                                               const String& string2) throw();

#endif   // __JUCE_STRING_JUCEHEADER__
/********* End of inlined file: juce_String.h *********/

/* #include "juce_CharacterFunctions.h" */

