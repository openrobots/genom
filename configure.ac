dnl	$LAAS$
dnl
dnl configure.in --
dnl	process this file with autoconf to produce a configure script.
dnl
AC_REVISION($LAAS$)

AC_INIT(src/genom.c)
AC_CONFIG_HEADER(genom-config.h)

PACKAGE=genom
GENOM_MAJOR=1
GENOM_MINOR=1
GENOM_REV=B
GENOM_VERSION=${GENOM_MAJOR}.${GENOM_MINOR}${GENOM_REV}

AC_SUBST(PACKAGE)
AC_SUBST(GENOM_MAJOR)
AC_SUBST(GENOM_MINOR)
AC_SUBST(GENOM_REV)
AC_SUBST(GENOM_VERSION)

AC_DEFINE_UNQUOTED(GENOM_VERSION, "$GENOM_VERSION", [The complete version of GenoM])
AC_DEFINE_UNQUOTED(GENOM_MAJOR, "$GENOM_MAJOR", [The major revision of GenoM])
AC_DEFINE_UNQUOTED(GENOM_MINOR, "$GENOM_MINOR", [The minor revision of GenoM])

AC_CANONICAL_HOST

AC_ARG_WITH(libsonly,
           AC_HELP_STRING([--with-libsonly],
               [only compile run-time libraries]),
           [LIBSONLY=yes],[LIBSONLY=no])
AC_SUBST(LIBSONLY)


dnl --- Programs names --------------------------------------------------
dnl
GENOM=genom
TCLSERV=tclserv
AC_SUBST(GENOM)
AC_SUBST(TCLSERV)


dnl --- Checks for programs ---------------------------------------------
dnl
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

AC_PROG_CC
AC_PROG_CPP
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_INSTALL

AC_ARG_VAR(LATEX, "LaTeX text formatting and typesetting")
AC_CHECK_PROGS(LATEX, [pdflatex latex], [:])

ROBOT_CFLAGS
ROBOT_CPP_USES_STDIN
ROBOT_PROG_MKDEP


dnl --- RTAI support ----------------------------------------------------
dnl

# configure options
AC_ARG_WITH(rtai,
           AC_HELP_STRING([--with-rtai=yes|no],
               [configure to use RTAI]),
           [opt_rtai=$withval],[opt_rtai=no])
AC_ARG_WITH(rtai-includes,
           AC_HELP_STRING([--with-rtai-includes=DIR],
               [RTAI headers are in DIR]),
           [opt_rtai_includes=$withval])
AC_ARG_WITH(linux-kernel-includes,
           AC_HELP_STRING([--with-linux-kernel-includes=DIR],
               [linux kernel headers are in DIR]),
           [opt_lnxkern_includes=$withval])

# make related stuff
if test "x$opt_rtai" = "xyes"; then
   OSAPI=rtai
   AC_MSG_RESULT([Enabling RTAI kernel support])
   AC_CHECK_RTAI_INCLUDES(RTAI_INC, $opt_rtai_includes)
   AC_CHECK_LINUXKERNEL_INCLUDES(LNXKERN_INC, $opt_lnxkern_includes)

elif test "x$opt_rtai" = "xno"; then :
   OSAPI=posix
   AC_MSG_RESULT([Configuring to use POSIX])
else
   AC_MSG_ERROR([invalid option --with-rtai=$opt_rtai], 2)
fi
AC_SUBST(OSAPI)


dnl --- Checks for functions --------------------------------------------
dnl

AC_CHECK_PTHREADS

dnl ---- Checks for headers ---------------------------------------------
dnl
AC_CHECK_HEADERS(sys/cdefs.h)

AC_C_BIGENDIAN([ENDIANNESS=IS_BIG_ENDIAN], [ENDIANNESS=IS_LITTLE_ENDIAN],
          [ENDIANNESS=IS_UNKNOWN_ENDIAN])
AC_SUBST(ENDIANNESS)


dnl --- Checks for libraries --------------------------------------------
dnl

AC_ARG_WITH(pocolibs,
           AC_HELP_STRING([--with-pocolibs=DIR],
               [pocolibs are installed in DIR (prefix)]),
           [opt_pocolibs=$withval],[opt_pocolibs=$prefix])
AC_ARG_WITH(pocolibs-exec-prefix,
           AC_HELP_STRING([--with-pocolibs-exec-prefix=DIR],
               [pocolibs libraries are installed in DIR (exec-prefix)]),
           [opt_pocolibs_eprefix=$withval],[opt_pocolibs_eprefix=$exec_prefix])

PKG_CHECK_MODULES(PORTLIB, pocolibs >= 2.0)
AC_SUBST(PORTLIB_CFLAGS)
AC_SUBST(PORTLIB_LIBS)

ROBOT_LIB_TCL

dnl Check for perl
AC_ARG_VAR(PERL, full path of Perl interpreter)
AC_PATH_PROG(PERL, perl, no)
if test "x$PERL" = "xno"; then
    AC_MSG_ERROR(Cannot find the perl executable)
fi

dnl --- Create the Makefiles --------------------------------------------
dnl
CFLAGS="${C_DEBUG_FLAGS} ${C_WARNING_FLAGS} ${CFLAGS}"

AC_OUTPUT(
	config.mk
	Makefile
	genom.pc
	lib/Makefile
	src/Makefile
	include/Makefile
	canvas/Makefile
        canvas/autogen
        canvas/autoconf/configure.ac.begin
        canvas/autoconf/configure.ac.end
	tclserv/server/Makefile
	tclserv/client/Makefile
	doc/user_guide/Makefile
	emacs/Makefile
	scripts/Makefile
)