#ifndef XPC_CONFIG_H
#define XPC_CONFIG_H

/**
 * \file          xpc-config-mingw32.h
 * \library       xpc_suite
 * \author        Chris Ahlstrom
 * \date          2012-01-08
 * \update        2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 * License exceptions:
 *
 *    As a special exception to the GNU General Public License, permission
 *    is granted for additional uses of the text contained in its release of
 *    the XPC library.
 *
 *    The exception is that, if you link the the XPC library with other
 *    files to produce an executable, this does not by itself cause the
 *    resulting executable to be covered by the GNU General Public License.
 *    Your use of that executable is in no way restricted on account of
 *    linking the the XPC library code into it.
 *
 *    This exception does not however invalidate any other reasons why the
 *    executable file might be covered by the GNU General Public License.
 *
 *    This exception applies only to the code released under the name the
 *    XPC library.  If you copy code from other releases into a copy of the
 *    XPC library, as the General Public License permits, the exception does
 *    not apply to the code that you add in this way.  To avoid misleading
 *    anyone as to the status of such modified files, you must delete this
 *    exception notice from them.
 *
 *    If you write modifications of your own for the XPC library, it is your
 *    choice whether to permit this exception to apply to your
 *    modifications.  If you do not wish that, delete this exception notice.
 *    The modifications then fall completely under the General Public
 *    License.
 *
 *    MODIFIED MANUALLY FOR USAGE WITH MingW32!
 *
 *       *  *  *  DO NOT COPY OVER THIS FILE!  *  *  *
 *
 *    This module provides mingw32 access to non-socket libraries and headers
 *    based on the settings made in config.h (by autoconf), but modified
 *    manually for completeness.
 *
 *    This file is a copy of the latest (hopefully!) config.h or xpc-config.h
 *    generated by autoconf, with the entries hand-carried into the present
 *    file, for the benefit of the mingw32 developer.
 *
 */

#define XPC_WIN32_CONFIG_H             /* used for checking the environment   */

/*
 * config.h.  Generated by configure.
 * config.h.in.  Generated from configure.ac by autoheader.
 *
 * MODIFIED MANUALLY FOR USAGE WITH MingW32.
 * DO NOT COPY OVER THIS FILE.
 */

/*
 *    The XPC library currently requires features present only with Windows
 *    NT 4.0 and later.
 *
 *    http://msdn.microsoft.com/library/en-us/winprog/winprog/
 *       using_the_windows_headers.asp
 *
 *    Minimum system required    Macros to define
 *    =======================    ================
 *
 *    Windows "Longhorn"         _WIN32_WINNT >= 0x0600, WINVER >= 0x0600
 *    Windows Server 2003        _WIN32_WINNT> = 0x0502, WINVER >= 0x0502
 *    Windows XP                 _WIN32_WINNT >= 0x0501, WINVER >= 0x0501
 *    Windows 2000               _WIN32_WINNT >= 0x0500, WINVER >= 0x0500
 *    Windows NT 4.0             _WIN32_WINNT >= 0x0400, WINVER >= 0x0400
 *    Windows Me                 _WIN32_WINDOWS >= 0x0500, WINVER >= 0x0500
 *    Windows 98                 _WIN32_WINDOWS >= 0x0410, WINVER >= 0x0410
 *    Windows 95                 _WIN32_WINDOWS >= 0x0400, WINVER >= 0x0400
 *
 *    Faster Builds with Smaller Header Files: WIN32_LEAN_AND_MEAN
 *
 */

#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0400
#endif

/*
 *    Define the WIN32 macro automatically for Windows application compiled
 *    with Borland, Visual Studio, or Mingw32.
 *
 *    Required for compiling with critical sections.
 *
 */

#ifndef WIN32

#if defined __BORLANDC__ && defined _Windows
#define WIN32
#elif defined _MSC_VER && defined _WIN32
#define WIN32
#endif

#endif

/*
 *    Currently, much of the XPC library requires support for multi-threading.
 *
 */

#ifdef USE_MULTI_THREADING

