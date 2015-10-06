#*****************************************************************************
# Makefile.am (man)
#-----------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	xpccut++
# \author     	Chris Ahlstrom
# \date       	2008-08-09
# \update     	2015-10-05
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
#     This makefile provides the skeleton needed to build the XPCCUT++'s
#     man-page directory using GNU autotools.
#
#     dist-bzip2 is not available on older versions of autotools.
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
#  EXTRA_DIST
#-----------------------------------------------------------------------------

EXTRA_DIST = $(man_MANS)

#*****************************************************************************
# The documentation to build
#-----------------------------------------------------------------------------

man_MANS = xpccut.1 xpccut++.1

#*****************************************************************************
# End of man/Makefile.am
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#-----------------------------------------------------------------------------
