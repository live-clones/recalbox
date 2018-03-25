#!/bin/bash -e

while [[ $# -gt 1 ]]
do
key="$1"
case $key in
    --from-version)
    FROM_VERSION="$2"
    shift
    ;;
    --uuid)
    UUID="$2"
    shift
    ;;
    --service-url)
    UPGRADE_URL="$2"
    shift
    ;;
    --arch)
    ARCH="$2"
    shift
    ;;
    *)
    ;;
esac
shift
done

if [[ -z ${FROM_VERSION} || -z ${UPGRADE_URL} || -z ${ARCH} || -z ${UUID} ]]; then
    echo -e "Usage:\n$0 --uuid UUID --from-version VERSION --service-url https://url-to-use.com --arch [rpi1|rpi2...]" && exit 1
fi

source /recalbox/scripts/upgrade/recalbox-upgrade.inc.sh

VERSION_URL=$(curl -sfG "${UPGRADE_URL}/${RECALBOX_URL}" \
	--data-urlencode "arch=${ARCH}" \
	--data-urlencode "boardversion=${FROM_VERSION}" \
	--data-urlencode "uuid=${UUID}")
if [[ "$?" != "0" ]]; then
  echo "no upgrade available"
  exit 2
fi

AVAILABLE_VERSION=$(curl -sfG "${VERSION_URL}/${RECALBOX_URL}/${ARCH}/recalbox.version" \
	--data-urlencode "arch=${ARCH}" \
	--data-urlencode "boardversion=${FROM_VERSION}" \
	--data-urlencode "uuid=${UUID}" \
	--data-urlencode "source=recalbox")
if [[ "${AVAILABLE_VERSION}" != "${FROM_VERSION}" ]]; then
  echo "${VERSION_URL}"
  exit 0
fi


echo "no upgrade available"
exit 1
