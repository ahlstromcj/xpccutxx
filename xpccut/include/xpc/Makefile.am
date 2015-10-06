#******************************************************************************
# Makefile.am (xpccut/include/xpc)
#------------------------------------------------------------------------------
##
# \file     	Makefile.am
# \library     xpccut
# \author      Chris Ahlstrom
# \date        2011-12-26
# \updates     2013-10-05
# \version     $Revision$
# \license     $XPC_SUITE_GPL_LICENSE$
#
#   	This module provides an Automake makefile for the XPC library's
#     ./include/xpc directory.
#
# \note
#   	The normal and re-named configuration header files created are:
#
#     	- 	config.h
#        -  xpc-config.h
#
#		All project share the same file, which is now locating in
#		$(top_srcdir)/include/xpc.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# EXTRA_DIST
#------------------------------------------------------------------------------
#
# 		Note that we have to add "*.h" to ensure these files get distributed,
# 		since we moved them from src.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.h

#*****************************************************************************
#	Macros
#-----------------------------------------------------------------------------
#
#	This section is recommended by:
#
#		http://olympus.het.brown.edu/cgi-bin/info2www?(gettext)Makefile
#
#-----------------------------------------------------------------------------

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

prefix = @prefix@
libdir = @xpclibdir@
datadir = @datadir@
datarootdir = @datarootdir@
xpclibdir = @xpclibdir@
xpcdocdir = @xpcdocdir@
xpcdoxygendir = @xpcdoxygendir@
xpcincludedir = @xpcincludedir@

#******************************************************************************
# Locale directory
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

#******************************************************************************
# Include directory
#------------------------------------------------------------------------------
#
# 		This define is necessary to cause the proper subdirectory to be made
# 		during installation.  'xpcincludedir' is defined in the configure.ac
# 		script.
#
#  	Without this value, the header files are stored in
#  	"$(prefix)/include/xpc-1.0" instead of
#  	"$(prefix)/include/xpc-1.0/xpc".
#
#------------------------------------------------------------------------------

pkgincludedir=$(xpcincludedir)/xpc

#******************************************************************************
# Header files
#------------------------------------------------------------------------------
#
# Do not distribute the configure-generated config.h (but it turns out it is
# distributed during the install, which is okay.)
#
# dist-hook:
#    rm -f $(distdir)/config.h
#
#------------------------------------------------------------------------------

pkginclude_HEADERS = \
	fuzz.h \
	macros_subset.h \
   portable_subset.h \
	unit_test.h \
	unit_test_options.h \
	unit_test_status.h

#******************************************************************************
# Installing xpc-config.h
#------------------------------------------------------------------------------
#
#  However, now the top-level Makefile takes care of this header file, since
#  it is used by all the sub-projects of XPC Basic.  The directory
#  xpc_basic/include/xpc is now the location of this file.
#
#		install-data-local:
#			cp $(PACKAGE)-config.h $(pkgincludedir)
#
#------------------------------------------------------------------------------

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/xpc-1.0/xpc if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(pkgincludedir) manually"

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

#******************************************************************************
# Makefile.am (xpccut/include/xpc)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
