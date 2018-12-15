#ifndef XPCCUT_MACROS_SUBSET_H
#define XPCCUT_MACROS_SUBSET_H

/**
 * \file          macros_subset.h
 * \library       xpccut
 * \author        Chris Ahlstrom
 * \date          2008-03-27
 * \update        2018-12-06
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides macros to support the special features of the XPC unit-test
 *    library, libxpccut.
 *
 * \warning
 *    This module is a subset of the macros.h module included in the XPC
 *    library, libxpc.  It is a stripped down version used so that we can
 *    support unit testing without having a dependency on any other library.
 *
 *    It also includes items from the os.h and gettext_support.h modules
 *    from the xpccut library.
 *
 *    Also see the portable_subset.h module in the present library.
 *
 *    Please do not include this file in any code outside of the unit-test
 *    library, libxpccut, except for unit-test applications.  For outside
 *    code, use the other xpccut header files instead.
 *
 * \note
 *    The "xpc-config.h" header file included below should be the one needed
 *    for your project and for the current operating system.
 */

#include <xpc/configuration.h>         /* defines all available HAVE macros   */

#ifdef WIN32                           /* defined by the Microsoft compiler   */

#ifndef XPC_WIN32_CONFIG_H
#error Picking up non-Windows version of xpc-config.h, check solution settings.
#endif

#else                                  /* if not Win32                        */

#ifdef XPC_WIN32_CONFIG_H
#error Picking up Windows version of xpc-config.h, check your setup.
#endif

#endif                                 /* end of Win32                        */

/**
 *    Verify that this important macro has been defined.
 */

#ifndef XPC_API_VERSION
#ifdef WIN32
#error Windows version of xpc-config.h does not define XPC_API_VERSION
#else
#error Configure-generated xpc-config.h does not define XPC_API_VERSION
#endif
#endif

#if XPC_HAVE_UNISTD_H
#include <unistd.h>                    /* here, gives us _POSIX_VERSION       */
#endif

/**
 *    Although we prefer to include header files only where needed, the
 *    following header files are so universally-used that it makes sense to
 *    include them here.
 *
 *    Note that xpc-config.h includes all of the headers, but we want to
 *    avoid using it, except in one or two places.
 */

#if XPC_HAVE_STDLIB_H
#include <stdlib.h>                    /* malloc()/free()                     */
#endif

#if XPC_HAVE_STRING_H
#include <string.h>                    /* strcmp(), strlen(), etc.            */
#endif

/**
 *    As noted for the gettext() support, we need this header file.
 *
 *    However, gettext() support is still in progress, and not quite
 *    finished.
 */

#if XPC_HAVE_LIBINTL_H
#include <libintl.h>                   /* gettext(), etc.                     */
#endif

/**
 *    If the compiler defines _WIN32, also define WIN32.
 *    We want to pepper our code with "WIN32", not "_WIN32".
 */

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

/**
 *    We want this macro defined in POSIX-style functions can be used.
 */

#ifdef _POSIX_VERSION                  /* defined in unistd.h                 */

#ifndef POSIX
#define POSIX                    1
#endif

#ifdef WIN32
#error WIN32 and POSIX macros defined simultaneously.  Fix macros_subset.h.
#endif

#endif

/**
 *    POSIX_SUCCESS is returned from a string function when it has processed a
 *    message that is not an error.  It is also a non-error value for most
 *    POSIX-conformant functions.
 */

#ifndef POSIX_SUCCESS
#define POSIX_SUCCESS              0
#endif

/**
 *    POSIX_ERROR is returned from a string function when it has processed an
 *    error.  It indicates that an error is in force.  Normally, the caller
 *    then uses this indicator to set a class-based error message.
 */

#ifndef POSIX_ERROR
#define POSIX_ERROR              (-1)
#endif

/**
 *    This macro tests the integer value against POSIX_SUCCESS.
 */

#ifndef is_posix_success
#define is_posix_success(x)      ((x) == POSIX_SUCCESS)
#endif

/**
 *    This macro tests the integer value against POSIX_ERROR (-1).
 */

#ifndef is_posix_error
#define is_posix_error(x)        ((x) == POSIX_ERROR)
#endif

/**
 *    This macro tests the integer value against POSIX_SUCCESS (0).
 */

#ifndef not_posix_success
#define not_posix_success(x)     ((x) != POSIX_SUCCESS)
#endif

/**
 *    This macro tests the integer value against POSIX_ERROR (-1).
 */

#ifndef not_posix_error
#define not_posix_error(x)       ((x) != POSIX_ERROR)
#endif

