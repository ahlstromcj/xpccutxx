dnl ***************************************************************************
dnl xpc_gettext.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file       	xpc_gettext.m4
dnl \library    	xpc_suite subproject
dnl \author     	Chris Ahlstrom
dnl \date       	05/08/2008-05/08/2008
dnl \version    	$Revision$
dnl \license    	$XPC_SUITE_GPL_LICENSE$
dnl
dnl   Tests whether the user wants gettext support.
dnl
dnl   Set up for gettext.  This macro is used to get the arguments supplied
dnl   to the configure script (./configure --enable-gettext)
dnl   
dnl   It defines the symbol GETTEXT, which is added to CFLAGS for the
dnl   compiler call as "-DUSE_GETTEXT".
dnl
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_GETTEXT],
[
   GETTEXT=
   if test -n "$GCC"; then

      AC_MSG_CHECKING([whether to enable gettext])
      AC_ARG_ENABLE(gettext,
         [  --enable-gettext=(no/yes) Turn on gettext support (default=yes)],
         [
          case "${enableval}" in
           yes) gettext=yes ;;
            no) gettext=no  ;;
             *) AC_MSG_ERROR(bad value ${enableval} for --enable-gettext) ;;
          esac
         ],
         [
            gettext=yes
         ])

      if test "x$gettext" = "xyes" ; then
         GETTEXT="-DUSE_GETTEXT"
         AC_MSG_RESULT(yes)
      else
         GETTEXT=""
         AC_MSG_RESULT(no)
      fi
   fi
   AC_SUBST([GETTEXT])
   AC_DEFINE_UNQUOTED([GETTEXT], [$GETTEXT],
   [Define GETTEXT=-DUSE_GETTEXT if gettext support is wanted.])
])

dnl ***************************************************************************
dnl xpc_gettext.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
