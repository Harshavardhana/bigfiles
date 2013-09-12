dnl  Copyright (c) 2013 Red Hat, Inc. <http://www.redhat.com>
dnl  This file is part of BigFiles.
dnl  Licensed under the Apache License, Version 2.0 (the "License");
dnl  you may not use this file except in compliance with the License.
dnl  You may obtain a copy of the License at
dnl         http://www.apache.org/licenses/LICENSE-2.0
dnl  Unless required by applicable law or agreed to in writing, software
dnl  distributed under the License is distributed on an "AS IS" BASIS,
dnl  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl  See the License for the specific language governing permissions and
dnl  limitations under the License.


AC_INIT([bigfiles],[0.1alpha],[fharshav@redhat.com],,[https://github.com/harshavardhana/bigfiles.git])

AM_INIT_AUTOMAKE

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES(yes)])

if make --help 2>&1 | grep -q no-print-directory; then
   AM_MAKEFLAGS="$AM_MAKEFLAGS --no-print-directory";
fi

if make --help 2>&1 | grep -q quiet; then
   AM_MAKEFLAGS="$AM_MAKEFLAGS --quiet"
fi

if libtool --help 2>&1 | grep -q quiet; then
   AM_LIBTOOLFLAGS="--quiet";
fi

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
                bigfiles.pc
                bigfile/Makefile
                bigfile/src/Makefile
                drivers/Makefile
                libbigfiles/Makefile
                libbigfiles/src/Makefile
                tests/Makefile
                tests/src/Makefile])

AC_CANONICAL_HOST

AC_PROG_CC
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_ARG_WITH(pkgconfigdir,
            [  --with-pkgconfigdir=DIR      pkgconfig file in DIR @<:@LIBDIR/pkgconfig@:>@],
            [pkgconfigdir=$withval],
            [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST(pkgconfigdir)

PKG_CHECK_MODULES([GFAPI], [glusterfs-api >= 5])

AC_SUBST(GFAPI_CFLAGS)
AC_SUBST(GFAPI_LIBS)

dnl
dnl Word sizes...
dnl
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
SIZEOF_SHORT=$ac_cv_sizeof_short
SIZEOF_INT=$ac_cv_sizeof_int
SIZEOF_LONG=$ac_cv_sizeof_long
SIZEOF_LONG_LONG=$ac_cv_sizeof_long_long
AC_SUBST(SIZEOF_SHORT)
AC_SUBST(SIZEOF_INT)
AC_SUBST(SIZEOF_LONG)
AC_SUBST(SIZEOF_LONG_LONG)

AC_CHECK_TOOL([LD],[ld])

AC_CHECK_FUNC([dlopen], [has_dlopen=yes], AC_CHECK_LIB([dl], [dlopen], , AC_MSG_ERROR([Dynamic linking library required to build glusterfs])))

dnl some os may not have GNU defined strnlen function
AC_CHECK_FUNC([strnlen], [have_strnlen=yes])
if test "x${have_strnlen}" = "xyes"; then
   AC_DEFINE(HAVE_STRNLEN, 1, [define if found strnlen])
fi
AC_SUBST(HAVE_STRNLEN)

dnl FreeBSD > 5 has execinfo as a Ported library for giving a workaround
dnl solution to GCC backtrace functionality
AC_CHECK_HEADERS([execinfo.h], [have_backtrace=yes],
               AC_CHECK_LIB([execinfo], [backtrace], [have_backtrace=yes]))
dnl               AC_MSG_ERROR([libexecinfo not found libexecinfo required.])))

if test "x${have_backtrace}" = "xyes"; then
   AC_DEFINE(HAVE_BACKTRACE, 1, [define if found backtrace])
fi
AC_SUBST(HAVE_BACKTRACE)

dnl check for memory statistics function
AC_CHECK_FUNC([malloc_stats], [have_malloc_stats=yes])
if test "x${have_malloc_stats}" = "xyes"; then
   AC_DEFINE(HAVE_MALLOC_STATS, 1, [define if found malloc_stats])
fi
AC_SUBST(HAVE_MALLOC_STATS)

dnl Linux, Solaris, Cygwin
AC_CHECK_MEMBERS([struct stat.st_atim.tv_nsec])
dnl FreeBSD, NetBSD
AC_CHECK_MEMBERS([struct stat.st_atimespec.tv_nsec])
case $host_os in
        *netbsd*)
        CFLAGS+=" -D_INCOMPLETE_XOPEN_C063"
        ;;
esac

dnl Check for argp
AC_CHECK_HEADER([argp.h], AC_DEFINE(HAVE_ARGP, 1, [have argp]))
AC_CONFIG_SUBDIRS(argp-standalone)
BUILD_ARGP_STANDALONE=no
if test "x${ac_cv_header_argp_h}" = "xno"; then
   BUILD_ARGP_STANDALONE=yes
   ARGP_STANDALONE_CPPFLAGS='-I${top_srcdir}/argp-standalone'
   ARGP_STANDALONE_LDADD='${top_builddir}/argp-standalone/libargp.a'
