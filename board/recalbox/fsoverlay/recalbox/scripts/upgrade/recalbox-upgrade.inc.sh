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
    echo -ne "\e[2K\r$@"
}
