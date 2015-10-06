#*****************************************************************************
# Makefile.am (xpccut++/pkgconfig)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	XPCCUT++
# \author     	Chris Ahlstrom
# \update     	2008-05-07
# \update     	2015-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		A pc.in file is provided to allow other code to use pkgconfig to make
# 		sure they can link with libxpccut.a and the shared object files.
#
#     This makefile also provides a way to make sure the pkgconfig
#     files are included in the 'dist' target.
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# Version variables
#-----------------------------------------------------------------------------
#
# 		Please note that, for package-configuration, we replace PACKAGE with
# 		our own variable, XPC_SUITE_NAME.  The reason is that we want all
# 		libraries in the XPC suite to be installed to the same sub-directory
# 		(currently "xpccut++-1.1").
#
#-----------------------------------------------------------------------------

libdir = @libdir@
xpclibdir = @xpclibdir@
xpcincludedir = @xpcincludedir@
XPC_PROJECT_NAME = @XPC_PROJECT_NAME@
XPC_API_VERSION = @XPC_API_VERSION@

#******************************************************************************
# Package-configuration directory
#------------------------------------------------------------------------------
#
# 	This define is necessary to cause the proper subdirectory to be made
# 	during installation.  'libdir' is a standard automake definition.
#
#------------------------------------------------------------------------------

pkgconfigdir = $(libdir)/pkgconfig

#******************************************************************************
# DATA
#------------------------------------------------------------------------------
#
# 	Constructs the name of our package-configuration file for the current
#	library project.  Here, it will be of the form "xpccut-1.1.pc".
#
#	This file will be used to specify the compiler and linker parameters
#	needed to use this library as installed on the developer's machine.
#
#------------------------------------------------------------------------------

pkgconfig_DATA = \
 xpccut-$(XPC_API_VERSION).pc \
 xpccut++-$(XPC_API_VERSION).pc

#*****************************************************************************
# EXTRA_DIST
#-----------------------------------------------------------------------------

EXTRA_DIST = README

#****************************************************************************
# Makefile.am (xpccut++/pkgconfig)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#----------------------------------------------------------------------------
