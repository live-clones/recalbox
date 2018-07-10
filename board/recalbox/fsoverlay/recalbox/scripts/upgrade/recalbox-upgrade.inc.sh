#!/bin/bash

# Clean files
export RECALBOX_URL=v1/upgrade

function clean {
    [[ "${UPGRADE_DIR}" != "" ]] && rm -rf "${UPGRADE_DIR}"/*
}

# Clean files and exit
function cleanBeforeExit {
    clean
    exit $1
}

# Echo in stderr
function echoerr {
    >&2 echo $@
    recallog -f preupgrade.log "$@"
}

function echoES() {
    # move cursor up, erase till the end of the line
    echo -ne "\e[1A\e[K"
    echo "$@"
}
