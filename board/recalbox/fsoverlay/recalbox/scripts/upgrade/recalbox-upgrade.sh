#!/bin/bash -e


COMMAND="$1"

if [[ -z ${COMMAND} ]]; then
  echo -e "Usage:\n$0 COMMAND" && exit 1
fi


BINDIR="${BINDIR:-/recalbox/scripts/upgrade/}"
SYSTEMSETTINGS="recalbox_settings"
RECALBOX_SYSTEM_DIR="/recalbox/share/system"
INSTALLED_VERSION=$(cat /recalbox/recalbox.version)
ARCH=$(cat /recalbox/recalbox.arch)
UPGRADETYPE="$($SYSTEMSETTINGS  -command load -key updates.type)"

SERVICE_URL="https://recalbox-releases.s3.nl-ams.scw.cloud"
REVIEW_URL="https://recalbox-reviews.s3.nl-ams.scw.cloud"

UUID=$("$BINDIR/../system/uuid.sh" --uuid-file "${RECALBOX_SYSTEM_DIR}/uuid")

if [[ "${UPGRADETYPE}" != "stable" ]]; then
  UPGRADETYPE="${REVIEW_URL}/${UPGRADETYPE}"
else
  UPGRADETYPE="${SERVICE_URL}/${UPGRADETYPE}"
fi

## COMMANDS
if [ "${COMMAND}" == "canupgrade" ];then
  if [[ "${UPGRADETYPE}" == "stable" ]]; then
    "$BINDIR/recalbox-canupgrade.sh" --from-version "${INSTALLED_VERSION}" --service-url "${SERVICE_URL}" --arch "${ARCH}" --uuid "${UUID}"
    exit $?
  else
    "$BINDIR/recalbox-canupgrade-from-archive.sh" --from-version "${INSTALLED_VERSION}" --upgrade-url "${UPGRADETYPE}" --arch "${ARCH}"
    exit $?
  fi
  echo "no upgrade available"
  exit 12
fi

if [ "${COMMAND}" == "upgrade" ];then
  if [[ "${UPGRADETYPE}" == "stable" ]]; then
    UPGRADE_URL=$("$BINDIR/recalbox-canupgrade.sh" --from-version "${INSTALLED_VERSION}" --service-url "${SERVICE_URL}" --arch "${ARCH}" --uuid "${UUID}")
    [[ "$?" != "0" ]] && exit 1
  else
    UPGRADE_URL="${UPGRADETYPE}"
  fi
  "$BINDIR/recalbox-do-upgrade.sh" --upgrade-dir "${RECALBOX_SYSTEM_DIR}/upgrade" --upgrade-url "${UPGRADE_URL}" --arch "${ARCH}" 2> "/tmp/recalbox.do.upgrade.log"
  exitcode="$?"
  cat "/tmp/recalbox.do.upgrade.log" | recallog -s "${INIT_SCRIPT}" -t "UPGRADE"
  exit "${exitcode}"
fi

if [ "${COMMAND}" == "diffremote" ];then
   if [[ "${UPGRADETYPE}" == "stable" ]]; then
    UPGRADE_URL=$("$BINDIR/recalbox-canupgrade.sh" --from-version "${INSTALLED_VERSION}" --service-url "${SERVICE_URL}" --arch "${ARCH}" --uuid "${UUID}")
    [[ "$?" != "0" ]] && exit 1
  else
    UPGRADE_URL="${UPGRADETYPE}"
  fi
  "$BINDIR/recalbox-upgrade-diff-remote.sh" --from-version "${INSTALLED_VERSION}" --upgrade-url "${UPGRADE_URL}" --arch "${ARCH}" --changelog "/recalbox/recalbox.changelog"
  exit $?
fi

if [ "${COMMAND}" == "difflocal" ];then
  "$BINDIR/recalbox-upgrade-diff-local.sh" --from-changelog "${RECALBOX_SYSTEM_DIR}/recalbox.changelog.done" --to-changelog "/recalbox/recalbox.changelog"
  exit $?
fi



echo "command not found"
exit 20