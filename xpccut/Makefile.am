#*****************************************************************************
# Makefile.am (xpccut)
#-----------------------------------------------------------------------------
##
# \file          Makefile.am
# \library       xpccut
# \author        Chris Ahlstrom
# \date          2008-03-06
# \updates       2012-10-05
# \version       $Revision$
# \license       $XPC_SUITE_GPL_LICENSE$
#
#  	This file is a makefile for the XPCCUT library project.
#
#     This makefile provides the skeleton needed to build the XPCCUT project
#     directory using GNU autotools.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------
#
#		Always use Automake in foreign mode (adding foreign to
#		AUTOMAKE_OPTIONS in Makefile.am). Otherwise, it requires too many
#		boilerplate files from the GNU coding standards that aren't useful to
#		us. 
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# EXTRA_DIST
#-----------------------------------------------------------------------------
#
#      Extra files in this directory.
#
#      The reference says to use "DISTFILES = ABOUT-NLS".  But vim doesn't
#      highlight that as a keyword, and we don't see reference to it elsewhere
#      on the web.
#
#-----------------------------------------------------------------------------

EXTRA_DIST = README

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
#    $(SHELL) $(top_srcdir)/scripts/config.status --recheck
#
#-----------------------------------------------------------------------------

version = $(XPC_API_MAJOR):$(XPC_API_MINOR):$(XPC_API_PATCH)

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------
#
#   	SUBDIRS contains the subdirectories of the current directory that must
#   	be built. It must be defined manually; Automake will never guess a
#   	directory is to be built.  It is possible to define it conditionally
#   	so that some directory will be omitted from the build.
#
#     Canonical order:
#
#        SUBDIRS = doc intl lib src po
#			SUBDIRS = m4 include src tests doc man po pkgconfig w32 intl contrib
#
#-----------------------------------------------------------------------------

SUBDIRS = include src tests w32

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------
#
#      DIST_SUBDIRS is used by targets that need to recurse into /all/
#      directories, even those which have been conditionally left out of the
#      build.
#
#      Precisely, DIST_SUBDIRS is used by:
#
#         -   make dist
#         -   make distclean
#         -   make maintainer-clean.
#
#      All other recursive targets use SUBDIRS.
#
#      Automake will define DIST_SUBDIRS automatically from the possibles
#      values of SUBDIRS in all conditions.
#
#      If SUBDIRS contains AC_SUBST variables, DIST_SUBDIRS will not be
#      defined correctly because Automake doesn't know the possible values of
#      these variables. In this case DIST_SUBDIRS needs to be defined
#      manually. 
#
#-----------------------------------------------------------------------------

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
#-----------------------------------------------------------------------------

#*****************************************************************************
# make coverage (or gcov)
#-----------------------------------------------------------------------------

coverage: gcov

gcov:
	./tests/unit_test_test
	cd tests
	gcov -b -f *.c > output.log
	cd ../src
	gcov -b -f -o .libs *.c > output.log
	@echo "-----------------------------------"
	@echo "Examine the cumulative data in:"
	@echo "   src/output.log"
	@echo "   tests/output.log"
	@echo "   src/*.c.gcov"
	@echo "   tests/*.c.gcov"
	@echo "-----------------------------------"

#*****************************************************************************
# make profiling (or gprof)
#-----------------------------------------------------------------------------

profiling: gprof

gprof:
	./tests/unit_test_test

#*****************************************************************************
# make test
#-----------------------------------------------------------------------------

test: check

#*****************************************************************************
# make diff
#-----------------------------------------------------------------------------
#
# 		`make diff' produces a diff against the previous version, given both
# 		.tar.gz's in the current directory.  This should only be done when an
# 		official release is made (and only if you care to provide diffs).
#
#-----------------------------------------------------------------------------

xpccut_pre = 1.1.0

