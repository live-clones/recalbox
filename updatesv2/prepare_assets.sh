#!/bin/sh
set -eu

if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" ]; then
  echo "This script need 4 parameters"
  exit 1
fi

RELEASE_TYPE="${1}"
IMAGE_DIR="${2}"
RELEASE_DIR="${3}"
SKIP_IMAGES="${4}"


RELEASE_DIR_UPGRADE="${RELEASE_DIR}/v2/upgrade"
rm -rf "${RELEASE_DIR}"
mkdir -p "${RELEASE_DIR_UPGRADE}"
# Copy all files from image dir to release dir
cp -r "${IMAGE_DIR}/"* "${RELEASE_DIR_UPGRADE}"

if [ "${SKIP_IMAGES}" == "true" ];then
  echo "SKIP_IMAGES is true. Removing images."
  rm -rf "${RELEASE_DIR_UPGRADE}"/*/*.img*
fi

# Noobs compatibility
UPGRADE_DIR="${IMAGE_DIR}/"
SOURCE_NOOBS_DIR="./noobs/"
NOOBS_DIR="${RELEASE_DIR}/v1/noobs"
mkdir -p "${NOOBS_DIR}"

# Copy skeleton
cp -r "${SOURCE_NOOBS_DIR}"/* "${NOOBS_DIR}"
# Copy dist files
for arch in $(ls "${UPGRADE_DIR}" | grep rpi); do
  echo "Creating directory ${NOOBS_DIR}/${arch}"
  mkdir -p "${NOOBS_DIR}/${arch}"
  for file in $(ls "${UPGRADE_DIR}/${arch}" | grep ".tar.xz"); do
    echo "Copying file ${UPGRADE_DIR}/${arch}/${file} to ${NOOBS_DIR}/${arch}/${file}"
    cp "${UPGRADE_DIR}/${arch}/${file}" "${NOOBS_DIR}/${arch}/${file}"
  done
done

# Template html
cp "${RELEASE_TYPE}.template.html" "${RELEASE_DIR}/index.html"

OLDIFS="${IFS}"
IFS='
'
echo "Subsitute all env var in the template"
for ENVVAR in $(env | grep "CI_\|GITLAB"); do
  echo "envvar=$ENVVAR"
  NAME="$(echo $ENVVAR | cut -d'=' -f1)"
  VALUE="$(echo $ENVVAR | cut -d'=' -f2-)"
  sed -i "s|${NAME}|${VALUE}|g" "${RELEASE_DIR}/index.html"
done
IFS="${OLDIFS}"
