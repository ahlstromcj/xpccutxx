dnl \file          configure.ac
dnl \library       xpccut
dnl \author        Chris Ahlstrom
dnl \date          2011-12-26
dnl \update        2015-10-06
dnl \version       $Revision$
dnl \license       $XPC_SUITE_GPL_LICENSE$
dnl
dnl   WARNING:  Whenever you update this file, run the following to back up
dnl             this file:  "./bootstrap --backup"
dnl
dnl   This script is for the XPCCUT library projects.
dnl   This script supports the automake process employed by GNU software.
dnl   This script is used for generating the "configure" script.
dnl   To generate this script, run the "bootstrap" script.  To clean up
dnl   this script and any built modules, run the script with the "--clean"
dnl   option.
dnl
dnl \options
dnl   There are many options over and above the default autoconf options.
dnl   Run './configure --help' for more information, or see the Doxygen
dnl   xpc_automake.dox document.  The most common option used is
dnl   --enable-debug.  Also see the Doxygen document for more references.
dnl
dnl \prerequisites
dnl   -  autoconf
dnl   -  autoconf-archive
dnl   -  automake
dnl   -  gcc
dnl   -  g++
dnl   -  cvs
dnl   -  libtool
dnl   -  pkg-config
dnl   -  valgrind (only recommended)
dnl   -  more.....
dnl
dnl \references
dnl   -# http://sources.redhat.com/autobook/
dnl      "GNU Autoconf, Automake, and Libtool", by Gary Vaughn et. al.
dnl      There was once a hardcopy of the "Autobook", but you can find the
dnl      latest version of it online at that URL.
dnl   -# http://autoconf-archive.cryp.to/autoconf-archive-2008-11-07.tar.bz2
dnl   -# http://www.freesoftwaremagazine.com/books/agaal/building_shared_libraries_once_using_autotools
dnl      An excellent coverage and explanation of many concepts related to
dnl      shared libraries and libtool, including a inset discussing Windows
dnl      DLLs.
dnl   -# http://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html
dnl      Shared libraries (specifically version numbering).
dnl
dnl ---------------------------------------------------------------------------
dnl
dnl 0. Autoconf initialization (the current version is 2.67)

AC_PREREQ(2.60)
AC_REVISION($Revision: 1.1.0$)dnl

dnl Modified per as per the following URL:
dnl
dnl      http://www.adp-gmbh.ch/misc/tools/configure/configure_in.html
dnl
dnl Full version: AC_INIT(xpc, [$VERSION], [bug-report], [tar-name])

AC_INIT(xpc, [1.1], [ahlstromcj@gmail.com])
AC_CONFIG_AUX_DIR([aux-files])
AC_CONFIG_SRCDIR([src/dummy.c])

dnl Do we need the following?  No, but it cleans up a lot of warning messages.

AC_CANONICAL_SYSTEM

dnl Arguments have been moved from AM_INIT_AUTOMAKE to AC_INIT above.
dnl See http://www.delorie.com/gnu/docs/automake/automake_24.html

AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([include/xpc/config.h])

dnl Although we sequester our 'config.h' file inside an 'xpc' directory,
dnl so it won't be confused with the 'config.h' files of other developer's
dnl projects, it seems safer to also alter the filename using this handy
dnl macro from the autoconf-archives package.  It also prepends "XPC_"
dnl to a lot of the macro definitions, for the best guarantee of avoiding
dnl naming conflicts.

AX_PREFIX_CONFIG_H([include/xpc/xpc-config.h], [xpc])

dnl New for autoconf 2.60, prevents spurious datarootdir warnings.

AC_DEFUN([AC_DATAROOTDIR_CHECKED])

dnl 1. Package version information.  The first item is the name that covers
dnl    a number of libraries, all of whose names start with "xpc".

PACKAGE=xpc
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_SUBST(PACKAGE)

VERSION="1.1.2"
AC_DEFINE_UNQUOTED(VERSION, "$VERSION")
AC_SUBST(VERSION)

XPC_SUITE_NAME="XPCCUT"
AC_SUBST([XPC_SUITE_NAME])

