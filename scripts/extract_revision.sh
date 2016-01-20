#!/bin/bash

TOPSRCDIR="$@"
GITCHECK="${TOPSRCDIR}/.git/config"
OLDBUILDCPP="${TOPSRCDIR}/server/buildid.cpp"

if ! test -d "${TOPSRCDIR}"
then
    echo Non existent source directory \"${TOPSRCDIR}\" >&2
    echo -1
    exit 1
fi

# If sources are under git control then use "git describe"
if test -f "${GITCHECK}"
then
    if (cd "${TOPSRCDIR}" && git describe)
    then
        echo Getting buildid from Git >&2
        exit 0
    fi
fi

# We don't seem to be under source control, so use the existing buildid.
if test -f "${OLDBUILDCPP}"
then
    if grep buildId "${OLDBUILDCPP}" | head -n 1 | sed "s/^.* = \([-0-9]*\);$/\1/"
    then
        echo Using existing buildid >&2
        exit 0
    fi
fi

# We could not find any useful build ID, so mark it clearly as an error.
echo Unknown buildid >&2
echo 1
exit 0
