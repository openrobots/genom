#	$LAAS$

#
# Copyright (c) 2002-2003 LAAS/CNRS                   --  Fri Mar 15 2002
# All rights reserved.
#
# Redistribution  and  use in source   and binary forms,  with or without
# modification, are permitted provided that  the following conditions are
# met:
#
#   1. Redistributions  of  source code must  retain  the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must  reproduce the above copyright
#      notice,  this list of  conditions and  the following disclaimer in
#      the  documentation   and/or  other  materials   provided with  the
#      distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE  AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY  EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES   OF MERCHANTABILITY AND  FITNESS  FOR  A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO  EVENT SHALL THE AUTHOR OR  CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING,  BUT  NOT LIMITED TO, PROCUREMENT  OF
# SUBSTITUTE  GOODS OR SERVICES;  LOSS   OF  USE,  DATA, OR PROFITS;   OR
# BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

dnl --- Compute CFLAGS --------------------------------------------------

AC_DEFUN(ROBOT_CFLAGS,
[
   C_DEBUG_FLAGS=-g

   if test x"${GCC}" = xyes; then
      C_OPTIMIZE_FLAGS=-O2
      C_WARNING_FLAGS="-Wall"
   fi
])


dnl --- Test that CPP accepts '-' as a file name ------------------------

