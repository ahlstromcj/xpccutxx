#*****************************************************************************
# Makefile.am (xpccut/include)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpccut
# \author     	Chris Ahlstrom
# \date       	2011-12-26
# \update     	2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This file provides an GNU Autotools makefile for the xpccut library's
#     ./include directory.
#
# 		This file is a stub Makefile.am for the include directory, which
# 		itself has subdirectories.
#
# 		In EXTRA_DIST and MAINTAINERCLEANFILES, we don't currently support a
# 		config.h.in (xpc-config.h is what we use).
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
#  Extra files in this directory
#-----------------------------------------------------------------------------

EXTRA_DIST =

#*****************************************************************************
#  Macros
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

readmeincludedir=$(xpcincludedir)

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
# SUBDIRS
#-----------------------------------------------------------------------------

SUBDIRS = xpc

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------

DIST_SUBDIRS = $(SUBDIRS)

#******************************************************************************
# Installing xpc-config.h
#------------------------------------------------------------------------------
#
#     This is now done in the xpc_basic/include/xpc make-file.
#
#------------------------------------------------------------------------------

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/xpc-x.y if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(xpcincludedir) manually"

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
	@echo "top_srcdir    = $(top_srcdir) [xpc_suite root directory]"
	@echo "builddir      = $(builddir)"

#******************************************************************************
# Makefile.am (xpccut/include)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
