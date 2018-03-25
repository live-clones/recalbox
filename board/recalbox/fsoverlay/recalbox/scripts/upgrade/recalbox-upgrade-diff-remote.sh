#!/bin/bash -e

while [[ $# -gt 1 ]]
do
key="$1"
case ${key} in
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
    --changelog)
    CHANGELOG="$2"
    shift
    ;;
    --verbose)
    VERBOSE="YES"
    ;;
    *)
    ;;
esac
shift
done

if [[ -z ${UPGRADE_URL} || -z ${ARCH} || -z ${FROM_VERSION} || -z ${CHANGELOG} ]]; then
  echo -e "Usage:\n$0 --verbose --from-version currentVersion --upgrade-url https://url-to-use.com --arch [rpi1|rpi2...] --changelog /recalbox/existing.changelog" && exit 1
fi

source /recalbox/scripts/upgrade/recalbox-upgrade.inc.sh

NEW_CHANGELOG="$(curl -sf "${UPGRADE_URL}/${RECALBOX_URL}/${ARCH}/recalbox.changelog?source=recalbox" || exit 2)"
NEW_VERSION="$(curl -sf "${UPGRADE_URL}/${RECALBOX_URL}/${ARCH}/recalbox.version?source=recalbox" || exit 2)"
OLD_CHANGELOG="$(cat "${CHANGELOG}" || exit 1)"

changes="$(diff --changed-group-format='%>' --unchanged-group-format='' <(echo "${OLD_CHANGELOG}") <(echo "${NEW_CHANGELOG}") || true)"

if [[ "${VERBOSE}" == "YES" ]];then
  echo -e "Update details:
Current version:       ${FROM_VERSION}
New version available: ${NEW_VERSION}
Changes:
${changes:-- No changes detected}"
else
 echo -ne "${changes}"
fi

exit 0