#if defined __BORLANDC__ && ! defined __MT__
#error Please enable multithreading
#endif

#if defined _MSC_VER && ! defined _MT
#error Please enable multithreading (C/C++->Code Generation->Use Runtime Library)
#endif

#endif

/*
 *    At present, compiling the XPC library as a DLL is not supported, so
 *    this code is commented out.
 *
 */

#ifdef USE_DLL_BUILDING

#ifdef _MSC_VER
#ifndef XPC_NO_DLL
#ifndef _DLL
#error Enable DLL linking (Project->Settings->C/C++->Code Generation->Use Runtime Library)
#endif
#endif
#endif

#endif

/*
 *    Define WIN32 for Windows application compiled with Borland.
 *
 */

#ifndef WIN32

#if defined __BORLANDC__ && defined _Windows
#define WIN32
#elif defined _MSC_VER && defined _WIN32
#define WIN32
#endif

#endif

/*
 *    WINVER is set to Make sure we're consistent with _WIN32_WINNT.
 *
 *    WIN32_LEAN_AND_MEAN is set long enough to include Winsock without
 *    bringing in the whole of windows.h.
 *
 */

#ifndef WINVER
#define WINVER _WIN32_WINNT
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define XPC_WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/*
 * #include <xpc/xwinsock.h>         // Microsoft header madness workaround
 * #include <ws2tcpip.h>
 */

#ifdef XPC_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef XPC_WIN32_LEAN_AND_MEAN
#endif

#ifdef WIN32
#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#endif

#if _WIN32_WINNT >= 0x0501
#define XPC_IPV6
#endif

/*
 * MINGW:  This yields gcc error:
 *
 * /i586-mingw32msvc/include/sys/types.h:104: error: two or more data types
 * in declaration specifiers
 *
 * #ifdef WIN32
 * #ifndef ssize_t
 * #define ssize_t int
 * #endif
 * #endif
 */

#undef __DLLRTL
#undef XPC_EXPORT
#undef XPC_MEMBER
#undef XPC_MEMBER_EXPORT
#undef XPC_CLASS_EXPORT
#undef XPC_EMPTY
#define XPC_EMPTY


#if defined __MINGW32__ && ! defined __MSVCRT__
#define XPC_NOMSVCRT
#endif

#if defined __MINGW32__ || defined __CYGWIN32__

#ifdef __MINGW32__               /* __MINGW32__ */

#define XPC_HAVE_MODULES    1
#define alloca(x)       __builtin_alloca(x)
#define THROW(x)        throw x
#define THROWS(x)       throw(x)
#define NEW_THROWS      throw()
#define THROWS_EMPTY    throw()
typedef unsigned int    uint;

#define snprintf       _snprintf

/*
 * #define vsnprintf      _vsnprintf
 */

#ifndef ETC_PREFIX
#define ETC_PREFIX      "C:/"
#endif

#else                            /* __MINGW32__ */

typedef DWORD size_t;

#endif                           /* __MINGW32__ */

#else /* ! defined __MINGW32__ && ! defined __CYGWIN32__ */

#ifdef XPC_NO_DLL
#define __DLLRTL
#define __EXPORT
#else
#define __DLLRTL        __declspec(dllexport)
#define __EXPORT        __declspec(dllimport)
#endif

#if ! defined _MSC_VER || _MSC_VER >= 1300
#define XPC_HAVE_GETADDRINFO
#endif

#define XPC_HAVE_MEMMOVE
#define XPC_HAVE_SNPRINTF

/*
 * For Visual Studio Express, don't warn about strcpy()
 * usage.  We know what we're doing, thank you very much.
 *
 * For running Visual Studio Express (9.0), don't
 * define the vsnprintf macro.  _MSC_VER would be
 * better, but we can't find out exactly what it's
 * value will be, even looking at the version information
 * for cl.exe.
 *
 * This causes an issue in Microsoft's stdio.h file:
 *
 *       #define vsnprintf       _vsnprintf
 */

#ifdef _VC80_UPGRADE