dnl 1.a. Set up version information for this XPC projects.
dnl
dnl      This versioning is the libtool version.  Obviously, these numbers
dnl      should not be cut-n-paste in all projects.
dnl
dnl      Warning: These values are inserted into libtool's "version-info"
dnl      value, and have to follow libtool rules:
dnl
dnl      Format is 'current:revision:age', where
dnl
dnl      current = Number of the current interface.  If the interface has
dnl      changed, increment this number and set 'revision' to 0.  If the new
dnl      interface has removed elements of the preceding interface, backward
dnl      compatibility has been broken, and you must set 'age' to 0.
dnl
dnl      revision = Implementation number of the most recent interface,
dnl      where 0 is the first.  The runtime loader always uses the library
dnl      with the highest revision.  If you change any source modules,
dnl      increment this number.
dnl
dnl      age = The number of additional interface supported by this library.
dnl      The age must the less than or equal to 'current'.  If the new
dnl      interface is a superset of the preceding interface, 'age' must be
dnl      incremented.
dnl
dnl      A shared library has a name called the 'soname', which consists
dnl      of "lib", the library name, ".so", a period, and a version number
dnl      incremented whenever the interface changes.  Here is an example of
dnl      a 'soname':
dnl
dnl         libxpccut.so.1
dnl
dnl      This file is often a symbolic link to the 'real' name, which adds
dnl      a period, a minor number, and, optionally, a period, and the
dnl      release number.  Here are two examples:
dnl
dnl         libxpccut.so.1.2
dnl         libxpccut.so.1.2.0
dnl
dnl      Finally, there is the 'linker' name, which is the bare library
dnl      name.
dnl
dnl         libxpccut.so
dnl
dnl      The linker will use this designator for that library:
dnl
dnl         -lxpccut
dnl
dnl      Our philosophy is that the API version numbers will match the
dnl      libtool version numbers.
dnl
dnl      Also, we will keep the major and minor version numbers of all XPC
dnl      projects in synch -- they can be considered as pieces of one big
dnl      package.

XPC_API_MAJOR="1"
AC_SUBST([XPC_API_MAJOR])

XPC_API_MINOR="1"
AC_SUBST([XPC_API_MINOR])

XPC_API_PATCH="0"
AC_SUBST([XPC_API_PATCH])

XPC_OFFICIAL_VERSION="$XPC_API_MAJOR.$XPC_API_MINOR.$XPC_API_PATCH"

dnl For version 1.1.0, we reset the libtool version information.

XPC_LT_CURRENT="0"
AC_SUBST([XPC_LT_CURRENT])

XPC_LT_REVISION="0"
AC_SUBST([XPC_LT_REVISION])

XPC_LT_AGE="0"
AC_SUBST([XPC_LT_AGE])

XPC_LIBTOOL_VERSION="$XPC_LT_CURRENT.$XPC_LT_REVISION.$XPC_LT_AGE"

dnl 1.b. Set up the version strings.  The primary version string of interest
dnl      is XPC_API_VERSION.

XPC_API_VERSION="$XPC_API_MAJOR.$XPC_API_MINOR"
AC_SUBST([XPC_API_VERSION])

dnl 1.c. Report the version information in the progress output.

AC_MSG_CHECKING(major version)
AC_MSG_RESULT($XPC_API_MAJOR)
AC_MSG_CHECKING(minor version)
AC_MSG_RESULT($XPC_API_MINOR)
AC_MSG_CHECKING(patchlevel)
AC_MSG_RESULT($XPC_API_PATCH)

dnl 1.d. Set up the suite name and current-project name.

XPC_PROJECT_NAME="XPCCUT"
AC_SUBST([XPC_PROJECT_NAME])

dnl 2. Libtool
dnl
dnl Initialize libtool in the default manner, which turns on shared
dnl libraries if available, and enable static libraries if they do not conflict
dnl with shared libraries.  Also sets the shell variable LIBTOOL_DEPS.
dnl
dnl The latest GNU documentation specifies version 2 of libtool, but Debian
dnl testing provides only 1.5 (Debian unstable now provides 2.2).  So we don't
dnl use these new macros, just the older, deprecated macros.
dnl
dnl LT_PREREQ([1.5])
dnl LT_INIT
dnl AC_SUBST([LIBTOOL_DEPS])
dnl
dnl See <http://www.delorie.com/gnu/docs/libtool/libtool_27.html> for the
dnl usage of LIBTOOL_DEPS.
dnl
dnl Uncomment the following if you want more straightforward debugging.  An
dnl alternative is to specify --disable-shared when running 'configure'.
dnl
dnl AC_DISABLE_SHARED

AC_PROG_LIBTOOL

LT_RELEASE="$XPC_API_MAJOR.$XPC_API_MINOR"
AC_SUBST([LT_RELEASE])

