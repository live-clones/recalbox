#!/bin/sh
set -euo pipefail

OUTPUT="${1}"

git fetch origin "${CI_MERGE_REQUEST_TARGET_BRANCH_NAME}"

# Removing modified packages
for package in $(git diff --name-status HEAD..origin/master| grep package | awk -F'/' '{ print $2 }');do 
    echo "Removing package ${package}"
    rm -rf "${OUTPUT}/build"/${package}-*
done


# Removing romfs when board or config modified
if git diff --name-status HEAD..origin/master | grep -e "board\|config"; then
    echo "Removing romfs"
    rm -rf "${OUTPUT}/build"/recalbox-romfs*
fi
