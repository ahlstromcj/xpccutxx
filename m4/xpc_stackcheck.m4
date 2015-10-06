dnl ***************************************************************************
dnl xpc_stackcheck.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file       	xpc_stackcheck.m4
dnl \library    	xpc_suite subproject
dnl \author     	Chris Ahlstrom
dnl \date       	02/29/2008-02/29/2008
dnl \version    	$Revision$
dnl \license    	$XPC_SUITE_GPL_LICENSE$
dnl
dnl   Tests whether the user wants stack-checking, --enable-stackcheck.
dnl
dnl   Enabling this feature can cause problems such as segfaults.  These
dnl   problems do not necessarily indicate stack problems.
dnl
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_STACKCHECK],
[
   STACKCHK=
   if test -n "$GCC"; then

      AC_MSG_CHECKING(whether to enable stack checking)
      AC_ARG_ENABLE(stackcheck,
         [  --enable-stackcheck=(no/yes) Turn on stack checking (default=no)],
         [
          case "${enableval}" in
           yes) stackcheck=yes ;;
            no) stackcheck=no  ;;
             *) AC_MSG_ERROR(bad value ${enableval} for --enable-stackcheck) ;;
          esac
         ],
         [
            stackcheck=no
         ])

      AM_CONDITIONAL(DOSTACKCHECK, test x$stackcheck = xyes)

      STACKCHK=""
      if test "x$stackcheck" = "xyes" ; then
         STACKCHK="-fstack-check"
         AC_MSG_RESULT(yes)
      else
         AC_MSG_RESULT(no)
      fi
   fi
   AC_SUBST([STACKCHK])
   AC_DEFINE_UNQUOTED([STACKCHK], [$STACKCHK],
   [Define STACKCHK=-fstack-check, if the user wants stack-checking.])
])

dnl ***************************************************************************
dnl xpc_stackcheck.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
