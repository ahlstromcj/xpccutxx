#*****************************************************************************
# Makefile.am (xpccut++/include/xpc)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpccut++
# \author     	Chris Ahlstrom
# \date       	2008-02-29
# \updates    	2015-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#		This is an "include/xpc" makefile for the xpccut++ project.
#  	It is useful for installing header files in a subdirectory of the
#  	project include directory, /usr/include, /usr/local/include, etc.
#
# \note
#     The normal and re-named configuration header files created are:
#
#        -  config.h
#        -  xpc-config.h
#
#     Note the second name -- it is not xpccut-config.h, as you might
#     expect.  All suite projects share the same configuration.
#
# \todo
#     If the ...include/xpc-1.1/xpc directory is empty, then remove the
#     xpc-1.0 directory upon uninstalling.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#****************************************************************************
# EXTRA_DIST
#----------------------------------------------------------------------------
#
#     Note that we have to add "*.h" to ensure these files get distributed,
#     since we had to move them from src/Makefile.am.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.hpp *.h

#*****************************************************************************
#	Packaging.
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

#******************************************************************************
# Include directory
#------------------------------------------------------------------------------
#
#  This define is necessary to cause the proper subdirectory to be made
#  during installation.  'xpcincludedir' is defined in the configure.ac
#  script.
#
#  Without this value, the header files are stored in
#  "$(prefix)/include/xpc-1.0" instead of "$(prefix)/include/xpc-1.0/xpc".
#
#------------------------------------------------------------------------------

pkgincludedir=$(xpcincludedir)/xpc

#****************************************************************************
# Header files
#----------------------------------------------------------------------------
#
# Do not distribute the configure-generated config.h (but it turns out it is
# distributed during the install, which is okay.)
#
# dist-hook:
#    rm -f $(distdir)/config.h
#
#------------------------------------------------------------------------------

pkginclude_HEADERS = \
 cut.hpp \
 cut_fuzz.hpp \
 cut_options.hpp \
 cut_sequence.hpp \
 cut_status.hpp

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
#     We'd like to remove /usr/local/include/xpc-1.0/xpc if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(pkgincludedir) manually"

#****************************************************************************
# Makefile.am (xpccut++/include/xpc)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#----------------------------------------------------------------------------