fi

AC_SUBST(ARGP_STANDALONE_CPPFLAGS)
AC_SUBST(ARGP_STANDALONE_LDADD)

AC_CHECK_HEADER([malloc.h], AC_DEFINE(HAVE_MALLOC_H, 1, [have malloc.h]))

dnl Check the distribution
BF_DISTRIBUTION=
AC_CHECK_FILE([/etc/debian_version])
AC_CHECK_FILE([/etc/SuSE-release])
AC_CHECK_FILE([/etc/redhat-release])

if test "x$ac_cv_file__etc_debian_version" = "xyes"; then
   BF_DISTRIBUTION=Debian
fi
if test "x$ac_cv_file__etc_SuSE_release" = "xyes"; then
   BF_DISTRIBUTION=SuSE
fi
if test "x$ac_cv_file__etc_redhat_release" = "xyes"; then
   BF_DISTRIBUTION=Redhat
fi

AC_SUBST(BF_DISTRIBUTION)

BF_HOST_OS=""

dnl check for gcc -Werror=format-security
saved_CFLAGS=$CFLAGS
CFLAGS="-Wformat -Werror=format-security"
AC_MSG_CHECKING([whether $CC accepts -Werror=format-security])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM()], [cc_werror_format_security=yes], [cc_werror_format_security=no])
echo $cc_werror_format_security
if test "x$cc_werror_format_security" = "xno"; then
    CFLAGS="$saved_CFLAGS"
else
    CFLAGS="$saved_CFLAGS $CFLAGS"
fi

dnl check for gcc -Werror=implicit-function-declaration
saved_CFLAGS=$CFLAGS
CFLAGS="-Werror=implicit-function-declaration"
AC_MSG_CHECKING([whether $CC accepts -Werror=implicit-function-declaration])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM()], [cc_werror_implicit=yes], [cc_werror_implicit=no])
echo $cc_werror_implicit
if test "x$cc_werror_implicit" = "xno"; then
    CFLAGS="$saved_CFLAGS"
else
    CFLAGS="$saved_CFLAGS $CFLAGS"
fi

case $host_os in
     linux*)
        BF_HOST_OS="BF_LINUX_HOST_OS"
        BF_CFLAGS="${ARGP_STANDALONE_CPPFLAGS} -O0"
        BF_LDADD="${ARGP_STANDALONE_LDADD}"
        ;;
esac

AC_SUBST(CFLAGS)

dnl readline check
BUILD_READLINE=no
AC_CHECK_LIB([readline -lcurses], [readline], [RLLIBS="-lreadline -lcurses"])
AC_CHECK_LIB([readline -ltermcap], [readline], [RLLIBS="-lreadline -ltermcap"])
AC_CHECK_LIB([readline -lncurses], [readline], [RLLIBS="-lreadline -lncurses"])

if test "x$RLLIBS" != "x"; then
   AC_DEFINE(HAVE_READLINE, 1, [readline enabled CLI])
   BUILD_READLINE=yes
fi

dnl bigfile section - enabled by default
BUILD_BIGFILE=no
have_python2=no
have_Python_h=no

AM_PATH_PYTHON()
if echo $PYTHON_VERSION | grep ^2; then
  have_python2=yes
fi
AC_CHECK_HEADERS([python$PYTHON_VERSION/Python.h],[have_Python_h=yes],[])

if test "x$have_python2" = "xyes" -a "x$have_Python_h" = "xyes"; then
   BUILD_BIGFILE=yes
else
   AC_MSG_ERROR([bigfile requires python-devel/python-dev package - disabling proactively])
fi

if test "x$BUILD_BIGFILE" = "xyes"; then
   BUILD_PYTHON_INC=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_inc()"`
   BUILD_PYTHON_LIB=python$PYTHON_VERSION
   BIGFILE_SUBDIR=bigfile
   echo "building bigfile with -isystem $BUILD_PYTHON_INC -l $BUILD_PYTHON_LIB"
   AC_SUBST(BUILD_PYTHON_INC)
   AC_SUBST(BUILD_PYTHON_LIB)
   AC_SUBST(BIGFILE_SUBDIR)
fi

AC_SUBST(BF_HOST_OS)
AC_SUBST(BF_CFLAGS)
AC_SUBST(BF_LDFLAGS)
AC_SUBST(BF_LDADD)
AC_SUBST(RLLIBS)

AC_SUBST(AM_MAKEFLAGS)
AC_SUBST(AM_LIBTOOLFLAGS)

BF_CPPFLAGS='-D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D$(BF_HOST_OS)'
AC_SUBST([BF_CPPFLAGS])

AC_OUTPUT

echo
echo "Bigfiles configure summary"
echo "==========================="
echo "readline             : $BUILD_READLINE"
echo "bigfile              : $BUILD_BIGFILE"
echo
