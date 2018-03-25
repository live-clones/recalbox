#!/bin/bash -e

while [[ $# -gt 1 ]]
do
key="$1"
case $key in
    --from-version)
    FROM_VERSION="$2"
    shift
    ;;
    --upgrade-url)
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

source /recalbox/scripts/upgrade/recalbox-upgrade.inc.sh

if [[ -z ${FROM_VERSION} || -z ${UPGRADE_URL} || -z ${ARCH} ]]; then
    echo -e "Usage:\n$0 --from-version VERSION --upgrade-url https://url-to-use.com --arch [rpi1|rpi2...]" && exit 1
fi

AVAILABLE_VERSION=$(curl -s "${UPGRADE_URL}/${RECALBOX_URL}/${ARCH}/recalbox.version?source=recalbox")
if [[ "$?" != "0" ]];then
  echo "cannot contact ${UPGRADE_URL}"
  exit 2
fi
if [[ "${AVAILABLE_VERSION}" != "${FROM_VERSION}" ]];then
  echo "${AVAILABLE_VERSION}"
  exit 0
fi

echo "no upgrade available"
exit 1