#pragma warning ( disable : 4996 )
#define snprintf        _snprintf

#else

#define snprintf        _snprintf
#define vsnprintf       _vsnprintf

#endif

#define XPC_EXPORT(t)   __EXPORT t
#define XPC_MEMBER(t)   t
#define XPC_MEMBER_EXPORT(t) __EXPORT XPC_MEMBER(t XPC_EMPTY)
#define XPC_CLASS_EXPORT __EXPORT

#define SECS_BETWEEN_EPOCHS ((__int64)(11644473600))
#define SECS_TO_100NS ((__int64)(10000000))

#define THROW(x)        throw x
#define THROWS(x)       throw(x)
#define USING(x)
#define NEW_THROWS      throw()
#define THROWS_EMPTY    throw()

#define XPC_HAVE_MODULES 1
#undef  XPC_HAVE_PTHREAD_RWLOCK
#undef  PTHREAD_MUTEXTYPE_RECURSIVE

/* define endian macros */

#define __BYTE_ORDER __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __BYTE_ALIGNMENT 1

#pragma warning (disable:4786)
#if _MSC_VER >= 1300
#pragma warning (disable:4290)
#endif

#ifndef ETC_PREFIX
#define ETC_PREFIX "c:/"
#endif

#endif /* !defined(__MINGW32__) && !defined(__CYGWIN32__) */

/* have exceptions */

#ifdef XPC_NO_EXCEPTIONS
#undef XPC_EXCEPTIONS
#else
#define XPC_EXCEPTIONS 1
#endif

/*
 * use namespace
 */

#define XPC_NAMESPACES 1

#define COMMON_DEADLOCK_DEBUG
#define COMMON_TPPORT_TYPE_DEFINED
#define XPC_HAVE_NEW_INIT
#define XPC_HAVE_SSTREAM
#define XPC_HAVE_EXCEPTION

#ifdef __BORLANDC__
#define XPC_HAVE_LOCALTIME_R
#endif

/*
 * Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
 * systems. This function is required for `alloca.c' support on those systems.
 */

/* #undef XPC_CRAY_STACKSEG_END */

/*
 * Define to 1 if you have `alloca' as a function or a macro.
 */

#ifndef XPC_HAVE_ALLOCA
#define XPC_HAVE_ALLOCA  1
#endif

/*
 * Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
 */

#ifndef XPC_HAVE_ALLOCA_H
#define XPC_HAVE_ALLOCA_H  0
#endif

/*
 * Define to 1 if you have the <ctype.h> header file.
 */

#ifndef XPC_HAVE_CTYPE_H
#define XPC_HAVE_CTYPE_H 1
#endif

/*
 * Define to 1 if translation of program messages to the user's native
 * language is requested.
 *
 * For mingw32, not really sure if this exists, so assume it does not.
 */

#ifndef XPC_ENABLE_NLS
#define XPC_ENABLE_NLS  0
#endif

/*
 * Define to 1 if you have the <argz.h> header file.
 *
 * For mingw32, not really sure if this exists, so assume it does not.
 */

#ifndef XPC_HAVE_ARGZ_H
#define XPC_HAVE_ARGZ_H  0
#endif

/*
 * Define to 1 if you have the <arpa/inet.h> header file.
 */

#ifndef XPC_HAVE_ARPA_INET_H
#define XPC_HAVE_ARPA_INET_H  0
#endif

/*
 * Define to 1 if you have the <asm/atomic.h> header file.
 */

#ifndef XPC_HAVE_ASM_ATOMIC_H
#define XPC_HAVE_ASM_ATOMIC_H  0
#endif

/*
 * Define to 1 if you have the `dcgettext' function.
 */

#ifndef XPC_HAVE_DCGETTEXT
#define XPC_HAVE_DCGETTEXT  0
#endif

/*
 * Define to 1 if you have the <errno.h> header file.
 */

#ifndef XPC_HAVE_ERRNO_H
#define XPC_HAVE_ERRNO_H 1
#endif

/*
 * Define to 1 if you have the `feof_unlocked' function.
 */

