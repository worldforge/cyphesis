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
AC_DEFUN(AM_PATH_PSQL,
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
           sed 's/PostgreSQL \([[0-9]]*\).\([[0-9]]*\)\(.\([[0-9]]*\)\)\?/\1/'`
    psql_minor_version=`$PG_CONFIG $psql_args --version | \
           sed 's/PostgreSQL \([[0-9]]*\).\([[0-9]]*\)\(.\([[0-9]]*\)\)\?/\2/'`
    psql_micro_version=`$PG_CONFIG $psql_config_args --version | \
           sed 's/PostgreSQL \([[0-9]]*\).\([[0-9]]*\)\(.\([[0-9]]*\)\)\?/\4/'`
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
     AC_MSG_RESULT(yes)
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
# Configure paths for Libsigc++
# Based on Gtk-- script by Erik Andersen and Tero Pulkkinen 

dnl Test for Libsigc++, and define SIGC_CFLAGS and SIGC_LIBS
dnl   to be used as follows:
dnl AM_PATH_SIGC(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(AM_PATH_SIGC,
[dnl 
dnl Get the cflags and libraries from the sigc-config script
dnl

dnl
dnl Prefix options
dnl
AC_ARG_WITH(sigc-prefix,
[  --with-sigc-prefix=PREFIX
                          Prefix where libsigc++ is installed (optional)]
, sigc_config_prefix="$withval", sigc_config_prefix="")

AC_ARG_WITH(sigc-exec-prefix,
[  --with-sigc-exec-prefix=PREFIX 
                          Exec prefix where  libsigc++ is installed (optional)]
, sigc_config_exec_prefix="$withval", sigc_config_exec_prefix="")

AC_ARG_ENABLE(sigctest, 
[  --disable-sigctest     Do not try to compile and run a test libsigc++ 
                          program],
, enable_sigctest=yes)

dnl
dnl Prefix handling
dnl
  if test x$sigc_config_exec_prefix != x ; then
     sigc_config_args="$sigc_config_args --exec-prefix=$sigc_config_exec_prefix"
     if test x${SIGC_CONFIG+set} != xset ; then
        SIGC_CONFIG=$sigc_config_exec_prefix/bin/sigc-config
     fi
  fi
  if test x$sigc_config_prefix != x ; then
     sigc_config_args="$sigc_config_args --prefix=$sigc_config_prefix"
     if test x${SIGC_CONFIG+set} != xset ; then
        SIGC_CONFIG=$sigc_config_prefix/bin/sigc-config
     fi
  fi

dnl
dnl See if sigc-config is alive
dnl
  AC_PATH_PROG(SIGC_CONFIG, sigc-config, no)
  sigc_version_min=$1

dnl
dnl  Version check
dnl
  AC_MSG_CHECKING(for libsigc++ - version >= $sigc_version_min)
  no_sigc=""
  if test "$SIGC_CONFIG" = "no" ; then
    no_sigc=yes
  else
    sigc_version=`$SIGC_CONFIG --version`

    SIGC_CFLAGS=`$SIGC_CONFIG $sigc_config_args --cflags`
    SIGC_LIBS=`$SIGC_CONFIG $sigc_config_args --libs`
    SIGC_MACROS=`$SIGC_CONFIG $sigc_config_args --macros`

    sigc_major_version=`echo $sigc_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sigc_minor_version=`echo $sigc_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sigc_micro_version=`echo $sigc_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    sigc_major_min=`echo $sigc_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sigc_minor_min=`echo $sigc_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sigc_micro_min=`echo $sigc_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    sigc_version_proper=`expr \
        $sigc_major_version \> $sigc_major_min \| \
        $sigc_major_version \= $sigc_major_min \& \
        $sigc_minor_version \> $sigc_minor_min \| \
        $sigc_major_version \= $sigc_major_min \& \
        $sigc_minor_version \= $sigc_minor_min \& \
        $sigc_micro_version \>= $sigc_micro_min `

    if test "$sigc_version_proper" = "1" ; then
      AC_MSG_RESULT([$sigc_major_version.$sigc_minor_version.$sigc_micro_version])
    else
      AC_MSG_RESULT(no)
      no_sigc=yes
    fi

    if test "X$no_sigc" = "Xyes" ; then
      enable_sigctest=no
    fi

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

dnl
dnl
dnl
    if test "x$enable_sigctest" = "xyes" ; then
      AC_MSG_CHECKING(if libsigc++ sane)
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS $SIGC_CFLAGS"
      LIBS="$LIBS $SIGC_LIBS"

      rm -f conf.sigctest
      AC_TRY_RUN([
#include <stdio.h>
#include <sigc++/signal_system.h>

#ifdef SIGC_CXX_NAMESPACES
using namespace SigC;
#endif

int foo1(int i) 
  {
   return 1;
  }

int main(int argc,char **argv)
  {
   if (sigc_major_version!=$sigc_major_version ||
       sigc_minor_version!=$sigc_minor_version ||
       sigc_micro_version!=$sigc_micro_version)
     { printf("(%d.%d.%d) ",
         sigc_major_version,sigc_minor_version,sigc_micro_version);
       return 1;
     }
   Signal1<int,int> sig1;
   sig1.connect(slot(foo1));
   sig1(1);
   return 0;
  }

],[
  AC_MSG_RESULT(yes)
],[
  AC_MSG_RESULT(no)
  no_sigc=yes
]
,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi

  dnl
  dnl
  if test "x$no_sigc" = x ; then
     ifelse([$2], , :, [$2])     
  else
     SIGC_CFLAGS=""
     SIGC_LIBS=""
     SIGC_MACROS=""
     ifelse([$3], , :, [$3])
  fi

  AC_LANG_RESTORE

  AC_SUBST(SIGC_CFLAGS)
  AC_SUBST(SIGC_LIBS)
  AC_SUBST(SIGC_MACROS)
])


dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN(PKG_CHECK_MODULES, [
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


sigc++.m4
# Configure paths for Atlas
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_ATLAS([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Atlas, and define ATLAS_CFLAGS and ATLAS_LIBS
dnl
AC_DEFUN(AM_PATH_ATLAS,
[dnl 
dnl Get the cflags and libraries from the atlas-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(atlas-prefix,[  --with-atlas-prefix=PFX   Prefix where Atlas is installed (optional)],
            atlas_prefix="$withval", atlas_prefix="")
AC_ARG_WITH(atlas-exec-prefix,[  --with-atlas-exec-prefix=PFX Exec prefix where Atlas is installed (optional)],
            atlas_exec_prefix="$withval", atlas_exec_prefix="")
AC_ARG_ENABLE(atlastest, [  --disable-atlastest       Do not try to compile and run a test Atlas program],
		    , enable_atlastest=yes)

  if test x$atlas_exec_prefix != x ; then
     atlas_args="$atlas_args --exec-prefix=$atlas_exec_prefix"
     if test x${ATLAS_CONFIG+set} != xset ; then
        ATLAS_CONFIG=$atlas_exec_prefix/bin/atlas-config
     fi
  fi
  if test x$atlas_prefix != x ; then
     atlas_args="$atlas_args --prefix=$atlas_prefix"
     if test x${ATLAS_CONFIG+set} != xset ; then
        ATLAS_CONFIG=$atlas_prefix/bin/atlas-config
     fi
  fi

  AC_PATH_PROG(ATLAS_CONFIG, atlas-config, no)
  min_atlas_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for Atlas - version >= $min_atlas_version)
  no_atlas=""
  if test "$ATLAS_CONFIG" = "no" ; then
    no_atlas=yes
  else
    ATLAS_CFLAGS=`$ATLAS_CONFIG $atlasconf_args --cflags`
    ATLAS_LIBS=`$ATLAS_CONFIG $atlasconf_args --libs`

    atlas_major_version=`$ATLAS_CONFIG $atlas_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    atlas_minor_version=`$ATLAS_CONFIG $atlas_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    atlas_micro_version=`$ATLAS_CONFIG $atlas_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_atlastest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $ATLAS_CFLAGS"
      LIBS="$LIBS $ATLAS_LIBS"
dnl
dnl Now check if the installed Atlas is sufficiently new. (Also sanity
dnl checks the results of atlas-config to some extent)
dnl
      rm -f conf.atlastest
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
  system ("touch conf.atlastest");
  */
  { FILE *fp = fopen("conf.atlastest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_atlas_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_atlas_version");
     exit(1);
   }

   if (($atlas_major_version > major) ||
      (($atlas_major_version == major) && ($atlas_minor_version > minor)) ||
      (($atlas_major_version == major) && ($atlas_minor_version == minor) && ($atlas_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'atlas-config --version' returned %d.%d.%d, but the minimum version\n", $atlas_major_version, $atlas_minor_version, $atlas_micro_version);
      printf("*** of Atlas required is %d.%d.%d. If atlas-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If atlas-config was wrong, set the environment variable ATLAS_CONFIG\n");
      printf("*** to point to the correct copy of atlas-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_atlas=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_atlas" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$ATLAS_CONFIG" = "no" ; then
       echo "*** The atlas-config script installed by Atlas could not be found"
       echo "*** If Atlas was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the ATLAS_CONFIG environment variable to the"
       echo "*** full path to atlas-config."
     else
       if test -f conf.atlastest ; then
        :
       else
          echo "*** Could not run Atlas test program, checking why..."
          CFLAGS="$CFLAGS $ATLAS_CFLAGS"
          LIBS="$LIBS $ATLAS_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Atlas or finding the wrong"
          echo "*** version of Atlas. If it is not finding Atlas, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Atlas was incorrectly installed"
          echo "*** or that you have moved Atlas since it was installed. In the latter case, you"
          echo "*** may want to edit the atlas-config script: $ATLAS_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     ATLAS_CFLAGS=""
     ATLAS_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(ATLAS_CFLAGS)
  AC_SUBST(ATLAS_LIBS)
  rm -f conf.atlastest
])
# Configure paths for varconf
# Stolen by Michael Koch from Atlas
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_VARCONF([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for Varconf, and define VARCONF_CFLAGS and VARCONF_LIBS
dnl
AC_DEFUN(AM_PATH_VARCONF,
[dnl 
dnl Get the cflags and libraries from the varconf-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(varconf-prefix,[  --with-varconf-prefix=PFX   Prefix where Varconf is installed (optional)],
            varconf_prefix="$withval", varconf_prefix="")
AC_ARG_WITH(varconf-exec-prefix,[  --with-varconf-exec-prefix=PFX Exec prefix where Varconf is installed (optional)],
            varconf_exec_prefix="$withval", varconf_exec_prefix="")
AC_ARG_ENABLE(varconftest, [  --disable-varconftest       Do not try to compile and run a test Varconf program],
		    , enable_varconftest=yes)

  if test x$varconf_exec_prefix != x ; then
     varconf_args="$varconf_args --exec-prefix=$varconf_exec_prefix"
     if test x${VARCONF_CONFIG+set} != xset ; then
        VARCONF_CONFIG=$varconf_exec_prefix/bin/varconf-config
     fi
  fi
  if test x$varconf_prefix != x ; then
     varconf_args="$varconf_args --prefix=$varconf_prefix"
     if test x${VARCONF_CONFIG+set} != xset ; then
        VARCONF_CONFIG=$varconf_prefix/bin/varconf-config
     fi
  fi

  AC_PATH_PROG(VARCONF_CONFIG, varconf-config, no)
  min_varconf_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for Varconf - version >= $min_varconf_version)
  no_varconf=""
  if test "$VARCONF_CONFIG" = "no" ; then
    no_varconf=yes
  else
    VARCONF_CFLAGS=`$VARCONF_CONFIG $varconfconf_args --cflags`
    VARCONF_LIBS=`$VARCONF_CONFIG $varconfconf_args --libs`

    varconf_major_version=`$VARCONF_CONFIG $varconf_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    varconf_minor_version=`$VARCONF_CONFIG $varconf_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    varconf_micro_version=`$VARCONF_CONFIG $varconf_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_varconftest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $VARCONF_CFLAGS"
      LIBS="$LIBS $VARCONF_LIBS"
dnl
dnl Now check if the installed Varconf is sufficiently new. (Also sanity
dnl checks the results of varconf-config to some extent)
dnl
      rm -f conf.varconftest
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
  system ("touch conf.varconftest");
  */
  { FILE *fp = fopen("conf.varconftest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_varconf_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_varconf_version");
     exit(1);
   }

   if (($varconf_major_version > major) ||
      (($varconf_major_version == major) && ($varconf_minor_version > minor)) ||
      (($varconf_major_version == major) && ($varconf_minor_version == minor) && ($varconf_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'varconf-config --version' returned %d.%d.%d, but the minimum version\n", $varconf_major_version, $varconf_minor_version, $varconf_micro_version);
      printf("*** of Varconf required is %d.%d.%d. If varconf-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If varconf-config was wrong, set the environment variable VARCONF_CONFIG\n");
      printf("*** to point to the correct copy of varconf-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_varconf=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_varconf" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$VARCONF_CONFIG" = "no" ; then
       echo "*** The varconf-config script installed by Varconf could not be found"
       echo "*** If Varconf was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the VARCONF_CONFIG environment variable to the"
       echo "*** full path to varconf-config."
     else
       if test -f conf.varconftest ; then
        :
       else
          echo "*** Could not run Varconf test program, checking why..."
          CFLAGS="$CFLAGS $VARCONF_CFLAGS"
          LIBS="$LIBS $VARCONF_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Varconf or finding the wrong"
          echo "*** version of Varconf. If it is not finding Varconf, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Varconf was incorrectly installed"
          echo "*** or that you have moved Varconf since it was installed. In the latter case, you"
          echo "*** may want to edit the varconf-config script: $VARCONF_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     VARCONF_CFLAGS=""
     VARCONF_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(VARCONF_CFLAGS)
  AC_SUBST(VARCONF_LIBS)
  rm -f conf.varconftest
])
# Configure paths for skstream
# Stolen by Michael Koch
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_SKSTREAM([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for skstream, and define SKSTREAM_CFLAGS and SKSTREAM_LIBS
dnl
AC_DEFUN(AM_PATH_SKSTREAM,
[dnl 
dnl Get the cflags and libraries from the skstream-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(skstream-prefix,[  --with-skstream-prefix=PREFIX
                          Prefix where skstream is installed (optional)],
            skstream_prefix="$withval", skstream_prefix="")
AC_ARG_WITH(skstream-exec-prefix,[  --with-skstream-exec-prefix=PREFIX
                          Exec prefix where skstream is installed (optional)],
            skstream_exec_prefix="$withval", skstream_exec_prefix="")
AC_ARG_ENABLE(skstreamtest, [  --disable-skstreamtest     Do not try to compile and run a test skstream program],
		    , enable_skstreamtest=yes)

  if test x$skstream_exec_prefix != x ; then
     skstream_args="$skstream_args --exec-prefix=$skstream_exec_prefix"
     if test x${SKSTREAM_CONFIG+set} != xset ; then
        SKSTREAM_CONFIG=$skstream_exec_prefix/bin/skstream-config
     fi
  fi
  if test x$skstream_prefix != x ; then
     skstream_args="$skstream_args --prefix=$skstream_prefix"
     if test x${SKSTREAM_CONFIG+set} != xset ; then
        SKSTREAM_CONFIG=$skstream_prefix/bin/skstream-config
     fi
  fi

  AC_PATH_PROG(SKSTREAM_CONFIG, skstream-config, no)
  min_skstream_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for skstream - version >= $min_skstream_version)
  no_skstream=""
  if test "$SKSTREAM_CONFIG" = "no" ; then
    no_skstream=yes
  else
    SKSTREAM_CFLAGS=`$SKSTREAM_CONFIG $skstreamconf_args --cflags`
    SKSTREAM_LIBS=`$SKSTREAM_CONFIG $skstreamconf_args --libs`

    skstream_major_version=`$SKSTREAM_CONFIG $skstream_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    skstream_minor_version=`$SKSTREAM_CONFIG $skstream_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    skstream_micro_version=`$SKSTREAM_CONFIG $skstream_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_skstreamtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SKSTREAM_CFLAGS"
      LIBS="$LIBS $SKSTREAM_LIBS"
dnl
dnl Now check if the installed skstream is sufficiently new. (Also sanity
dnl checks the results of skstream-config to some extent)
dnl
      rm -f conf.skstreamtest
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
  system ("touch conf.skstreamtest");
  */
  { FILE *fp = fopen("conf.skstreamtest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_skstream_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_skstream_version");
     exit(1);
   }

   if (($skstream_major_version > major) ||
      (($skstream_major_version == major) && ($skstream_minor_version > minor)) ||
      (($skstream_major_version == major) && ($skstream_minor_version == minor) && ($skstream_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'skstream-config --version' returned %d.%d.%d, but the minimum version\n", $skstream_major_version, $skstream_minor_version, $skstream_micro_version);
      printf("*** of skstream required is %d.%d.%d. If skstream-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If skstream-config was wrong, set the environment variable SKSTREAM_CONFIG\n");
      printf("*** to point to the correct copy of skstream-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_skstream=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_skstream" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$SKSTREAM_CONFIG" = "no" ; then
       echo "*** The skstream-config script installed by skstream could not be found"
       echo "*** If skstream was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SKSTREAM_CONFIG environment variable to the"
       echo "*** full path to skstream-config."
     else
       if test -f conf.skstreamtest ; then
        :
       else
          echo "*** Could not run skstream test program, checking why..."
          CFLAGS="$CFLAGS $SKSTREAM_CFLAGS"
          LIBS="$LIBS $SKSTREAM_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding skstream or finding the wrong"
          echo "*** version of skstream. If it is not finding skstream, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means skstream was incorrectly installed"
          echo "*** or that you have moved skstream since it was installed. In the latter case, you"
          echo "*** may want to edit the skstream-config script: $SKSTREAM_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SKSTREAM_CFLAGS=""
     SKSTREAM_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SKSTREAM_CFLAGS)
  AC_SUBST(SKSTREAM_LIBS)
  rm -f conf.skstreamtest
])
