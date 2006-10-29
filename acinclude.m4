# Configure paths for psql
# Stolen by Michael Koch
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_PSQL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for psql, and define PG_CFLAGS and PG_LIBS
dnl
AC_DEFUN([AM_PATH_PSQL],
[dnl 
dnl Get the cflags and libraries from the pg_config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(psql-prefix,[  --with-psql-prefix=PREFIX
                          Prefix where psql is installed (optional)],
            psql_prefix="$withval", psql_prefix="")
AC_ARG_WITH(psql-exec-prefix,[  --with-psql-exec-prefix=PREFIX
                          Exec prefix where psql is installed (optional)],
            psql_exec_prefix="$withval", psql_exec_prefix="")
AC_ARG_ENABLE(psqltest, [  --disable-psqltest     Do not try to compile and run a test psql program],
		    , enable_psqltest=yes)

  if test x$psql_exec_prefix != x ; then
     psql_args="$psql_args --exec-prefix=$psql_exec_prefix"
     if test x${PG_CONFIG+set} != xset ; then
        PG_CONFIG=$psql_exec_prefix/bin/pg_config
     fi
  fi
  if test x$psql_prefix != x ; then
     if test x${PG_CONFIG+set} != xset ; then
        PG_CONFIG=$psql_prefix/bin/pg_config
     fi
  fi

  AC_PATH_PROG(PG_CONFIG, pg_config, no)
  min_psql_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for psql - version >= $min_psql_version)
  no_psql=""
  if test "$PG_CONFIG" = "no" ; then
    no_psql=yes
  else
    PG_CFLAGS="-I`$PG_CONFIG $psqlconf_args --includedir`"
    PG_LIBS="-L`$PG_CONFIG $psqlconf_args --libdir`"

    psql_major_version=`$PG_CONFIG $psql_args --version | \
           sed 's/PostgreSQL \([[0-9]]*\).\([[0-9]]*\)\(.\([[0-9]]*\)\).*$/\1/'`
    psql_minor_version=`$PG_CONFIG $psql_args --version | \
           sed 's/PostgreSQL \([[0-9]]*\).\([[0-9]]*\)\(.\([[0-9]]*\)\).*$/\2/'`
    psql_micro_version=`$PG_CONFIG $psql_config_args --version | \
           sed 's/PostgreSQL \([[0-9]]*\).\([[0-9]]*\)\(.\([[0-9]]*\)\).*$/\4/'`
    if test "x$psql_micro_version" = "x" ; then
      psql_micro_version=0
    fi
    if test "x$enable_psqltest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $PG_CFLAGS"
      LIBS="$LIBS $PG_LIBS"
dnl
dnl Now check if the installed psql is sufficiently new. (Also sanity
dnl checks the results of pg_config to some extent)
dnl
      rm -f conf.psqltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.psqltest");
  */
  { FILE *fp = fopen("conf.psqltest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_psql_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_psql_version");
     exit(1);
   }

   if (($psql_major_version > major) ||
      (($psql_major_version == major) && ($psql_minor_version > minor)) ||
      (($psql_major_version == major) && ($psql_minor_version == minor) && ($psql_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'pg_config --version' returned %d.%d.%d, but the minimum version\n", $psql_major_version, $psql_minor_version, $psql_micro_version);
      printf("*** of psql required is %d.%d.%d. If pg_config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If pg_config was wrong, set the environment variable PG_CONFIG\n");
      printf("*** to point to the correct copy of pg_config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_psql=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_psql" = x ; then
     AC_MSG_RESULT($psql_major_version.$psql_minor_version.$psql_micro_version)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$PG_CONFIG" = "no" ; then
       echo "*** The pg_config script installed by psql could not be found"
       echo "*** If psql was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the PG_CONFIG environment variable to the"
       echo "*** full path to pg_config."
     else
       if test -f conf.psqltest ; then
        :
       else
          echo "*** Could not run psql test program, checking why..."
          CFLAGS="$CFLAGS $PG_CFLAGS"
          LIBS="$LIBS $PG_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding psql or finding the wrong"
          echo "*** version of psql. If it is not finding psql, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means psql was incorrectly installed"
          echo "*** or that you have moved psql since it was installed. In the latter case, you"
          echo "*** may want to edit the pg_config script: $PG_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     PG_CFLAGS=""
     PG_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(PG_CFLAGS)
  AC_SUBST(PG_LIBS)
  rm -f conf.psqltest
])

# pkg.m4 - Macros to locate and utilise pkg-config.            -*- Autoconf -*-
# 
# Copyright © 2004 Scott James Remnant <scott@netsplit.com>.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# As a special exception to the GNU General Public License, if you
# distribute this file as part of a program that contains a
# configuration script generated by Autoconf, you may include it under
# the same distribution terms that you use for the rest of that program.

# PKG_PROG_PKG_CONFIG([MIN-VERSION])
# ----------------------------------
AC_DEFUN([PKG_PROG_PKG_CONFIG],
[m4_pattern_forbid([^_?PKG_[A-Z_]+$])
m4_pattern_allow([^PKG_CONFIG(_PATH)?$])
AC_ARG_VAR([PKG_CONFIG], [path to pkg-config utility])dnl
if test "x$ac_cv_env_PKG_CONFIG_set" != "xset"; then
	AC_PATH_TOOL([PKG_CONFIG], [pkg-config])
fi
if test -n "$PKG_CONFIG"; then
	_pkg_min_version=m4_default([$1], [0.9.0])
	AC_MSG_CHECKING([pkg-config is at least version $_pkg_min_version])
	if $PKG_CONFIG --atleast-pkgconfig-version $_pkg_min_version; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		PKG_CONFIG=""
	fi
		
fi[]dnl
])# PKG_PROG_PKG_CONFIG

# PKG_CHECK_EXISTS(MODULES, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# Check to see whether a particular set of modules exists.  Similar
# to PKG_CHECK_MODULES(), but does not set variables or print errors.
#
#
# Similar to PKG_CHECK_MODULES, make sure that the first instance of
# this or PKG_CHECK_MODULES is called, or make sure to call
# PKG_CHECK_EXISTS manually
# --------------------------------------------------------------
AC_DEFUN([PKG_CHECK_EXISTS],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
if test -n "$PKG_CONFIG" && \
    AC_RUN_LOG([$PKG_CONFIG --exists --print-errors "$1"]); then
  m4_ifval([$2], [$2], [:])
m4_ifvaln([$3], [else
  $3])dnl
fi])


# _PKG_CONFIG([VARIABLE], [COMMAND], [MODULES])
# ---------------------------------------------
m4_define([_PKG_CONFIG],
[if test -n "$PKG_CONFIG"; then
    if test -n "$$1"; then
        pkg_cv_[]$1="$$1"
    else
        PKG_CHECK_EXISTS([$3],
                         [pkg_cv_[]$1=`$PKG_CONFIG --[]$2 "$3" 2>/dev/null`],
			 [pkg_failed=yes])
    fi
else
	pkg_failed=untried
fi[]dnl
])# _PKG_CONFIG

# _PKG_SHORT_ERRORS_SUPPORTED
# -----------------------------
AC_DEFUN([_PKG_SHORT_ERRORS_SUPPORTED],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])
if $PKG_CONFIG --atleast-pkgconfig-version 0.20; then
        _pkg_short_errors_supported=yes
else
        _pkg_short_errors_supported=no
fi[]dnl
])# _PKG_SHORT_ERRORS_SUPPORTED


