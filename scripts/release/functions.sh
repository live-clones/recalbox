#!/bin/bash
set -eux

GANDI_RECALBOX_API_URL="https://api.gandi.net/v5/livedns/domains/recalbox.com/records/"

# Add an IP to the A records of DOMAIN.recalbox.com
function addIpToGandiRecord(){
  local API_KEY="$1"
  local DOMAIN="$2"
  local NEW_IP="$3"

  local CURRENT_IPS="$(curl -v -H "Content-Type: application/json" -H "Authorization: Apikey ${API_KEY}" "${GANDI_RECALBOX_API_URL}/${DOMAIN}" | jq -c ".[].rrset_values")"
  local IPS="$(echo "${CURRENT_IPS}" | jq ". += [\"${NEW_IP}\"]")"
  echo "Setting new records for ${DOMAIN} : ${IPS}"
  curl -v -X PUT -H "Content-Type: application/json" -H "Authorization: Apikey ${API_KEY}" -d "{\"rrset_values\": ${IPS}}" "${GANDI_RECALBOX_API_URL}/${DOMAIN}/A"
}

# Create a server for downloads, settings its IP in the IP global variable
function createServer(){
  local ORGANIZATION="$1"
  local TOKEN="$2"
  local TAG="$3"

  scw login --token="${TOKEN}" --organization="${ORGANIZATION}" -s
  local SERVER_ID="$(scw run -e "${TAG}" --commercial-type=DEV1-S -d docker)"
  for i in {1..10}; do
    if scw exec "${SERVER_ID}" ls; then break; fi
    sleep 10
  done
  scw cp --user=root docker-compose.yml "${SERVER_ID}":/root
  scw exec "${SERVER_ID}" docker-compose up -d
  IP=$(scw ps --no-trunc | grep "${SERVER_ID}" | grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}')
}

# Upload a file or a directory on the server
function uploadFilesToServer(){
  local ORGANIZATION="$1"
  local TOKEN="$2"
  local SERVER_ID="$3"
  local FROM="$4"
  local TO="$5"

  scw login --token="${TOKEN}" --organization="${ORGANIZATION}" -s
  scw cp --user=root "${FROM}" "${SERVER_ID}":"${TO}"
}

# Upload a file or a directory on the server
function stopServer(){
  local ORGANIZATION="$1"
  local TOKEN="$2"
  local SERVER_ID="$3"
  scw login --token="${TOKEN}" --organization="${ORGANIZATION}" -s
  scw exec "${SERVER_ID}" docker-compose stop nginx-data
}

# Upload a file or a directory on the server
function startServer(){
  local ORGANIZATION="$1"
  local TOKEN="$2"
  local SERVER_ID="$3"
  scw login --token="${TOKEN}" --organization="${ORGANIZATION}" -s
  scw exec "${SERVER_ID}" docker-compose start
}

# Upload a file or a directory on all servers tagged with TAG
function uploadFilesToServers(){
  local ORGANIZATION="$1"
  local TOKEN="$2"
  local TAG="$3"
  local FROM="$4"
  local TO="$5"
  scw login --token="${TOKEN}" --organization="${ORGANIZATION}" -s
  for SERVER in $(scw ps -q -f tags="${TAG}"); do
    echo "Stopping server ${SERVER}"
    stopServer "${ORGANIZATION}" "${TOKEN}" "${SERVER}"
  done
  for SERVER in $(scw ps -q -f tags="${TAG}"); do
    echo "Uploading release files on ${SERVER}"
    uploadFilesToServer "${ORGANIZATION}" "${TOKEN}" "${SERVER}" "${FROM}" "${TO}" 
  done
  for SERVER in $(scw ps -q -f tags="${TAG}"); do
    echo "Starting server ${SERVER}"
    startServer "${ORGANIZATION}" "${TOKEN}" "${SERVER}"
  done
}

# Upload a folder to a bucket
function uploadFolderToBucket(){

  if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" ]; then
    echo "This script need 5 parameters"
    exit 1
  fi

  local AWS_ACCESS_KEY_ID="${1}"
  local AWS_SECRET_ACCESS_TOKEN="${2}"
  local BUCKET="${3}"
  local RELEASE_DIR="${4}"
  local PATH_IN_BUCKET="${5:-}"

cat >s3cfg <<EOF
[default]
# Endpoint
host_base = https://s3.fr-par.scw.cloud
host_bucket = https://s3.fr-par.scw.cloud
bucket_location = fr-par

# Login credentials
access_key = ${AWS_ACCESS_KEY_ID}
secret_key = ${AWS_SECRET_ACCESS_TOKEN}
EOF

  for FOLDER_TO_UPLOAD in $(ls ${RELEASE_DIR}); do
    docker run --rm -v $(pwd)/s3cfg:/root/.s3cfg -v "$(pwd)/${RELEASE_DIR}/${FOLDER_TO_UPLOAD}:/${FOLDER_TO_UPLOAD}" garland/docker-s3cmd \
      s3cmd put -r --acl-public "/${FOLDER_TO_UPLOAD}" "s3://${BUCKET}/${PATH_IN_BUCKET}/"
  done
}

# Download a folder from bucket
function downloadBucketFolderTo(){

  if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" ]; then
    echo "This script need 5 parameters"
    exit 1
  fi

  local AWS_ACCESS_KEY_ID="${1}"
  local AWS_SECRET_ACCESS_TOKEN="${2}"
  local BUCKET="${3}"
  local DEST_DIR="${4}"
  local PATH_IN_BUCKET="${5}"

cat >s3cfg <<EOF
[default]
# Endpoint
host_base = https://s3.fr-par.scw.cloud
host_bucket = https://s3.fr-par.scw.cloud
bucket_location = fr-par

# Login credentials
access_key = ${AWS_ACCESS_KEY_ID}
secret_key = ${AWS_SECRET_ACCESS_TOKEN}
EOF
  docker run --rm -v $(pwd)/s3cfg:/root/.s3cfg -v "$(pwd)/${DEST_DIR}:/download" garland/docker-s3cmd \
    s3cmd get -r --acl-public "s3://${BUCKET}/${PATH_IN_BUCKET}/" "/download" 
}

# Copy a remote folder from a path to an other path
function copyFolder(){

  if [ -z "${1}" -o -z "${2}" -o -z "${3}" -o -z "${4}" ]; then
    echo "This script need 5 parameters"
    exit 1
  fi

  local AWS_ACCESS_KEY_ID="${1}"
  local AWS_SECRET_ACCESS_TOKEN="${2}"
  local FROM="${3}"
  local TO="${4}"

cat >s3cfg <<EOF
[default]
# Endpoint
host_base = https://s3.fr-par.scw.cloud
host_bucket = https://s3.fr-par.scw.cloud
bucket_location = fr-par

# Login credentials
access_key = ${AWS_ACCESS_KEY_ID}
secret_key = ${AWS_SECRET_ACCESS_TOKEN}
EOF
    docker run --rm -v $(pwd)/s3cfg:/root/.s3cfg garland/docker-s3cmd \
      s3cmd cp -r --acl-public "s3://${FROM}" "s3://${TO}"
}

# Clean and restart proxies
function restartProxiesAfterUpgrade(){
  local SSH_KEY="${1}"
  local SSH_USER="${2}"

  chmod 400 "${SSH_KEY}"
  for ip in $(nslookup upgrade.recalbox.com 1.1.1.1 | grep Address | tail -n+2 | awk '{print $2}');do 
    ssh -o "StrictHostKeyChecking no" -i "${SSH_KEY}" "${SSH_USER}@${ip}" "bash ./restart.sh"
  done
}