/**
 *    This macro set the integer value to POSIX_SUCCESS (0).  The parameter
 *    must be an lvalue, as the assignment operator is used.
 */

#ifndef set_posix_success
#define set_posix_success(x)     ((x) = POSIX_SUCCESS)
#endif

/**
 *    This macro set the integer value to POSIX_ERROR (-1).  The parameter
 *    must be an lvalue, as the assignment operator is used.
 */

#ifndef set_posix_error
#define set_posix_error(x)       ((x) = POSIX_ERROR)
#endif

/**
 *    Provides a unique typedef for boolean values.
 *    See the documentation for the macro.h module for full details.
 */

#ifndef def_cbool_t
typedef int cbool_t;                   /* our C boolean type                  */
#define def_cbool_t                    /* activate the sentry                 */
#endif

/**
 *    Provides a cast to the bool type, for use in C++.
 *
 *    This macro avoids Visual Studio warning C4800 about a performance hit,
 *    but it doesn't avoid the performance hit (it may make it worse!)
 */

#ifdef __cplusplus
#define xpccut_boolcast(x)     ((x) ? true : false)
#else
#define xpccut_boolcast(x)     (x)
#endif

#ifndef __cplusplus

/**
 *    false is zero (0).
 *    This value is defined only for C code, not for C++ code.
 */

#ifndef false
#define false         0
#endif

/**
 *    true is one (1).  We are all one.
 *    This value is defined only for C code, not for C++ code.
 */

#ifndef true
#define true          1
#endif

#endif

/**
 *    cfalse is the same value whether the code is C or C++.
 *
 *    This value is to be used only in those rare cases where a cbool_t
 *    value is needed directly in C++ code.
 */

#ifndef cfalse
#define cfalse        0
#endif

/**
 *    ctrue is one (1).  We are all one.
 *    This value is defined for C code and for C++ code.
 */

#ifndef ctrue
#define ctrue         1
#endif

/**
 *    We were going to try to support gettext() in every library but xpccut,
 *    but, if building from the xpc_suite root, then all libraries need to
 *    support it, since they all use the _() macro in their code.
 *    Therefore, they all must get a valid declaration of gettext() from the
 *    libintl.h header file.
 */

#if (XPC_ENABLE_NLS == 1)

#ifndef ENABLE_NLS
#define ENABLE_NLS  1
#endif

#ifndef USE_GETTEXT
#define USE_GETTEXT
#endif

#define _(msg)             gettext (msg)
#define gettext_noop(msg)  msg
#define N_(msg)            gettext_noop (msg)

#else                               /* no XPC_ENABLE_NLS defined             */

#undef USE_GETTEXT

/**
 *    Provides a placeholder for future support of gettext() in the
 *    unit-test library.
 */

#define _(msg)             (msg)

/**
 *    Provides a placeholder for future support of gettext() in the
 *    unit-test library.
 */

#define N_(msg)             msg

#endif                              /* End of XPC_ENABLE_NLS                 */

/**
 *    This macro replaces the oft-disparaged \a NULL macro.
 *    See the documentation for the macro.h module for full details.
 */

#ifndef nullptr                     /* nullptr define starts                  */
#ifdef __cplusplus                  /* C++                                    */
#define nullptr            0        /* for fopen() and generic pointers       */
#else                               /* C                                      */
#define nullptr   ((void *) 0)      /* try this on for C size                 */
#endif                              /* C/C++                                  */
#endif                              /* nullptr define ends                    */

/**
 *    This macro sets a variable to a null pointer (see the nullptr macro).
 *    See the documentation for the macro.h module for full details.
 */

#ifndef set_nullptr
#define set_nullptr(p)              ((p) = nullptr)
#endif

/**
 *    The XPC_NO_NULLPTR macro is defined if the user supplied the
 *    argument "--disable-nullptr" to the "configure" script.
 *    See the documentation for the macro.h module for full details.
 */

#ifndef cut_not_nullptr                    /* { */

#ifdef XPC_NO_NULLPTR

#error "Null-pointer checking is disabled.  You fool!"

#define cut_not_nullptr(p)              true
#define cut_not_nullptr_2(p, q)         true
#define cut_not_nullptr_3(p, q, r)      true
#define cut_not_nullptr_4(p, q, r, s)   true

#define cut_is_nullptr(p)               false
#define cut_is_nullptr_2(p, q)          false
#define cut_is_nullptr_3(p, q, r)       false
#define cut_is_nullptr_4(p, q, r, s)    false

#else

/**
 *    Checks a pointer for being valid.
 *    See the documentation for the macro.h module for full details.
 */

