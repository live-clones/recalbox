#!/bin/sh
set -eu

if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" ]; then
  echo "This script need 5 parameters"
  exit 1
fi

AWS_ACCESS_KEY_ID="${1}"
AWS_SECRET_ACCESS_TOKEN="${2}"
BUCKET="${3}"
RELEASE_DIR="${4}"
PATH_IN_BUCKET="${5:-}"

cat >s3cfg <<EOF
[default]
# Endpoint
host_base = https://s3.nl-ams.scw.cloud
host_bucket = https://s3.nl-ams.scw.cloud
bucket_location = nl-ams

# Login credentials
access_key = ${AWS_ACCESS_KEY_ID}
secret_key = ${AWS_SECRET_ACCESS_TOKEN}
EOF

for FOLDER_TO_UPLOAD in $(ls ${RELEASE_DIR}); do
  docker run --rm -v $(pwd)/s3cfg:/root/.s3cfg -v "$(pwd)/${RELEASE_DIR}/${FOLDER_TO_UPLOAD}:/${FOLDER_TO_UPLOAD}" garland/docker-s3cmd \
    s3cmd put -r --acl-public "/${FOLDER_TO_UPLOAD}" "s3://${BUCKET}/${PATH_IN_BUCKET}/"
done
