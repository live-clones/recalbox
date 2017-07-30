#!/bin/bash

basedir=$(dirname $(realpath $0))

source "$basedir/../bunit.sh"
export PATH="$(bunitPath):$PATH"

recalboxupgrade="${basedir}/../../upgrade/recalbox-upgrade.sh"

export VERBOSE=${VERBOSE:""}

function prepareCurlHeadersFor {
    UPGRADE_URL="$1"
    UPGRADE_DIR="$2"
    ARCH="$3"
    when curl isCalled with "-sfI ${UPGRADE_URL}/${ARCH}/root.tar.xz" thenEcho "Content-Length: 151221624"
    when curl isCalled with "-sfI ${UPGRADE_URL}/${ARCH}/boot.tar.xz" thenEcho "Content-Length: 10122162"
    when curl isCalled with "-sfI ${UPGRADE_URL}/${ARCH}/boot.tar.xz.sha1" thenEcho "Content-Length: 100"
    when curl isCalled with "-sfI ${UPGRADE_URL}/${ARCH}/root.tar.xz.sha1" thenEcho "Content-Length: 101"
    when curl isCalled with "-sfI ${UPGRADE_URL}/${ARCH}/root.list" thenEcho "Content-Length: 1000"
}

function prepareCurlFor {

    UPGRADE_URL="$1"
    UPGRADE_DIR="$2"
    ARCH="$3"
    prepareCurlHeadersFor $UPGRADE_URL $UPGRADE_DIR $ARCH

    # Files
    when curl isCalled with "-fs ${UPGRADE_URL}/${ARCH}/boot.tar.xz -o ${UPGRADE_DIR}/boot.tar.xz" thenExit 0
    when curl isCalled with "-fs ${UPGRADE_URL}/${ARCH}/root.tar.xz -o ${UPGRADE_DIR}/root.tar.xz" thenExit 0
    when curl isCalled with "-fs ${UPGRADE_URL}/${ARCH}/boot.tar.xz.sha1 -o ${UPGRADE_DIR}/boot.tar.xz.sha1" thenExit 0
    when curl isCalled with "-fs ${UPGRADE_URL}/${ARCH}/root.tar.xz.sha1 -o ${UPGRADE_DIR}/root.tar.xz.sha1" thenExit 0
    when curl isCalled with "-fs ${UPGRADE_URL}/${ARCH}/root.list -o ${UPGRADE_DIR}/root.list" thenExit 0
}

function prepareCurlAndDfFor {
    UPGRADE_URL="$1"
    UPGRADE_DIR="$2"
    ARCH="$3"
    prepareCurlFor $UPGRADE_URL $UPGRADE_DIR $ARCH
    when df isCalled thenEcho "\nnone           228216968 162386688  10241024  75% /"
}



###################################################################################
init "when file header download failed then exit with network error"

when curl isCalled with "-sfI https://url-to-use.com/rpi1/root.tar.xz" thenExit 1

assertThat "$recalboxupgrade --upgrade-dir /tmp/recalbox-upgrade --upgrade-url https://url-to-use.com --arch rpi1" exitedWith 2



###################################################################################
init "when file download failed then exit with network error"

prepareCurlHeadersFor https://url-to-use.com /tmp/recalbox-upgrade rpi3

when curl isCalled with "-fs https://url-to-use.com/rpi3/boot.tar.xz -o $/tmp/recalbox-upgrade/boot.tar.xz" thenExit 1

assertThat "$recalboxupgrade --upgrade-dir /tmp/recalbox-upgrade --upgrade-url https://url-to-use.com --arch rpi3" exitedWith 7



###################################################################################
init "when no space left on device then exit with no space code"

prepareCurlFor https://url-to-use.com /tmp/recalbox-upgrade rpi1

when df isCalled thenEcho "\nnone           228216968 162386688  10  75% /"

assertThat "$recalboxupgrade --upgrade-dir /tmp/recalbox-upgrade --upgrade-url https://url-to-use.com --arch rpi1" exitedWith 6



###################################################################################
init "when sha1sum do not find files then exit with sha1 error"

prepareCurlAndDfFor https://url-to-use.com /tmp/recalbox-upgrade rpi1

assertThat "$recalboxupgrade --upgrade-dir /tmp/recalbox-upgrade --upgrade-url https://url-to-use.com --arch rpi1" exitedWith 8



###################################################################################
init "when files downloaded and sha1sum different from checksum file then exit with sha1 error"

prepareCurlAndDfFor https://url-to-use.com /tmp/recalbox-upgrade rpi1

when sha1sum isCalled thenEcho "123456789 thefile"
when cat isCalled thenEcho "abadsha1"

assertThat "$recalboxupgrade --upgrade-dir /tmp/recalbox-upgrade --upgrade-url https://url-to-use.com --arch rpi1" exitedWith 8



###################################################################################
init "when files downloaded and sha1 checked then exit with no error"

prepareCurlAndDfFor https://url-to-use.com /tmp/recalbox-upgrade rpi1

when sha1sum isCalled thenEcho "123456789 thefile"
when cat isCalled thenEcho "123456789"

assertThat "$recalboxupgrade --upgrade-dir /tmp/recalbox-upgrade --upgrade-url https://url-to-use.com --arch rpi1" exitedWith 0


###################################################################################
end