#****************************************************************************
# Makefile.am (xpccut++/tests)
#----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	libxpccut++ subproject
# \author     	Chris Ahlstrom
# \date       	2008-02-29
# \update      2013-10-06
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#		This is a Makefile for the tests directory for the
#  	xpccut++ library project.
#
#  	This module provides an automake makefile for the unit-test, developer
#  	tests, or integration test applications.  This makefile provides the
#  	items needed to build the test applications using GNU autotools.
#
#----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#****************************************************************************
# 	EXTRA_DIST
#----------------------------------------------------------------------------
#
#	Add any extra distributable files here, so they are added to the tar
#	package.
#
#----------------------------------------------------------------------------

EXTRA_DIST = test_script

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
# Installed directories.
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
#		top_srcdir    = ../../.. [xpc_suite directory]
#		builddir      = /home/ahlstrom/ca/mls/git/xpc_suite-1.1/debug
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

cutlibdir = $(builddir)/xpccut/src/.libs
cutpplibdir = $(builddir)/xpccut++/src/.libs

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

#****************************************************************************
# AM_CPPFLAGS [formerly "INCLUDES"]
#----------------------------------------------------------------------------
#
#	Specifies the header-file locations for uninstalled libraries in the XPC
#	source tree.
#
#----------------------------------------------------------------------------

AM_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/xpccut/include \
 -I$(top_srcdir)/xpccut++/include \
 -I../../include

#****************************************************************************
# Project-specific dependency files
#----------------------------------------------------------------------------
#
#  Provdies the specific list of dependencies, to assure that the make
#  detects all changes, if they are available.
#
#  Since this is our own library and test application, we will assume
#  the xpccut-1.0 project has already been built.
#
#----------------------------------------------------------------------------

dependencies = $(cutlibdir)/libxpccut.a $(cutpplibdir)/libxpccut++.a

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

libraries = -L$(cutlibdir) -lxpccut -L$(cutpplibdir) -lxpccut++

#****************************************************************************
# The program(s) to build, but not install
#----------------------------------------------------------------------------

noinst_PROGRAMS = cut_unit_test

#****************************************************************************
# cut_unittest_SOURCES
#----------------------------------------------------------------------------
#
#  Note that we assume the developer has also downloaded and built, in this
#  same directory tree, the C library, xpccut.  If a bug is found, the
#  developer will need both sets of source code anyway.
#
#     if STANDALONE
#     cut_unit_test_DEPENDENCIES = ../src/.libs/libxpccut++.a
#     else
#
# We do not need "-ldl"!
#
# cut_unit_test_LDADD = @LIBINTL@ -lpthread $(libraries)
#
#----------------------------------------------------------------------------

cut_unit_test_SOURCES = cut_unit_test.cpp
cut_unit_test_LDADD = -lpthread $(libraries)
cut_unit_test_DEPENDENCIES = $(dependencies)

#****************************************************************************
# TESTS
#----------------------------------------------------------------------------
#
#     This variable is used in the "make check" command.  We add a target to
#     implement "make test", also.
#
#----------------------------------------------------------------------------

testsubdir = test-results
TESTS_ENVIRONMENT =
TESTS = test_script

test: check

#****************************************************************************
# distclean
#----------------------------------------------------------------------------

distclean-local:
	-rm -rf $(testsubdir)

#****************************************************************************
# Makefile.am (xpccut++/tests)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#----------------------------------------------------------------------------
