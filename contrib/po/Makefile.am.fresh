#*****************************************************************************
# Makefile.am (XPCCUT)
#-----------------------------------------------------------------------------
##
# \file        Makefile.am
# \library     xpccut
# \author      Chris Ahlstrom
# \date        2011-12-26
# \updates     2015-10-06
# \version     $Revision$
# \license     $XPC_SUITE_GPL_LICENSE$
#
#   WARNING:  Whenever you update this file, run the following to back up
#             this file:  "./bootstrap --backup"
#
#     This is a project "root directory" makefile for the whole XPC suite.
#     This makefile provides the skeleton needed to build the XPC suite
#     directories using GNU autotools.
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# Extra files in the top-level directory
#-----------------------------------------------------------------------------

EXTRA_DIST = bootstrap build pack README TODO VERSION

#*****************************************************************************
# Packaging.
#-----------------------------------------------------------------------------
#
#     This section is recommended for the top-level Makefile.am by the
#     reference.
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

#*****************************************************************************
# Installed directories.
#-----------------------------------------------------------------------------
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
#		libdir        = /usr/local/lib/xpc
#		datadir       = /usr/local/share
#		datarootdir   = /usr/local/share
#		xpclibdir     = /usr/local/lib/xpc
#		xpcdocdir     = /usr/local/share/doc/xpc-1.1
#		xpcdoxygendir = doc
#		xpcincludedir = /usr/local/include/xpc-1.1
#		localedir     = /usr/local/share/locale
#
#  'localedir' is the normal system directory for installed localization
#  files.
#
#-----------------------------------------------------------------------------

prefix = @prefix@
libdir = @xpclibdir@
datadir = @datadir@
datarootdir = @datarootdir@
xpcdocdir = @xpcdocdir@
xpcdoxygendir = @xpcdoxygendir@
xpcincludedir = @xpcincludedir@
xpclibdir = @xpclibdir@
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
# aclocal support
#------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

#*****************************************************************************
# libtool
#-----------------------------------------------------------------------------
#	${SHELL} $(top_srcdir)/config.status --recheck
#-----------------------------------------------------------------------------

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(top_srcdir)/config.status --recheck

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------
#
#  	po causes an error, need to fix it, later.
#
#-----------------------------------------------------------------------------

SUBDIRS = m4 include xpccut xpccut++ man doc pkgconfig

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------

DISTFILES = ABOUT-NLS
DIST_SUBDIRS = $(SUBDIRS)

#*****************************************************************************
# Tests
#-----------------------------------------------------------------------------
#
#      Uncomment these items only if your project does not support "make
#      check" or "make test".
#
# .PHONY: test check
#
# test: check
#
# check:
#   @echo "'make check' is not implemented in the xpc library."
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# all-local
#-----------------------------------------------------------------------------

all-local:
	@echo "Top source-directory 'top_srcdir' is $(top_srcdir)"
	@echo "* * * * * All build items completed * * * * *"

#******************************************************************************
# Debugging targets
#------------------------------------------------------------------------------

show:
	@echo "Install directories:"
	@echo
	@echo "prefix           = $(prefix)"
	@echo "datadir          = $(datadir)"
	@echo "datarootdir      = $(datarootdir)"
	@echo "libdir           = $(libdir)"
	@echo "xpclibdir        = $(xpclibdir)"
	@echo "xpcdocdir        = $(xpcdocdir)"
	@echo "xpcdoxygendir    = $(xpcdoxygendir)"
	@echo "xpcincludedir    = $(xpcincludedir)"
	@echo "localedir        = $(localedir)"
	@echo
	@echo "Local directories:"
	@echo
	@echo "top_srcdir       = $(top_srcdir) [XPC Basic root directory]"
	@echo "srcdir           = $(srcdir)"
	@echo "top_builddir     = $(top_builddir)"
	@echo "builddir         = $(builddir)"

showver:
	@echo "PACKAGE          = $(PACKAGE)"
	@echo "VERSION          = $(VERSION)"
	@echo "XPC_API_MAJOR    = $(XPC_API_MAJOR)"
	@echo "XPC_API_MINOR    = $(XPC_API_MINOR)"
	@echo "XPC_API_PATCH    = $(XPC_API_PATCH)"
	@echo "XPC_API_VERSION  = $(XPC_API_VERSION)"
	@echo "XPC_LT_CURRENT   = $(XPC_LT_CURRENT)"
	@echo "XPC_LT_REVISION  = $(XPC_LT_REVISION)"
	@echo "XPC_LT_AGE       = $(XPC_LT_AGE)"

#****************************************************************************
# Makefile.am (XPC Basic top-level)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#----------------------------------------------------------------------------
