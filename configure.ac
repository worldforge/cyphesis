dnl Initialise autoconf

AC_INIT(cyphesis, 0.3.8)
AC_CONFIG_SRCDIR([server/server.cpp])

dnl Detect the canonical host and target environment

AC_CANONICAL_SYSTEM

dnl Initialise automake

AM_INIT_AUTOMAKE([dist-bzip2 check-news])
AM_CONFIG_HEADER(config.h)

dnl Need libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

COMMON_LIBS=
PGSQL_LIBS=

dnl Test for C++ compiler

AC_PROG_CXX

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
    ]
)

AC_LANG(C++)

AC_CHECK_FUNCS([sigset sigaction epoll_create])

AC_CHECK_FUNC(cos,,[AC_CHECK_LIB(m,cos)])

dnl allow the user to provide the directory where python is installed in

AC_CACHE_CHECK(for location of python, python_prefix,
[
    python_prefix=/usr
    AC_ARG_WITH(python,
    [  --with-python=DIR       Prefix where python is installed in [default=/usr]],
    [
        if test $withval != yes; then
            python_prefix=$withval
        fi
    ])
])

STATIC_LIBSTDCPP=
STATIC_LIBGCC=

AC_ARG_ENABLE(static_link,
  [  --enable-static-link    enable static link of some libs [default=no]],
  [
    if test "$enableval" = "yes"; then
      link_static="true"
      STATIC_LIBSTDCPP=`$CXX -print-file-name=libstdc++.a`
      STATIC_LIBGCC=`$CXX -print-file-name=libgcc.a`
    else
      link_static="false"
    fi
  ],[
    link_static="false"
  ]
)
AM_CONDITIONAL(LINK_STATIC, test x"$link_static" = xtrue)


dnl then check for the header file Python.h and set
dnl python_include_path and python_version
dnl appropriately to what we have found
dnl
dnl if found define HAVE_PYTHON_H
dnl

AC_CHECK_HEADER(python2.4/Python.h,
[
    python_include_path=-I${python_prefix}/include/python2.4
    python_version=python2.4
],[
    AC_CHECK_HEADER(python2.3/Python.h,
    [
        python_include_path=-I${python_prefix}/include/python2.3
        python_version=python2.3
    ],[
        AC_CHECK_HEADER(python2.2/Python.h,
        [
            python_include_path=-I${python_prefix}/include/python2.2
            python_version=python2.2
        ],[
            AC_MSG_WARN([Cannot find Python 2.2 or later. Checking for older versions which may not support cyphesis fully.])
            AC_CHECK_HEADER(python2.1/Python.h,
            [
                python_include_path=-I${python_prefix}/include/python2.1
                python_version=python2.1
            ],[
                AC_CHECK_HEADER(python2.0/Python.h,
                [
                    python_include_path=-I${python_prefix}/include/python2.0
                    python_version=python2.0
                ],AC_MSG_ERROR([Cannot find python headers. Please install Python. If Python is installed somewhere other than in /usr the please use the --with-python=DIR option to point to the prefix where Python is installed.]))
            ])
        ])
    ])
])

PYTHON_INCLUDES=${python_include_path}
CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"

dnl build the library path from the found version
python_lib_path=${python_prefix}/lib/${python_version}/config

PYTHON_UTIL_LIBS=

AC_CHECK_LIB(dl,dlopen,
[
    PYTHON_UTIL_LIBS="$PYTHON_UTIL_LIBS -ldl"
])

AC_CHECK_LIB(util,openpty,
[
    PYTHON_UTIL_LIBS="$PYTHON_UTIL_LIBS -lutil"
])

AC_CHECK_FUNC(pthread_mutex_trylock, ,
[
    ac_save_CXXFLAGS="$CXXFLAGS"
    CXXFLAGS="$CXXFLAGS -pthread"
    AC_CHECK_FUNC(pthread_create, ,
    [
        CXXFLAGS="$ac_save_CXXFLAGS"
        AC_CHECK_LIB(pthread, pthread_create, ,
        [
            AC_MSG_WARN([Cannot find pthread library. Python lib check may fail])
        ])
    ])
])

AC_CHECK_LIB(${python_version}, 
    Py_Initialize,
    [
        python_libs="-L${python_lib_path} -l${python_version}"
    ],AC_MSG_ERROR([Cannot find python libraries. Do you have python development installed?]),
    [-L${python_lib_path} ${PYTHON_UTIL_LIBS}]
)

