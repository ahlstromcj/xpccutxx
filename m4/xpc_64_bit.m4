dnl ***************************************************************************
dnl xpc_64_bit.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file       	xpc_64_bit.m4
dnl \library    	xpc_suite subproject
dnl \author     	Chris Ahlstrom
dnl \date       	03/04/2008-03/04/2008
dnl \version    	$Revision$
dnl \license    	$XPC_SUITE_GPL_LICENSE$
dnl
dnl   Tests whether the user wants 64-bit compiling (in a 32-bit environment).
dnl
dnl   Set up for 64 bits.  This macro is used to get the arguments supplied
dnl   to the configure script (./configure --enable-debug)
dnl   
dnl   It defines the symbol LP64, which is added to the CFLAGS for the
dnl   compiler call.
dnl
dnl   In addition, the support is turned on via the -m64 flag.
dnl
dnl   On UNIXen, the 64-bit model is LP64.  On Windows, it is an LLP64 model.
dnl
dnl   32-bit sets int, long and pointer to 32 bits and generates code that runs
dnl   on any i386 system.  64-bit sets int to 32 bits and long and pointer to
dnl   64 bits and generates code for AMD's x86-64 architecture.
dnl
dnl   Right now, using lp64 doesn't work.  Probably just need versions of
dnl   binutils that support 64-bit builds, or a dev package such as
dnl   libc6-dev-amd64.  Some standard functions [e.g. stpcpy()] fail the tests,
dnl   both on Gentoo and Debian.
dnl
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_SIXTYFOUR],
[
   BITFLAGS=
   if test -n "$GCC"; then

      AC_MSG_CHECKING([whether to enable 64 bit compilation, very problematic])
      AC_ARG_ENABLE(lp64,
         [  --enable-lp64=(no/yes) Turn on 64 bit code (default=no)],
         [
          case "${enableval}" in
           yes) lp64=yes ;;
            no) lp64=no  ;;
             *) AC_MSG_ERROR(bad value ${enableval} for --enable-lp64) ;;
          esac
         ],
         [
            lp64=no
         ])

      AM_CONDITIONAL(DOBIT64, test x$lp64 = xyes)
      if test "x$lp64" = "xyes" ; then
         BITFLAGS="-m64 -DLP64=1"
         AC_MSG_RESULT(yes)
      else
         BITFLAGS="-DLP64=0"
         AC_MSG_RESULT(no)
      fi
   fi
   AC_SUBST([BITFLAGS])
   AC_DEFINE_UNQUOTED([BITFLAGS], [$BITFLAGS],
   [Define BITFLAGS=-m64 if 64-bit support is wanted.])
])

dnl ***************************************************************************
dnl xpc_64_bit.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
