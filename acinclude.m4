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

dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN([PKG_CHECK_MODULES], [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     echo "*** The pkg-config script could not be found. Make sure it is"
     echo "*** in your path, or set the PKG_CONFIG environment variable"
     echo "*** to the full path to pkg-config."
     echo "*** Or see http://www.freedesktop.org/software/pkgconfig to get pkg-config."
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            $1_LIBS=`$PKG_CONFIG --libs "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])


