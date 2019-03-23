#!/bin/sh
set -eux

if [ -z "${1}" -o -z "${2}" -o -z "${3}" ]; then
  echo "This script need 3 parameters"
  exit 1
fi

NETLIFY_AUTH_TOKEN="${1}"
NETLIFY_DIRECTORY_TO_UPLOAD="${2}"
NETLIFY_PROJECT_ID="${3}"

npm install -g netlify-cli

netlify deploy \
-d "${NETLIFY_DIRECTORY_TO_UPLOAD}" \
-s "${NETLIFY_PROJECT_ID}" \
-a "${NETLIFY_AUTH_TOKEN}" \
--prod