#!/bin/bash

check_header_file() {
    NAMES=$(grep "^class .* {$" $@ | sed "s/^class \([A-Za-z]\+\) .*$/\1/")
    FILE=$(basename $@ .h)
    for name in ${NAMES}
    do
        if [ "$FILE" != "$name" ]
        then
            echo $FILE: $name
        fi
    done
}

for i in */*.h
do
    check_header_file $i
done