diff: diffcheck
	@(echo "To apply these patches, cd to the main directory of the package"; \
	  echo "and then use \`patch -p1 < xpccut-XXX.diff'."; \
	  echo "Before building the program, run \`autogen.sh'."; ) > \
	    $(PACKAGE)-$(xpccut_pre)-$(VERSION).diff
	-diff -rc2P --exclude=configure --exclude=config.h.in --exclude=*.info \
	    --exclude=*.gmo --exclude=aclocal.m4 \
	    $(PACKAGE)-$(xpccut_pre) $(PACKAGE)-$(VERSION) >> \
	    $(PACKAGE)-$(xpccut_pre)-$(VERSION).diff
	gzip --force --best $(PACKAGE)-$(xpccut_pre)-$(VERSION).diff

diffcheck:
	for d in $(PACKAGE)-$(xpccut_pre) $(PACKAGE)-$(VERSION) ; do \
	    if test ! -d $$d ; then \
	        if test -r $$d.tar.gz ; then \
	            tar -zxf $$d.tar.gz ; \
                else \
	            echo subdir $$d does not exist. ; \
	            exit 1 ; \
                fi ; \
	    fi ; \
	done

#*****************************************************************************
# dist
#-----------------------------------------------------------------------------
#
#		Taken from the reference noted elsewhere in this Makefile template.
#
#-----------------------------------------------------------------------------
#
# distdir = $(PACKAGE)-$(VERSION)
#
# dist: Makefile
#	rm -fr $(distdir)
#	mkdir $(distdir)
#	chmod 777 $(distdir)
#	for file in $(DISTFILES); do \
#		ln $$file $(distdir) 2>/dev/null || cp -p $$file $(distdir); \
#	done
#	for subdir in $(SUBDIRS); do 					\
#		mkdir $(distdir)/$$subdir || exit 1; 	\
#		chmod 777 $(distdir)/$$subdir; 			\
#		(cd $$subdir && $(MAKE) $@) || exit 1; \
#	done
#	tar chozf $(distdir).tar.gz $(distdir)
#	rm -fr $(distdir)
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# po-check
#-----------------------------------------------------------------------------
# 
# 		From coreutils.
#
# 		Verify that all source files using _() are listed in po/POTFILES.in.
# 		The idea is to run this before making pretests, as well as official
# 		releases, so that ...
#
#-----------------------------------------------------------------------------

po-check:
	if test -f po/POTFILES.in; then					\
	  grep -E -v '^(#|$$)' po/POTFILES.in				\
	    | grep -v '^src/false\.c$$' | sort > $@-1;			\
	  files=;							\
	  for file in $$($(CVS_LIST_EXCEPT)) `find * -name '*.[ch]'`; do \
	    case $$file in						\
	    djgpp/* | man/*) continue;;					\
	    esac;							\
	    case $$file in						\
	    *.[ch])							\
	      base=`expr " $$file" : ' \(.*\)\..'`;			\
	      { test -f $$base.l || test -f $$base.y; } && continue;;	\
	    esac;							\
	    files="$$files $$file";					\
	  done;								\
	  grep -E -l '\b(N?_|gettext *)\([^)"]*("|$$)' $$files		\
	    | sort -u > $@-2;						\
	  diff -u $@-1 $@-2 || exit 1;					\
	  rm -f $@-1 $@-2;						\
	fi

#******************************************************************************
# install-exec-hook
#------------------------------------------------------------------------------
#
#		We want our libraries stored in lib/xpc-x.y, not just lib.
#
# 			install-exec-hook:
#				mkdir -p $(xpclibdir)
#				mv $(libdir)/libxpccut* $(xpclibdir)
#
#		However, we need to add the installation path to either
#		/etc/ld.so.conf or to a file in the directory /etc/ld.so.conf.d, and
#		then run ldconfig.
#
#		We can assume that the user is running "make install" as root.
#
#------------------------------------------------------------------------------

install-exec-hook:
	$(top_srcdir)/scripts/ld_config.sh $(xpclibdir)

#*****************************************************************************
# all-local
#-----------------------------------------------------------------------------

all-local:
	@echo "Top source-directory 'top_srcdir' is $(top_srcdir)"
	@echo "* * * * * All xpccut build items completed * * * * *"

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

#*****************************************************************************
# Makefile.am (xpccut top-level)
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#-----------------------------------------------------------------------------
