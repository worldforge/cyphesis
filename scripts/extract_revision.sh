#!/bin/bash

TOPSRCDIR="$@"
CHANGELOG="${TOPSRCDIR}/ChangeLog"
CVSCHECK="${TOPSRCDIR}/CVS/Root"
GITCHECK="${TOPSRCDIR}/.git/config"
OLDBUILDCPP="${TOPSRCDIR}/server/buildid.cpp"

if ! test -d "${TOPSRCDIR}"
then
    echo Non existent source directory \"${TOPSRCDIR}\" >&2
    echo -1
    exit 1
fi

# If sources are under CVS control then the RCS Id in ChangeLog is our build ID.
if test -f "${CVSCHECK}"
then
    if tail -n 1 "${CHANGELOG}"  | sed "s/^.* 1\.\([0-9]*\).*$/\1/"
    then
        echo Getting CVS buildid revision from ChangeLog RCS Id >&2
        exit 0
    fi
fi

# If sources are under git control then the number of ChangeLog commits is our build ID.
if test -f "${GITCHECK}"
then
    if (cd "${TOPSRCDIR}" && git log ChangeLog | grep "^commit [a-f0-9]\+$" | wc -l)
    then
        echo Getting GIT buildid from number of ChangeLog commits >&2
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
