#!/bin/bash

check_header_file() {
    NAMES=$(grep "^class .* {$" $@ | sed "s/^class \([A-Za-z]\+\) .*$/\1/")
    FILE=$(basename $@ .h)
    for name in ${NAMES}
    do
        if [ "$FILE" == "$name" ]
        then
            true
        elif [ "${FILE}Base" == "$name" ]
        then
            true
        elif [ "Python${FILE}" == "$name" ]
        then
            # PythonFoo is the standard python version derived from Foo
            true
        elif [ "$(echo ${FILE}| sed s/Factory/Kit/)" == "$name" ]
        then
            # FooKit is the standard base class for FooFactory
            true
        else
            echo $1: $name
        fi
    done
}

for i in */*.h
do
    check_header_file $i
done
