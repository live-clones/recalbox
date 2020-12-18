#!/bin/bash
set -euxo pipefail


git fetch origin "${CI_MERGE_REQUEST_TARGET_BRANCH_NAME}"

# Removing modified packages
for package in $(git diff --name-status HEAD..origin/master| grep package | awk -F'/' '{ print $2 }');do 
    echo rm -rf output/build/${package}*
done


# Removing romfs when board or config modified
if git diff --name-status HEAD..origin/master | grep -e "board\|config"; then
    echo rm output/build/recalbox-romfs*
fi
