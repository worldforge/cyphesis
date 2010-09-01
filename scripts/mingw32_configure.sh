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

MINGW=/usr/i686-pc-mingw32/sys-root/mingw

export PKG_CONFIG_LIBDIR=${MINGW}/lib/pkgconfig

DLLS="
libgcc_s_sjlj-1.dll \
libgcrypt-11.dll \
libgpg-error-0.dll \
libpq.dll \
libsigc-2.0-0.dll \
"

./configure --host=i686-pc-mingw32 --build=i686-pc-linux-gnu --target=i686-pc-mingw32 --prefix=/usr/i686-pc-mingw32/sys-root/mingw --enable-binreloc=no --with-python=/usr/i686-pc-mingw32/sys-root/mingw --with-psql-prefix=/usr/i686-pc-mingw32/sys-root/mingw --with-libgcrypt-prefix=/usr/i686-pc-mingw32/sys-root/mingw

for dll in ${DLLS}
do
  if [ -f ${MINGW}/bin/${dll} ]
  then
    echo Found bin/${dll}
    cp ${MINGW}/bin/${dll} ./server
  elif [ -f ${MINGW}/lib/${dll} ]
  then
    echo Found lib/${dll}
    cp ${MINGW}/lib/${dll} ./server
  else
    echo Unable to find ${dll}
  fi
done
