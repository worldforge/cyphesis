#!/bin/sh
cd lib
for i in `rpm -ql python | grep lib\/python | grep -v .pyc$ | grep -v .pyo$ | sed 's/\/usr\/lib\///' ` ; do
    if [ -f /usr/lib/$i ] ; then
        if [ ! -d `dirname $i` ] ; then
            mkdir -p `dirname $i`
        fi
        cp /usr/lib/$i $i
    fi
done