LT_CURRENT="$XPC_API_MAJOR"
AC_SUBST([LT_CURRENT])

LT_REVISION="$XPC_API_MINOR"
AC_SUBST([LT_REVISION])

LT_AGE="$XPC_API_PATCH"
AC_SUBST([LT_AGE])

dnl 3. Set up expandable installation directory(ies).
dnl
dnl These directories are for the libraries in the XPC library suite, but
dnl some files are stored in a more generic area.
dnl
dnl We want all of the header files and library files for the whole suite
dnl to be in the same sub-directory, xpc-1.1.  This will avoid clutter and put
dnl all our code in one place.
dnl
dnl      /usr/include/xpc-1.1/xpc
dnl      /usr/lib/xpc-1.1
dnl      /usr/man/man1
dnl      /usr/share/doc/xpc-1.1/doc/dox/html
dnl
dnl There's no way to build the doxygen document and then install it with
dnl relative references preserved.  We'll handle the whole XPC
dnl documentation in the overarching project directory.
dnl
dnl      xpc_suite/doc/dox/html
dnl
dnl All-suite directories.  Compare these macros with the contents of
dnl the pkgconfig/xpc-1.1.pc.in configuration file.  As an example,
dnl installation should yield "/usr/local/include/xpc-1.1" with a
dnl subdirectory of "xpc" where the actual header files are installed.

xpcincludedir="${includedir}/xpc-${XPC_API_VERSION}"
AC_SUBST(xpcincludedir)

xpclibdir="${libdir}/xpc-${XPC_API_VERSION}"
AC_SUBST(xpclibdir)

dnl XPC-specific documentation directory.

xpcdocdir="${datadir}/doc/xpc-${XPC_API_VERSION}"
AC_SUBST(xpcdocdir)

xpcdoxygendir="${datadir}/doc/xpc-${XPC_API_VERSION}"
AC_SUBST(xpcdoxygendir)

dnl Additional directory items not used here:
dnl
dnl   pkgdocdir, pkgdatadir, pkglibdir

dnl 4. Get external flags.

CFLAGS="${CFLAGS}"

dnl 5. Checks for programs.
dnl
dnl 5.a. Plain C support.
dnl
dnl Check for POSIX

AC_ISC_POSIX

dnl Check for the presence of Doxygen.  Not an error to be missing, but
dnl we don't want to try to build documentation if it is not present.
dnl
dnl See http://www.gnu.org/software/autoconf-archive/ax_prog_doxygen.html

AC_PROG_DOXYGEN

dnl 5.b. gcc support.
dnl
dnl echo "gcc version trigger:  $gcc_version_trigger"
dnl echo "gcc version full:     $gcc_version_full"
dnl echo "gcc version:          $gcc_version"
dnl echo "gcc version major:    $gcc_version_major"
dnl echo "gcc version minor:    $gcc_version_minor"

AC_GCC_VERSION

dnl C++ support.  Run tests in C++.  Note the order; avoids messy warnings.

AC_LANG_CPLUSPLUS dnl or AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CXXCPP

dnl Use the C99 standard, even though GNU does not completely support it yet.

CFLAGSTD=""

echo "checking gcc version...  $gcc_version"

if test "$gcc_version" = "2.95.4" ; then
   echo "turning off GNU99/C99 flag"
   CFLAGSTD=""
fi

echo "checking gcc major version... $gcc_version_major"

if test "$gcc_version_major" -ge "3" ; then

   echo "setting version 3-and-up options"
   CFLAGSTD="-std=gnu99"

   if test "$gcc_version_major" -ge "4" ; then
      echo "setting version 4-and-up options"
      CFLAGSTD="${CFLAGSTD}"
      echo "checking gcc minor version... $gcc_version_minor"

      if test "$gcc_version_minor" -ge "2" ; then
         echo "setting version 4.2-and-up options"
         CFLAGSTD="${CFLAGSTD} -fgnu89-inline"
      fi
   fi
fi

echo "CFLAGSTD is now set to $CFLAGSTD"

dnl 5.c. Mingw support.
dnl
dnl Checks for the MingW32 compiler environment. If present, sets shell
dnl variable MINGW32 to 'yes'. If not present, sets MINGW32 to the
dnl empty string.
dnl
dnl Also check to see if the user specified the MingW32 installation
dnl location.  On Debian, the installation is in /usr/i586-mingw32msvc/.
dnl The Mingw versions of the executables start with "i586".  For example,
dnl the compiler is "i586-mingw32msvc-gcc-3.4.5".
dnl
dnl http://www.wxwidgets.org/wiki/index.php/Cross-Compiling_Under_Linux
dnl
dnl is a good place to start learning about building Windows applications.

