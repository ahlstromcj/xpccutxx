#*****************************************************************************
# Makefile.am (xpccut/tests)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpccut
# \author     	Chris Ahlstrom
# \date       	2008-03-02
# \updates     2013-10-06
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		This module provides an Automake makefile for the XPC CUT library test
# 		applications.
#
#  	This makefile provides the skeleton needed to build the test
#  	directory using GNU autotools.
#
# \note
# 		Once the library is installed, the following applies to building
# 		executables using the library and using libtool:
#
#			Libraries have been installed in: /usr/local/lib/xpc-1.1
#
#		If you ever happen to want to link against installed libraries in a
#		given directory, LIBDIR, you must either use libtool, and specify
#		the full pathname of the library, or use the `-LLIBDIR' flag during
#		linking and do at least one of the following:
#
#			- add LIBDIR to the `LD_LIBRARY_PATH' environment variable
#			  during execution
#			- add LIBDIR to the `LD_RUN_PATH' environment variable
#			  during linking
#			- use the `-Wl,--rpath -Wl,LIBDIR' linker flag
#			- have your system administrator add LIBDIR to `/etc/ld.so.conf'
#
#		See any operating system documentation about shared libraries for
#		more information, such as the ld(1) and ld.so(8) manual pages.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#****************************************************************************
# EXTRA_DIST
#----------------------------------------------------------------------------
#
#		"test" is the script that runs the unit-test application with a number
#		of parameters in order to get better test coverage of all of the
#		unit-test application's options.
#
#----------------------------------------------------------------------------

EXTRA_DIST = README test test_script *.txt *.input

#******************************************************************************
# Packaging items from configure.ac
#-------------------------------------------------------------------------------
#
#  XPCCUT_LIBS is defined by the PKG_CHECK_MODULES macro.  The xpccut
#  library must be installed officially in order to be used to build the
#  unit-test application of the xpc library.  2012-06-17:  NO LONGER TRUE.
#
#-------------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

XPC_API_MAJOR = @XPC_API_MAJOR@
XPC_API_MINOR = @XPC_API_MINOR@
XPC_API_PATCH = @XPC_API_PATCH@
XPC_API_VERSION = @XPC_API_VERSION@

XPC_LT_CURRENT = @XPC_LT_CURRENT@
XPC_LT_REVISION = @XPC_LT_REVISION@
XPC_LT_AGE = @XPC_LT_AGE@

XPCCUT_LIBS = @XPCCUT_LIBS@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------

prefix = @prefix@
libdir = @xpclibdir@
datadir = @datadir@
datarootdir = @datarootdir@
xpclibdir = @xpclibdir@
xpcdocdir = @xpcdocdir@
xpcdoxygendir = @xpcdoxygendir@
xpcincludedir = @xpcincludedir@

#******************************************************************************
# localedir
#------------------------------------------------------------------------------
#
#  'localedir' is the normal system directory for installed localization
#  files.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------
#
#		top_srcdir    = ../../.. [XPC Basic directory]
#		builddir      = /home/ahlstrom/ca/mls/git/XPC Basic-1.1/debug
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

cutlibdir = $(builddir)/xpccut/src/.libs

#*****************************************************************************
# libtool
#-----------------------------------------------------------------------------
#
#     GNU automake assigns, for us: LIBTOOL_DEPS = @LIBTOOL_DEPS@
#
#-----------------------------------------------------------------------------
#
# LIBTOOL_DEPS = @LIBTOOL_DEPS@
#
# libtool: $(LIBTOOL_DEPS)
#    $(SHELL) ./scripts/config.status --recheck
#
#-----------------------------------------------------------------------------

version = $(XPC_API_MAJOR):$(XPC_API_MINOR):$(XPC_API_PATCH)

#******************************************************************************
# AM_CPPFLAGS [formerly "INCLUDES"]
#------------------------------------------------------------------------------
#
# 	'AM_CPPFLAGS' is the set of directories needed to access all of the
# 	XPC-suite library header files used in this project.
#
#------------------------------------------------------------------------------

AM_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/xpccut/include \
 -I../../include

#****************************************************************************
# Project-specific dependency files
#----------------------------------------------------------------------------
#
#  Provdies the specific list of dependencies, to assure that the make
#  detects all changes, if they are available.
#
#  Since this is our own library and test application, we will assume
#  the xpccut-1.1 project has already been built.
#
#----------------------------------------------------------------------------

dependencies = $(cutlibdir)/libxpccut.a

#****************************************************************************
# Project-specific library files
#----------------------------------------------------------------------------
#
#	Specifies the names of the libraries to be linked with the test
#	application.
#
# 	The ordering of libraries is important, to avoid unresolved externals.
# 	If library libA needs library libB, then -lB should come after -lA.
#
# 	In some cases, libraries have to be included more than once.
# 	This is a minor limitation in the GNU linker.
#
#----------------------------------------------------------------------------