#ifndef XPC_HAVE_FEOF_UNLOCKED
#define XPC_HAVE_FEOF_UNLOCKED  0
#endif

/*
 * Define to 1 if you have the `fgets_unlocked' function.
 */

#ifndef XPC_HAVE_FGETS_UNLOCKED
#define XPC_HAVE_FGETS_UNLOCKED  0
#endif

/*
 * Define to 1 if you have the <ftw.h> header file.
 */

#ifndef XPC_HAVE_FTW_H
#define XPC_HAVE_FTW_H  0
#endif

/*
 * Define to 1 if you have the `getcwd' function.
 */

#ifndef XPC_HAVE_GETCWD
#define XPC_HAVE_GETCWD  1
#endif

/*
 * Define to 1 if you have the `getegid' function.
 */

#ifndef XPC_HAVE_GETEGID
#define XPC_HAVE_GETEGID  0
#endif

/*
 * Define to 1 if you have the `geteuid' function.
 */

#ifndef XPC_HAVE_GETEUID
#define XPC_HAVE_GETEUID  0
#endif

/*
 * Define to 1 if you have the `getgid' function.
 */

#ifndef XPC_HAVE_GETGID
#define XPC_HAVE_GETGID  0
#endif

/*
 * Define to 1 if you have the `getpagesize' function.
 */

#ifndef XPC_HAVE_GETPAGESIZE
#define XPC_HAVE_GETPAGESIZE  1
#endif

/*
 * Define if the GNU gettext() function is already present or preinstalled.
 */

#ifndef XPC_HAVE_GETTEXT
#define XPC_HAVE_GETTEXT  0
#endif

/*
 * Define to 1 if you have the `getuid' function.
 */

#ifndef XPC_HAVE_GETUID
#define XPC_HAVE_GETUID  0
#endif

/*
 * Define to 1 if you have the <fcntl.h> header file.
 */

#ifndef XPC_HAVE_FCNTL_H
#define XPC_HAVE_FCNTL_H 0
#endif

/*
 * Define to 1 if you have the <io.h> header file.
 */

#ifndef XPC_HAVE_IO_H
#define XPC_HAVE_IO_H 1
#endif

/*
 * Define to 1 if you have the <inttypes.h> header file.
 */

#ifndef XPC_HAVE_INTTYPES_H
#define XPC_HAVE_INTTYPES_H 0
#endif

/*
 * Define if you have the iconv() function.
 */

#ifndef XPC_HAVE_ICONV
#define XPC_HAVE_ICONV  0
#endif

/*
 * Define if you have <langinfo.h> and nl_langinfo(CODESET).
 */

#ifndef XPC_HAVE_LANGINFO_CODESET
#define XPC_HAVE_LANGINFO_CODESET  0
#endif

/*
 * Define if your <locale.h> file defines LC_MESSAGES.
 */

#ifndef XPC_HAVE_LC_MESSAGES
#define XPC_HAVE_LC_MESSAGES  0
#endif

/*
 * Define to 1 if you have the <libintl.h> header file.
 */

#ifndef XPC_HAVE_LIBINTL_H
#define XPC_HAVE_LIBINTL_H  0
#endif

/*
 * Define to 1 if you have the <locale.h> header file.
 */

#ifndef XPC_HAVE_LOCALE_H
#define XPC_HAVE_LOCALE_H  1
#endif

/*
 * Define to 1 if you have the <magic.h> header file.
 */

#ifndef XPC_HAVE_MAGIC_H
#define XPC_HAVE_MAGIC_H  0
#endif

/*
 * Define to 1 if you have the <malloc.h> header file.
 */

#ifndef XPC_HAVE_MALLOC_H
#define XPC_HAVE_MALLOC_H  1
#endif

/*
 * Define to 1 if you have the <memory.h> header file.
 */

#ifndef XPC_HAVE_MEMORY_H
#define XPC_HAVE_MEMORY_H  1
#endif

/*
 * Define to 1 if you have the `mempcpy' function.
 */

#ifndef XPC_HAVE_MEMPCPY
#define XPC_HAVE_MEMPCPY  1
#endif