PYTHON_LIBS=${python_libs}

AC_CHECK_LIB(crypto, MD5,
    [
        AC_DEFINE(CYPHESIS_MD5_PASSWORDS, 1, [Use md5 hash for passwords])
    ],
    [
        AC_MSG_ERROR([Cannot find a way of encrypting passwords. Please install the openssl development libraries.])
    ])

PKG_CHECK_MODULES(DEPS, atlascpp-0.5 >= 0.4.94 skstream-0.3 >= 0.3.3 varconf-1.0 >= 0.6.2 mercator-0.2,
    [
        CPPFLAGS="$CPPFLAGS $DEPS_CFLAGS"
        COMMON_LIBS="$COMMON_LIBS $DEPS_LIBS"
    ],
    [
        AC_MSG_ERROR([Cannot find valid versions of required WorldForge libraries. Please see http://worldforge.org/dev/eng/servers/cyphesis for details of the libraries required and where to obtain them.])
    ])

PKG_CHECK_MODULES(HOWL, howl,
    [
        CPPFLAGS="$CPPFLAGS $HOWL_CFLAGS"
        COMMON_LIBS="$COMMON_LIBS $HOWL_LIBS"
        AC_DEFINE(HAVE_LIBHOWL, 1, [Define to 1 if you have howl libs.])
    ],
    [
        AC_MSG_NOTICE([Howl libraries not found. Ommiting DNS-SD function.])
    ])

AM_PATH_PSQL(7.1.0,
    [
        if test "$PG_CFLAGS" != "-I/usr/include"; then
            CPPFLAGS="$CPPFLAGS $PG_CFLAGS"
        fi
        if test "$PG_LIBS" != "-L/usr/lib"; then
            PGSQL_LIBS="$PGSQL_LIBS $PG_LIBS"
        fi
    ],
    AC_MSG_ERROR([Cannot find PostgreSQL config or config failed. Please install PostgreSQL and ensure than pg_config is on the PATH.])
)

ac_save_LIBS="$LIBS"
LIBS="$LIBS $PG_LIBS"
AC_CHECK_LIB(pq,PQconnectdb,
[
    PGSQL_LIBS="$PGSQL_LIBS -lpq"
],AC_MSG_ERROR([Cannot find PostgreSQL bindings! Have you installed the PostgreSQL development software?]))
LIBS="$ac_save_LIBS"

READLINE_LIBS=

AC_CHECK_LIB(termcap,tgetent, 
    [READLINE_LIBS="$READLINE_LIBS -ltermcap"],
    AC_CHECK_LIB(ncurses, tgetent, 
        [READLINE_LIBS="$READLINE_LIBS -lncurses"],
        AC_MSG_ERROR([Cannot find tgetent in termcap or ncurses libraries!])
    )
)

AC_CHECK_HEADERS(termios.h unistd.h signal.h)
AC_CHECK_LIB(readline,readline,
    [
        READLINE_LIBS="$READLINE_LIBS -lreadline"
    ],
    [
        AC_MSG_ERROR([Cannot find readline!])
    ],[ $READLINE_LIBS ]
)
    
AC_TRY_COMPILE(
  [
    #include <stdio.h>
    #include <readline/readline.h>
  ],
    readline("test > "), 
    AC_DEFINE(READLINE_CXX_SANE, 1, [Define if readline headers are C++ aware]),
    AC_MSG_WARN(Readline C++ workaround enabled)
)

AC_SUBST(VARCONF_LIBS)
AC_SUBST(READLINE_LIBS)
AC_SUBST(COMMON_LIBS)
AC_SUBST(PYTHON_LIBS)
AC_SUBST(PYTHON_UTIL_LIBS)
AC_SUBST(PGSQL_LIBS)

AC_SUBST(STATIC_LIBSTDCPP)
AC_SUBST(STATIC_LIBGCC)

dnl Generate files
AC_OUTPUT([
    cyphesis.spec
    cyphesis-config
    Makefile
    physics/Makefile
    common/Makefile
    modules/Makefile
    rulesets/Makefile
    server/Makefile
    client/Makefile
    tools/Makefile
    data/Makefile
    tests/Makefile
    man/Makefile
], [chmod +x cyphesis-config])
