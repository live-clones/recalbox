#!/bin/bash -e

UPGRADE_DIR="/usr/share/nginx/html/v1/upgrade"
NOOBS_DIR="/usr/share/nginx/html/v1/noobs"
OLD_NOOBS_DIR="/usr/share/nginx/html/noobs"

for arch in $(ls "${UPGRADE_DIR}" | grep rpi); do
  echo "Creating directory ${NOOBS_DIR}/${arch}"
  mkdir -p "${NOOBS_DIR}/${arch}"
  for file in $(ls "${UPGRADE_DIR}/${arch}" | grep ".tar.xz"); do
    echo "Linking file ${UPGRADE_DIR}/${arch}/${file} to ${NOOBS_DIR}/${arch}/${file}"
    ln -s "${UPGRADE_DIR}/${arch}/${file}" "${NOOBS_DIR}/${arch}/${file}"
  done
done

# Only usefull until noobs did not change os_list_v3.json
echo "Linking directory ${NOOBS_DIR} to ${OLD_NOOBS_DIR}"
ln -s "${NOOBS_DIR}" "${OLD_NOOBS_DIR}"
