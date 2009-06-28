#!/bin/bash

check_coverage() {
    source_file=$1
    source_dir=$(dirname ${source_file})
    base_file=$(basename ${source_file} .cpp)
    test_program=tests/${base_file}test
    coverage_data=${source_dir}/${base_file}.gcda

    if [ ! -f ${test_program}.cpp ]
    then
        if [ ${report} -eq 1 ]
        then
            echo No test for ${source_file}
        fi
        return
    fi

    if [ ${report} -eq 1 ]
    then
        return
    fi
    
    if [ ! -x ${test_program} ]
    then
        echo Test programs have not been built for ${source_file}
        exit 1
    fi

    if [ ${source_file} -nt ${test_program} ]
    then
        echo Test build is out of date for ${source_file}
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
        echo No coverage data ${coverage_data} for ${source_file}
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

usage() {
    echo "Automate running coverage tools, and generate concise report."
    echo "usage: run_coverage_tests.sh [OPTION]"
    echo "       run_coverage_tests.sh [SOURCEFILE]"
    echo "Options:"
    echo "  -r Report source files with a coverage test"
    echo "  -c Re-configure the build to include coverage data"
    echo "  -h Display this usage data"
    cat <<EOF

To pruduce results run this build requires an in-tree build with gcc
configured to include coverage data. Use the -c option to run configure
with the right options, and run "make clean all check" to fully prepare
the build. Running without arguments shows results for all source files
in the configured directories.

Each file is covered by a test derived from it's name. For example the
following file:

rulesets/Character.cpp

should be covered by;

tests/Charactertest.cpp

To see line by line data, run this script on Character.cpp only:

./scripts/run_coverage_tests.sh rulesets/Character.cpp

and the line by line report will be in this file:

rulesets/Character.cpp.gcov
EOF
}

DIRS="physics common modules rulesets"

declare -i report=0
declare -i configure=0

while getopts "hrc" options
do
  case $options in
    r ) report=1;;
    c ) configure=1;;
    h ) usage
         exit 0;;
    * ) usage
         exit 1;;
  esac
done

if [ ${configure} -eq 1 ]
then
    CXXFLAGS="-g --coverage" ./configure --prefix=/opt/worldforge --enable-debug=yes --enable-binreloc=no
    exit 0
fi

shift $((OPTIND-1))

if [ -n "$1" ]
then
    if [ -f "$1" ]
    then
        check_coverage $1
        exit 0
    else
        usage
        exit 1
    fi
fi

for dir in ${DIRS}
do
    for source_file in ${dir}/*.cpp
    do
        check_coverage ${source_file}
    done
done

