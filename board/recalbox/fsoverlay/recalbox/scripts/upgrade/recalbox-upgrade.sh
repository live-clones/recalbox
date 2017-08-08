#!/bin/bash -e


COMMAND="$1"
BINDIR="${BINDIR:-/recalbox/scripts/upgrade/}"
systemsetting="python /usr/lib/python2.7/site-packages/configgen/settings/recalboxSettings.pyc"

if [[ -z ${COMMAND} ]]; then
  echo -e "Usage:\n$0 COMMAND" && exit 1
fi


UPGRADETYPE="$($systemsetting  -command load -key updates.type)"

if [[ "${UPGRADETYPE}" == "beta" || "${UPGRADETYPE}" == "unstable" ]]; then
  UPGRADETYPE="stable"
fi

INSTALLED_VERSION=`cat /recalbox/recalbox.version`
ARCH=`cat /recalbox/recalbox.arch`


## COMMANDS
if [ "${COMMAND}" == "canupgrade" ];then
  if [[ "${UPGRADETYPE}" == "stable" ]]; then
    echo "TODO"
  else
    "$BINDIR/recalbox-canupgrade-from-archive.sh" --from-version "$INSTALLED_VERSION" --upgrade-url "${UPGRADETYPE}" --arch "${ARCH}"
    exit $?
  fi
  echo "no upgrade available"
  exit 12
fi

if [ "${COMMAND}" == "upgrade" ];then
  if [[ "${UPGRADETYPE}" == "stable" ]]; then
    echo "TODO"
  else
    "$BINDIR/recalbox-do-upgrade.sh" --upgrade-dir "/recalbox/share/system/upgrade" --upgrade-url "${UPGRADETYPE}" --arch "${ARCH}"
    exit $?
  fi
fi

if [ "${COMMAND}" == "diffremote" ];then
  if [[ "${UPGRADETYPE}" == "stable" ]]; then
    echo "TODO"
  else
    "$BINDIR/recalbox-upgrade-diff-remote.sh" --from-version "${INSTALLED_VERSION}" --upgrade-url "${UPGRADETYPE}" --arch "${ARCH}" --changelog "/recalbox/recalbox.changelog"
    exit $?
  fi
  echo "no upgrade available"
  exit 12
fi

if [ "${COMMAND}" == "difflocal" ];then
  "$BINDIR/recalbox-upgrade-diff-local.sh" --from-changelog "/recalbox/share/system/recalbox.changelog.done" --to-changelog "/recalbox/recalbox.changelog"
  exit $?
fi



echo "command not found"
exit 20