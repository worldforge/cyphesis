#!/bin/bash

set -e

# Sort out a PostgreSQL client library build. Download source, configure
# for mingw build, and build and install in:
#    src/interfaces/libpq
#    src/bin/pg_config

sudo true

export PKG_CONFIG_LIBDIR=/usr/i686-pc-mingw32/sys-root/mingw/lib/pkgconfig

./configure --host=i686-pc-mingw32 --build=i686-pc-linux-gnu --target=i686-pc-mingw32 --prefix=/usr/i686-pc-mingw32/sys-root/mingw # --enable-binreloc=no --with-python=/usr/i686-pc-mingw32/sys-root/mingw --with-psql-prefix=/usr/i686-pc-mingw32/sys-root/mingw --with-libgcrypt-prefix=/usr/i686-pc-mingw32/sys-root/mingw

pushd src/include
touch utils/fmgroids.h
sudo make install
popd

pushd src/interfaces/libpq
make
sudo make install
popd

pushd src/bin/pg_config
make
sudo make install
popd