AC_XPC_MINGW32
AC_ARG_WITH(mingw32,[  --with-mingw32[=DIR]    MingW32 installation location])

dnl Checks for the Cygwin environment. If present, sets shell variable
dnl CYGWIN to 'yes'; if not present, sets CYGWIN to the empty string.

AC_CYGWIN

dnl Clear out the X11 flags for the case where we are cross-compiling
dnl for i586-mingw32 targets, under UNIX.

if test "x$CYGWIN" = "xyes" -o "x$MINGW32" = "xyes" ; then
   X_CFLAGS=""
   X_LIBS=""
fi

dnl 6.0  Top portion of the config.h/xpc-config.h header files.
dnl      The xpc-config.h header file has "XPC_" prepended to these
dnl      automatically for us.  Also, we can't use
dnl      #define XPC_API_VERSION "${XPC_API_VERSION}",
dnl      since the macro call in quotes does /not/ get expanded.
dnl
dnl      Note: The AX_PREFIX_CONFIG_H macro called way above prepends
dnl            "XPC_" to these values when it generated xpc-config.h.

AH_TOP(

#define VERSION_DATE_SHORT "2015-10-05"
#define VERSION "1.1.2"
#define API_VERSION "1.1"

)

dnl See if our own XPCCUT library has been installed.  If so, this macro
dnl will create XPCCUT_LIBS and XPCCUT_CFLAGS as specified in the
dnl xpccut-1.0.pc file that was installed with the xpccut library.
dnl
dnl PKG_CHECK_MODULES([XPCCUT], [xpccut-1.0 >= 1.0])
dnl
dnl Right now, though, we're going to use only the library as built
dnl in this source tree.

dnl 7. Checks for build configuration.
dnl
dnl 7.a. Compiling with debugging, coverage, or profiling options.
dnl
dnl      --enable-debug
dnl      --enable-coverage
dnl      --enable-profiling

AC_XPC_DEBUGGING

dnl AC_XPC_ERRORLOG
dnl AC_XPC_PROFILING
dnl AC_XPC_ENDIAN
dnl AC_XPC_SIXTYFOUR

dnl 7.f. Set up for stack checking.  --enable-stackcheck
dnl
dnl Enabling this feature can cause problems such as segfaults.  These
dnl problems do not necessarily indicate stack problems.
dnl
dnl -fstack-check currently removed.  It seems to make some applications
dnl segfault very early, spuriously.  We need to research this switch.

AC_XPC_STACKCHECK

dnl 7.g. Set up for inactive null-pointer checking.  --disable-nullptr
dnl
dnl Use this option only if your code is certain to work, and you need
dnl a little extra speed.

AC_XPC_NULLPTR

dnl 7.h. Set up for inactive "this-pointer" checking.  --disable-thispointer
dnl
dnl "This-pointer checking" is used when passing a structure to a
dnl function via a pointer.  Normally, it is a very paranoid check.
dnl Such paranoia is necessary in C code, but if the C code is wrapped in
dnl a class, the class can insure that the object exists, so this check can
dnl be avoided.

AC_XPC_THISPTR

dnl 7.f. Check for readline support.  --without-readline
dnl
dnl Right now it is off by default.

dnl 9.  Set up other options in the compiler macros.
dnl
dnl -Werror currently removed.  It causes massive errors from the C/C++
dnl header files under gcc.  Without it, these messages don't even appear as
dnl warnings.
dnl
dnl -W is replaced by -Wextra, which supercedes it.
dnl
dnl Some features of the C code (e.g. NAN support in nan_inf.c) require that
dnl an -std=gnu99 or -std=c99 option be added to the gcc command line.
dnl Apparently this option is (in some manner) in force for C++ code.  For
dnl example, the NAN macro in /usr/include/bits/nan.h was picked up if
dnl /usr/include/math.h was included in a C++ file, but not in a C file,
dnl unless the gnu99 C standard was specified.
dnl
dnl In addition, we're going to support C++11 to the extent that the GNU
dnl implementation supports it.  Therefore, "-std=c++11" is specified.

