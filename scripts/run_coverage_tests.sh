#!/bin/bash

check_coverage() {
    source_file=$1
    source_dir=$(dirname ${source_file})
    base_file=$(basename ${source_file} .cpp)
    test_program=tests/${base_file}test
    coverage_data=${source_dir}/${base_file}.gcda
    if [ ! -x ${test_program} ]
    then
        return
    fi

    echo ${source_file} ${base_file} ${test_program}

    if [ ${source_file} -nt ${test_program} ]
    then
        echo Test build is out of date.
        exit 1
    fi

    if [ ! -f ${coverage_data} ]
    then
        echo No coverage data
        exit 1
    fi

    if [ ${test_program} -nt ${coverage_data} ]
    then
        echo Coverage is out of date.
        exit 1
    fi

    (cd ${source_dir} && gcov ${source_file}) | grep -A 1 "^File '$(basename ${source_file})" | grep ^Lines
}

DIRS="rulesets"

for dir in ${DIRS}
do
    for source_file in ${dir}/*.cpp
    do
        check_coverage ${source_file}
    done
done
    