libraries = -L$(cutlibdir) -lxpccut

#******************************************************************************
# libmath
#-----------------------------------------------------------------------------
#
# 		One day, we got errors about sqrt() undefined, which we fixed by
# 		adding -lm.  Then one day we got errors about various items in
# 		sys/stat.h being multiply-defined, and it turned out to be the -lm.
#
# 		We make it a define for how to handle it more easily.  You might have
# 		to change it for your system.
#
#-----------------------------------------------------------------------------

libmath = -lm

#****************************************************************************
# The test programs to build
#----------------------------------------------------------------------------
#
# Warning:
#
#		It is very important to note that all of the applications need to
#		include the pthread library.  It can get linked in by the text-synch
#		feature, which will include a mutex object that can be used with the
#		--synch option.
#
#		This option is not ready, but we're still going to support the
#		pthreads library, whether in Linux, Mingw, or Windows.
#
#----------------------------------------------------------------------------

noinst_PROGRAMS =	unit_test_test

#****************************************************************************
# unit_test_test
#----------------------------------------------------------------------------
#
# 		Another option is to provide the following:
#
# 		   check_PROGRAMS = unit_test_test
#
#		This allows the makefile to support "make check".  However, it
#		supports it without it, using the TESTS macro defined below.
#
# unit_test_test_LDADD = @LIBINTL@ -lpthread $(libraries)
#
#----------------------------------------------------------------------------

unit_test_test_SOURCES = unit_test_test.c
unit_test_test_LDADD = -lpthread $(libraries)
unit_test_test_DEPENDENCIES = $(dependencies)

#****************************************************************************
# Testing
#----------------------------------------------------------------------------
#
#		This variable is used in the "make check" command.  We add a target to
#		implement "make test", also.
#
# 		If the variable TESTS is defined, its value is taken to be a list of
# 		programs to run in order to do the testing. The programs can either be
# 		derived objects or source objects; the generated rule will look both
# 		in srcdir and `.'. Programs needing data files should look for them in
# 		srcdir (which is both an environment variable and a make variable) so
# 		they work when building in a separate directory,
# 		and for the distcheck target.
#
# 		The number of failures will be printed at the end of the run. If a
# 		given test program exits with a status of 77, then its result is
# 		ignored in the final count. This feature allows non-portable tests to
# 		be ignored in environments where they don't make sense.
#
# 		The variable TESTS_ENVIRONMENT can be used to set environment
# 		variables for the test run; the environment variable srcdir is set in
# 		the rule. If all your test programs are scripts, you can also set
# 		TESTS_ENVIRONMENT to an invocation of the shell (e.g. `$(SHELL) -x');
# 		this can be useful for debugging the tests.
#
# 		Automake ensures that each program listed in TESTS is built before any
# 		tests are run; you can list both source and derived programs in TESTS.
# 		For instance, you might want to run a C program as a test. To do this
# 		you would list its name in TESTS and also in check_PROGRAMS, and then
# 		specify it as you would any other program. 
#
#----------------------------------------------------------------------------

testsubdir = test-results
TESTS_ENVIRONMENT = 
TESTS = test_script

test: check

#****************************************************************************
# unit_test_lt (libtool version of unit_test_test)
#----------------------------------------------------------------------------
#
#  noinst_PROGRAMS =	unit_test_test unit_test_lt
#
#	unit_test_lt_SOURCES = unit_test_test.c
#	unit_test_lt_LDADD = @LIBINTL@ -lpthread ../src/libxpccut.la
#	unit_test_lt_DEPENDENCIES = ../src/libxpccut.la
#
#----------------------------------------------------------------------------

#****************************************************************************
# distclean
#----------------------------------------------------------------------------

distclean-local:
	-rm -rf $(testsubdir)

#******************************************************************************
# Debugging targets
#------------------------------------------------------------------------------

show:
	@echo "Install directories:"
	@echo
	@echo "prefix        = $(prefix)"
	@echo "datadir       = $(datadir)"
	@echo "datarootdir   = $(datarootdir)"
	@echo "libdir        = $(libdir)"
	@echo "xpclibdir     = $(xpclibdir)"
	@echo "xpcdocdir     = $(xpcdocdir)"
	@echo "xpcdoxygendir = $(xpcdoxygendir)"
	@echo "xpcincludedir = $(xpcincludedir)"
	@echo "localedir     = $(localedir)"
	@echo
	@echo "Local directories:"
	@echo
	@echo "top_srcdir    = $(top_srcdir) [XPC Basic root directory]"
	@echo "srcdir    		= $(srcdir)"
	@echo "top_builddir  = $(top_builddir)"
	@echo "builddir      = $(builddir)"

#*****************************************************************************
# Makefile.am (xpccut/tests)
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#----------------------------------------------------------------------------