APIDEF="-DAPI_VERSION=\"$XPC_API_VERSION\""
WARNFLAGS="-Wall -Wextra -pedantic $WARNINGS"
COMMONFLAGS="$WARNFLAGS -D_REENTRANT $APIDEF $DBGFLAGS $STACKCHK $NOERRLOG $NOTHISPTR"

dnl First set up CXXFLAGS without the C-only options in CFLAGSTD.  Then
dnl set up CFLAGS with CFLAGSTD.  For C++, we silence the warnings about the
dnl non-standard __VA_ARGS__ variadic macro.

CXXFLAGS="$CFLAGS $COMMONFLAGS -Wno-variadic-macros -std=c++11"
CFLAGS="$CFLAGS $CFLAGSTD $COMMONFLAGS"

dnl CFLAGS="${CFLAGS} $COMMONFLAGS $XPCCUT_CFLAGS"

dnl Add $WARNINGS_ENABLED to CXXFLAGS if you want to see all warnings, all
dnl the time.

WARNINGS_ENABLED="-Wall"
WARNINGS_DISABLED="-Wno-unused-parameter -Wno-non-virtual-dtor"

dnl 10. Checks for header files.

AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_HEADER_STDC

AC_CHECK_HEADERS([stdarg.h stdio.h stddef.h])
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADERS([limits.h ctype.h sys/time.h time.h errno.h])
AC_CHECK_HEADERS([errno.h sys/sysctl.h])
AC_CHECK_HEADERS([math.h setjmp.h])
AC_CHECK_HEADERS([netdb.h pthread.h syslog.h unistd.h])
AC_CHECK_HEADERS([netinet/in.h])

dnl AC_CHECK_HEADERS([arpa/inet.h])

AC_CHECK_TYPES([errno_t], [], [], [[#include <errno.h>]])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl Checks for library functions.

AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gettimeofday select strerror])

dnl 11. Checks for internationalization macros (i18n).
dnl
dnl These checks are derived from the GNU "hello world" project at
dnl
dnl      http://ftp.gnu.org/gnu/hello/
dnl
dnl The VideoLAN client project (vlc) also looks like a good, but very
dnl complex, example of internationalization.
dnl
dnl ALL_LINGUAS="de en_GB fr it ja no ru nl pl sv"
dnl ALL_LINGUAS="en es en@quot en@boldquot"
dnl
dnl Do we really need ALL_LINGUAS with the LINGUAS file present?
dnl
dnl If we do not want the intl directory, the macro is as used here, with
dnl the keyword 'external'.
dnl
dnl The unit test code supports internationalization.  Version 0.16 of
dnl gettext is suitable for Debian etch and above.
dnl
dnl AM_GNU_GETTEXT_VERSION([0.16])

AC_CHECK_HEADERS([libintl.h locale.h])

AM_PO_SUBDIRS

dnl i18n support currently broken
dnl
dnl AM_GNU_GETTEXT([external])
dnl AM_GNU_GETTEXT_VERSION([0.18.1])

dnl 11.a. Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_TYPES(ptrdiff_t)

dnl 11.b. Checks for library functions.

AC_FUNC_ALLOCA

dnl 11.c. Check for st_blksize in struct stat

AC_ST_BLKSIZE

dnl 11.d. Add the bottom part of config.h.

AH_BOTTOM(
   /* Bottom of header config */
)

dnl po/Makefile.in po/Makefile

AC_CONFIG_FILES([
 Makefile
 doc/Makefile
 doc/dox/Makefile
 man/Makefile
 m4/Makefile
 include/Makefile
 include/xpc/Makefile
 pkgconfig/Makefile
 pkgconfig/xpccut-1.1.pc
 pkgconfig/xpccut++-1.1.pc
 xpccut/Makefile
 xpccut/include/Makefile
 xpccut/include/xpc/Makefile
 xpccut/src/Makefile
 xpccut/tests/Makefile
 xpccut/w32/Makefile
 xpccut++/Makefile
 xpccut++/include/Makefile
 xpccut++/include/xpc/Makefile
 xpccut++/src/Makefile
 xpccut++/tests/Makefile
 xpccut++/w32/Makefile
 ])

AC_OUTPUT

cat << E_O_F

Then do 'make' to compile the XPCCUT libraries.
Do 'make check' or 'make test' to run the tests of the XPCCUT libraries.
Do 'make install' to install them (the po install FAILS).
Do 'make dist', etc. to create gzip, zip, and shar archives.

E_O_F

dnl configure.ac (XPCCUT)
dnl
dnl vim: ts=3 sw=3 et ft=config
