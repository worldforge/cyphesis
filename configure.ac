dnl Initialise autoconf

AC_INIT()
AC_CONFIG_SRCDIR([server/server.cpp])

dnl Detect the canonical host and target environment

AC_CANONICAL_SYSTEM

dnl Initialise automake

VERSION=0.3.2

AM_INIT_AUTOMAKE(cyphesis, $VERSION)
AM_CONFIG_HEADER(config.h)

dnl check if user wishes maintainer-mode ("--enable-maintainer-mode = yes")
AM_MAINTAINER_MODE

dnl Need libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

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

dnl Flags that are required by programs linking against libcyphesis
SYSTEM_LIBS=
CYPHESIS_LIBS=-lcyphesis
CYPHESIS_CFLAGS=
CYPHESIS_VERSION=$VERSION

AC_CHECK_FUNCS([sigset sigaction])

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

dnl then check for the header file Python.h and set
dnl python_include_path and python_version
dnl appropriately to what we have found
dnl
dnl if found define HAVE_PYTHON_H
dnl

python_two=no
AC_CHECK_HEADER(python2.3/Python.h,
[
    python_include_path=-I${python_prefix}/include/python2.3
    python_version=python2.3
    python_two=yes
],[
    AC_CHECK_HEADER(python2.2/Python.h,
    [
        python_include_path=-I${python_prefix}/include/python2.2
        python_version=python2.2
        python_two=yes
    ],[
        AC_MSG_WARN([Cannot find Python 2.2 or later. Checking for older versions which may not support cyphesis fully.])
        AC_CHECK_HEADER(python2.1/Python.h,
        [
            python_include_path=-I${python_prefix}/include/python2.1
            python_version=python2.1
            python_two=yes
        ],[
            AC_CHECK_HEADER(python2.0/Python.h,
            [
                python_include_path=-I${python_prefix}/include/python2.0
                python_version=python2.0
                python_two=yes
            ],[
                AC_CHECK_HEADER(python1.6/Python.h, 
                [
                    python_include_path=-I${python_prefix}/include/python1.6
                    python_version=python1.6
                ],[
                    AC_CHECK_HEADER(python1.5/Python.h, 
                    [
                        python_include_path=-I${python_prefix}/include/python1.5
                        python_version=python1.5
                    ],AC_MSG_ERROR([Cannot find python headers. Do you have python development installed?]))
                ])
            ])
        ])
    ])
])

if test x$python_two == xyes; then
    AC_DEFINE_UNQUOTED(HAVE_PYTHON_2, 1, [Using python 2.0 or later])
fi

PYTHON_INCLUDES=${python_include_path}
CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"

dnl build the library path from the found version
python_lib_path=${python_prefix}/lib/${python_version}/config

AC_CHECK_LIB(dl,dlopen)
AC_CHECK_LIB(util,openpty)

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
    [-L${python_lib_path}]
)

PYTHON_LIBS=${python_libs}
LIBS="$LIBS $PYTHON_LIBS"

AC_CHECK_LIB(crypto, MD5,
    [
        AC_DEFINE(CYPHESIS_MD5_PASSWORDS, 1, [Use md5 hash for passwords])
    ],
    [
        AC_MSG_ERROR([Cannot find a way of encrypting passwords. Please install the openssl development libraries.])
    ])

PKG_CHECK_MODULES(DEPS, atlascpp-0.5 >= 0.4.93 skstream-0.3 >= 0.2.90 varconf-1.0 >= 0.6.0 mercator-0.2,
    [
        CPPFLAGS="$CPPFLAGS $DEPS_CFLAGS"
        CYPHESIS_CFLAGS="$CYPHESIS_CFLAGS $DEPS_CFLAGS"
        LIBS="$LIBS $DEPS_LIBS"
        SYSTEM_LIBS="$SYSTEM_LIBS $DEPS_LIBS"
    ],
    [
        AC_MSG_ERROR([Cannot find valid versions of required WorldForge libraries.])
    ])

AM_PATH_PSQL(7.1.0,
    [
        if test "$PG_CFLAGS" != "-I/usr/include"; then
            CPPFLAGS="$CPPFLAGS $PG_CFLAGS"
            CYPHESIS_CFLAGS="$CYPHESIS_CFLAGS $PG_CFLAGS"
        fi
        if test "$PG_LIBS" != "-L/usr/lib"; then
            LIBS="$LIBS $PG_LIBS"
            SYSTEM_LIBS="$SYSTEM_LIBS $PG_LIBS"
        fi
    ],
    AC_MSG_ERROR([Cannot find PostgreSQL config or config failed])
)

AC_CHECK_LIB(pq,PQconnectdb, ,AC_MSG_ERROR([Cannot find PostgreSQL bindings! Have you installed the PostgreSQL development software?]))

SYSTEM_LIBS="$SYSTEM_LIBS -lpq"

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
AC_SUBST(SYSTEM_LIBS)
AC_SUBST(READLINE_LIBS)
AC_SUBST(CYPHESIS_CFLAGS)
AC_SUBST(CYPHESIS_LIBS)
AC_SUBST(CYPHESIS_VERSION)

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
], [chmod +x cyphesis-config])
