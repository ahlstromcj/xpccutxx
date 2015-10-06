dnl ***************************************************************************
dnl xpc_endian.m4
dnl ---------------------------------------------------------------------------
dnl
dnl \file       	xpc_endian.m4
dnl \library    	xpc_suite subproject
dnl \author     	Chris Ahlstrom
dnl \date       	03/04/2008-03/04/2008
dnl \version    	$Revision$
dnl \license    	$XPC_SUITE_GPL_LICENSE$
dnl
dnl   This macro detects the endian of the host system, and then sets up
dnl   the variables needed to allow include/xpc/c/cpu.h to set up the proper
dnl   byte order.
dnl
dnl   This macro is patterned after the ost_endian.m4 macro of the Common
dnl   C++ library.
dnl
dnl   Process this macro using AC_XPC_ENDIAN in configure.ac
dnl  
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_XPC_ENDIAN],[
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	AC_CHECK_HEADER(endian.h,[
		AC_DEFINE(HAVE_ENDIAN_H, [1], [have endian header])
		],[
		AC_CHECK_HEADER(sys/isa_defs.h,[
			AC_DEFINE(HAVE_SYS_ISA_DEFS_H, [1], [solaris endian])
			],[
			case "$target_cpu" in
			alpha* | i?86)
				AC_DEFINE(__BYTE_ORDER, [1234], [endian byte order])
				;;
			hppa* | m68* | mips* | powerpc* | sparc*)
				AC_DEFINE(__BYTE_ORDER, [4321])
				;;
			esac
		])
	])
])

dnl ***************************************************************************
dnl xpc_endian.m4
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
