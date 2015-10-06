/**
 * \file          configuration.h
 * \library       xpc_basic
 * \author        Chris Ahlstrom
 * \date          2012-01-07
 * \update        2015-10-05
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    All this file does is choose between the Windows and non-Windows
 *    versions of the XPC configuration header file.
 */

#ifdef WIN32                     // Windows 32-bit

#ifdef _MSC_VER                  // no MINGW32 macro available, it seems
#include <xpc-config-msvc.h>     // manually maintained Windows version
#else
#include <xpc-config-mingw32.h>  // manually maintained MingW Windows version
#endif

#else                            // not Windows 32-bit

#include <xpc/xpc-config.h>      // configure-generated version

#endif

/*
 * configuration.h
 *
 * vim: ts=3 sw=3 et ft=c
 */