AC_DEFUN(ROBOT_CPP_USES_STDIN,
[
   AC_REQUIRE_CPP()

   AC_CACHE_CHECK(
      [whether ${CPP} accepts files on standard input],
      ac_cv_robot_cpp_stdin,
      [
         if echo TEST | ${CPP} -DTEST=yes 2>&1 | grep yes >/dev/null; then
            ac_cv_robot_cpp_stdin=yes;
         elif echo TEST | ${CPP} - -DTEST=yes 2>&1 | grep yes >/dev/null; then
            ac_cv_robot_cpp_stdin=dash;
         else
            ac_cv_robot_cpp_stdin=no;
         fi
      ])

   if test x"${ac_cv_robot_cpp_stdin}" = xno; then
      AC_MSG_ERROR([The cpp program doesn't accept files on stdin])
   elif test x"${ac_cv_robot_cpp_stdin}" = xdash; then
      STDINCPP="${CPP} -"
   else
      STDINCPP="${CPP}"
   fi
   AC_SUBST(STDINCPP)
])


dnl --- Look for the laas mkdep executable ------------------------------

AC_DEFUN(ROBOT_PROG_MKDEP,
[
   AC_PATH_PROG(MKDEP, mkdep, no, $exec_prefix/bin:$prefix/bin:$PATH)
   if test "$MKDEP" = "no"; then
      AC_MSG_ERROR([You need the mkdep package])
   fi

   AC_CACHE_CHECK(
      [whether mkdep accepts -I/-D options], ac_cv_robot_mkdep,
      [
         if $MKDEP -I foo -D bar 1>/dev/null 2>&1; then
	    ac_cv_robot_mkdep=yes;
	 else
	    ac_cv_robot_mkdep=no;
         fi
      ])
   if test x"${ac_cv_robot_mkdep}" = xno; then
      AC_MSG_ERROR([You need the mkdep package])
   fi 
])


dnl --- Check for pthread support ---------------------------------------
dnl
dnl AC_CHECK_PTHREADS_LINK(action ok, action not ok, action cross-compiling)
AC_DEFUN(AC_CHECK_PTHREADS_LINK,
[
	AC_LANG_SAVE
	AC_LANG_C
	AC_TRY_RUN([
#include <pthread.h>
void *pipo(void *a) {   exit(0); }
 
int main() {
    pthread_t t;
    pthread_create(&t, NULL, pipo, NULL);
    pthread_join(t, NULL);
    exit(2);
}
	],[$1],[$2],:)
	AC_LANG_RESTORE
])

AC_DEFUN([AC_CHECK_PTHREADS],
[
AC_CACHE_CHECK(
	[if compiler recognizes -pthread],
	robot_cv_gcc_pthread,
	ac_save_CFLAGS=$CFLAGS
	CFLAGS="$CFLAGS -pthread"
	AC_CHECK_PTHREADS_LINK(
		robot_cv_gcc_pthread=yes, 
		robot_cv_gcc_pthread=no
	)
	CFLAGS=$ac_save_CFLAGS
)
if test "$robot_cv_gcc_pthread" = "yes"; then
	robot_threads=""
	CFLAGS="$CFLAGS -pthread"
	LDFLAGS="$LDFLAGS -pthread";
else
	AC_CACHE_CHECK(
		[if compiler recognizes -mt],
		robot_cv_cc_mt,
		ac_save_CFLAGS=$CFLAGS
		CFLAGS="$CFLAGS -mt"
		AC_CHECK_PTHREADS_LINK(
			robot_cv_cc_mt=yes,
			robot_cv_cc_mt=no
		)
		CFLAGS=$ac_save_CFLAGS
	)
	if test "$robot_cv_cc_mt" = "yes"; then
		CFLAGS="$CFLAGS -mt";
		LDFLAGS="$LDFLAGS -mt";
	else
		AC_CHECK_LIB(pthread,pthread_create,robot_threads=-lpthread,
        	     [AC_CHECK_LIB(c_r,pthread_create,robot_threads=-lc_r)])
		ac_save_LIBS="$LIBS"
		if test -n "$robot_threads" ; then
			LIBS="$robot_threads $LIBS"
		fi
		AC_CHECK_PTHREADS_LINK([
			:
		],[
			AC_MSG_ERROR([No working thread support found (fatal)])
			exit 2
		])
	fi
fi
])


dnl --- look for RTAI includes ------------------------------------------
dnl AC_CHECK_RTAI_INCLUDES(var, path)
AC_DEFUN([AC_CHECK_RTAI_INCLUDES],
[
   AC_MSG_CHECKING([for RTAI includes])
   AC_CACHE_VAL(ac_cv_path_rtai,
    [
       IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS=":"
        ac_tmppath="$2:/usr/realtime/include:/usr/src/rtai/include"
        for ac_dir in $ac_tmppath; do 
            test -z "$ac_dir" && ac_dir=.
            if eval test -f $ac_dir/rtai.h; then
               eval ac_cv_path_rtai="$ac_dir"
               break
            fi
       done
       IFS="$ac_save_ifs"
    ])
   $1="$ac_cv_path_rtai"
   if test -n "[$]$1"; then
      AC_MSG_RESULT([$]$1)
   else
      AC_MSG_ERROR([cannot find RTAI includes], 2)
   fi
   AC_SUBST($1)
])


dnl --- look for linux kernel includes ----------------------------------
dnl AC_CHECK_LINUXKERNEL_INCLUDES(var, path)
AC_DEFUN([AC_CHECK_LINUXKERNEL_INCLUDES],
[
   AC_MSG_CHECKING([for linux kernel includes])
   AC_CACHE_VAL(ac_cv_path_kernel,
    [
       IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS=":"
        ac_tmppath="$2:/usr/realtime/include:/usr/src/linux/include:/usr/include"
        for ac_dir in $ac_tmppath; do 
            test -z "$ac_dir" && ac_dir=.
            if eval test -f $ac_dir/linux/kernel.h; then
               eval ac_cv_path_kernel="$ac_dir"
               break
            fi
       done
       IFS="$ac_save_ifs"
    ])
   $1="$ac_cv_path_kernel"
   if test -n "[$]$1"; then
      AC_MSG_RESULT([$]$1)
   else
      AC_MSG_ERROR([cannot find linux kernel includes], 2)
   fi
   AC_SUBST($1)
])


dnl --- Look for includes in a path ------------------------------------
dnl ROBOT_PATH_INC(PACKAGE, VARIABLE, INC, [, VALUE-IF-NOT-FOUND, [, PATH]])
dnl
AC_DEFUN(ROBOT_PATH_INC,
[
   AC_MSG_CHECKING([for $1 headers])
   $2=""
   AC_CACHE_VAL(ac_cv_path_h_$2,
    [
        AC_LANG_PUSH(C)
        ac_cv_path_found=no
	ac_save_cppflags=$CPPFLAGS
	ac_save_ifs=$IFS; IFS=":"
        eval ac_tmppath=":$5"
        for ac_dir in $ac_tmppath; do 
	    IFS=$ac_save_ifs
            test -z "$ac_dir" || CPPFLAGS="-I$ac_dir"
	    
	    AC_COMPILE_IFELSE([#include "$3"],
			      [eval ac_cv_path_h_$2="$ac_dir"; ac_cv_path_found="yes"; break])
	done
	CPPFLAGS=$ac_save_cppflags
        AC_LANG_POP
    ])
   if test "x$ac_cv_path_found" = "xyes"; then
      if test -n "$ac_cv_path_h_$2"; then
        $2="-I$ac_cv_path_h_$2"
        $1_INCLUDE_PATH="$ac_cv_path_h_$2"
        AC_MSG_RESULT($ac_cv_path_h_$2)
      else
        AC_MSG_RESULT(found)
      fi
   else
      AC_MSG_RESULT([not found])
      ifelse([$4], , , $4)
   fi
   AC_SUBST($2)
])


dnl --- Look for a library in a path ------------------------------------
dnl ROBOT_PATH_LIB(PACKAGE, VARIABLE, LIB, FNCT [,VALUE-IF-NOT-FOUND [,PATH]])
dnl
AC_DEFUN(ROBOT_PATH_LIB,
[
   AC_MSG_CHECKING([for $1 libraries])
   $2=""
   AC_CACHE_VAL(ac_cv_path_l_$2,
    [
	AC_LANG_PUSH(C)
        ac_cv_path_found=no
	ac_save_ldflags=$LDFLAGS
	ac_save_libs=$LIBS
        LIBS="-l$3 $ac_save_libs"
	ac_save_ifs=$IFS; IFS=":"
        eval ac_tmppath=":$6"
        for ac_dir in $ac_tmppath; do 
            IFS=$ac_save_ifs
            test -z "$ac_dir" || LDFLAGS="-L$ac_dir $ac_save_ldflags"
	    
	    AC_LINK_IFELSE(AC_LANG_CALL([], [$4]),
			   [eval ac_cv_path_l_$2=$ac_dir ; ac_cv_path_found="yes"; break])
 	done
        LIBS=$ac_save_libs
	LDFLAGS=$ac_save_ldflags
        AC_LANG_POP
    ])
   if test "x$ac_cv_path_found" = "xyes"; then
      if test -n "$ac_cv_path_l_$2"; then
        $2="-L$ac_cv_path_l_$2 -R$ac_cv_path_l_$2"
        $1_LIB_PATH=$ac_cv_path_l_$2
        AC_MSG_RESULT($ac_cv_path_l_$2)
      else
        AC_MSG_RESULT(found)
      fi
   else
      AC_MSG_RESULT(no)
      ifelse([$5], , , $5)
   fi
   AC_SUBST($2)
])


dnl --- Look for a file in a path ---------------------------------------
dnl ROBOT_PATH_FILE(PACKAGE, VARIABLE, LIB, [, VALUE-IF-NOT-FOUND, [, PATH]])
dnl
AC_DEFUN(ROBOT_PATH_FILE,
[
   AC_ARG_WITH($1,
	       AC_HELP_STRING([--with-$1=DIR],
        		      [$1 is in DIR]),
               opt_pathlib_$2=$withval)

   AC_MSG_CHECKING([for $1 files])
   AC_CACHE_VAL(ac_cv_path_$2,
    [
	IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS=":"
        ac_tmppath="[$]opt_pathlib_$2:$5"
        for ac_dir in $ac_tmppath; do 
            test -z "$ac_dir" && ac_dir=.
            if eval test -f $ac_dir/$3; then
               eval ac_cv_path_$2="$ac_dir"
               break
            fi
	done
	IFS="$ac_save_ifs"
    ])
   $2="$ac_cv_path_$2"
   if test -n "[$]$2"; then
      AC_MSG_RESULT([$]$2)
   else
      AC_MSG_RESULT(no)
      ifelse([$4], , , $4)
   fi
   AC_SUBST($2)
])


dnl --- Look for tcl ----------------------------------------------------
dnl
AC_DEFUN(ROBOT_LIB_TCL,
[
   AC_ARG_WITH(tcl,
      AC_HELP_STRING([--with-tcl=DIR], [directory containing tclConfig.sh. 'no' to disable.]),
      [tcl_prefix=$withval],
      [for ac_dir in \
         ${exec_prefix}/lib      \
         /usr/local/lib/tcl8.4   \
         /usr/local/lib/tcl8.3   \
         /usr/local/lib          \
         /usr/pkg/lib            \
         /usr/lib                \
         /usr/lib/tcl8.4         \
         /usr/lib/tcl8.3         \
        ; \
       do
         if test -r "$ac_dir/tclConfig.sh"; then
            tcl_prefix=$ac_dir
            break
         fi
       done])

   if test "x${tcl_prefix}" = "xno"; then
	HAS_TCL=no
   else

   AC_MSG_CHECKING([for tclConfig.sh])
   if test ! -r "${tcl_prefix}/tclConfig.sh"; then
      AC_MSG_RESULT([not found: DISABLING tclServ compilation])
      AC_MSG_RESULT([Please use --with-tcl to specify a valid path to your tclConfig.sh file]) 
   else

   TCL_CONFIG_PATH=${tcl_prefix}
   file=${tcl_prefix}/tclConfig.sh
   . $file
   AC_MSG_RESULT("${tcl_prefix}/tclConfig.sh")

   dnl substitute variables in TCL_LIB_FILE
   eval TCL_LIB_FILE=${TCL_LIB_FILE}

   AC_MSG_CHECKING([for tcl headers])
   test -z "$tcl_test_include" && tcl_test_include=tcl.h
   for ac_dir in \
      $TCL_PREFIX/include/tcl$TCL_VERSION       \
      $TCL_PREFIX/include                       \
      /usr/local/include/tcl$TCL_VERSION        \
      /usr/local/include                        \
      /usr/include                              \
      /Library/Frameworks/Tcl.framework/Headers \
      $extra_include                            \
      ; \
   do
      if test -r "$ac_dir/$tcl_test_include"; then
         ac_tcl_includes=$ac_dir
         break
      fi
   done
   if test "x$ac_tcl_includes" = "x"; then
      AC_MSG_RESULT([not found (fatal)])
   else

   AC_MSG_RESULT($ac_tcl_includes)

   AC_MSG_CHECKING([for tcl library])
   test -z "$tcl_test_lib" && tcl_test_lib="${TCL_LIB_FILE}"
   for ac_dir in \
      $TCL_EXEC_PREFIX/lib                    \
      $TCL_PREFIX/lib                         \
      /usr/local/lib                          \
      /usr/lib                                \
      /Library/Frameworks/Tcl.framework       \
      $extra_lib                              \
      ; \
   do
      if test -r "$ac_dir/$tcl_test_lib"; then
         ac_tcl_libs=$ac_dir
         break
      fi
   done
   if test "x$ac_tcl_libs" = "x"; then
      AC_MSG_RESULT([not found (fatal)])
   else

   AC_MSG_RESULT($ac_tcl_libs/$TCL_LIB_FILE)

   if test "$ac_tcl_includes" != "/usr/include"; then
      TCL_CPPFLAGS="-I$ac_tcl_includes"
   else 
      TCL_CPPFLAGS=""
   fi
   if test "$ac_tcl_libs" != "/usr/include"; then
      TCL_LDFLAGS="-L$ac_tcl_libs -R$ac_tcl_libs"
   else 
      TCL_LDFLAGS=""
   fi

   HAS_TCL=yes

   fi # libtcl not found
   fi # tcl.h not found
   fi # tclConfig.sh not found
   fi # --with-tcl=no

   AC_SUBST(HAS_TCL)
   AC_SUBST(TCL_CONFIG_PATH)
   AC_SUBST(TCL_CPPFLAGS)
   AC_SUBST(TCL_LDFLAGS)
   AC_SUBST(TCL_LIBS)
   AC_SUBST(TCL_LIB_FLAG)
   AC_SUBST(TCL_LIB_SPEC)
])
