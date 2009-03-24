#!/bin/bash

check_header_file() {
    GUARD=$(echo $1 | sed "s/\([A-Z0-9a-z]\)\([A-Z0-9]\)\([a-z]\)/\1_\2\3/g" | sed "s/\([a-z]\)\([A-Z0-9]\)\([A-Z0-9]\)/\1_\2\3/g" | tr "[:lower:]" "[:upper:]" | sed "s/[/\.]/_/g")
    INFILE=$(grep "^#ifndef .*_H$" $@ | sed "s/^#ifndef \([A-Z0-9_]\+\)$/\1/")
    if [ "${GUARD}" != "${INFILE}" ]
    then
        if [ "${INFILE}" == "" ]
        then
            echo "No guard in $1"
        else
            true
            sed -i "s/${INFILE}/${GUARD}/" $1
            echo "Mismatch in $1 ${INFILE} ${GUARD} corrected."
        fi
    else
        if [ $(grep ${GUARD} $1 | wc -l) != "3" ]
        then
            echo "Broken guards in $1"
        fi
    fi
    return 0
}

for i in */*.h
do
    check_header_file $i
done
