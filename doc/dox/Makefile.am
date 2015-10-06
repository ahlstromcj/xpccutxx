#*******************************************************************************
# Makefile.am (XPCCUT dox)
#-------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	XPCCUT
# \author     	Chris Ahlstrom
# \date       	2008-03-04
# \update     	2015-10-06
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		Makefile for the doc/dox subdirectory of the XPCCUT package.
# 		Process this file with automake to produce Makefile.in.
#
#     dist-bzip2 is not available on older versions of autotools.
#
# \todo
# 		-	If the share/doc/xpc-1.1/html directory is empty upon
# 			uninstalling, then remove the xpc-1.1 directory.
# 		-	Figure out a way to use a timestamp-file to keep from building the
# 			documentation every time.
#
#-------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in dox-warnings.log

#*******************************************************************************
# EXTRA_DIST
#-------------------------------------------------------------------------------
#
# 	   Some of the sample files are not included in the tar-file, since only
# 	   a developer of an automake project would ever care about them.
#
# 	   The rest are included automatically by the automake process.
#
# xpc_automake.dox xpc_mainpage_generic.dox xpc_mingw.dox xpc_nice_classes.dox
# xpc_philosophy.dox xpc_subversion.dox xpc_suite.dox
#
#------------------------------------------------------------------------------

EXTRA_DIST = make_dox doxygen.cfg *.dox debian/* debugging/* xpccut/* xpccut++/*

DOXYGEN = @DOXYGEN@
top_srcdir = @top_srcdir@
xpcdocdir = @xpcdocdir@
xpcdoxygendir = @xpcdoxygendir@

#*******************************************************************************
# SUBDIRS
#-------------------------------------------------------------------------------
#
# 		Additional directories containing additional information that may (or
# 		may not) be helpful to the developer.
#
# 		Causes an abort in making, since each of these doesn't have a
# 		Makefile.am.  See the EXTRA_DIST section below, instead.
#
# 		SUBDIRS = Notes design inspiration test
#
#-------------------------------------------------------------------------------

#*******************************************************************************
# html
#-------------------------------------------------------------------------------
#
# 		'$(datadir)' is where UNIX stores application data for installation, and
# 		is usually /usr/share or /usr/local/share.  It is a given, provided by
# 		autoconf.
#
# 		'$(docdir)' is where UNIX stores documentation for installation, and
# 		is usually /usr/share/doc or /usr/local/share/doc.  It is a given,
# 		provided by autoconf.  However, we want our documentation to be kept
# 		in our own area (e.g. /usr/share/doc/xpccut-1.1).
#
# 		In configure.ac, we define xpccutdocdir as
# 		'${datadir}/doc/xpccut-1.1', and then override docdir below.
#
#		This setup makes the documenation consistent with where the headers
#		are installed (e.g. '/usr/include/xpccut-1.1').
#
#	   $(xpccutdocdir) is defined in the configure.ac file.
#
#	   Another way to do it, even more cumbersome:
#
#			docdir = $(datadir)/doc/$(PACKAGE)/$(API_VERSION)/html
#
#-------------------------------------------------------------------------------

docdir = $(xpcdocdir)

#-------------------------------------------------------------------------------
# Cannot use this directive, as it calls "install" which fails trying to
# copy the subdirectories shown to /usr/local/share/doc/xpc-1.1.  We have to
# do it ourselves.
#
#    		doc_DATA = html/* pdf/*
#
#-------------------------------------------------------------------------------

#*******************************************************************************
# Doxygen documentation
#-------------------------------------------------------------------------------
#
#	.PHONY
#
#		The .PHONY directive prevents 'make' from getting confused by an
#		actual file called 'docs' or 'clean', and allows 'make' to continue in
#		spite of errors from doxygen or rm.  A phony target is one that is
#		just a name for some commands to be run on the behest of a specific
#		request.  Declaring a target as phony leaves it out of the search for
#		implicit build rules, enhancing efficiency.
#
#		See "*Note Phony Targets::" in the 'info make' command, if you have
#		GNU installed on your system.
#
#		There are some automake files out there that allow a lot of
#		command-line configuration of doxygen:
#
# 	http://sources.redhat.com/ml/automake/2003-12/msg00170/dx_automake.tar.gz
#
# 	http://www.bioinf.uni-freiburg.de/~mmann/HowTo/automake.html#doxSteps
#
# 		However, we just want to be able to build what we have with the
# 		doxygen.cfg that we have.
#
# 	.PHONY: docs clean
#
#	For the 'clean', we don't want to remove these files:
#
#		rm -f Makefile.in Makefile
#
# 	DOXYGEN
#
#		This variable is created by the AC_PROG_DOXYGEN macro defined in
#		m4/xpc_doxygen.m4 and used in the configure.ac script.
#
#-------------------------------------------------------------------------------

.PHONY: clean
.PHONY: docs
.PHONY: pdf

all: html/* pdf/*

html/*: docs

# The $(srcdir) component points us to the correct place whether the build
# is in-source or out-of-source.
#
# $(DOXYGEN) $(srcdir)/doxygen.cfg > dox-progress.log 2> dox-errors.log
#
# Can't use pushd/popd because they are bash builtins and are not available.

docs: dox-stamp
	$(srcdir)/make_dox $(srcdir) docs

# We do a little trick to make the margins of the document narrower, to get
# more text per page, by adding a directive to the tex file and rebuilding
# the PDF file.

# make --directory=latex pdf
# sed -e '35a\\\usepackage[margin=2cm]{geometry}' -i latex/refman.tex

pdf/*: docs
	sed -e 's/letterpaper,/letterpaper,margin=2cm,/' -i latex/refman.tex
	-make --directory=latex pdf
	mkdir -p pdf
	cp latex/refman.pdf pdf/XPC_CUT_Libraries_1.1.pdf

dox-stamp: $(EXTRA_DIST)
	touch dox-stamp

clean:
	-rm -rf html
	-rm -rf latex
	-rm -f *.log dox-stamp
	-rm -rf pdf

#******************************************************************************
# Installing documentation
#------------------------------------------------------------------------------
#
# 	   We need to add an install-data-hook to copy the generated
# 	   documentation directories to the destination directory.  The normal
# 	   method doesn't work because /usr/bin/install will only install files,
# 	   and automake doesn't give it the switch needed to install directories.
#
#------------------------------------------------------------------------------

install-data-local:
	@echo "mkdir -p '$(xpcdoxygendir)'"
	mkdir -p $(xpcdoxygendir)
	@echo "cp -r -p '$(top_builddir)/doc/dox/html $(xpcdoxygendir)'"
	cp -r -p $(top_builddir)/doc/dox/html $(xpcdoxygendir)
	@echo "cp -r -p '$(top_builddir)/doc/dox/pdf $(xpcdoxygendir)'"
	cp -r -p $(top_builddir)/doc/dox/pdf $(xpcdoxygendir)

#*******************************************************************************
# uninstall-hook
#-------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/share/doc/xpccut-1.1/html if it is
#     empty, and then remove /usr/local/share/doc/xpccut-1.1 too, if empty.
#		However, we don't have a good way to do it yet.
#
#-------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(docdir) manually"
	@echo "Note:  you may want to remove $(xpcdocdir) manually"

#******************************************************************************
# Makefile.am (dox)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
