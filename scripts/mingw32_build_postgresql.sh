#!/bin/bash

set -e

# Sort out a PostgreSQL client library build. Download source, configure
# for mingw build, and build and install in:
#    src/interfaces/libpq
#    src/bin/pg_config

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

sudo -p "Please enter your password for sudo access or hit ctrl-c to abort:" true

export PKG_CONFIG_LIBDIR=${cross_sysroot}/lib/pkgconfig

./configure --with-system-tzdata=/usr/share/zoneinfo --host=${cross_target} --build=i686-pc-linux-gnu --target=${cross_target} --prefix=${cross_sysroot}

pushd src/include
touch utils/fmgroids.h
sudo -n make install
popd

pushd src/interfaces/libpq
make
sudo -n make install
popd

pushd src/bin/pg_config
make
sudo -n make install
popd
