#******************************************************************************
# Makefile.am (XPCCUT/include/xpc)
#------------------------------------------------------------------------------
##
# \file     	Makefile.am
# \library     XPCCUT
# \author      Chris Ahlstrom
# \date        2011-12-27
# \update      2015-10-05
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

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile config.h stamp-h1 xpc-config.h

#******************************************************************************
# Macros
#------------------------------------------------------------------------------

xpcincludedir = @xpcincludedir@

#******************************************************************************
# Include directory for installation
#------------------------------------------------------------------------------

xpcxpcincludedir=$(xpcincludedir)/xpc

#******************************************************************************
# EXTRA_DIST
#------------------------------------------------------------------------------
#
# 		Note that we have to add "*.h" to ensure these files get distributed,
# 		since we moved them from src.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.h

#******************************************************************************
# Include directory
#------------------------------------------------------------------------------
#
# 		This define is necessary to cause the proper subdirectory to be made
# 		during installation.  'xpcincludedir' is defined in the configure.ac
# 		script.
#
#  	Without this value, the header files are stored in
#  	"$(prefix)/include/xpc-1.1" instead of
#  	"$(prefix)/include/xpc-1.1/xpc".
#
#------------------------------------------------------------------------------

pkgincludedir=$(xpcincludedir)/xpc

#******************************************************************************
# HEADERS
#------------------------------------------------------------------------------
#
# 		Distributes the Microsoft version of the xpc-config.h file.
#
#------------------------------------------------------------------------------

include_HEADERS = 		\
	xpc-config-msvc.h		\
	configuration.h

pkginclude_HEADERS = 	\
	xpc-config-msvc.h		\
	configuration.h

#******************************************************************************
# Installing xpc-config.h
#------------------------------------------------------------------------------
#
# 	   We need to add an install-data-hook to copy the generated xpc-config.h
# 	   file to the destination directory.  The method for this is described
# 	   at http://sources.redhat.com/autobook/autobook/autobook_108.html
# 	   "Install hooks".
#
#		Note that rather than have an xpc-config.h file for each sub-project,
#		XPC Basic just has one, in xpc_basic/include/xpc.
#
#     We must adjust for out-of-source builds as well.
#
#		install-data-hook is better than install-data-local because it isn't
#		affected by parallel builds.
#
#------------------------------------------------------------------------------

install-data-hook:
	cp $(top_builddir)/include/xpc/$(PACKAGE)-config.h $(xpcxpcincludedir)

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/xpc-1.1/xpc if it is
#     empty.  However, we don't have a good way to do it yet if multiple XPC
#     major projects have been installed.  For now, we assume the user wants
#     it all gone, and we blow it away.
#
#		'rm -rf $(xpcxpcincludedir)' removes xpc, but not xpc-1.1
#
#------------------------------------------------------------------------------

uninstall-hook:
	rm -rf $(xpcincludedir)

#******************************************************************************
# Makefile.am (XPC/include/xpc)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
