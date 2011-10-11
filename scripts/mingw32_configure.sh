#!/bin/bash

# Before running this, the following is required.
# 1. Build worldforge libraries using the mingw32 spec files included.
# 2. Install Python windows binary under wine, and copy includes and 
#    libpython26.a to the sys-root. Link libpython2.6.a to libpython26.a
# 3. Sort out a PostgreSQL client library build. Download source, configure
#    for mingw build, and build and install in:
#        src/interfaces/libpq
#        src/bin/pg_config
# 4. Make a wine wrapper for pg_config

set -u
set -e

cross_target=$(ls /usr/bin/i?86-*-gcc | sed "s/^\/usr\/bin\/\(.*\)-gcc\$/\1/")
cross_root=/usr/${cross_target}

if [ ! -d ${cross_root} ] ; then
  echo "Unable to find cross compiler root ${cross_root}"
  exit 1
fi

cross_sysroot=${cross_root}
if [ -d ${cross_root}/sys-root/mingw ] ; then
  cross_sysroot=${cross_root}/sys-root/mingw
fi

configure_flags=
if [ -f ./cyphesis.spec ]
then
  configure_flags="${configure_flags} --enable-binreloc=no --with-python=${cross_sysroot} --with-psql-prefix=${cross_sysroot} --with-libgcrypt-prefix=${cross_sysroot}"
fi

export PKG_CONFIG_LIBDIR=${cross_sysroot}/lib/pkgconfig

./configure --host=${cross_target} --build=i686-pc-linux-gnu --target=${cross_target} --prefix=${cross_sysroot} ${configure_flags}

DLLS="
libgcc_s_sjlj-1.dll \
libgcrypt-11.dll \
libgpg-error-0.dll \
libpq.dll \
libsigc-2.0-0.dll \
"

if [ -f ./cyphesis.spec ]
then
  for dll in ${DLLS}
  do
    if [ -f ${cross_sysroot}/bin/${dll} ]
    then
      echo Found bin/${dll}
      cp ${cross_sysroot}/bin/${dll} ./server
    elif [ -f ${cross_sysroot}/lib/${dll} ]
    then
      echo Found lib/${dll}
      cp ${cross_sysroot}/lib/${dll} ./server
    else
      echo Unable to find ${dll}
    fi
  done
fi
