#!/bin/sh
set -eu

if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" -o -z "${5}" ]; then
  echo "This script need 5 parameters"
  exit 1
fi

RELEASE_TYPE="${1}"
IMAGE_DIR="${2}"
RELEASE_DIR="${3}"
NETLIFY_DIR="${4}"
SKIP_IMAGES="${5}"

RELEASE_DIR_UPGRADE="${RELEASE_DIR}/v2/upgrade"
rm -rf "${RELEASE_DIR}"
mkdir -p "${RELEASE_DIR_UPGRADE}"
# Copy all files from image dir to release dir
cp -r "${IMAGE_DIR}/"* "${RELEASE_DIR_UPGRADE}"

if [ "${SKIP_IMAGES}" == "true" ]; then
  echo "SKIP_IMAGES is true. Removing images."
  rm -rf "${RELEASE_DIR_UPGRADE}"/*/*.img*
fi

# Template html
cp "${RELEASE_TYPE}.template.html" "${RELEASE_DIR}/index.html"

OLDIFS="${IFS}"
IFS='
'
echo "Subsitute all env var in the template"
for ENVVAR in $(env | grep "CI_\|GITLAB\|RECALBOX"); do
  echo "envvar=$ENVVAR"
  NAME="$(echo $ENVVAR | cut -d'=' -f1)"
  VALUE="$(echo $ENVVAR | cut -d'=' -f2-)"
  sed -i "s|${NAME}|${VALUE}|g" "${RELEASE_DIR}/index.html"
done
IFS="${OLDIFS}"

# Netlify directory
mkdir "${NETLIFY_DIR}"
cp "${RELEASE_DIR}/index.html" "${NETLIFY_DIR}/index.html"