# PKG_CHECK_MODULES(VARIABLE-PREFIX, MODULES, [ACTION-IF-FOUND],
# [ACTION-IF-NOT-FOUND])
#
#
# Note that if there is a possibility the first call to
# PKG_CHECK_MODULES might not happen, you should be sure to include an
# explicit call to PKG_PROG_PKG_CONFIG in your configure.ac
#
#
# --------------------------------------------------------------
AC_DEFUN([PKG_CHECK_MODULES],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
AC_ARG_VAR([$1][_CFLAGS], [C compiler flags for $1, overriding pkg-config])dnl
AC_ARG_VAR([$1][_LIBS], [linker flags for $1, overriding pkg-config])dnl

pkg_failed=no
AC_MSG_CHECKING([for $1])

_PKG_CONFIG([$1][_CFLAGS], [cflags], [$2])
_PKG_CONFIG([$1][_LIBS], [libs], [$2])

m4_define([_PKG_TEXT], [Alternatively, you may set the environment variables $1[]_CFLAGS
and $1[]_LIBS to avoid the need to call pkg-config.
See the pkg-config man page for more details.])

if test $pkg_failed = yes; then
        _PKG_SHORT_ERRORS_SUPPORTED
        if test $_pkg_short_errors_supported = yes; then
	        $1[]_PKG_ERRORS=`$PKG_CONFIG --short-errors --errors-to-stdout --print-errors "$2"`
        else 
	        $1[]_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
        fi
	# Put the nasty error message in config.log where it belongs
	echo "$$1[]_PKG_ERRORS" >&AS_MESSAGE_LOG_FD

	ifelse([$4], , [AC_MSG_ERROR(dnl
[Package requirements ($2) were not met:

$$1_PKG_ERRORS

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.

_PKG_TEXT
])],
		[$4])
elif test $pkg_failed = untried; then
	ifelse([$4], , [AC_MSG_FAILURE(dnl
[The pkg-config script could not be found or is too old.  Make sure it
is in your PATH or set the PKG_CONFIG environment variable to the full
path to pkg-config.

_PKG_TEXT

To get pkg-config, see <http://www.freedesktop.org/software/pkgconfig>.])],
		[$4])
else
	$1[]_CFLAGS=$pkg_cv_[]$1[]_CFLAGS
	$1[]_LIBS=$pkg_cv_[]$1[]_LIBS
        AC_MSG_RESULT([yes])
	ifelse([$3], , :, [$3])
fi[]dnl
])# PKG_CHECK_MODULES

# Check for binary relocation support
# Hongli Lai
# http://autopackage.org/

AC_DEFUN([AM_BINRELOC],
[
	AC_ARG_ENABLE(binreloc,
		[  --enable-binreloc       compile with binary relocation support
                          (default=enable when available)],
		enable_binreloc=$enableval,enable_binreloc=auto)

	AC_ARG_ENABLE(binreloc-threads,
		[  --enable-binreloc-threads      compile binary relocation with threads support
	                         (default=yes)],
		enable_binreloc_threads=$enableval,enable_binreloc_threads=yes)

	BINRELOC_CFLAGS=
	BINRELOC_LIBS=
	if test "x$enable_binreloc" = "xauto"; then
		AC_CHECK_FILE([/proc/self/maps])
		AC_CACHE_CHECK([whether everything is installed to the same prefix],
			       [br_cv_valid_prefixes], [
				if test "$bindir" = '${exec_prefix}/bin' -a "$sbindir" = '${exec_prefix}/sbin' -a \
					"$datadir" = '${prefix}/share' -a "$libdir" = '${exec_prefix}/lib' -a \
					"$libexecdir" = '${exec_prefix}/libexec' -a "$sysconfdir" = '${prefix}/etc'
				then
					br_cv_valid_prefixes=yes
				else
					br_cv_valid_prefixes=no
				fi
				])
	fi
	AC_CACHE_CHECK([whether binary relocation support should be enabled],
		       [br_cv_binreloc],
		       [if test "x$enable_binreloc" = "xyes"; then
		       	       br_cv_binreloc=yes
		       elif test "x$enable_binreloc" = "xauto"; then
			       if test "x$br_cv_valid_prefixes" = "xyes" -a \
			       	       "x$ac_cv_file__proc_self_maps" = "xyes"; then
				       br_cv_binreloc=yes
			       else
				       br_cv_binreloc=no
			       fi
		       else
			       br_cv_binreloc=no
		       fi])

	if test "x$br_cv_binreloc" = "xyes"; then
		BINRELOC_CFLAGS="-DENABLE_BINRELOC"
		AC_DEFINE(ENABLE_BINRELOC,,[Use binary relocation?])
		if test "x$enable_binreloc_threads" = "xyes"; then
			AC_CHECK_LIB([pthread], [pthread_getspecific])
		fi

		AC_CACHE_CHECK([whether binary relocation should use threads],
			       [br_cv_binreloc_threads],
			       [if test "x$enable_binreloc_threads" = "xyes"; then
					if test "x$ac_cv_lib_pthread_pthread_getspecific" = "xyes"; then
						br_cv_binreloc_threads=yes
					else
						br_cv_binreloc_threads=no
					fi
			        else
					br_cv_binreloc_threads=no
				fi])

		if test "x$br_cv_binreloc_threads" = "xyes"; then
			BINRELOC_LIBS="-lpthread"
			AC_DEFINE(BR_PTHREAD,1,[Include pthread support for binary relocation?])
		else
			BINRELOC_CFLAGS="$BINRELOC_CFLAGS -DBR_PTHREADS=0"
			AC_DEFINE(BR_PTHREAD,0,[Include pthread support for binary relocation?])
		fi
	fi
	AC_SUBST(BINRELOC_CFLAGS)
	AC_SUBST(BINRELOC_LIBS)
])
dnl Autoconf macros for libgcrypt
dnl       Copyright (C) 2002, 2004 Free Software Foundation, Inc.
dnl
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without
dnl modifications, as long as this notice is preserved.
dnl
dnl This file is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


dnl AM_PATH_LIBGCRYPT([MINIMUM-VERSION,
dnl                   [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libgcrypt and define LIBGCRYPT_CFLAGS and LIBGCRYPT_LIBS.
dnl MINIMUN-VERSION is a string with the version number optionalliy prefixed
dnl with the API version to also check the API compatibility. Example:
dnl a MINIMUN-VERSION of 1:1.2.5 won't pass the test unless the installed 
dnl version of libgcrypt is at least 1.2.5 *and* the API number is 1.  Using
dnl this features allows to prevent build against newer versions of libgcrypt
dnl with a changed API.
dnl
AC_DEFUN([AM_PATH_LIBGCRYPT],
[ AC_ARG_WITH(libgcrypt-prefix,
            AC_HELP_STRING([--with-libgcrypt-prefix=PFX],
                           [prefix where LIBGCRYPT is installed (optional)]),
     libgcrypt_config_prefix="$withval", libgcrypt_config_prefix="")
  if test x$libgcrypt_config_prefix != x ; then
     if test x${LIBGCRYPT_CONFIG+set} != xset ; then
        LIBGCRYPT_CONFIG=$libgcrypt_config_prefix/bin/libgcrypt-config
     fi
  fi

  AC_PATH_PROG(LIBGCRYPT_CONFIG, libgcrypt-config, no)
  tmp=ifelse([$1], ,1:1.2.0,$1)
  if echo "$tmp" | grep ':' >/dev/null 2>/dev/null ; then
     req_libgcrypt_api=`echo "$tmp"     | sed 's/\(.*\):\(.*\)/\1/'`
     min_libgcrypt_version=`echo "$tmp" | sed 's/\(.*\):\(.*\)/\2/'`
  else
     req_libgcrypt_api=0
     min_libgcrypt_version="$tmp"
  fi

  AC_MSG_CHECKING(for LIBGCRYPT - version >= $min_libgcrypt_version)
  ok=no
  if test "$LIBGCRYPT_CONFIG" != "no" ; then
    req_major=`echo $min_libgcrypt_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\)/\1/'`
    req_minor=`echo $min_libgcrypt_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\)/\2/'`
    req_micro=`echo $min_libgcrypt_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\)/\3/'`
    libgcrypt_config_version=`$LIBGCRYPT_CONFIG --version`
    major=`echo $libgcrypt_config_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/'`
    minor=`echo $libgcrypt_config_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/'`
    micro=`echo $libgcrypt_config_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/'`
    if test "$major" -gt "$req_major"; then
        ok=yes
    else 
        if test "$major" -eq "$req_major"; then
            if test "$minor" -gt "$req_minor"; then
               ok=yes
            else
               if test "$minor" -eq "$req_minor"; then
                   if test "$micro" -ge "$req_micro"; then
                     ok=yes
                   fi
               fi
            fi
        fi
    fi
  fi
  if test $ok = yes; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
  if test $ok = yes; then
     # If we have a recent libgcrypt, we should also check that the
     # API is compatible
     if test "$req_libgcrypt_api" -gt 0 ; then
        tmp=`$LIBGCRYPT_CONFIG --api-version 2>/dev/null || echo 0`
        if test "$tmp" -gt 0 ; then
           AC_MSG_CHECKING([LIBGCRYPT API version])
           if test "$req_libgcrypt_api" -eq "$tmp" ; then
             AC_MSG_RESULT(okay)
           else
             ok=no
             AC_MSG_RESULT([does not match (want=$req_libgcrypt_api got=$tmp)])
           fi
        fi
     fi
  fi
  if test $ok = yes; then
    LIBGCRYPT_CFLAGS=`$LIBGCRYPT_CONFIG --cflags`
    LIBGCRYPT_LIBS=`$LIBGCRYPT_CONFIG --libs`
    ifelse([$2], , :, [$2])
  else
    LIBGCRYPT_CFLAGS=""
    LIBGCRYPT_LIBS=""
    ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LIBGCRYPT_CFLAGS)
  AC_SUBST(LIBGCRYPT_LIBS)
])