/*
 * Define to 1 if you have a working `mmap' system call.
 */

#ifndef XPC_HAVE_MMAP
#define XPC_HAVE_MMAP  1
#endif

/*
 * Define to 1 if you have the `munmap' function.
 */

#ifndef XPC_HAVE_MUNMAP
#define XPC_HAVE_MUNMAP  1
#endif

/*
 * Define to 1 if you have the <netdb.h> header file.
 */

#ifndef XPC_HAVE_NETDB_H
#define XPC_HAVE_NETDB_H  0
#endif

/*
 * Define to 1 if you have the <netinet/in.h> header file.
 */

#ifndef XPC_HAVE_NETINET_IN_H
#define XPC_HAVE_NETINET_IN_H  0
#endif

/*
 * Define to 1 if you have the <nl_types.h> header file.
 */

#ifndef XPC_HAVE_NL_TYPES_H
#define XPC_HAVE_NL_TYPES_H  0
#endif

/*
 * Define to 1 if you have the <poll.h> header file.
 */

#ifndef XPC_HAVE_POLL_H
#define XPC_HAVE_POLL_H  0
#endif

/*
 * Define to 1 if you have the <pthread.h> header file.
 */

#ifndef XPC_HAVE_PTHREAD_H
#define XPC_HAVE_PTHREAD_H  0
#endif

/*
 * Define to 1 if the system has the type `ptrdiff_t'.
 */

#ifndef XPC_HAVE_PTRDIFF_T
#define XPC_HAVE_PTRDIFF_T  1
#endif

/*
 * Define to 1 if you have the `putenv' function.
 */

#ifndef XPC_HAVE_PUTENV
#define XPC_HAVE_PUTENV  1
#endif

/*
 * Define to 1 if you have the <semaphore.h> header file.
 */

#ifndef XPC_HAVE_SEMAPHORE_H
#define XPC_HAVE_SEMAPHORE_H  0
#endif

/*
 * Define to 1 if you have the `setenv' function.
 */

#ifndef XPC_HAVE_SETENV
#define XPC_HAVE_SETENV  1
#endif

/*
 * Define to 1 if you have the `setlocale' function.
 */

#ifndef XPC_HAVE_SETLOCALE
#define XPC_HAVE_SETLOCALE  1
#endif

/*
 * Define to 1 if you have the <stdarg.h> header file.
 */

#ifndef XPC_HAVE_STDARG_H
#define XPC_HAVE_STDARG_H  1
#endif

/*
 * Define to 1 if you have the <stddef.h> header file.
 */

#ifndef XPC_HAVE_STDDEF_H
#define XPC_HAVE_STDDEF_H  1
#endif

/*
 * Define to 1 if you have the <stdint.h> header file.
 */

#ifndef XPC_HAVE_STDINT_H
#define XPC_HAVE_STDINT_H 0
#endif

/*
 * Define to 1 if you have the <stdio.h> header file.
 */

#ifndef XPC_HAVE_STDIO_H
#define XPC_HAVE_STDIO_H 1
#endif

/*
 * Define to 1 if you have the <stdlib.h> header file.
 */

#ifndef XPC_HAVE_STDINT_H
#define XPC_HAVE_STDLIB_H 1
#endif

/*
 * Define to 1 if you have the <strings.h> header file.
 */

#undef XPC_HAVE_STRINGS_H
#ifndef XPC_HAVE_STRINGS_H
#define XPC_HAVE_STRINGS_H  0
#endif

/*
 * Define to 1 if you have the <string.h> header file.
 */

#ifndef XPC_HAVE_STRING_H
#define XPC_HAVE_STRING_H  1
#endif

/*
 * Define to 1 if you have the `strcasecmp' function.
 */

#ifndef XPC_HAVE_STRCASECMP
#define XPC_HAVE_STRCASECMP  0
#endif

/*
 * Define to 1 if you have the `strchr' function.
 */

#ifndef XPC_HAVE_STRCHR
#define XPC_HAVE_STRCHR  1
#endif

