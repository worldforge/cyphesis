#!/bin/sh

if [ ! -f ./bin/cyphesis ] ; then
    echo Unable to locate ./bin/cyphesis
    echo This script must be run from the top of the binary distribution.
    exit 1
fi

TOP=`pwd`
export PYTHONHOME=${TOP}
DIROPTS="--cyphesis:directory=${TOP}/share --cyphesis:confdir=${TOP}/etc --cyphesis:vardir=${TOP}/var"

echo ${TOP}/bin/cyphesis --cyphesis:daemon=true ${DIROPTS}

RETVAL=$?
if [ $RETVAL -ne 0 ] ; then
    echo Failed to start cyphesis server.
    echo Check syslog for details.
    exit 1
fi

sleep 5

echo ${TOP}/bin/cyclient ${DIROPTS}
