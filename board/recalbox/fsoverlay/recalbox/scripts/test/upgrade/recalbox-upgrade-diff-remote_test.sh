#!/bin/bash

basedir=$(dirname $(realpath $0))

source "$basedir/../bunit.sh"
export PATH="$(bunitPath):$PATH"

upgradestatus="${basedir}/../../upgrade/recalbox-upgrade-diff-remote.sh"


###################################################################################
# The script do not use upgrade dir anymore
#init "when calling with upgrade dir then create the directory if it does not exist"

#UPGRADE_DIR="/tmp/fakeupgradir"

#when mkdir isCalled with "-p $UPGRADE_DIR" thenExit 0
#when curl isCalled thenExit 0
#when diff isCalled thenEcho ""
#when cat isCalled with "/tmp/changelog" thenEcho "- a news in review 1"

#$upgradestatus --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog

#assertThat mkdir hasBeenCalledWith "-p $UPGRADE_DIR"



###################################################################################
init "when curl returns error then returns network error"

UPGRADE_DIR="/tmp/upgradedir"

when curl isCalled thenExit 1

assertThat "$upgradestatus --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog" exitedWith 2



###################################################################################
init "when calling then uses diff"

UPGRADE_DIR="/tmp/upgradedir"

when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.changelog" thenExit 0
when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.version" thenEcho "review-2"
when diff isCalled thenEcho "- change1\\\n- change2"
when cat isCalled with "/tmp/changelog" thenEcho "- a news in review 1"


assertThat "$upgradestatus --verbose --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog" echoed "Update details:
Current version:       review-1
New version available: review-2
Changes:
- change1
- change2"



###################################################################################
init "when calling with changelog then return changes with verbose"

UPGRADE_DIR="/tmp/upgradedir"

when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.changelog" thenEcho "- a news in review 2"
when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.version" thenEcho "review-2"

when cat isCalled with "/tmp/changelog" thenEcho "- a news in review 1"

assertThat "$upgradestatus --verbose --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog" echoed "Update details:
Current version:       review-1
New version available: review-2
Changes:
- a news in review 2"



###################################################################################
init "when calling with changelog then return changes"

UPGRADE_DIR="/tmp/upgradedir"

when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.changelog" thenEcho "- a news in review 2"
when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.version" thenEcho "review-2"

when cat isCalled with "/tmp/changelog" thenEcho "- a news in review 1"

assertThat "$upgradestatus --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog" echoed "- a news in review 2"



###################################################################################
init "when nothing in changelog but new version then show empty message with verbose informations"

UPGRADE_DIR="/tmp/upgradedir"

when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.changelog" thenEcho "- a news in review 1"
when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.version" thenEcho "review-2"

when cat isCalled with "/tmp/changelog" thenEcho "- a news in review 1"

assertThat "$upgradestatus --verbose --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog" echoed "Update details:
Current version:       review-1
New version available: review-2
Changes:
- No changes detected"



###################################################################################
init "when nothing in changelog but new version then show empty message"

UPGRADE_DIR="/tmp/upgradedir"

when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.changelog" thenEcho "- a news in review 1"
when curl isCalled with "-f https://url-to-use.com/rpi1/recalbox.version" thenEcho "review-2"

when cat isCalled with "/tmp/changelog" thenEcho "- a news in review 1"

assertThat "$upgradestatus --upgrade-dir "$UPGRADE_DIR" --upgrade-url https://url-to-use.com --arch rpi1 --from-version review-1 --changelog /tmp/changelog" echoed ""


###################################################################################
end