/*
 * Define to 1 if you have the `strdup' function.
 */

#ifndef XPC_HAVE_STRDUP
#define XPC_HAVE_STRDUP  1
#endif

/*
 * Define to 1 if you have the <limits.h> header file.
 */

#ifndef XPC_HAVE_LIMITS_H
#define XPC_HAVE_LIMITS_H 1
#endif

/*
 * Define to 1 if you have the <linux/netlink.h> header file.
 */

#ifndef XPC_HAVE_LINUX_NETLINK_H
#define XPC_HAVE_LINUX_NETLINK_H  0
#endif

/*
 * Define to 1 if you have the <sys/file.h> header file.
 */

#ifndef XPC_HAVE_SYS_FILE_H
#define XPC_HAVE_SYS_FILE_H 0
#endif

/*
 * Define to 1 if you have the <time.h> header file.
 */

#ifndef XPC_HAVE_TIME_H
#define XPC_HAVE_TIME_H 1
#endif

/* Name of suite */

#define PACKAGE_SUITE "xpc"

/* Name of package */

#define PACKAGE "xpccut"

/*
 * Define to the address where bug reports for this package should be sent.
 */

#define PACKAGE_BUGREPORT "ahlstromcj@gmail.com"

/*
 * Define to the full name of this package.
 */

#define PACKAGE_NAME "xpccut"

/*
 * Define to the full name and version of this package.
 */

#define PACKAGE_STRING "xpccut 1.1.0"

/*
 * Define to the one symbol short name of this package.
 */

#define PACKAGE_TARNAME "xpccut"

/*
 * Define to the version of this package.
 */

#define XPC_PACKAGE_VERSION "1.1"

/*
 * Define to 1 if you have the `strtoul' function.
 */

#ifndef XPC_HAVE_STRTOUL
#define XPC_HAVE_STRTOUL  1
#endif

/*
 * Define to 1 if `st_blksize' is member of `struct stat'.
 */

#ifndef XPC_HAVE_STRUCT_STAT_ST_BLKSIZE
#define XPC_HAVE_STRUCT_STAT_ST_BLKSIZE  1
#endif

/*
 * Define to 1 if your `struct stat' has `st_blksize'. Deprecated, use
 * `HAVE_STRUCT_STAT_ST_BLKSIZE' instead.
 */

#ifndef XPC_HAVE_ST_BLKSIZE
#define XPC_HAVE_ST_BLKSIZE  1
#endif

/*
 * Define to 1 if you have the <syslog.h> header file.
 */

#ifndef XPC_HAVE_SYSLOG_H
#define XPC_HAVE_SYSLOG_H  0
#endif

/*
 * Define to 1 if you have the <sys/param.h> header file.
 */

#ifndef XPC_HAVE_SYS_PARAM_H
#define XPC_HAVE_SYS_PARAM_H  0
#endif

/*
 * Define to 1 if you have the <sys/socket.h> header file.
 */

#ifndef XPC_HAVE_SYS_SOCKET_H
#define XPC_HAVE_SYS_SOCKET_H  0
#endif

/*
 * Define to 1 if you have the <sys/stat.h> header file.
 */

#ifndef XPC_HAVE_SYS_STAT_H
#define XPC_HAVE_SYS_STAT_H  1
#endif

/*
 * Define to 1 if you have the <sys/time.h> header file.
 */

#ifndef XPC_HAVE_SYS_TIME_H
#define XPC_HAVE_SYS_TIME_H  0
#endif

/*
 * Define to 1 if you have the <winsock2.h> header file.  This file
 * has some of the time types used in UNIX code.
 */

#ifndef XPC_HAVE_WINSOCK2_H
#define XPC_HAVE_WINSOCK2_H  1
#endif

/*
 * Define to 1 if you have the <sys/types.h> header file.
 */

#ifndef XPC_HAVE_SYS_TYPES_H
#define XPC_HAVE_SYS_TYPES_H  1
#endif

/*
 * Define to 1 if you have the <sys/un.h> header file.
 */

