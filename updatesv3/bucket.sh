#!/bin/sh
set -eux

if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" -o -z "${5}" ]; then
  echo "This script need 5 parameters"
  exit 1
fi

SCRIPT=`realpath $0`
BASEDIR=`dirname $SCRIPT`
OPERATION="${1}" # upload or download
AWS_ACCESS_KEY_ID="${2}"
AWS_SECRET_ACCESS_KEY="${3}"
BUCKET="${4}" # bucket name
FROM_FOLDER="${5}" # for download : bucket folder to download | for upload : folder to upload
TO_FOLDER="${6:-}" # for download : folder where to download | for upload : bucket folder where files will be uploaded

cat >s3cfg <<EOF
[default]
# Endpoint
host_base = https://s3.fr-par.scw.cloud
host_bucket = https://s3.fr-par.scw.cloud
bucket_location = fr-par

# Login credentials
access_key = ${AWS_ACCESS_KEY_ID}
secret_key = ${AWS_SECRET_ACCESS_KEY}
EOF

if [ "${OPERATION}" == "download" ]; then
  docker run --rm -v "$(pwd)/s3cfg:/root/.s3cfg" -v "$(pwd)/${TO_FOLDER}:/download" garland/docker-s3cmd \
    s3cmd get -r --acl-public "s3://${BUCKET}/${FROM_FOLDER}" "/download" 
fi

if [ "${OPERATION}" == "upload" ]; then
    docker run --rm -v /tmp/s3cfg:/root/.s3cfg -v "$(pwd)/${FROM_FOLDER}/:/${FROM_FOLDER}" garland/docker-s3cmd \
      s3cmd put -r --acl-public "/${FROM_FOLDER}" "s3://${BUCKET}/${TO_FOLDER}"
fi

if [[ "${OPERATION}" == "delete" ]]; then
    docker run --rm -v /tmp/s3cfg:/root/.s3cfg garland/docker-s3cmd \
      s3cmd del -r --acl-public "s3://${BUCKET}/${FROM_FOLDER}"
fi
