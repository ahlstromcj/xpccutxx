dnl ***************************************************************************
dnl xpc_profile.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file       	xpc_profile.m4
dnl \library    	xpc_suite subproject
dnl \author     	Chris Ahlstrom
dnl \date       	03/04/2008-03/04/2008 (DEPRECATED on 01/23/2009)
dnl \version    	$Revision$
dnl \license    	$XPC_SUITE_GPL_LICENSE$
dnl
dnl   DEPRECATED
dnl      Superceded by additional support in xpc_debug.m4.
dnl   DEPRECATED
dnl
dnl   Tests whether the user wants code profiling.
dnl
dnl   Set up for profiling.  This macro is used to get the arguments supplied
dnl   to the configure script (./configure --enable-profiling)
dnl   
dnl   It defines the symbol PROFLAGS, which is added to the CFLAGS for the
dnl   compiler call
dnl
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_PROFILING],
[
   PROFLAGS=
   if test -n "$GCC"; then

      AC_MSG_CHECKING(whether to enable profiling support)
      AC_ARG_ENABLE(profilesupport,
         [  --enable-profile=(no/yes/gprof) Turn on profiling support(default=no)],
         [
          case "${enableval}" in
           yes) profilesupport=yes    ;;
            no) profilesupport=no     ;;
         gprof) profilesupport=gprof  ;;
             *) AC_MSG_ERROR(bad value ${enableval} for --enable-profile) ;;
          esac
         ],
         [
            profilesupport=no
         ])

      AM_CONDITIONAL(DOPROFILESUPPORT, test x$profilesupport = xyes)

      if test "x$profilesupport" = "xyes" ; then
         PROFLAGS="-p"
      elif test "x$profilesupport" = "xgprof" ; then
         PROFLAGS="-pg"
      fi
   fi
   AC_SUBST([PROFLAGS])
   AC_DEFINE_UNQUOTED([PROFLAGS], [$PROFLAGS],
   [PROFLAGS to -p, -pg, or nothing, for building profiling information.])
])

dnl ***************************************************************************
dnl xpc_profile.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 ft=config
dnl ---------------------------------------------------------------------------
