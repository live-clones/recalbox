#!/bin/bash
# Required: apt-get install jq
# Syntax: scripts/linux/autobump.sh

commit=''

get_latest_commit()
{
	PROJECTID=$1
	commit=`curl -s "https://gitlab.com/api/v4/projects/$PROJECTID/repository/commits" | jq -r '.[0].id'`
}

subproject_bump()
{
	PACKAGE=$1
	STRING=$2
	PROJECTID=$3
	get_latest_commit $PROJECTID
	echo $PACKAGE set to commit $commit
	sed -i "s/$STRING[ =].*/$STRING = $commit/g" package/$PACKAGE/$PACKAGE.mk
}

echo "Bumping subprojects..."

subproject_bump recalbox-configgen RECALBOX_CONFIGGEN_VERSION 3260991
subproject_bump recalbox-emulationstation2 RECALBOX_EMULATIONSTATION2_VERSION 2391891
subproject_bump recalbox-themes RECALBOX_THEMES_VERSION 4601705
subproject_bump recalbox-manager2 RECALBOX_MANAGER2_VERSION 10464536
subproject_bump recalbox-hardware RECALBOX_HARDWARE_VERSION 12750999
subproject_bump recalbox-settings RECALBOX_SETTINGS_VERSION 12966260
subproject_bump recalbox-wpaf RECALBOX_WPAF_VERSION 21050838

echo "All subprojects bumped to latest commit"