#define cut_not_nullptr(p)              ((p) != nullptr)

/**
 *    Checks two pointers for being valid.
 */

#define cut_not_nullptr_2(p, q)         \
   (cut_not_nullptr(p) && cut_not_nullptr(q))

/**
 *    Checks three pointers for being valid.
 */

#define cut_not_nullptr_3(p, q, r)      \
   (cut_not_nullptr_2(p,q) && cut_not_nullptr(r))

/**
 *    Checks four pointers for being valid.
 */

#define cut_not_nullptr_4(p, q, r, s)   \
   (cut_not_nullptr_3(p,q,r) && cut_not_nullptr(s))

/**
 *    Checks a pointer for being invalid.
 */

#define cut_is_nullptr(p)               ((p) == nullptr)

/**
 *    Checks two pointers for being invalid.  Returns 'true' if either of
 *    them are invalid.
 */

#define cut_is_nullptr_2(p, q)          (cut_is_nullptr(p) || cut_is_nullptr(q))

/**
 *    Checks three pointers for being invalid.  Returns 'true' if any of
 *    them are invalid.
 */

#define cut_is_nullptr_3(p, q, r)       \
   (cut_is_nullptr_2(p,q) || cut_is_nullptr(r))

/**
 *    Checks four pointers for being invalid.  Returns 'true' if any of
 *    them are invalid.
 */

#define cut_is_nullptr_4(p, q, r, s)    \
   (cut_is_nullptr_3(p,q,r) || cut_is_nullptr(s))

#endif   /* #ifdef XPC_NO_NULLPTR            */
#endif   /* #ifndef cut_not_nullptr       }  */

/**
 *    Checks a pointer for being valid, assuming it is like a "this" pointer.
 *
 *    It also allows this checking to be enabled via the
 *    "--enable-thisptr" option of the "configure" script.
 *
 *    This macro should be use in only one restricted case:  where a pointer
 *    to a structure is used, in a function, as if it were a "this" pointer
 *    in C++.  More specifically, use cut_is_thisptr() where the code might be
 *    wrapped in a class at some point, where one can then
 *    assume that the pointer is valid, and the check for a null pointer can
 *    be ignored.  The "--enable-thisptr" configuration option allows
 *    this check to be enabled for debugging.
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
         if (cut_is_thisptr(p))
         {
            // do work using the pointer p
         }
         else
         {
            // warn the user or take evasive action.
         }
      }
\endverbatim
 *
 *    If all you want to do is show a stock error message, use the
 *    xpccut_thisptr() macro.
 */

#ifndef cut_is_thisptr

#ifdef XPC_NO_THISPTR
#define cut_is_thisptr(p)              true
#else
#define cut_is_thisptr(p)              ((p) != nullptr)
#endif

#endif   /* #ifndef cut_is_thisptr   */

/**
 *    Provides the opposite test of cut_is_thisptr().
 */

#ifndef cut_not_thisptr

#ifdef XPC_NO_THISPTR
#define cut_not_thisptr(p)             false
#else
#define cut_not_thisptr(p)             ((p) == nullptr)
#endif

#endif   /* #ifndef cut_not_thisptr  */

/**
 *    Provides the name of current function being compiled.
 *    GCC provides three magic variables which hold the name of the current
 *    function, as a string.  The first of these is `__func__', which is
 *    part of the C99 standard.  `__FUNCTION__' is another name for
 *    `__func__'.  `__PRETTY_FUNCTION__' is the decorated version of the
 *    function name.
 *
 *    Visual Studio defines only __FUNCTION__, so a definition is provided
 *    below.
 */

#ifdef WIN32                                          /* Win32                */

#ifndef __func__
#define __func__        __FUNCTION__
#endif

#else                                                 /* POSIX                */

#ifndef __func__
#if __STDC_VERSION__ < 199901L
#if __GNUC__ >= 2
#define __func__        __FUNCTION__                  /* bald func names      */
#else
#define __func__        "<unknown>"
#endif
#endif
#endif

#endif                                                /* end of POSIX/Win32   */

/**
 *    Place EXTERN_C_DEC before any external function declarations in the C
 *    modules, and place EXTERN_C_END after them.  These macros allow C++
 *    code to include those C header files without any extra typing.
 */

#ifndef EXTERN_C_DEC

#ifdef __cplusplus
#define EXTERN_C_DEC   extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C_DEC
#define EXTERN_C_END
#endif

#endif   /* #ifndef EXTERN_C_DEC */

#endif                                             /* XPCCUT_MACROS_SUBSET_H  */

/*
 * macros_subset.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
