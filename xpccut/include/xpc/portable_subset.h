#ifndef XPCCUT_PORTABLE_SUBSET_H
#define XPCCUT_PORTABLE_SUBSET_H

/**
 * \file          portable_subset.h
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-07
 * \updates       2015-10-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides some miscellaneous portable functions and macros to simplify
 *    the writing of the unit-test library modules.
 */

#define USE_XPCCUT_NULLPTR_TEST        /* See unit_test_test_07_01()          */

#include <xpc/macros_subset.h>         /* support for special XPC features    */

#if XPC_HAVE_WINSOCK2_H
#include <winsock2.h>				      /* struct timeval (Win32)              */
#endif

#if XPC_HAVE_SYS_TIME_H                /* see the OS's config.h header        */
#include <sys/time.h>                  /* struct timeval and gettimeofday()   */
#endif

/**
 *    Provides for writing an error message to stderr.
 *
 *    This function is similar to the xpc_errprint_func() macro.  But that
 *    macro is in a separate library, and we don't want the xpccut library
 *    to depend upon any other XPC library.
 */

#define xpccut_errprint_func(m)     xpccut_errprint_ex(m, __func__)

/**
 *    Provides for writing an error message to stderr with an additional
 *    function name.
 */

#define xpccut_errprint_3_func(m, f) xpccut_errprint_3(m, f, __func__)

/**
 *    Provides for writing a stock error message to stderr if a pointer is
 *    null.
 *    This macro allows a "this" pointer to be conditionally checked and
 *    reported on in one short macro call.
 *
 * \usage
 *    Use this macro to determine if the pointer is safe to use.  If the C
 *    function that is using this macro is wrapped in a class's member
 *    function, then define XPC_NO_THISPTR, since the class can ensure that
 *    the pointer is indeed valid, saving redundant checks to the pointer.
 *
\verbatim
      void pfunction (item_t * p)
      {
         if (xpccut_thisptr(p))
         {
            // do work using the pointer p
         }
      }
\endverbatim
 */

#ifndef xpccut_thisptr

#ifdef XPC_NO_THISPTR
#define xpccut_thisptr(p)           true
#else
#define xpccut_thisptr(p)           xpccut_thisptr_impl(p, __func__)
#endif

#endif   /* ifndef xpccut_thisptr   */

/**
 *    Provides a hardwired limit for string sizes in the status structure.
 *
 *    Although variable string lengths would be nice, they are a lot of work
 *    to manage in C code, and are not worth the effort for the present
 *    task.  The strings, even for descriptions, don't need to be very long.
 *    And there are not many of them.
 *
 *    Therefore, character arrays of set size are provided for the various
 *    strings in the unit_test_status_t structure.
 *
 *    256 is way more than we need.  Even 128 is too much, as we don't
 *    expect the strings to be longer than about 80 characters.
 */

#define XPCCUT_STRLEN         128

/**
 *    Global and portable functions for some common, basic tasks.
 */

EXTERN_C_DEC

extern void xpccut_silence_printing (void);
extern void xpccut_allow_printing (void);
extern cbool_t xpccut_is_silent (void);
extern void xpccut_errprint (const char * message);
extern void xpccut_errprint_ex (const char * message, const char * extra);
extern void xpccut_errprint_3
(
   const char * message,
   const char * funcname,
   const char * extra
);
extern void xpccut_infoprint (const char * message);
extern void xpccut_infoprint_ex (const char * message, const char * extra);
extern cbool_t xpccut_thisptr_impl
(
   const void * pointer,
   const char * extra
);
extern void xpccut_stringcopy
(
   char * destination,
   const char * source
);
extern char xpccut_get_response (void);
extern void xpccut_ms_sleep (unsigned long ms);
extern cbool_t xpccut_get_microseconds (struct timeval * c);
extern cbool_t xpccut_time_fix (struct timeval * c);
extern unsigned long xpccut_time_difference_ms
(
   struct timeval c1,
   struct timeval c2
);
extern void xpccut_stopwatch_start (void);
extern double xpccut_stopwatch_duration (void);
extern double xpccut_stopwatch_lap (void);
extern cbool_t xpccut_text_domain (void);

/*
 * This function is exposed only for testing coverage testing using
 * unit_test_test_07_01().
 */

#ifdef USE_XPCCUT_NULLPTR_TEST

extern cbool_t xpccut_nullptr
(
   const char * pointer,
   const char * message,
   const char * extra
);

#endif

EXTERN_C_END

#endif                           /* XPCCUT_PORTABLE_SUBSET_H                  */

/*
 * portable_subset.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
