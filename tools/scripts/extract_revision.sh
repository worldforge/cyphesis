#!/bin/bash

TOPSRCDIR="$@"
GITCHECK="${TOPSRCDIR}/.git/config"

if ! test -d "${TOPSRCDIR}"
then
    echo Non existent source directory \"${TOPSRCDIR}\" >&2
    echo -1
    exit 1
fi

# If sources are under Git version control then use "git rev-parse"
if test -f "${GITCHECK}"
then
    if (cd "${TOPSRCDIR}" && git rev-parse HEAD)
    then
        echo Getting buildid from Git >&2
        exit 0
    fi
fi

# Not in Git version control, therefore this is a released build.
echo Released build, no SHA1 buildid. >&2
echo Released
exit 0
