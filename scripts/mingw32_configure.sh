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

export PKG_CONFIG_LIBDIR=/usr/i686-pc-mingw32/sys-root/mingw/lib/pkgconfig

./configure --host=i686-pc-mingw32 --build=i686-pc-linux-gnu --target=i686-pc-mingw32 --prefix=/usr/i686-pc-mingw32/sys-root/mingw --enable-binreloc=no --with-python=/usr/i686-pc-mingw32/sys-root/mingw --with-psql-prefix=/usr/i686-pc-mingw32/sys-root/mingw --with-libgcrypt-prefix=/usr/i686-pc-mingw32/sys-root/mingw