#ifndef XPC_HAVE_SYS_UN_H
#define XPC_HAVE_SYS_UN_H  0
#endif

/*
 * Define to 1 if you have the <sys/utsname.h> header file.
 */

#ifndef XPC_HAVE_SYS_UTSNAME_H
#define XPC_HAVE_SYS_UTSNAME_H  0
#endif

/*
 * Define to 1 if you have the <sys/wait.h> header file.
 */

#ifndef XPC_HAVE_SYS_WAIT_H
#define XPC_HAVE_SYS_WAIT_H  0
#endif

/*
 * Define to 1 if you have the `tsearch' function.
 */

#ifndef XPC_HAVE_TSEARCH
#define XPC_HAVE_TSEARCH  1
#endif

/*
 * Define to 1 if you have the <unistd.h> header file.
 */

#ifndef XPC_HAVE_UNISTD_H
#define XPC_HAVE_UNISTD_H  0
#endif

/*
 * Define to 1 if you have the `__argz_count' function.
 */

#ifndef XPC_HAVE___ARGZ_COUNT
#define XPC_HAVE___ARGZ_COUNT  1
#endif

/*
 * Define to 1 if you have the `__argz_next' function.
 */

#ifndef XPC_HAVE___ARGZ_NEXT
#define XPC_HAVE___ARGZ_NEXT  1
#endif

/*
 * Define to 1 if you have the `__argz_stringify' function.
 */

#ifndef XPC_HAVE___ARGZ_STRINGIFY
#define XPC_HAVE___ARGZ_STRINGIFY  1
#endif

/*
 * Define as const if the declaration of iconv() needs const.
 */

#ifndef XPC_ICONV_CONST
#define XPC_ICONV_CONST
#endif

/* Name of package */
#ifndef XPC_PACKAGE
#define XPC_PACKAGE  "xpc"
#endif

/*
 * Define to the address where bug reports for this package should be sent.
 */

#ifndef XPC_PACKAGE_BUGREPORT
#define XPC_PACKAGE_BUGREPORT  ""
#endif

/*
 * Define to the full name of this package.
 */

#ifndef XPC_PACKAGE_NAME
#define XPC_PACKAGE_NAME  ""
#endif

/*
 * Define to the full name and version of this package.
 */

#ifndef XPC_PACKAGE_STRING
#define XPC_PACKAGE_STRING  ""
#endif

/*
 * Define to the one symbol short name of this package.
 */

#ifndef XPC_PACKAGE_TARNAME
#define XPC_PACKAGE_TARNAME  ""
#endif

/*
 * Define to the version of this package.
 */

/*
 * #ifndef XPC_PACKAGE_VERSION
 * #define XPC_PACKAGE_VERSION  "1.1"
 * #endif
 */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef XPC_STACK_DIRECTION */

/*
 * Define to 1 if you have the ANSI C header files.
 */

#ifndef XPC_STDC_HEADERS
#define XPC_STDC_HEADERS  1
#endif

/*
 * Version number of package
 */

#ifndef XPC_VERSION
#define XPC_VERSION  "1.1.0"
#endif

#ifndef XPC_API_VERSION
#define XPC_API_VERSION  "1.1"
#endif

/*
 * Define to 1 if your processor stores words with the most significant byte
 * first (like Motorola and SPARC, unlike Intel and VAX).
  */

/* #undef XPC_WORDS_BIGENDIAN */

/*
 * Define to empty if `const' does not conform to ANSI C.
 */

/* #undef _xpc_const */

/*
 * Define to `__inline__' or `__inline' if that's what the C compiler
 * calls it, or to nothing if 'inline' is not supported under any name.
 */

#ifndef __cplusplus
/* #undef _xpc_inline */
#endif

/*
 * Define to `long' if <sys/types.h> does not define.
 */

/* #undef _xpc_off_t */

/*
 * Define to `unsigned' if <sys/types.h> does not define.
 */

/* #undef _xpc_size_t */

#endif      // XPC_CONFIG_H

/*
 * xpc-config-mingw32.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
