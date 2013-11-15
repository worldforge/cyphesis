#!/bin/bash

set -e

REGRESSION="rulesets/deeds/regression.py"

CLASSES=$(grep --exclude=data/tasks.xml --exclude=data/operations.xml \
            --exclude=data/werewolf.xml --exclude=data/acorn.xml \
            "^ \+<string name=\"id\">\([a-z_]\+\)<\/string>" data/*.xml |
          sed "s/^.*<string name=\"id\">\([a-z_]\+\)<\/string>/\"\1\", /" |
          sort)

# Strip off the trailing ,
CLASSES=$(echo ${CLASSES} | sed "s/, \?$//")

sed -i -e "s/^ALL_CLASSES=\[.*\]$/ALL_CLASSES=[${CLASSES}]/" ${REGRESSION}
