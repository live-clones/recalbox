#!/bin/bash

basedir=$(dirname $(realpath $0))

source "$basedir/../bunit.sh"
export PATH="$(bunitPath):$PATH"

upgradestatus="${basedir}/../../upgrade/recalbox-upgrade-diff-local.sh"



###################################################################################
init "when asking for diff on two changelogs then return diff"

when cat isCalled with "/tmp/changelog1" thenEcho "- a news in review 1"
when cat isCalled with "/tmp/changelog2" thenEcho "- a news in review 1\\\n- a news in review 2\\\n- a 2nd news in review 2"

assertThat "$upgradestatus --from-changelog /tmp/changelog1 --to-changelog /tmp/changelog2" echoed "- a news in review 2
- a 2nd news in review 2"


###################################################################################
init "when nothing in changelog but new version then show empty message"

when cat isCalled with "/tmp/changelog1" thenEcho "- a news in review 1"
when cat isCalled with "/tmp/changelog2" thenEcho "- a news in review 1"

assertThat "$upgradestatus --from-changelog /tmp/changelog1 --to-changelog /tmp/changelog2" echoed ""


###################################################################################
end