#!/bin/bash

check_coverage() {
    source_file=$1
    source_dir=$(dirname ${source_file})
    base_file=$(basename ${source_file} .cpp)
    test_program=tests/${base_file}test
    coverage_data=${source_dir}/${base_file}.gcda

    if [ ! -f ${test_program}.cpp ]
    then
        return
    fi
    
    if [ ! -x ${test_program} ]
    then
        echo Test programs have not been built.
        exit 1
    fi

    if [ ${source_file} -nt ${test_program} ]
    then
        echo Test build is out of date.
        exit 1
    fi

    if [ -f ${coverage_data} ]
    then
        rm -f ${coverage_data}
    fi

    ${test_program} > /dev/null 2>&1
    if [ $? -ne 0 ]
    then
        echo FAIL
        exit 1
    fi

    if [ ! -f ${coverage_data} ]
    then
        if [ ! -f ${source_dir}/${base_file}.gcno ]
        then
            return
        fi
        echo No coverage data
        exit 1
    fi

    if [ ${test_program} -nt ${coverage_data} ]
    then
        echo Coverage is out of date for ${source_file}
        exit 1
    fi


    coverage_percent=$( (cd ${source_dir} && gcov ${source_file}) | \
          grep -A 1 "^File '$(basename ${source_file})" | \
          grep ^Lines | \
          head -n 1 | \
          sed "s/^Lines executed:\([0-9]\+\)\.[0-9]\+% of .*$/\1/")

    echo ${source_file} ${coverage_percent}
}

DIRS="physics common modules rulesets"

for dir in ${DIRS}
do
    for source_file in ${dir}/*.cpp
    do
        check_coverage ${source_file}
    done
done
    
