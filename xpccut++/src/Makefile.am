#******************************************************************************
# Makefile.am (xpccut++/src)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpccut++
# \author     	Chris Ahlstrom
# \date       	2008-03-04
# \updates    	2015-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#   	This is a "src" makefile applied to the CUT++ library project.
#
#     This module provides a blank Automake makefile for the src directory
#     of the cut++ library.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# 	CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#******************************************************************************
# 	EXTRA_DIST
#------------------------------------------------------------------------------

EXTRA_DIST =

#******************************************************************************
# Items from configure.ac
#------------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

XPC_API_MAJOR = @XPC_API_MAJOR@
XPC_API_MINOR = @XPC_API_MINOR@
XPC_API_PATCH = @XPC_API_PATCH@
XPC_API_VERSION = @XPC_API_VERSION@

XPC_LT_CURRENT = @XPC_LT_CURRENT@
XPC_LT_REVISION = @XPC_LT_REVISION@
XPC_LT_AGE = @XPC_LT_AGE@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------
#
# 		The 'libdir' define is necessary to cause the proper subdirectory to
# 		be made during installation.  'xpclibdir' is defined in the
# 		configure.ac script.  The 'libdir' define is needed for work with
# 		libtool.  Not sure about 'pkglibdr'.
#
# 				pkglibdir=$(xpclibdir)
#
#	Directories and macros:
#
#		prefix        = /usr/local
#		datadir       = /usr/local/share
#		datarootdir   = /usr/local/share
#		libdir        = /usr/local/lib/xpc
#		xpclibdir     = /usr/local/lib/xpc
#		xpcdocdir     = /usr/local/share/doc/xpc-1.1
#		xpcdoxygendir = doc
#		xpcincludedir = /usr/local/include/xpc
#		localedir     = /usr/local/share/locale
#		top_srcdir    = ../../.. [xpc_suite directory]
#		builddir      = /home/ahlstrom/ca/mls/git/xpc_suite-1.1/debug
#
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
#	Specifies the header-file locations for uninstalled libraries, including
#	this library, whether built from xpc_suite or from cut.
#
#------------------------------------------------------------------------------

AM_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/xpc/include \
 -I$(top_srcdir)/xpccut/include \
 -I$(top_srcdir)/xpccut++/include \
 -I../../include

#******************************************************************************
# The library to build (and install).
#------------------------------------------------------------------------------
#
# 	Note that we cannot use $(top_srcdir) in lieu of "..", because then
# 	___libcut_a_SOURCES will not match the resulting token.
#
#------------------------------------------------------------------------------

lib_LTLIBRARIES = libxpccut++.la

#******************************************************************************
# SOURCES
#------------------------------------------------------------------------------

libxpccut___la_SOURCES =	\
 cut.cpp \
 cut_fuzz.cpp \
 cut_options.cpp \
 cut_sequence.cpp \
 cut_status.cpp

#******************************************************************************
# LIBADD
#------------------------------------------------------------------------------
#
#  We want to incorporate the specially-compiled version of libxpccut (C
#  library) in this C++ library, because the special version does not check
#  null pointers, and will never be installed by itself (we hope!).
#
#  See
#
#     http://www.gnu.org/software/autoconf/manual/automake/
#        Libtool-Convenience-Libraries.html
#
#------------------------------------------------------------------------------

libxpccut___la_LIBADD =	$(builddir)/xpccut/src/libxpccut.la

#******************************************************************************
# LDFLAGS = -version-info 0:0:0
#------------------------------------------------------------------------------

libxpccut___la_LDFLAGS = -version-info $(version)

#******************************************************************************
# DEPENDENCIES
#------------------------------------------------------------------------------

libxpccut___la_DEPENDENCIES = $(abs_top_builddir)/xpccut/src/libxpccut.la

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/xpc-1.0 if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(libdir) manually"

#******************************************************************************
# Makefile.am (xpccut++/src)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
