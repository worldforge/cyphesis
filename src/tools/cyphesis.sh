#!/bin/sh
# To create a binary to run with this script, a build should be done
# with --enable-static-link specified on configure, enable_database
# disabled in common/const and HAVE_EPOLL_CREATE undefined in
# config.h

if [ ! -f ./bin/cyphesis ] ; then
    echo Unable to locate ./bin/cyphesis
    echo This script must be run from the top of the binary distribution.
    exit 1
fi

TOP=$(pwd)
export PYTHONHOME=${TOP}
DIROPTS="--cyphesis:directory=${TOP}/share --cyphesis:confdir=${TOP}/etc --cyphesis:vardir=${TOP}/var"

${TOP}/bin/cyphesis --cyphesis:daemon=true "${DIROPTS}"

RETVAL=$?
if [ $RETVAL -ne 0 ] ; then
    echo Failed to start cyphesis server.
    echo Check syslog for details.
    exit 1
fi

sleep 5

${TOP}/bin/cyclient "${DIROPTS}"
