#*******************************************************************************
# Makefile.am (m4)
#-------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpccut
# \author     	Chris Ahlstrom
# \date       	2008-05-04
# \update      2013-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This module provides an Automake makefile for the XPC library suite's
#     main 'm4' directory.
#
#     This makefile provides mostly just a way to make sure the m4 files are
#     included in the 'dist' target.
#
#-------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile

PACKAGE = @PACKAGE@
VERSION = @VERSION@
API_VERSION = @API_VERSION@
datarootdir = @datarootdir@
xpcincludedir=@xpcincludedir@

#*******************************************************************************
# EXTRA_DIST
#-------------------------------------------------------------------------------

EXTRA_DIST = gettext.m4 iconv.m4 lib-ld.m4 lib-link.m4 lib-prefix.m4 nls.m4 po.m4 progtest.m4

#******************************************************************************
#	install-data-am
#------------------------------------------------------------------------------
#
#		This target is a dangerous target, and works only because the m4
#		Makefile.am does not have any data to install.  If it ever does, this
#		target will not be correct.  See xpc/m4/Makefile.am for more
#		description.
#
#     We don't want to install the m4 directory, after all.
#
#        install-data-am:
#           mkdir -p $(xpcincludedir)/xpc/m4
#
#------------------------------------------------------------------------------

#******************************************************************************
# Makefile.am (m4)
#------------------------------------------------------------------------------
# 	vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
